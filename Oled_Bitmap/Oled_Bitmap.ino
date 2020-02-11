/*
 * logo image example
 * for warefab konnect esp8266 oled
 * 
 * @muchirijohn
 * 
 */
 
//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "images.h"

Adafruit_SSD1306 display(-1);

void setup()   {                
  Serial.begin(9600);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
  //show bitmap
  //display.invertDisplay(true);
  display.drawBitmap(0, 0, warefab_logo, 128, 64, WHITE);
  display.display();
  delay(20000); 
}


void loop() {
  testscrolltext();
}

void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("WAREFAB");
  display.display();
  delay(1);
 
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);    
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}
