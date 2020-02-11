#define RXD1 14
#define TXD1 27

enum Message {
  DV_VERSION, 
  DV_ID,
  DV_PAC,
  DV_TOKEN
} msg_;

uint8_t msg[12] = "warefab";
const char *at_messages[] = {"AT$I=0", "AT$I=10", "AT$I=11", "AT$SF="};
//char packet[] = "01c9a838321e2e01091b3046";

const unsigned long int msg_delay = 60000;
unsigned long int prev_time = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  Serial.println("Sigfox Module Test");
  delay(100);
  checkIDPAC(DV_VERSION);
  checkIDPAC(DV_ID);
  checkIDPAC(DV_PAC);
}

void loop() {
  if(millis() - prev_time >= msg_delay){
    Serial.println("Sending Packets...");
    sendMessage(msg);
    //sendSigfoxMessage(packet);
    prev_time = millis();
  }
  while (Serial1.available()) {
    Serial.print((char)Serial1.read());
  }
  delay(1); //yield
}

void checkIDPAC(enum Message type_) {
  if (type_ == DV_VERSION) { //version
    Serial.print("\nVER - ");
  } else if (type_ == DV_ID) { //id
    Serial.print("ID - ");
  } else if (type_ == DV_PAC) { //pac
    Serial.print("PAC - ");
  }
  Serial1.print(at_messages[type_]);
  Serial1.print("\r\n");
  delay(200);
  while (Serial1.available()) {
    Serial.print((char)Serial1.read());
  }
}

void sendMessage(uint8_t msg[]) {

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

void sendSigfoxMessage(char *data) {
  Serial1.print(at_messages[DV_TOKEN]);
  while (*data) {
    Serial1.print(*data++);
  }
  Serial1.print("\r\n");
}
