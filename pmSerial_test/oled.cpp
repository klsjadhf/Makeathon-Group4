#include "oled.h"

OLED_TYPE display(-1,{-1,OLED_ADDR,I2C_SCL,I2C_SDA,I2C_FREQ});

void disp_oled_task(void * parameter);

// 64/8, 128/6 + 1(for '\0')
char dispBuffer[8][22] = {0};

TaskHandle_t oledTaskHandle = NULL;
bool writingBuffer = 0;

void init_oled(void){
  Serial.println("Init oled");

  display.setFixedFont( ssd1306xled_font6x8 );
//  display.setColor(RGB_COLOR16(0,0,255));
  display.begin();
  display.clear();
  display.printFixed(0, 8, "Hello!", STYLE_NORMAL);
  delay(500);
  display.clear();
  
  
  xTaskCreatePinnedToCore(
    disp_oled_task,          /* Task function. */
    "disp_oled_task",        /* String with name of task. */
    10000,                 /* Stack size in bytes. */
    NULL,       /* Parameter passed as input of the task */
    1,                     /* Priority of the task. */
    &oledTaskHandle,                  /* Task handle. */
    1                      // pin to core 1 (i2c begin was called from core 1, interrupt has to be processed on core 1)
  );  
}

void disp_oled_task(void * parameter){
  while(1){
    #ifdef OLED_DEBUG
    Serial.println("disp on oled");
    #endif //OLED_DEBUG

    while(writingBuffer){
      Serial.println("oled waiting");
    }

    display.clear();
    for(int i=0; i<8; i++){
      display.printFixed(0, 8*(i+1), dispBuffer[i], STYLE_NORMAL);
    }
    
    delay(REPRESH_PERIOD);
  }
  vTaskDelete(NULL);
}

void oledPrintOnLine(int line, char const* str){
  writingBuffer = 1;
//  vTaskSuspend(oledTaskHandle);
  strcpy(dispBuffer[line-1], str);
//  vTaskResume(oledTaskHandle);
  writingBuffer = 0;
}

void clearOled(void){
  for(int i=0; i<8; i++){
    dispBuffer[i][0] = 0;
  }
}
