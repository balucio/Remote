/*
  Device.h - Handle the controlled device
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#ifndef Device_h
#define Device_h

class Device {
  
  public:
    Device(String name, int pulse, int code_len, unsigned long int code_on, unsigned long int code_off);

  private:
    String name;
    int pulse;
    int code_len;
    int code_on;
    int code_off;
};

#endif

