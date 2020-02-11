#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <fonts/FreeMonoBold9pt7b.h>

#include <TinyGPS++.h>

#define RMC_ONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
#define B_Pin 4

#define RXD2 16
#define TXD2 17
 
TinyGPSPlus gps;

uint8_t t_ = 1;

Adafruit_SSD1306 display(-1);

void setup() {
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(200);
  Serial2.print(RMC_ONLY);
  delay(200);

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(WHITE);
}

void loop() {
  while (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      displayInfo();
      digitalWrite(B_Pin, t_);
      t_ = !t_;
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    while (true);
  }
}


void displayInfo()
{
  display.setCursor(0,10);
  display.clearDisplay();
  display.println(" GPS DATA");
  display.print(F("TM:"));
  //time data
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) display.print(F("0"));
    uint8_t t_ = gps.time.hour() + 3;
    display.print(t_);
    display.print(F(":"));
    if (gps.time.minute() < 10) display.print(F("0"));
    display.print(gps.time.minute());
    display.print(F(":"));
    if (gps.time.second() < 10) display.print(F("0"));
    display.println(gps.time.second());
  }
  //location data
  if (gps.location.isValid())
  {
    display.print(F("LT: "));
    display.println(gps.location.lat(), 6);
    display.print(F("LN:"));
    display.print(gps.location.lng(), 6);
  }
  display.display();
}
