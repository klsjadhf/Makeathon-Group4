#ifndef _BATT_H_
#define _BATT_H_

  #include <Arduino.h>

  #define BATT_DEBUG 1

  #define BATT_PIN 35
  #define POLL_INT 10000 //ms between each check
  #define LOW_BATT_THRES 10

  void init_batt(void (*onLowBatt)(void));
  extern int BattLvl;
#endif //_BATT_H_
