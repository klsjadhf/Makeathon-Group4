#ifndef _VOC_H_
#define _VOC_H_

  #include <Arduino.h>
  #include <Adafruit_CCS811.h>
  
  #define CCS_DEBUG 1

// might need to put ccs811 on different i2c to avoid conflict with oled
// adafruit lib cannot change pin easily. change oled pin instead
//  #define CCS_I2C 2
//  #define CCS_SDA 32
//  #define CCS_SCL 33

  // ms between each reading
  #define CCS_R_INT 500

  extern Adafruit_CCS811 ccs;

  void init_ccs(void);

#endif
