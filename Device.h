/*
  Device.h - Handle the controlled device
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#ifndef Device_h
#define Device_h

#include "WString.h"
#include "Key.h"

class Device {
  
  public:

    static const int MAX_NAME_LENGTH = 16;
 
    Device(String &dev_name);
    ~Device();

    String getName();
    Key * getKeys();
    
    void appendKey( String &key_name, int pulse,  long int code );
    void removeKey( String &key_name );

  private:
    String dev_name;
    Key * first_key = NULL;
    Key * last_key = NULL;

    Key * findPreviousKeyByName( String &key_name );

};

#endif


