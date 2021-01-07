#ifndef __PM_H
#define __PM_H

  #include <PMserial.h>

  #define PM_DEBUG 0

  // uart0 used for communication with pc
  // lib uses uart1. might need to change pins to avoid conflict with flash
//  #define PM_SERIAL 2
  #define PM_TX 18
  #define PM_RX 19

  // ms between each reading
  #define PM_R_INT 500

  enum AIR_QUALITY{
    GOOD,
    MODERATE,
    UNHEALTHY,
    V_UNHEALTHY,
    HAZARDOUS,
  };

  extern SerialPM pms;
  extern String air_quality_str;
  extern int air_quality_status;

  void init_pm(void (*onChange)(void));

#endif
