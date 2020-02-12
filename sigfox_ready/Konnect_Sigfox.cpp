#include <stdio.h>

#include "Konnect_Sigfox.h"

const char *at_messages[] = {"AT$I=0", "AT$I=10", "AT$I=11", "AT$SF="};
//char packet[] = "01c9a838321e2e01091b3046";

void Sigfox::init(){
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  delay(100);
  checkIDPAC(DV_VERSION);
  checkIDPAC(DV_ID);
  checkIDPAC(DV_PAC);
}

void Sigfox::checkIDPAC(enum Message type_) {
  if (type_ == DV_VERSION) { //version
    Serial.print("\nVER - ");
  } else if (type_ == DV_ID) { //id
    Serial.print("ID - ");
  } else if (type_ == DV_PAC) { //pac
    Serial.print("PAC - ");
  }
  Serial1.print(at_messages[type_]);
  Serial1.print("\r\n");
  delay(500);
  while (Serial1.available()) {
    Serial.print((char)Serial1.read());
  }
}

void Sigfox::sendMessage(uint8_t msg[]) {

  String status = "";
  char output[2];
  char chm[2];
  uint8_t *ch = &msg[0];
  uint8_t cnt = 0;
  
  Serial1.print("AT$SF=");
  /*for(int i= 0;i<size;i++){
    Serial.print(String(msg[i], HEX));
    }*/
  while (*ch) {
    sprintf(chm, "%02X", *ch++);
    Serial1.print(chm);
    if(cnt >= 11) break;
    cnt++;
  }
  Serial1.print("\r\n");
}

void Sigfox::sendSigfoxMessage(char *data) {
  Serial1.print(at_messages[DV_TOKEN]);
  while (*data) {
    Serial1.print(*data++);
  }
  Serial1.print("\r\n");
}
