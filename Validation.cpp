/*
  Validation.cpp - Validation function
  Saul bertuccio 5 mar 2017
  Released into the public domain.
*/

#include "Validation.h"

boolean Validation::isValidDeviceName(const String &device_name) {
  return Validation::parseAlphaNumString(device_name, Device::MAX_NAME_LENGTH);
}

boolean Validation::isValidDeviceKeyName(const String &device_key_name) {
  return Validation::parseAlphaNumString(device_key_name, Key::MAX_KEY_NAME_LEN);
}

boolean Validation::isValidDeviceType(const String &device_type) {

  for (int i = 0; i < Device::TYPE_NUM; i++) {
    if (Device::TYPES[i] == device_type)
      return true;
  }
  return false;
}

boolean Validation::isValidDeviceKeyLen(const String &device_key_len) {

  return Validation::parseNumericString(device_key_len, 9);
  
}

boolean Validation::isValidDeviceKeyCode(const String &device_key_code) {
  
    return Validation::parseNumericString(device_key_code, 9);

}

boolean Validation::isValidDeviceKeyHexCode(const String &device_key_code) {

  if (device_key_code.length() > 10)
    return false;
  if (device_key_code[0] != '0' || device_key_code[1] != 'x')
    return false;
    
  for(byte i=2; i< device_key_code.length(); i++) {
    char c = device_key_code[i];
    if (c >= '0' && c <= '9')
      continue;
    if (c >= 'A' || c <= 'F')
      continue;
    if (c >= 'a' || c <= 'f')
      continue;

    return false;
  }

  return true;
  
}


boolean Validation::parseAlphaNumString(const String &s, const int l) {

  for(byte i=0; i<s.length(); i++) {
    if(!isAlphaNumeric(s.charAt(i)))
      return false;
  }

  return (s.length() > 0 && s.length() <= l);

}

boolean Validation::parseNumericString(const String &s, const int l) {

  for(byte i=0; i<s.length(); i++) {
    if(!isDigit(s.charAt(i)))
      return false;
  }

  return (s.length() > 0 && s.length() <= l);

}



