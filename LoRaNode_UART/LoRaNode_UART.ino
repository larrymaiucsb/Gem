/* 
 *  ESP32 UART Input & LoRa Output.
 *  by Alejandro D.
 *  6 May 2022
 */

#include <LoRaNow.h>
//#include "SoftwareSerial.h"

#define sck 14
#define miso 12
#define mosi 13
#define ss 16
#define dio0 26

//#define TXD1 TXO
//#define RXD1 RXI

String buffet = "";

void setup() {
  
  Serial.begin(115200);
  delay(1000);

  LoRaNow.setFrequencyUS(); //904.1 MHz - Used in USA, Canada, and South America
  LoRaNow.setPinsSPI(sck, miso, mosi, ss, dio0);

  if(!LoRaNow.begin()) {
    Serial.println("LoRaNow initialization failed. Check connections and retry.");
    //while(true);
  }

  LoRaNow.onSleep(onSleep);
}

void loop() {

  while(Serial.available()) {
    buffet += char(Serial.read());
  }

  LoRaNow.loop();
}


void onMessage(uint8_t *buffer, size_t size)
{
  //Serial.print("Receive Message: ");
  //Serial.write(buffer, size);
  //Serial.println();
  //Serial.println();
}

void onSleep()
{
  delay(200); //Fake sleep

  LoRaNow.print(buffet);
  buffet = "";
  
  LoRaNow.send();
 
}
