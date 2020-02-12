/*
 * Temperature and humidity demo
 * Konnnect esp32-sigfox
 * 
 * @author https://www.warefab.com
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <fonts/FreeMonoBold9pt7b.h>

#define sht30_addr 0x44

uint8_t t_ = 1;

uint8_t cTemp = 0;
uint8_t fTemp = 0;
uint8_t humidity = 0;
//display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(WHITE);
}

void loop()
{
  printResult();
  delay(250);
}

void printResult() {
  getTempHum();
  display.setCursor(0,10);
  display.clearDisplay();
  display.println("TEMP & HUM");
  display.print(" T : ");
  display.print(cTemp);
  display.println("C");
  display.print(" RH : ");
  display.print(humidity);
  display.println("%");
  display.display();
}

void getTempHum(){
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
