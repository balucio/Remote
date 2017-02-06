/*
  Device.cpp - Handle the controlled device implementation
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#include "Arduino.h"
#include "Device.h"

Device::Device(String name, int pulse, int code_len, unsigned long int code_on, unsigned long int code_off )
{
  this->name = name;
  this->pulse = pulse;
  this->code_len = code_len;
  this->code_on = code_on;
  this->code_off = code_off;
}

