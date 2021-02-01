#ifndef _LED_H_
#define _LED_H_

  #include <Arduino.h>

  #define LED_DEBUG 1
  
  //pins for rgb led
  #define LED_R 13
  #define LED_G 12
  #define LED_B 14
  //using active low led
  #define LED_ON LOW
  #define LED_OFF HIGH

  enum COLOUR{
    LED_BLACK,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_YELLOW,
  };

  void init_led(void);
  void setLed(COLOUR colour);
  void blinkLed(COLOUR colour, float freq);
  void stopBlink(void);

#endif
