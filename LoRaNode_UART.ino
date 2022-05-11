/* 
 *  ESP32 UART Input & LoRa Output.
 *  by Alejandro D.
 *  6 May 2022
 */

#include <LoRaNow.h>
#include "SoftwareSerial.h"

#define sck 14
#define miso 12
#define mosi 13
#define ss 16
#define dio0 26

#define TXD1 27
#define RXD1 25

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600,SERIAL_8N1,RXD1, TXD1);
  delay(1000);
  Serial.print("*** LoRaWAN Node ***\n");

  LoRaNow.setFrequencyUS(); //904.1 MHz - Used in USA, Canada, and South America
  LoRaNow.setPinsSPI(sck, miso, mosi, ss, dio0);

  if(!LoRaNow.begin()) {
    Serial.println("LoRaNow initialization failed. Check connections and retry.");
    while(true);
  }

  //LoRaNow.onMessage(onMessage);
  //LoRaNow.onSleep(onSleep);
  //LoRaNow.showStatus(Serial);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("-- Start of Message ---");
  while(Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
  Serial.println("\r--- End of Message ---\n");
  delay(5000); //Sleep

 // LoRaNow.loop();
}


void onMessage(uint8_t *buffer, size_t size)
{
  Serial.print("Receive Message: ");
  Serial.write(buffer, size);
  Serial.println();
  Serial.println();
}

void onSleep()
{
  String Final;
  Serial.println("Sleep");
  delay(5000); //Sleep
  Serial.println("Send Message");

  if (Serial2.available()) {
    int testData = Serial2.read();
    Final = testData;
  }
  
  Serial.println(Final);
//  LoRaNow.print(Final);
//  LoRaNow.print(millis());
//  LoRaNow.send();
 
}
