#include "ccs.h"

Adafruit_CCS811 ccs;

void read_ccs_task(void * parameter);

void init_ccs(void){
  Serial.println("init CCS811");

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }

  xTaskCreatePinnedToCore(
    read_ccs_task,          /* Task function. */
    "read_ccs_task",        /* String with name of task. */
    10000,                 /* Stack size in bytes. */
    NULL,       /* Parameter passed as input of the task */
    1,                     /* Priority of the task. */
    NULL,                  /* Task handle. */
    1                      // pin to core 1 (i2c begin was called from core 1, interrupt has to be processed on core 1)
  );  
}

void read_ccs_task(void * parameter){
  while(1){
    if(ccs.available()){
      if(!ccs.readData()){
        #if CCS_DEBUG
        Serial.print("\nCO2: ");
        Serial.print(ccs.geteCO2());
        Serial.print("ppm, TVOC: ");
        Serial.println(ccs.getTVOC());
        #endif //CCS_DEBUG
      }
    }
    else{
      Serial.println("ERROR!");
      while(1);
    }
    delay(CCS_R_INT);
  }
  vTaskDelete(NULL);
}
