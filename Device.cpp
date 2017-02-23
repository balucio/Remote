/*
  Device.cpp - Handle the controlled device implementation
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#include "Device.h"

Device::Device(String &dev_name)
  : dev_name(dev_name)
{}

Device::~Device() {

  Key * k = first_key;

  while (first_key) {
    first_key = k->getNext();
    delete k;
  }
}

String Device::getName() {  return dev_name; }
Key * Device::getKeys() { return  first_key; }

void Device::appendKey( String &key_name, int pulse, long int code ) {

  if (key_name.length() >Key::MAX_KEY_NAME_LEN)
    return;

  Key * key = new Key( key_name, pulse, code );

  if ( first_key == NULL ) {
    first_key = key;
    last_key = key;
  } else {
    last_key->setNext(key);
    last_key = key;
  }
}

void Device::removeKey( String &key_name ) {
  
  Key * prev_key = this->findPreviousKeyByName( key_name );

  if ( prev_key == NULL) {
    
    if ( first_key != NULL && first_key->getName() == key_name ) {
      
      prev_key = first_key;
      first_key = prev_key->getNext();
      delete prev_key;

      if ( first_key == NULL ) {
        last_key = NULL;
      }

    } else {

      return;
    }
  }

  Key * next_key = prev_key->getNext();

  prev_key->setNext(next_key->getNext());

  if (next_key == last_key) {
    
    last_key = prev_key;

  }

  delete next_key;
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


