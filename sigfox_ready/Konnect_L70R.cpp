/*
 * gps lib
 * 
 * @author https://www.warefab.com
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Konnect_L70R.h"

void L70R::init(){
  g_time = 0;
  lat = 0;
  lat_ns = 0;
  lng = 0;
  lng_ew = 0;
  speed = 0;
  g_date = 0;
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(200);
  Serial2.print(RMC_ONLY);
  delay(500);
}

uint8_t L70R::parse(char *nmea, uint16_t len) {
  uint16_t size = len;
  double result;
  uint8_t x = 0;
  uint8_t y = 0;

  char buf[12] = { 0 };
  //char ut[40];

  p_gps = &gpsbuf[0];

  for (size = 0; size < len; size++) {
    *(p_gps + size) = *(nmea + size);
  }
  *(p_gps + size + 1) = 0;

  if (strstr((char*) gpsbuf, "$GPRMC") && strstr((char*) gpsbuf, "*")) {
    Serial.println(gpsbuf);

    //strtok((char*) gpsbuf, ","); //nmea msg
    //p_gps = &gpsbuf[0];
    for (size = 0; size < len; size++) {
      if (*(p_gps + size) == ',') {
        if (x > 2) {
          result = strtod(buf, NULL);
        } else {
          result = 0.00;
        }
        //sprintf(ut, "%d -> %s, %lf\r\n", y, buf, result);
        //usart_puts(&huart1, (char*) ut);
        switch (y) {
        case 0: //nmea - gprmc
        case 2: //validity
        case 4: //lat dir
          lat_ns = buf[0];
        case 6: //lng dir
          lng_ew = buf[0];
        case 8: //magnetic variation
          break;
        case 1: //g_time
          g_time = ((uint32_t) result) + 30000;
          if (g_time > 240000)
            g_time = g_time - 240000;
          break;
        case 3: //latitude
          lat = convertRawCoords((uint32_t) (result * 10000.0));
          break;
        case 5: //longitude
          lng = convertRawCoords((uint32_t) (result * 10000.0));
          break;
        case 7: //ground course speed km/hr
          speed = (uint8_t) (result * 1.852); // * 100.0);
          break;
        case 9: //date
          g_date = (uint32_t) result;
          break;
        default:
          break;
        };

        if (y >= 9)
          return 1;

        memset(buf, 0, 12);
        x = 0;
        y++;
        //size++;
      } else {
        buf[x++] = *(p_gps + size); //*p_gps++;
      }
    }
  }
  return 0;
}

/*
 * convert raw gps coords dd.mmmm to dd.dddd
 */
uint32_t L70R::convertRawCoords(uint32_t coord) {
  uint16_t crd_d1 = coord / 1000000;
  uint32_t crd_d2 = ((coord % 1000000) * 10) / 60;
  crd_d2 = (crd_d1 * 100000) + crd_d2;
  return crd_d2;
}

char* L70R::dateTimeFormat(char *buf_tk, uint32_t dt_, char sep) {
  uint8_t tk1 = (dt_ / 10000.0);
  uint8_t tk2 = ((uint16_t) (dt_ / 100.0)) % 100;
  uint8_t tk3 = (uint8_t) (dt_ % 100);
  sprintf(buf_tk, "%02d%c%02d%c%02d", tk1, sep, tk2, sep, tk3);
  return buf_tk;
}
