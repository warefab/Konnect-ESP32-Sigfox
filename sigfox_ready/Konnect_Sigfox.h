
#ifndef Konnect_Sigfox_H_
#define Konnect_Sigfox_H_

#include <Arduino.h>

#define RXD1 14
#define TXD1 27

enum Message {
  DV_VERSION, 
  DV_ID,
  DV_PAC,
  DV_TOKEN
};

class Sigfox{
  
  public:

  void init();
  void checkIDPAC(enum Message type_);
  void sendMessage(uint8_t msg[]);
  void sendSigfoxMessage(char *data);
  
};

#endif
