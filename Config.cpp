/*
  Config.cpp - Handle the configuration on EEPROM implementation
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#include "Arduino.h"
#include "EEPROM.h"
#include "Ethernet.h"
#include "Config.h"
#include "Device.h"

Config::Config()
{
  EEPROM.begin(EEPROM_SIZE);
}


char * Config::getEssidName()
{
  return getData(ESSID_START, ESSID_LENGTH);
}

char * Config::getEssidPassword()
{
  return getData(EESID_PASS_START, ESSID_PASS_LENGTH);
}


IPAddress Config::getIp()
{
  IPAddress ip;
  get(ip, IP_ADDRESS_START);
  return ip;
}

IPAddress Config::getSubnet()
{
  IPAddress mk;
  get(mk, MASK_ADDRESS_START);
  return mk;
}

IPAddress Config::getGateway()
{
  IPAddress gw;
  get(gw, GW_ADDRESS_START);
  return gw;
}

int Config::getReceiverPin()
{
  byte rp;
  get(rp, PINS_START);
  return (int) (rp >> 4);
  
}

int Config::getTrasmitterPin()
{

  byte tp;
  get(tp, PINS_START);
  return tp & 15;
}

Device * Config::getDevice(int devNum)
{
  String raw;
  int start = FIRST_DEVICE_START + ( (devNum - 1) * DEVICE_DATA_LENGTH );

  if (devNum > MAX_DEVICE)
    return NULL;

  String name = getData(start, DEVICE_NAME_LENGTH);

  byte pulse, code_len;
  start += DEVICE_NAME_LENGTH;
  get(pulse, start);
  start += DEVICE_PULSE_LENGTH;
  get(code_len, start);

  unsigned long int code_on, code_off;
  start += DEVICE_CODE_BIT_LENGTH;
  get(code_on, start);
  start += DEVICE_CODES_MAX_LENGTH;
  get(code_off, start);
  
  return new Device(name, pulse, code_len, code_on, code_off);
}


char * Config::getData(int idx, int len)
{
  char *data = new char[len + 1];
  len += idx;
  
  for (int i = idx, j = 0 ; i < len; ++i, ++j)
  {
    data[j] = char(EEPROM.read(i));
  }

  data[len] = NULL;  
  return data;
}


void Config::setData(const char *data, int idx, int len)
{

  int i = 0;

  while (data[i] && i < len )
  {
    EEPROM.write(idx + i, data[i]);
    i++;
  }

  i += idx;
  
  while ( i < len )
  {
    EEPROM.write(i++, 0);
  }
}

