/*
  WifiNetworks.cpp - Handle found wifi networks
  Saul bertuccio 14 feb 2017
  Released into the public domain.
*/

#include "Arduino.h"
#include "WifiNetworks.h"

WifiNetworks::WifiNetworks(int num )
  : num(num), curr(0) {
  if (num > 0)
    nets = new WifiNetworks::Nets[num];
}

WifiNetworks::~WifiNetworks() {

  delete [] nets;
}

int WifiNetworks::netNum() {
  return curr;
}

String WifiNetworks::getName(int i) { return nets[i].nname; }
int WifiNetworks::getProtection(int i) { return nets[i].protection; }
long int WifiNetworks::getQuality(int i) { return nets[i].quality; }

void WifiNetworks::append(String nname, int protection, long int quality) {

  if (curr > num)
    return;
  int pos = getPosition(nname);

  if (pos == -1)
    return;

  if (pos < curr)
    for (int i = curr; i > pos; i--) {
      nets[i].nname = nets[i-1].nname;
      nets[i].protection = nets[i-1].protection;
      nets[i].quality = nets[i-1].quality;
  }

  ++curr;

  nets[pos].nname = nname;
  nets[pos].protection = protection;
  nets[pos].quality = quality;
}


int WifiNetworks::getPosition(String nname) {

  nname.toLowerCase();
  String tmp;
  int i = 0;
  while (i < curr) {
    tmp = nets[i].nname;
    tmp.toLowerCase();
    if (tmp == nname)
      return -1;
    if (tmp > nname)
      return i;
    i++;
  }
  return i;
}


