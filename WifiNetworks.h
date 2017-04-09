/*
  ConnectionManager.h - Handle wifi networks data
  Saul bertuccio 14 feb 2017
  Released into the public domain.
*/

#ifndef WifiNetworks_h
#define WifiNetworks_h

#include "WString.h"

class WifiNetworks {

  typedef struct {
    String nname;
    int protection;
    long int quality;
  } Nets;

  public:
    WifiNetworks( int num);
    ~WifiNetworks();

    int netNum();

    void append(String name, int protection, long int quality);

    String getName(int i);
    int getProtection(int i);
    long int getQuality(int i);
    
  private:

    int getPosition(String nname);

    int num, curr;
    Nets * nets;
};

#endif



