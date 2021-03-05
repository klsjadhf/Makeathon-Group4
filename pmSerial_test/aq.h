#ifndef _AQ_H_
#define _AQ_H_

  #include <Arduino.h>
  #include "pm.h"
  #include "ccs.h"

  #define AQ_DEBUG 0

  enum AIR_QUALITY{
    AQ_ERROR = -1,
    GOOD,
    MODERATE,
    UNHEALTHY,
    V_UNHEALTHY,
    HAZARDOUS,
  };

  extern String air_quality_str;
  extern AIR_QUALITY air_quality_status;
  extern int PSI;

  void begin_aq(void (*onChange)(void));

#endif
