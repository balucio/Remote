/*
  DeviceLoader.h - Handle the controlled device
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#ifndef DeviceHandler_h
#define DeviceHandler_h

#include "Device.h"
#include "FS.h"

class DeviceHandler {
  
  public:
 
    DeviceHandler();

    int deviceCount();
    String * listDeviceNames();

    Device * loadDevice( String &device_name );

    boolean renameDevice( String &old_name, String &new_name);
    boolean deleteDevice( String &device_name );
    boolean saveDevice( Device * device );  

  private:
    boolean is_opened = false;
    int device_num = 0;

};

#endif


