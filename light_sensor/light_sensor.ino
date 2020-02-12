/*
 * Light Sensor demo
 * Konnect ESP32-Sigfox dev board
 * 
 * @author https://www.warefab.com
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <fonts/FreeMonoBold9pt7b.h>

#define ldr_pin 36

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setFont(&FreeMonoBold9pt7b);
}

void loop() {
  getAmbientLight();
  delay(10);
}

void getAmbientLight() {
  uint16_t ldr = 0;
  ldr = analogRead(ldr_pin);

  ldr = map(ldr, 0, 4095, 0, 100);

  display.setCursor(0, 10);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.println("LIGHT SENSE");
  display.println();
  display.print("VAL : ");
  display.print(ldr);
  display.print("%");
  display.display();
}
