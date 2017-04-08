/*
  Device.cpp - Handle the controlled device implementation
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#include "Device.h"

String Device::TYPES[Device::TYPE_NUM] = { "IRDA", "RF433"};
String Device::TYPES_DESCRIPTION[Device::TYPE_NUM] = { "Infrarossi", "Radio Freq. 433Mhz" };

Device::Device(const String &dev_name, const String &dev_type)
  : dev_name(dev_name),
    dev_type(dev_type)
{}

Device::~Device() {

  while (first_key) {
    Key * k = first_key->getNext();
    delete first_key;
    first_key = k;
  }
}

void Device::setName(const String &new_name) { dev_name = new_name; }
void Device::setType(const String &new_type) { dev_type = new_type; }

String Device::getName() {  return dev_name; }
String Device::getType() {  return dev_type; }

boolean Device::isValidPropertyById(int id, const String &val) {
  
  // IRDA
  if (dev_type == TYPES[0])
    return false;
    
  // RF433
  if (dev_type == TYPES[1])
    return RfKey::validations[id](val);
}

int Device::getKeysPropertyNum() {

  // IRDA
  if (dev_type == TYPES[0])
    return 0;
    
  // RF433
  if (dev_type == TYPES[1])
    return RfKey::props_num;

}

String * Device::getKeysPropertyNames() {

  int len = 0;
  String * prop = NULL;
  
  // IRDA
  if (dev_type == TYPES[0])
    return NULL;
    
  // RF433
  if (dev_type == TYPES[1])
    return getProperties(RfKey::props_names, RfKey::props_num);

}

Key * Device::getKeys() { return first_key; };


boolean Device::addKey(String * key_data) {

  Key *key;

  // Check name
  if (key_data[0].length() > Key::MAX_KEY_NAME_LEN)
    return false;

  // IRDA
  if (dev_type == TYPES[0])
    return false;
    
  // RF433
  if (dev_type == TYPES[1])
    key = new RfKey( key_data[0], key_data[1].toInt(), key_data[2].toInt() );

  if (!key)
    return false;

  if ( first_key == NULL ) {
    first_key = key;
    last_key = key;
  } else {
    last_key->setNext(key);
    last_key = key;
  }

Serial.println("Nome tasto " + key_data[0]);
Serial.println("Lunghezza " + key_data[1]);
Serial.println("codice " + key_data[2]);

  return true;
}

boolean Device::removeKey(const String &key_name ) {
  
  Key * prev_key = this->findPreviousKeyByName( key_name );

  if ( prev_key == NULL) {
    
    if ( first_key == NULL || first_key->getName() != key_name ) {
      return false;
    }
      
    prev_key = first_key;
    first_key = prev_key->getNext();
    delete prev_key;

    if ( first_key == NULL ) {
      last_key = NULL;
    }

    return true;
  }

  Key * next_key = prev_key->getNext();

  prev_key->setNext(next_key->getNext());

  if (next_key == last_key) {
    
    last_key = prev_key;

  }

  delete next_key;
  return true;
}

Key * Device::findPreviousKeyByName(const String &key_name) {

  if (first_key == NULL)
    return NULL;

  Key * key = first_key;

  while (Key * next = key->getNext()) {
    
    if ( next->getName() == key_name ) {
      return key;
    }
 
    key = next;
  }

  return NULL;
  
}


String * Device::getProperties(const String names[], int len) {

    String * props = new String[len];
    
    for (int i = 0; i < len; i++) {
    props[i] = names[i];
  }

  return props;
}


