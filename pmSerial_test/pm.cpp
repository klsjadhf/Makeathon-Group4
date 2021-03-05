#include "pm.h"
#include "oled.h"

SerialPM pms(PMS5003, PM_RX, PM_TX);

void read_pm_task(void * parameter);
void print_pm_error(SerialPM::STATUS pms_status);

extern DisplaySSD1306_128x64_I2C display;

// initalise pms5003 sensor
void init_pm(void){
  Serial.println("init pm... ");
  oledPrintOnLine(4, "Init PMS5003");
  // initalise serial port for sensor
  pms.init();
  // create new task to continuously read the pm2.5 sensor and update struct
  xTaskCreate(
    read_pm_task,          /* Task function. */
    "read_pm_task",        /* String with name of task. */
    10000,                 /* Stack size in bytes. */
    NULL,       /* Parameter passed as input of the task */
    1,                     /* Priority of the task. */
    NULL                   /* Task handle. */
  );  
  Serial.println("done");
}

void read_pm_task(void * parameter){
  while(1){
    pms.read(); // read the PM s
    
    if (pms){ // successfull read sensor
      String dispOut = "PM 2.5: " + String(pms.pm25) + " ug/m3";
      oledPrintOnLine(4, dispOut.c_str());
    }
    else{
      oledPrintOnLine(4, "PMS5003 error");
    }
    
    
    #if PM_DEBUG
      Serial.print("\nReading PMS5003. Status: ");
      if (pms){ // successfull read sensor
        Serial.println("Ok");
        // print formatted results
        Serial.printf("PM1.0 %2d, PM2.5 %2d, PM10 %2d [ug/m3]\n",
                      pms.pm01, pms.pm25, pms.pm10);
    
        if (pms.has_number_concentration())
          Serial.printf("N0.3 %4d, N0.5 %3d, N1.0 %2d, N2.5 %2d, N5.0 %2d, N10 %2d [#/100cc]\n",
                        pms.n0p3, pms.n0p5, pms.n1p0, pms.n2p5, pms.n5p0, pms.n10p0);
      }
      else
        print_pm_error(pms.status);
    #endif //PM_DEBUG
    delay(PM_R_INT);
  }
  vTaskDelete(NULL);
}

void print_pm_error(SerialPM::STATUS pms_status){
  switch (pms_status){
    case pms.OK: // should never come here
      break;     // included to compile without warnings
    case pms.ERROR_TIMEOUT:
      Serial.println(F(PMS_ERROR_TIMEOUT));
      break;
    case pms.ERROR_MSG_UNKNOWN:
      Serial.println(F(PMS_ERROR_MSG_UNKNOWN));
      break;
    case pms.ERROR_MSG_HEADER:
      Serial.println(F(PMS_ERROR_MSG_HEADER));
      break;
    case pms.ERROR_MSG_BODY:
      Serial.println(F(PMS_ERROR_MSG_BODY));
      break;
    case pms.ERROR_MSG_START:
      Serial.println(F(PMS_ERROR_MSG_START));
      break;
    case pms.ERROR_MSG_LENGTH:
      Serial.println(F(PMS_ERROR_MSG_LENGTH));
      break;
    case pms.ERROR_MSG_CKSUM:
      Serial.println(F(PMS_ERROR_MSG_CKSUM));
      break;
    case pms.ERROR_PMS_TYPE:
      Serial.println(F(PMS_ERROR_PMS_TYPE));
      break;
  }
}
