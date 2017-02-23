/*
  Key.h - Code foreach Key of the device
  Saul bertuccio 7 feb 2017
  Released into the public domain.
*/

#ifndef Key_h
#define Key_h

#include "WString.h"

class Key {
  
  public:

    static const int MAX_KEY_NAME_LEN;

    Key(String key_name, int pulse, long int code);
    
    
    String getName();
    long int getCode();
    int getPulse();
    Key * getNext();

    void setNext(Key * next);

  private:
    String key_name;
    long int code;
    int pulse;
    Key * next = NULL;
};

#endif


