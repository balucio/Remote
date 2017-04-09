/*
  DeviceKey.cpp - base data foreach key of device
  Saul bertuccio 7 feb 2017
  Released into the public domain.
*/

#include "Key.h"

const int Key::MAX_KEY_NAME_LEN = 10;

Key::Key(const String & key_name)
  : key_name(key_name)
{}

Key::~Key(){};


String Key::getName(){ return key_name; }
Key * Key::getNext() { return next; }

void Key::setNext(Key * next) { this->next = next; }

String Key::getPropertyById(int id) { return String(""); }
String Key::getPropertyNameById(int id) { return String(""); }


