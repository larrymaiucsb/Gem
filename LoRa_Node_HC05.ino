/*
  ESP32 LoRa 1-Channel Node communication with HC05 BT Module on STM32 board.
  LoRaNow Simple Node Code from LoRaNow Library created 01 04 2019.
  This code uses BT from ESP32 and connects to HC05. Recieves message from HC05 and
  sends message over LoRa spectrum and listen expecting some valid message from the gateway
*/

#include <LoRaNow.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define BT_DISCOVER_TIME 1000000
#define BT_MESSAGE_LEN 20
#define sck 14
#define miso 12
#define mosi 13
#define ss 16
#define dio0 26

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(9600);
  SerialBT.enableSSP();
  SerialBT.begin(115200);
  SerialBT.begin("ESP32test", false); //Bluetooth device name, isMaster
  Serial.println("\n*** AVAILABLE FOR PAIRING ***\n");
  //LORA NODE CODE FROM HERE DOWN
  //Serial.begin(115200);
  Serial.println("LoRaNow Simple Node");
  LoRaNow.setFrequencyUS(); // 904.1 MHz - Used in USA, Canada and South America
  //LoRaNow.setFrequencyAU(); // 917.0 MHz - Used in Australia, Brazil and Chile
  //LoRaNow.setFrequency(frequency);
  //LoRaNow.setSpreadingFactor(sf);
  //LoRaNow.setPins(ss, dio0);
  LoRaNow.setPinsSPI(sck, miso, mosi, ss, dio0); // Only works with ESP32

  if (!LoRaNow.begin()) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  LoRaNow.onMessage(onMessage);
  LoRaNow.onSleep(onSleep);
  LoRaNow.showStatus(Serial);
}

void loop() {
  LoRaNow.loop();
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
  Serial.println("Sleep");
  delay(5000); // "kind of a sleep"
  Serial.println("Send Message");
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    String temp = "";
    int testData = SerialBT.read();
    for(int i=0; i<BT_MESSAGE_LEN; i++) {
      char curr = testData;
      if(curr == 'Y') {
        //Append new line, start getting next measurement.
        temp = temp + '\n';
        testData = SerialBT.read();
      }
      else {
        //Append number to curr measurement, get next value.
        temp = temp + curr;
        testData = SerialBT.read(); 
      }
  }
  Serial.println(temp);
  LoRaNow.print(temp);
  LoRaNow.print(millis());
  LoRaNow.send();
  }
}
