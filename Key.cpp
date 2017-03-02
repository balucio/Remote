/*
  DeviceKey.cpp - Code foreach device Key
  Saul bertuccio 7 feb 2017
  Released into the public domain.
*/

#include "Key.h"

const int Key::MAX_KEY_NAME_LEN = 10;

Key::Key(String key_name, int pulse, long int code)
  : key_name(key_name),
    pulse(pulse),
    code(code)
{}

String Key::getName(){ return key_name; }
int Key::getPulse() { return pulse; }
long int Key::getCode() { return code; }
Key * Key::getNext() { return next; }
 
void Key::setNext(Key * next) { this->next = next; }

