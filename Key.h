/*
  Key.h - base data foreach key of device
  Saul bertuccio 7 feb 2017
  Released into the public domain.
*/

#ifndef Key_h
#define Key_h

#include "Arduino.h"
#include "WString.h"

typedef boolean (*IsValidFnc)(const String &);

class Key {
  
  public:

    static const int MAX_KEY_NAME_LEN;

    Key(const String & key_name);
    virtual ~Key();

    String getName();
    Key * getNext();

    void setNext(Key * next);

    virtual String getPropertyById(int id);
    virtual String getPropertyNameById(int id);

  private:
    String key_name;
    Key * next = NULL;
};

#endif




