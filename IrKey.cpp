/*
  IrKey.cpp - Infrared data foreach key of device
  Saul bertuccio 10 apr 2017
  Released into the public domain.
*/

#include "IrKey.h"

const String IrKey::props_names[] =  { "name", "code" };

const IsValidFnc IrKey::validations[] = {
  &Validation::isValidDeviceName,
  &Validation::isValidDeviceKeyHexCode
};

IrKey::IrKey(const String & key_name, const String & code)
  : Key(key_name),
    code(code)
{}

String IrKey::getCode() {
  return code;
}

String IrKey::getPropertyById(int id) {

  switch (id) {
    case (0): return getName();
    case (1): return code;
    default: return String("");
  }
}


String IrKey::getPropertyNameById(int id) {

  if (id < 0 || id >= IrKey::props_num)
    return String("");

  return IrKey::props_names[id];

}


