/*
  ConnectionManager.h - Handle basic configuration data
  Saul bertuccio 11 feb 2017
  Released into the public domain.
*/

#ifndef ConnectionManager_h
#define ConnectionManager_h

#include "ESP8266WiFi.h"
#include "WString.h"

#include "WifiNetworks.h"

const char AP_SSID[] PROGMEM = "esp8266_remote\0";
const char AP_PASS[] PROGMEM = "administrator\0";
const int CONN_ATTEMPT = 10;

class ConnectionManager {


  public:

    static void initConnection();
    static boolean isApMode();
    static IPAddress getStationIpAddress();
    static IPAddress getIpAddress();
    static String getSsid();
    static String getPassword();
    static WifiNetworks getNetworkList();
    static String encryptionCodeToString(int code);
    static boolean connectWifi(String ssid, String pass, bool force);

    static boolean isIp(String str);


  private:

    ConnectionManager();

    static void setupAp(String ssid, String pass);
    static int waitForConnection(bool use_timeout);

};

#endif


