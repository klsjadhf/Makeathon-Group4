#ifndef _OLED_H_
#define _OLED_H_

  #include <Arduino.h>
  #include <lcdgfx.h>

  #define OLED_DEBUG 0

  #define REFRESH_RATE 15
  #define REPRESH_PERIOD ((1/REFRESH_RATE)*1000)
  
  // oled
  #define OLED_TYPE DisplaySSD1306_128x64_I2C
  #define OLED_ADDR 0x3C
  #define I2C_SDA 21
  #define I2C_SCL 22
  #define I2C_FREQ 400000
  /*rstPin, SPlatformI2cConfig{ config.busId,
                                       static_cast<uint8_t>(config.addr ?: 0x3C),
                                       config.scl,
                                       config.sda,
                                       config.frequency ?: 400000 } ) {}
                                       */
  void init_oled(void);
  void oledPrintOnLine(int line, char const* str);
  void clearOled(void);

  extern OLED_TYPE display;

#endif
