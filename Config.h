/*
  Config.h - Handle the remote EEPROM Configuration.
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#ifndef Config_h
#define Config_h

#include "Arduino.h"
#include "EEPROM.h"
#include "Ethernet.h"
#include "Device.h"

#define EEPROM_SIZE 512

#define ESSID_START 0
#define ESSID_LENGTH 32

#define ESSID_PASS_LENGTH 64
#define EESID_PASS_START ESSID_LENGTH

#define ADDRESS_LENGTH 4
#define IP_ADDRESS_START ( EESID_PASS_START + ESSID_PASS_LENGTH )
#define MASK_ADDRESS_START ( IP_ADDRESS_START + ADDRESS_LENGTH )
#define GW_ADDRESS_START ( MASK_ADDRESS_START + ADDRESS_LENGTH )

#define PINS_LENGTH 1
#define PINS_START ( GW_ADDRESS_START + ADDRESS_LENGTH )

#define CONFIGURED_DEVICE_LENGTH 1
#define CONFIGURED_DEVICE_START ( PINS_START + PINS_LENGTH )

#define FIRST_DEVICE_START ( CONFIGURED_DEVICE_START + CONFIGURED_DEVICE_LENGTH )

#define DEVICE_NAME_LENGTH 10
#define DEVICE_PULSE_LENGTH 1
#define DEVICE_CODE_BIT_LENGTH 1
#define DEVICE_CODES_MAX_LENGTH 4

#define DEVICE_DATA_LENGTH ( DEVICE_NAME_LENGTH + DEVICE_PULSE_LENGTH + DEVICE_CODE_BIT_LENGTH + ( DEVICE_CODES_MAX_LENGTH * 2 ) )
#define MAX_DEVICE ( ( EEPROM_SIZE - FIRST_DEVICE_START ) / DEVICE_DATA_LENGTH )

class Config {
  
  public:
    Config();
    
    char * getEssidName();
    char * getEssidPassword();

    IPAddress getIp();
    IPAddress getSubnet();

    IPAddress getGateway();

    int getReceiverPin();
    int getTrasmitterPin();

    Device * getDevice(int devNum);

  private:

    char * getData(int idx, int len);
    void setData(const char *data, int idx, int len);

    //Functionality to 'get' and 'put' objects to and from EEPROM.
    template< typename T > void get( T &t, int idx )
    {
      EEPROM.get(idx, t);
    }

    template< typename T > void put( const T &t, int idx )
    {
      EEPROM.put(idx, t);
    }
};

#endif

