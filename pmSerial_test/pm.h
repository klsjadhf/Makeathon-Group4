#ifndef _PM_H_
#define _PM_H_

  #include <Arduino.h>
  #include <PMserial.h>

  #define PM_DEBUG 0

  #define PM_TX 32
  #define PM_RX 33

  // ms between each reading
  #define PM_R_INT 500

  extern SerialPM pms;

  void init_pm(void);

#endif
