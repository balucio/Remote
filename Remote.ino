/*
  Remote.ino - Handle RF 433 remote device
  Saul bertuccio 5 feb 2017
  Released into the public domain.
*/

#include "Arduino.h"
ADC_MODE(ADC_VCC);


#include "DeviceHandler.h"
#include "ConnectionManager.h"

#include "Portal.h"

Portal portal;

String ssid, pass;

void setup() {
  
  Serial.begin(115200);
  delay(2000);
  
  Serial.print("ESP8266 chip id: ");
  Serial.println(ESP.getChipId());
  Serial.print("ESP8266 velocita flash: ");
  Serial.println(ESP.getFlashChipSpeed());
  Serial.print("Tensione di alimentazione : ");
  Serial.println(ESP.getVcc()); 
  Serial.print("Ultimo motivo reset: ");
  Serial.println(ESP.getResetReason());

  Serial.println("Configuro connessione Wifi");
  ConnectionManager::initConnection();
  Serial.println("Avvio webserver");
  while (!portal.setup()) {
      Serial.println("Impossibile configurare Server HTTP");
      delay(10000);
  }
}

void loop() {
  portal.handleRequest();

  if (portal.needRestart()) {
    Serial.println("Riavvio il dispositivo");
    delay(5000);
    ESP.restart();
  }
}





