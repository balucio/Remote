/*
  Remote.ino - Handle RF 433 remote device
  Saul bertuccio 5 feb 2017
  Released into the public domain.
*/

#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"

#include "Config.h"


ESP8266WebServer server(80);
Config cfg;

void setup() {
  
  Serial.begin ( 9600 );

  char * essid = cfg.getEssidName();
  char * password = cfg.getEssidPassword();
  char * msg = new char[256];

  if ( strlen (essid) )
  {
       Serial.println ("Unable to get ESSID from Flash, going in config mode");
       return;
  }

  sprintf(msg, "Got a Wifi ESSID %s and password %s, now I try to connect.", essid, password);
  Serial.println (msg);

  // Wait for connection
  WiFi.disconnect();
  WiFi.persistent(false);
  
  WiFi.begin(essid, password);
  isWifiConnected();
 
}

bool isWifiConnected(void) {
  
  int c = 0;
  Serial.println("Waiting for Wifi to connect");

  while ( c++ < 20 ) {

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected, whit IP address: ");
      Serial.println(WiFi.localIP());
      return true;
    }
   
    delay(1000);
    Serial.println("Attempt to connect to Wifi failed, status: " + WiFi.status());    
  }

  Serial.println("Timeout, unable to connect to Wifi");
  return false;
}

void loop() {
  // put your main code here, to run repeatedly:
  while (true) {
   delay ( 10000 );
   Serial.println("riavvio");
  } 
}

