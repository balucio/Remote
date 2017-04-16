/*
  Device.cpp - Handle the controlled device implementation
  Saul bertuccio 4 feb 2017
  Released into the public domain.
*/

#include "Device.h"

String Device::TYPES[Device::TYPE_NUM] = { "IRDA", "RF433" };
String Device::TYPES_DESCRIPTION[Device::TYPE_NUM] = { "Infrarossi", "Radio Freq. 433Mhz" };

Device::Device(const String &dev_name, const String &dev_type)
  : dev_name(dev_name),
    dev_type(dev_type)
{}

Device::~Device() {

  while (first_key) {
    Key * k = first_key->getNext();
    delete first_key;
    first_key = k;
  }
}

void Device::setName(const String &new_name) { dev_name = new_name; }
void Device::setType(const String &new_type) { dev_type = new_type; }

String Device::getName() {  return dev_name; }
String Device::getType() {  return dev_type; }

boolean Device::isValidPropertyById(int id, const String &val) {
  
  // IRDA
  if (dev_type == TYPES[0])
    return IrKey::validations[id](val);
    
  // RF433
  if (dev_type == TYPES[1])
    return RfKey::validations[id](val);

  return false;
}

int Device::getKeysPropertyNum() {

  // IRDA
  if (dev_type == TYPES[0])
    return IrKey::props_num;
    
  // RF433
  if (dev_type == TYPES[1])
    return RfKey::props_num;

}

String * Device::getKeysPropertyNames() {

  int len = 0;
  String * prop = NULL;
  
  // IRDA
  if (dev_type == TYPES[0])
    return getProperties(IrKey::props_names, IrKey::props_num);
    
  // RF433
  if (dev_type == TYPES[1])
    return getProperties(RfKey::props_names, RfKey::props_num);

}

Key * Device::getKeys() { return first_key; };


boolean Device::addKey(String * key_data) {

  Key *key;

  // Check name
  if (key_data[0].length() > Key::MAX_KEY_NAME_LEN)
    return false;

  // IRDA
  if (dev_type == TYPES[0])
    key = new IrKey( key_data[0], key_data[1] );
    
  // RF433
  if (dev_type == TYPES[1])
    key = new RfKey( key_data[0], key_data[1].toInt(), key_data[2].toInt() );

  if (!key)
    return false;

  if ( first_key == NULL ) {
    first_key = key;
    last_key = key;
  } else {
    last_key->setNext(key);
    last_key = key;
  }
  return true;
}

boolean Device::sendKeyData(const String & key_name) {

  Key * k = findPreviousKeyByName(key_name);

  if ( k == NULL) {
    if (first_key != NULL && first_key->getName() == key_name) {
      k = first_key;
    } else {
      return false;
    }
  } else {
    k = k->getNext();
  }

  if (dev_type == TYPES[0])
    Device::sendIrKeyData(k);
    
  // RF433
  if (dev_type == TYPES[1]) {
    Device::sendRfKeyData(k);
  }

  return true;  
}

Key * Device::acquireKeyData() {

  // IRDA
  Key * k;
  
  if (dev_type == TYPES[0])
    k = Device::acquireIrKeyData();
    
  // RF433
  if (dev_type == TYPES[1])
    k = Device::acquireRfKeyData();

  return k;
}


boolean Device::removeKey(const String &key_name ) {
  
  Key * prev_key = this->findPreviousKeyByName( key_name );

  if ( prev_key == NULL) {
    
    if ( first_key == NULL || first_key->getName() != key_name ) {
      return false;
    }
      
    prev_key = first_key;
    first_key = prev_key->getNext();
    delete prev_key;

    if ( first_key == NULL ) {
      last_key = NULL;
    }

    return true;
  }

  Key * next_key = prev_key->getNext();

  prev_key->setNext(next_key->getNext());

  if (next_key == last_key) {
    
    last_key = prev_key;

  }

  delete next_key;
  return true;
}

Key * Device::findPreviousKeyByName(const String &key_name) {

  if (first_key == NULL)
    return NULL;

  Key * key = first_key;

  while (Key * next = key->getNext()) {
    
    if ( next->getName() == key_name ) {
      return key;
    }
 
    key = next;
  }

  return NULL;
  
}

String * Device::getProperties(const String names[], int len) {

    String * props = new String[len];
    
    for (int i = 0; i < len; i++) {
    props[i] = names[i];
  }

  return props;
}


void Device::sendIrKeyData(Key *key) {

  static String hex_decode = "0123456789ABCDEF"; 
  IrKey *k = static_cast<IrKey *>(key);
  String code = k->getCode().substring(2);
  code.toUpperCase();

  if (code.length() % 2 != 0) {
    Serial.println("Errore: Codice da inviare valido");
    return;
  }

  SoftwareSerial sw(IR_RX_PIN, IR_TX_PIN, false, 16);
  int i=0;
  sw.begin(9600);
  // Protocollo
  sw.write(0xA1);
  sw.write(0xF1);
  while (i < code.length()) {
    char n = hex_decode.indexOf(code[i++]) << 4;
    n +=  hex_decode.indexOf(code[i++]);
    sw.write(n);
  }
}


void Device::sendRfKeyData(Key *key) {
  RCSwitch sw = RCSwitch();
  RfKey *k = static_cast<RfKey *>(key);
  sw.enableTransmit(Device::RF_TX_PIN);
  sw.send(k->getCode(), k->getLength());
  sw.disableTransmit();
}

Key * Device::acquireIrKeyData() {

  static char const hex_encode[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  char code[11] = {'0', 'x', '\0'};
  SoftwareSerial sw(IR_RX_PIN, IR_TX_PIN, false, 16);
  sw.begin(9600);
  boolean is_code_ok = false;
  int attempt = 3;
  for (int i=0; i < attempt; i++) {
    delay(500);
    int p = 2;
    while (sw.available() > 0 && p < 15) {
      char c = sw.read();
      code[p++] = hex_encode[ ( c & 0xF0 ) >> 4  ];
      code[p++] = hex_encode[ (c & 0x0F ) ];
      delay(40);
    }
    sw.flush();
    code[p] = '\0';
    if (p>4) {
      is_code_ok=true;
      break;
    }
  }

  IrKey * ir = NULL;
  if (is_code_ok) {
    ir = new IrKey("DUMMY", String(code));
  }
  return ir;
}

Key * Device::acquireRfKeyData() {

  RCSwitch sw = RCSwitch();
  RfKey * rf = NULL;
  sw.enableReceive(Device::RF_RX_PIN);

  int attempt = 3;
  for (int i=0; i < attempt; i++) {
    if (rf)
      break;
    delay(500);
    if (sw.available()) {
      rf = new RfKey("DUMMY", sw.getReceivedBitlength(), sw.getReceivedValue());
      sw.resetAvailable();
    }
  }
  sw.disableReceive();
  return rf;
}


