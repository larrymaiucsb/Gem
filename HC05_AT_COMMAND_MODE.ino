/*
 ARDUINO UNO ELEGOO - HC05 Bluetooth Module
 Software serial multple serial test
 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.
 The circuit:
 RX is digital pin 11 (connect to TX of other device)
 TX is digital pin 10 (connect to RX of other device)
*/

// Target ESP32 LoRa Gateway BT Mac Address = a8:03:2a:f2:74:2a
#include <SoftwareSerial.h>

SoftwareSerial mySerial(11, 10); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600); //Serial monitor needs to be set to Both NL & CR, 9600 baud.
  mySerial.begin(38400);
  Serial.println("***AT COMMAND MODE***");
  mySerial.println("Hello, world! - mySerial");
}

void loop() { // run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
} 
