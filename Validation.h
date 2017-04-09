/*
  Validation.h - Handle validation
  Saul bertuccio 5 mar 2017
  Released into the public domain.
*/
#ifndef Validation_h
#define Validation_h

#include "Arduino.h"
#include "Device.h"
#include "Key.h"

class Validation {
  
  public:

    static boolean isValidDeviceName(const String &device_name);
    static boolean isValidDeviceType(const String &device_type);
    static boolean isValidDeviceKeyName(const String &device_key_name);
    static boolean isValidDeviceKeyLen(const String &device_key_len);
    static boolean isValidDeviceKeyCode(const String &device_key_code);

  private:

      static boolean parseAlphaNumString(const String &s, const int l);
      static boolean parseNumericString(const String &s, const int l);


};

#endif


