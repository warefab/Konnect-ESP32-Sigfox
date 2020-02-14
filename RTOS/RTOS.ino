/*
   3 Axis Accelerometer demo
   with a rtos task for blinking leds

   Konnnect esp32-sigfox

   @author https://www.warefab.com
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <fonts/FreeMonoBold9pt7b.h>

#include "lis2dh12.h"

Lis2dh12 acc;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BLINK_RUNNING_CORE 1
#define LED 15

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*blink rtos task*/
void taskBlink( void *pvParameters );
TaskHandle_t blinkTaskHandle = NULL;

void setup() {
  //init serial
  Serial.begin(115200);
  //init wire
  Wire.begin();
  //init oled
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(WHITE);
  //init acc
  acc.init(0x19);


  /*init rtos blink task*/
  xTaskCreatePinnedToCore(
    taskBlink, "taskBlink", 1024, NULL, 2,
    &blinkTaskHandle, BLINK_RUNNING_CORE);
}

void loop() {
  getAccData();
  delay(100);
}

/*
  led blink task on core 1
*/
void taskBlink(void *pvParameters) {
  (void) pvParameters;
  //init led - output
  pinMode(LED, OUTPUT);
  //loop blink
  for (;;) {
    digitalWrite(LED, HIGH);
    Serial.println("On");
    vTaskDelay(200);
    digitalWrite(LED, LOW);
    Serial.println("Off");
    vTaskDelay(800);
  }
}

//get 3 axis sensor data
uint8_t getAccData() {
  uint8_t state = acc.whoIAm();
  if (state == 0x33) {
    acc.readXYZ();
  } else {
    Serial.println("no acc");
    return 0;
  }
  delay(10);
  display.setCursor(0, 10);
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
