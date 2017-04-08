/*
  DeviceKey.cpp - rf data foreach key of device
  Saul bertuccio 11 mar 2017
  Released into the public domain.
*/

#include "RfKey.h"

const String RfKey::props_names[] =  { "name", "length", "code" };

const IsValidFnc RfKey::validations[] = {
  &Validation::isValidDeviceName,
  &Validation::isValidDeviceKeyLen,
  &Validation::isValidDeviceKeyCode
};

RfKey::RfKey(const String & key_name, int len, long int code)
  : Key(key_name),
    len(len),
    code(code)
{}

int RfKey::getLength() {
  return len;
}
long int RfKey::getCode() {
  return code;
}

String RfKey::getPropertyById(int id) {

  switch (id) {
    case (0): return getName();
    case (1): return String(len);
    case (2): return String(code);
    default: return String("");
  }
}
