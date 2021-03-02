#include "ccs.h"
//#include "lcdgfx.h"
#include "oled.h"

Adafruit_CCS811 ccs;
bool ccsError = 0;

extern SemaphoreHandle_t i2cMux;
extern DisplaySSD1306_128x64_I2C display;

void read_ccs_task(void * parameter);


int init_ccs(void){
  Serial.println("init CCS811");
  oledPrintOnLine(5, "Init CCS811");

  xSemaphoreTake( i2cMux, portMAX_DELAY );
  if(!ccs.begin()){
    xSemaphoreGive(i2cMux);
    Serial.println("Failed to start sensor! Please check your wiring.");
    oledPrintOnLine(5, "CCS811 init failed");
    return -1;
  }
  xSemaphoreGive(i2cMux);

  xTaskCreatePinnedToCore(
    read_ccs_task,          /* Task function. */
    "read_ccs_task",        /* String with name of task. */
    10000,                 /* Stack size in bytes. */
    NULL,       /* Parameter passed as input of the task */
    1,                     /* Priority of the task. */
    NULL,                  /* Task handle. */
    1                      // pin to core 1 (i2c begin was called from core 1, interrupt has to be processed on core 1)
  );  
  return 0;
}

void read_ccs_task(void * parameter){
  while(1){
    xSemaphoreTake( i2cMux, portMAX_DELAY );
    if(ccs.available()){
      if(!ccs.readData()){
        ccsError = 0;
        #if CCS_DEBUG
          Serial.print("CO2: ");
          Serial.print(ccs.geteCO2());
          Serial.print("ppm, TVOC: ");
          Serial.println(ccs.getTVOC());
        #endif //CCS_DEBUG
//        display.printFixed(0, 40, "                     ", STYLE_NORMAL);//clear line
//        display.printFixed(0, 40, "eCO2: ", STYLE_NORMAL);
//        display.printFixed(6*6, 40, String(ccs.geteCO2()).c_str(), STYLE_NORMAL);
          String dispOut = "eCO2: " + String(ccs.geteCO2()) + " ppm";
          oledPrintOnLine(5, dispOut.c_str());
      }
      else{
        ccsError = 1;
        Serial.println("ERROR!");
        String dispOut = "CCS811 error";
        oledPrintOnLine(5, dispOut.c_str());
      }
    }
    xSemaphoreGive(i2cMux);
    delay(CCS_R_INT);
  }
  vTaskDelete(NULL);
}
