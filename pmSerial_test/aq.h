#ifndef _AQ_H_
#define _AQ_H_

  #include <Arduino.h>
  #include "pm.h"
  #include "ccs.h"

  #define AQ_DEBUG 1

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

  void begin_aq(void (*onChange)(void));

#endif
