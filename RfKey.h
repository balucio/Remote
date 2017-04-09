/*
  RfKey.h - Rf data foreach key of device
  Saul bertuccio 11 mar 2017
  Released into the public domain.
*/

#ifndef RfKey_h
#define RfKey_h

#include "RCSwitch.h"
#include "Key.h"
#include "Validation.h"

class RfKey: public Key {

  public:
    static const int props_num = 3;
    static const String props_names[];
    static const IsValidFnc validations[];

    RfKey(const String & key_name, int len, long int code);

    long int getCode();
    int getLength();
    String getPropertyById(int id);
    String getPropertyNameById(int id);

  protected:
    long int code;
    int len;
};

#endif




