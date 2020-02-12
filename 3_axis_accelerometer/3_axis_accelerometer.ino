/*
 * 3 Axis Accelerometer demo
 * Konnnect esp32-sigfox
 * 
 * @author https://www.warefab.com
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <fonts/FreeMonoBold9pt7b.h>

#include "lis2dh12.h"

Lis2dh12 acc;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup(){
  Serial.begin(115200);
  Wire.begin();
  //init oled
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(WHITE);
  //init acc
  acc.init(0x19);
}

void loop(){
  getAccData();
  delay(200);
}


uint8_t getAccData(){
  uint8_t state = acc.whoIAm();
  if (state == 0x33) {
    acc.readXYZ();
  }else{
    Serial.println("no acc");
    return 0;
  }
  delay(10);
  display.setCursor(0,10);
  display.clearDisplay();
  display.println("3-AXIS ACC");
  display.print(" X : ");
  display.println(acc.x);
  display.print(" Y : ");
  display.println(acc.y);
  display.print(" Z : ");
  display.print(acc.z);
  display.display();

  return 1;
}
