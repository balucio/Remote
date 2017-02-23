/*
  DeviceHandler.cpp - Work as device loader and save device configuration
  Saul bertuccio 9 feb 2017
  Released into the public domain.
*/

#include "DeviceHandler.h"

DeviceHandler::DeviceHandler() {
  is_opened = SPIFFS.begin();
}

int DeviceHandler::deviceCount() { return device_num; }

String * DeviceHandler::listDeviceNames() {

  Dir dir = SPIFFS.openDir("/");

  String * names = NULL;
  device_num = 0;
  
  while (dir.next())
    device_num++;

  names = new String[device_num];
  dir = SPIFFS.openDir("/");

  device_num = 0;

  while (dir.next())
    names[device_num] = dir.fileName();

  return names;
}

Device * DeviceHandler::loadDevice( String &device_name ) {
  
  if (!SPIFFS.exists(device_name))
    Serial.println("file " + device_name + " does not exits");
    return NULL;

  File f = SPIFFS.open( device_name, "w");

  if (!f) {
    Serial.println("file open failed");
    return NULL;
  }

  Device * device = new Device( device_name );

  int sep1, sep2;
  String line, key_name;
  int key_pulse;
  long int key_code;
  

  while ( line = f.readStringUntil('\n') ) {
    sep1 = line.indexOf(';');
    if (sep1 == -1)
      continue;
    key_name = line.substring(0, sep1 - 1);
    sep2 = line.indexOf(';', sep1 + 1);
    if (sep2 == -1)
      continue;
    key_pulse = line.substring(sep1 + 1, sep2).toInt();
    key_code = line.substring(sep2 + 1).toInt();

    device->appendKey( key_name, key_pulse, key_code );
  }

  f.close();
  return device;
}

boolean DeviceHandler::deleteDevice( String &device_name ) {

  if (!SPIFFS.exists(device_name))
      return true;
  
  boolean r = SPIFFS.remove(device_name);

  if (!r) {
   Serial.print("DeleteDevice Error: unable to delete ");
   Serial.println(device_name);
  }

  return r;
};


boolean DeviceHandler::renameDevice( String &old_name, String &new_name ) {

  if (!SPIFFS.exists(old_name))
      return false;
  
  boolean r = SPIFFS.rename(old_name, new_name);

  if (!r) {
    Serial.print("Rename Device Error: unable rename ");
    Serial.print(old_name);
    Serial.print(" in ");
    Serial.println(new_name);
  }

  return r;
};

boolean DeviceHandler::saveDevice( Device * device ) {

  if ( device == NULL ) {
    Serial.println("SaveDevice Error: Null device");
    return false;
  }

  File f = SPIFFS.open(device->getName(), "w");

  if (!f) {
    Serial.println("SaveDevice Error: unable to open " + device->getName() + " file");
    return false;
  }

  Key * k = device->getKeys();

  int k_pad_len;
  String k_name;

  while (k) {
    k_name = k->getName().substring(0, Key::MAX_KEY_NAME_LEN);
    k_pad_len = k_name.length() - Key::MAX_KEY_NAME_LEN;
    f.print(k_name +  String("          ").substring(0, k_pad_len));
    f.println( String( k->getCode() ) );

    k = k->getNext();
  }

  return true;
}


