#ifndef _PM_H_
#define _PM_H_

  #include <Arduino.h>
  #include <PMserial.h>

  #define PM_DEBUG 1

  // uart0 used for communication with pc
  // lib uses uart1. might need to change pins to avoid conflict with flash
//  #define PM_SERIAL 2
  #define PM_TX 32
  #define PM_RX 33

  // ms between each reading
  #define PM_R_INT 500

  extern SerialPM pms;

  void init_pm(void);

#endif
