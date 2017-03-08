/*
  Device.cpp - Handle the controlled device implementation
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#include "Device.h"

Device::Device(const String &dev_name, const String &dev_type)
  : dev_name(dev_name),
    dev_type(dev_type)
{}

Device::~Device() {

  Key * k = first_key;

  while (first_key) {
    first_key = k->getNext();
    delete k;
  }
}

String Device::TYPES[Device::TYPE_NUM] = { "IRDA", "RF433"};
String Device::TYPES_DESCRIPTION[Device::TYPE_NUM] = { "Infrarossi", "Radio Freq. 433Mhz" };

void Device::setName(const String &new_name) { dev_name = new_name; }
void Device::setType(const String &new_type) { dev_type = new_type; }

String Device::getName() {  return dev_name; }
String Device::getType() {  return dev_type; }
Key * Device::getKeys() { return  first_key; }

boolean Device::appendKey( String &key_name, int pulse, long int code ) {

  if (key_name.length() > Key::MAX_KEY_NAME_LEN)
    return false;

  Key * key = new Key( key_name, pulse, code );

  if (!key) return false;

  if ( first_key == NULL ) {
    first_key = key;
    last_key = key;
  } else {
    last_key->setNext(key);
    last_key = key;
  }

  return true;
}

boolean Device::removeKey( String &key_name ) {
  
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

Key * Device::findPreviousKeyByName(String &key_name) {

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



