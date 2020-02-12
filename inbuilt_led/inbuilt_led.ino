/*
 * inbuilt led demo
 * Konnect ESP32-Sigfox dev board
 * 
 * @author https://www.warefab.com
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <fonts/FreeMonoBold9pt7b.h>

int x= 0; 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(15, OUTPUT);
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.clearDisplay();
  display.println("INBUILT LED");
  display.println();
  display.println("BLINK DEMO");
  display.display();
}

void loop() {
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(15, HIGH);
  Serial.println("On");
  delay(200);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  digitalWrite(15, LOW);
  Serial.println(x++);
  delay(800);

}
