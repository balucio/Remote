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

    static const char SEP=';';
    static const char EOL='\n';

    static String *getDevicesName();
    static int getDevicesNum();

    static int getDeviceTypesNum();
    static String * getDeviceTypes();
    static String * getDeviceTypesDescription();

    static boolean isValidDeviceName(const String &device_name);
    static boolean isValidDeviceType(const String &device_type);

    DeviceHandler();
    ~DeviceHandler();

    boolean setDevice( const String &device_name, const String &device_type );
    boolean setDevice( const String &device_name );
    boolean renameDevice( const String &new_name); 
    boolean deleteDevice();
    boolean saveDevice();
    Device & getDevice();

  private:
    boolean is_opened;
    Device * device;

    static Device * loadDeviceFile( const String &device_name );
    static boolean renameDeviceFile( const String &old_name, const String &new_name);
    static boolean deleteDeviceFile( const String &device_name );
    static boolean existsDeviceFile( const String &device_name );
    static boolean saveDeviceFile( Device * device );

    static int recountDevices(bool force);

    static boolean parseAlphaNumString(const String &s, const int l);
};

#endif


