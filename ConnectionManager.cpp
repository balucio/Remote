/*
  ConnectionManager.cpp - Handle initial device configuration
  Saul bertuccio 11 feb 2017
  Released into the public domain.
*/

#include "ConnectionManager.h"

ConnectionManager::ConnectionManager() { }

void ConnectionManager::initConnection() {

  String ssid = WiFi.SSID();
  String pass = WiFi.psk();

  boolean station = false;

  if (ssid.length() != 0) {
    // WiFi.persistent(false);
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    delay(1000);
    station = ConnectionManager::connectWifi(ssid, pass, false);
  }

  if (station) {
    Serial.print("Connesso alla rete Wifi ");
    Serial.println(ssid);
    Serial.print("Indirizzo IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("Impossible connettersia alla rete Wifi ");
    Serial.println(ssid);
    Serial.println("Commuto in modo AP");
    ConnectionManager::setupAp(FPSTR(AP_SSID), FPSTR(AP_PASS));
  }
}

boolean ConnectionManager::connectWifi(String ssid, String pass, bool force=false) {

    // Stato iniziale wifi
    int conn_status = WL_IDLE_STATUS;

    if (!force) {
      // Tento prima con le credenziali memorizzate
      Serial.print("Tento collegamento Wify con credenziali memorizzate.");
      WiFi.begin();
      conn_status = ConnectionManager::waitForConnection(false);
    }

    // Se fallisce tento con le credenziali passate
    if (conn_status != WL_CONNECTED && ssid.length() != 0) {
      Serial.print("Tento collegamento alla rete Wifi ");
      Serial.println(ssid);
      Serial.print("Password ");
      Serial.println(pass);

      if (force)
        WiFi.disconnect(true);
      WiFi.begin(ssid.c_str(), pass.c_str());

      conn_status = ConnectionManager::waitForConnection(true);
    }

    Serial.println("");
    Serial.print("Stato :");
    Serial.println(WiFi.status());

    return (conn_status == WL_CONNECTED);
}

String ConnectionManager::encryptionCodeToString(int code) {

  switch (code) {
    case  ENC_TYPE_WEP: return String("WEP");
    case ENC_TYPE_TKIP: return String("TKIP");
    case ENC_TYPE_CCMP: return String("CCMP");
    case ENC_TYPE_NONE: return String("Nessuna");
    case ENC_TYPE_AUTO: return String("Auto");
    default: return String("Nd.");
  }
}

WifiNetworks ConnectionManager::getNetworkList() {

  Serial.println("Avvio scansione reti");

  // WiFi.scanNetworks ritorna il numero di reti trovate
  int n = WiFi.scanNetworks();
  
  Serial.print("Scansione terminata: ");

  WifiNetworks nets = WifiNetworks(n);
  if (n == 0) {

    Serial.println("nessuna rete trovata");
    return nets;
 
  } else {
  
    Serial.print("trovate ");
    Serial.print(n);
    Serial.println(" reti");

    String ssid; // encryption;
    long rssi;
    int enc_code;
    
    for( int i = 0; i < n; i++ ) {
      
      ssid = WiFi.SSID(i);
      enc_code = WiFi.encryptionType(i);
      // encryption = ConnectionManager::encryptionCodeToString(enc_code);
      rssi = WiFi.RSSI(i);
      nets.append(ssid, enc_code, rssi);
    }
    return nets;

  }
}

void ConnectionManager::setupAp(String ssid, String password) {

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid.c_str(), password.c_str());
  delay(1000);

  Serial.print("Access Point SSID: ");
  Serial.println(ssid);
  Serial.print("Access Point Password: ");
  Serial.println(password);
  Serial.print("Indirizzo IP Access Point: ");
  Serial.println(WiFi.softAPIP());

}

String ConnectionManager::getSsid() { return WiFi.SSID(); }
String ConnectionManager::getPassword() { return WiFi.psk();}

IPAddress ConnectionManager::getStationIpAddress() {
  
  return ( WiFi.status() == WL_CONNECTED && WiFi.getMode() == WIFI_STA )
    ? WiFi.localIP()
    : IPAddress(0, 0, 0, 0)
 ;
}

IPAddress ConnectionManager::getIpAddress() {
  
  if ( WiFi.getMode() != WIFI_STA )
    return WiFi.softAPIP();
  
  return ConnectionManager::getStationIpAddress();
}

boolean ConnectionManager::isApMode() {
    return WiFi.getMode() != WIFI_STA;
}

int ConnectionManager::waitForConnection(bool use_timeout=true) {

  if (!use_timeout)
    return WiFi.waitForConnectResult();

  int attempt = CONN_ATTEMPT / 2;

  do {
    delay(1000);
    Serial.print(".");
  } while ( attempt-- > 0 && (WiFi.status() != WL_CONNECTED));

  Serial.println("");

  return WiFi.status();
}

boolean ConnectionManager::isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}


