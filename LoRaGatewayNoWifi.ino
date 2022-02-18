/*
  LoRaNow Simple Gateway with ESP32 setPins
  This code creates a webServer to show the LoRa messages.
  created 27 04 2019
  by Luiz H. Cassettari
*/

#include <LoRaNow.h>
#include <WiFi.h>
#include <WebServer.h>
#include <StreamString.h>

#define SCK 14
#define MISO 12
#define MOSI 13
#define SS 16
#define DIO0 26

const char *script = "<script>function loop() {var resp = GET_NOW('loranow'); var area = document.getElementById('area').value; document.getElementById('area').value = area + resp; setTimeout('loop()', 1000);} function GET_NOW(get) { var xmlhttp; if (window.XMLHttpRequest) xmlhttp = new XMLHttpRequest(); else xmlhttp = new ActiveXObject('Microsoft.XMLHTTP'); xmlhttp.open('GET', get, false); xmlhttp.send(); return xmlhttp.responseText; }</script>";
static StreamString string;

void setup(void)
{
  Serial.begin(115200);
  Serial.println("STARTING GATEWAY - SERIAL 115200");
  LoRaNow.setFrequencyUS(); // Select the frequency 904.1 MHz - Used in USA, Canada and South America
  LoRaNow.setPinsSPI(SCK, MISO, MOSI, SS, DIO0); // Only works with ESP32
  if (!LoRaNow.begin())
  {
    Serial.println("LoRa init failed. Check your connections.");
    while (true)
      ;
  }
  LoRaNow.onMessage(onMessage);
  LoRaNow.gateway();
}

void loop(void)
{
  LoRaNow.loop();
}

void onMessage(uint8_t *buffer, size_t size)
{
  unsigned long id = LoRaNow.id();
  byte count = LoRaNow.count();
  Serial.print("Node Id: ");
  Serial.println(id, HEX);
  Serial.print("Count: ");
  Serial.println(count);
  Serial.print("Message: ");
  Serial.write(buffer, size);
  Serial.println();
  Serial.println();

  // Send data to the node
  LoRaNow.clear();
  LoRaNow.print("LoRaNow Gateway Message ");
  LoRaNow.print(millis());
  LoRaNow.send();
}
