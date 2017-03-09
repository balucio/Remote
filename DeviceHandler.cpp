/*
  DeviceHandler.cpp - Work as device loader and save device configuration
  Saul bertuccio 9 feb 2017
  Released into the public domain.
*/

#include "DeviceHandler.h"

const String DeviceHandler::DATA_DIR = "/data/";

DeviceHandler::DeviceHandler():
  is_opened(SPIFFS.begin()),
  device(NULL)
{}


DeviceHandler::~DeviceHandler() {
  delete device;
}

boolean DeviceHandler::setDevice( const String &device_name, const String &device_type ) {

  if ( device && (device->getName() == device_name) )
    return true;

  if (device)
    delete device;

  if (device = loadDeviceFile( device_name )) {

    if (device->getType() != device_type) {
      device->setType(device_type);
       return DeviceHandler::saveDeviceFile( device );
    }
    
  } else if (device = new Device(device_name, device_type)) {

    if (!DeviceHandler::saveDeviceFile( device )) {
      delete device;
    } else {
      // Invalidate device counter
      DeviceHandler::recountDevices(true);
    }

  }
  return device != NULL;
}

boolean DeviceHandler::setDevice( const String &device_name ) {

  if ( device && (device->getName() == device_name) )
    return true;

  if (device)
    delete device;

  device = loadDeviceFile( device_name );
  return device != NULL;
}

boolean DeviceHandler::addDeviceKey( String &kname, int kpulse, int kcode ) {

  if (!device) return false;

  return device->appendKey( kname, kpulse, kcode )
    && DeviceHandler::saveDeviceFile( device );
}

boolean DeviceHandler::deleteDeviceKey( String &kname ) {

  if (!device) return false;
  return device->removeKey( kname )
    && DeviceHandler::saveDeviceFile( device );
}


Device & DeviceHandler::getDevice() { return *device; }

Device * DeviceHandler::loadDeviceFile( const String &device_name ) {

  String file_name = DATA_DIR + device_name;
  
  if (!SPIFFS.exists(file_name)) {
    Serial.println("GetDedvice: file " + file_name + " does not exits");
    return NULL;
  }

  File f = SPIFFS.open( file_name, "r");

  if (!f) {
    Serial.println("Errore apertura file dispositivo");
    return NULL;
  }

  // First line is device type
  String line = f.readStringUntil('\n');

  if (!line) {
    Serial.println("Impossibile leggere il tipo di dispositivo");
    return NULL;
  }

  line.remove( line.length() - 1);
  Device * d = new Device( device_name, line );

  int sep1, sep2;
  String key_name;
  int key_pulse;
  long int key_code;

  while ( f.available() ) {
    line = f.readStringUntil(DeviceHandler::EOL);
    sep1 = line.indexOf(DeviceHandler::SEP);
    if (sep1 == -1)
      continue;
    key_name = line.substring(0, sep1 - 1);
    sep2 = line.indexOf(DeviceHandler::SEP, sep1 + 1);
    if (sep2 == -1)
      continue;
    key_pulse = line.substring(sep1 + 1, sep2).toInt();
    key_code = line.substring(sep2 + 1).toInt();

    d->appendKey( key_name, key_pulse, key_code );
  }

  f.close();
  return d;
}

boolean DeviceHandler::renameDevice( const String &new_name) {

  if (!device) return false;

  if (DeviceHandler::existsDeviceFile( new_name ))
    return false;

  if (DeviceHandler::renameDeviceFile( device->getName(), new_name)) {
    device->setName(new_name);
    return true;
  }
  return false;
}

boolean DeviceHandler::renameDeviceFile( const String &old_name, const String &new_name) {
  return SPIFFS.rename(DATA_DIR + old_name, DATA_DIR + new_name);
};

boolean DeviceHandler::deleteDevice() {

  if (!device) return false;

  bool r = true;
  if (DeviceHandler::existsDeviceFile( device->getName() ))
    r = DeviceHandler::deleteDeviceFile( device->getName() );

  // Invalidate device counter
  DeviceHandler::recountDevices(true);

  if (r) { 
    delete device;
    device = NULL;
  }
  return r;
}

boolean DeviceHandler::deleteDeviceFile( const String &device_name ) { return SPIFFS.remove(DATA_DIR + device_name); }

boolean DeviceHandler::saveDevice() {

  if (!device)
    return false;

  return DeviceHandler::saveDeviceFile( device );
}

boolean DeviceHandler::saveDeviceFile( Device * device ) {

  String fname = DATA_DIR + device->getName();
  File f = SPIFFS.open(fname, "w");

  if (!f) {
    Serial.println("SaveDevice Error: unable to open " + fname + " file");
    return false;
  }

  // Salvo il tipo di dispositivo
  f.println( device->getType());

  Key * k = device->getKeys();

  String k_name;

  while (k) {
    k_name = k->getName().substring(0, Key::MAX_KEY_NAME_LEN);
    f.print(k_name + DeviceHandler::SEP);
    f.print(String( k->getPulse() ) + DeviceHandler::SEP);
    f.print(String( k->getCode() ) + DeviceHandler::EOL);
    k = k->getNext();
  }

  f.close();

  return true;
}

boolean DeviceHandler::existsDeviceFile( const String &device_name ) { return SPIFFS.exists(DATA_DIR + device_name); }

String * DeviceHandler::getDevicesName() {

  String * names = NULL;

  int c = getDevicesNum();

  if (c <= 0) return NULL;
    
  names = new String[c];

  Dir dir = SPIFFS.openDir(DATA_DIR);
  int l = DATA_DIR.length();

  while (dir.next())
    names[--c] = dir.fileName().substring(l);

  return names;

}

int DeviceHandler::getDevicesNum() { return recountDevices(false); }

int DeviceHandler::recountDevices(bool force) {

  static int device_num = -1;

  if (device_num >= 0 && !force)
    return device_num;

  Dir dir = SPIFFS.openDir(DATA_DIR);
  device_num = 0;
  
  while (dir.next())
    device_num++;

  return device_num;

}

int DeviceHandler::getDeviceTypesNum() { return Device::TYPE_NUM; }
String * DeviceHandler::getDeviceTypes() { return Device::TYPES; }
String * DeviceHandler::getDeviceTypesDescription() { return Device::TYPES_DESCRIPTION; }




