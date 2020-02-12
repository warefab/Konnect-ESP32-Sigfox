/*
   Send sensors data to sigfox
   Konnect ESP32-Sigfox dev board

   @author @author https://www.warefab.com
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Tone32.h>

#include "Konnect_L70R.h"
#include "Konnect_LIS2DH12.h"
#include "Konnect_Sigfox.h"
#include "images.h"

#define sht30_addr 0x44
#define acc_addr 0x19
//ldr
#define ldr_pin 36
//buzzer
#define BUZZER_PIN 32
#define BUZZER_CHANNEL 0

//mic
#define sample_time 50
#define mic_pin 39

//display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//gps
char gps_buf[128] = {0};
uint16_t gps_len = 0;
uint8_t gps_flag = 0;

//sht30

uint8_t t_ = 1;

uint8_t cTemp = 0;
uint8_t fTemp = 0;
uint8_t humidity = 0;

const unsigned long int msg_delay = 1000;
unsigned long int prev_time = 0;
uint8_t sigfox_delay = 0;

Lis2dh12 acc;
L70R gps;
Sigfox sigfox;

void setup() {
  Wire.begin();
  //init serial
  Serial.begin(115200);
  //init display
  initDisplay();
  //init sigfox
  sigfox.init();
  //init buzzer
  initBuzzer();
  //init gps
  gps.init();
  //init acc
  acc.init(acc_addr);
}

void loop() {
  /*listen for gps logs*/
  while (Serial2.available() > 0) {
    char ch = (char)Serial2.read();
    if (ch == '\n' || ch == '\r') {
      if (gps.parse(&gps_buf[0], gps_len)) {
        //displayInfo();
      }
      gps_len = 0;
    } else {
      gps_buf[gps_len++] = ch;
    }
  }

  /*listen for sigfox radio logs*/
  while (Serial1.available()) {
    Serial.print((char)Serial1.read());
  }

  /*send packets to sigfox after delay elapsed*/
  if (millis() - prev_time >= msg_delay) {
    sendSigfoxPacket();
    prev_time = millis();
  }

  /*yield*/
  delay(1);
}


void sendSigfoxPacket() {
  char buffer[126] = {0};
  //char packets[24];
  uint8_t flags = 0;
  //date/time
  char time[12] = { 0 };
  char date[12] = { 0 };
  char gps_ns, gps_ew;
  //ldr
  uint8_t ldr_output = getLdr();
  //mems mic, noise = 1, silent = 0
  uint16_t mic_val = getMicValue();
  uint8_t mic_status;
  //Ambient sensor, light = 1, dark = 0
  uint8_t ldr_status;
  //sht30 temperature and humidity
  getTempHum();
  //lis2dh12 acc x, y, z values
  uint8_t state = acc.whoIAm();
  if (state == 0x33) {
    acc.readXYZ();
  }

  //send readable data to debug port else to wisol module
  if (sigfox_delay < 120) {
    gps.dateTimeFormat(time, gps.g_time, ':');
    gps.dateTimeFormat(date, gps.g_date, '-');
    gps_ns = (gps.lat_ns == 0) ? ' ' : gps.lat_ns;
    gps_ew = (gps.lng_ew == 0) ? ' ' : gps.lng_ew;

    display.setCursor(0, 2);
    display.clearDisplay();

    //mic and ldr
    sprintf(buffer, "\n\rMIC:%04d, LDR:%04d", mic_val, ldr_output);
    display.println(buffer);
    Serial.println(buffer);

    //SHT30
    sprintf(buffer, "T/H = C:%d, H:%d", cTemp, humidity);
    display.println(buffer);
    Serial.println(buffer);

    //LIS2DH12
    sprintf(buffer, "ACC =  X:%03d, Y:%03d, Z:%0d", acc.x, acc.y, acc.z);
    display.println(buffer);
    Serial.println(buffer);

    //L70R
    float lat = (float)(gps.lat / 100000.0);
    float lng = (float)(gps.lng / 100000.0);
    display.print("GPS:");
    display.print(lat, 5);
    display.println(gps_ns);
    //longitude
    display.print(", ");
    display.print(lng, 5);
    display.println(gps_ew);
    sprintf(buffer,
            "GPS = TIME: %s,  LAT: %.5f%c, LNG : %.5f%c, SPEED : %d, DATE : %s",
            time, lat, gps_ns, lng, gps_ew, gps.speed, date);
    //log
    display.display();
    Serial.println(buffer);
    sigfox_delay++;
  } else {
    Serial.println("Sending Packets...");
    //check mic, if noise/not
    mic_status = (mic_val > 8) ? 1 : 0;
    if (mic_status == 1) { //bit 0
      flags |= (uint8_t) (1 << 0);
    }
    //light threshold, on/off
    ldr_status = (ldr_output >= 5) ? 1 : 0;
    if (ldr_status == 1) { //bit 1
      flags |= (uint8_t) (1 << 1);
    }
    //check if acc x axis is negative
    if (acc.x < 0) { //bit 2
      flags |= (uint8_t) (1 << 2);
    }
    //check if acc y axis is negative
    if (acc.y < 0) { //bit 3
      flags |= (uint8_t) (1 << 3);
    }
    //gps standby mode on/off
    if (gps_flag == 1) { //bit 4
      flags |= (uint8_t) (1 << 4);
    }
    //speed threshold
    if (gps.speed >= 80) { //bit 5
      flags |= (uint8_t) (1 << 5);
    }
    //latitude pos, if N=0, S=1
    if (gps.lat_ns == 'S') { //bit 5
      flags |= (uint8_t) (1 << 6);
    }
    //longitude pos, if E=0, W=1
    if (gps.lng_ew == 'W') { //bit 5
      flags |= (uint8_t) (1 << 7);
    }

    //format bytes to hex
    sprintf(buffer, "%06lX%06lX%02X%02X%02X%02X%02X%02X", gps.lat,
            gps.lng, ldr_output,
            (uint8_t) abs(acc.x), (uint8_t) abs(acc.y),
            cTemp, humidity, flags);
    //send to debug port
    Serial.println(buffer);
    //send to sigfox cloud, 36 sec delay
    sigfox.sendSigfoxMessage((char*) buffer);
    sigfox_delay = 0;
  }
}


/*
   get temperature and humidity values
*/
void getTempHum() {
  uint8_t data[6] = { 0 };
  uint8_t config[2] = {0x2c, 0x06};
  uint8_t x = 0;

  int temp = 0;

  Wire.beginTransmission(sht30_addr);
  Wire.write(config[0]);
  Wire.write(config[1]);
  //Wire.endTransmission();
  delay(2);
  Wire.requestFrom(sht30_addr, 6);    // request 6 byte
  while (Wire.available()) {
    data[x++] = (uint8_t)Wire.read();
  }
  Wire.endTransmission();

  temp = (data[0] * 256 + data[1]);
  cTemp = (short int) (-45 + (float) (175.0 * (temp / 65535.0)));
  fTemp = (short int) (-49 + (float) (315.0 * (temp / 65535.0)));
  humidity = (uint8_t) (100
                        * (float) ((data[3] * 256 + data[4]) / 65535.0));
}

/*
   get ambient light sensor value 0<>100%
*/
uint8_t getLdr() {
  uint16_t ldr = 0;
  ldr = analogRead(ldr_pin);

  ldr = map(ldr, 0, 4095, 0, 100);

  return ldr;
}

/*
   find analog mems mic ptp value
*/
uint16_t getMicValue() {
  uint16_t mic_adc = 0;
  uint16_t mic_max = 0;
  uint16_t mic_min = 1023;
  uint16_t mic_ptp = 0;
  unsigned long int prev_t = 0;

  for (uint8_t x = 0; x < 4; x++) {
    prev_t = millis();
    mic_ptp = 0;
    while (millis() - prev_t < sample_time) {
      mic_adc = analogRead(mic_pin);
      if (mic_adc > mic_max) {
        mic_max = mic_adc;
      } else if (mic_adc < mic_min) {
        mic_min = mic_adc;
      }
    }
    mic_ptp = mic_max - mic_min;
  }

  return mic_ptp;
}

/*
   init oled display
*/
void initDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.clearDisplay();
  display.drawBitmap(0, 0, warefab_logo, 128, 64, WHITE);
  display.display();
}

void initBuzzer() {
  tone(BUZZER_PIN, NOTE_C4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_D4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_E4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_F4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_G4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_A4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_B4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
}
