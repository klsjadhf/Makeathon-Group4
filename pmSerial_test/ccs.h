#ifndef _VOC_H_
#define _VOC_H_

  #include <Arduino.h>
  #include <Adafruit_CCS811.h>
  
  #define CCS_DEBUG 0

  // ms between each reading
  #define CCS_R_INT 500

  extern Adafruit_CCS811 ccs;
  extern bool ccsError;

  int init_ccs(void);

#endif
