/*
  IrKey.h - Infrared serial data foreach key of device
  Saul bertuccio 10 apr 2017
  Released into the public domain.
*/

#ifndef IrKey_h
#define IrKey_h

#include "SoftwareSerial.h"
#include "Key.h"
#include "Validation.h"

class IrKey: public Key {

  public:
    static const int props_num = 2;
    static const String props_names[];
    static const IsValidFnc validations[];

    IrKey(const String & key_name, const String & code);

    String getCode();
    String getPropertyById(int id);
    String getPropertyNameById(int id);

  protected:
    String code;
};

#endif




