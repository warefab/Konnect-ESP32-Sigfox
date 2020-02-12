/*
 * gps lib
 * Konnnect esp32-sigfox
 * 
 * @author https://www.warefab.com
*/


#ifndef Konnect_L70R_H_
#define Konnect_L70R_H_

#include <Arduino.h>

#define RXD2 16
#define TXD2 17

#define RMC_ONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"

class L70R {
  public:

    void init();
    uint8_t parse(char *, uint16_t);
    char* dateTimeFormat(char *, uint32_t, char);

    uint32_t g_time;
    uint32_t lat;
    char lat_ns;
    uint32_t lng;
    char lng_ew;
    uint16_t speed;
    uint32_t g_date;

  private:

    uint32_t convertRawCoords(uint32_t);

    char gpsbuf[128];
    char *p_gps;

};
#endif /* H_ */
