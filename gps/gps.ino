/*
 * gps demo
 * Konnnect esp32-sigfox
 * 
 * @author https://www.warefab.com
*/

 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <fonts/FreeMonoBold9pt7b.h>

#include "Konnect_L70R.h"

char gps_buf[512] = {0};
uint16_t gps_len = 0;

L70R gps;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  gps.init();

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
}

void loop() {
  while (Serial2.available() > 0) {
    char ch = (char)Serial2.read();
    if (ch == '\n' || ch == '\r') {
      if (gps.parse(&gps_buf[0], gps_len)) {
        displayInfo();
      }
      gps_len = 0;
    } else {
      gps_buf[gps_len++] = ch;
    }
  }
  delay(1);
}

void displayInfo()
{
  char tm_[20] = {0};
  float lat = (float)(gps.lat / 100000.0);
  float lng = (float)(gps.lng / 100000.0);
  
  display.setCursor(0, 10);
  display.clearDisplay();
  display.println(" GPS DATA");
  display.print(">");
  //time data
  gps.dateTimeFormat(&tm_[0], gps.g_time, ':'); 
  display.println(tm_);
  //location data
  display.print(F(">"));
  display.print(lat, 5);
  display.println((char)gps.lat_ns);
  //longitude
  display.print(">");
  display.print(lng, 5);
  display.println((char)gps.lng_ew);

  display.display();
}
