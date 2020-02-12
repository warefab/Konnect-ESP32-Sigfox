/* 
 * Analog mems mic demo
 * Konnect ESP32-Sigfox dev board
 * 
 * @author https://www.warefab.com
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <fonts/FreeMonoBold9pt7b.h>

#define sample_time 50
#define mic_pin 39

uint16_t mic_adc = 0;

unsigned long int prev_t = 0;

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
  findPtp();
  delay(1);
}

void findPtp() {
  uint16_t mic_max = 0;
  uint16_t mic_min = 1023;
  uint16_t mic_ptp = 0;

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

  display.setCursor(0, 10);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.println("MEMS MIC");
  display.println();
  display.print("PTP : ");
  display.print(mic_ptp);
  display.display();
}
