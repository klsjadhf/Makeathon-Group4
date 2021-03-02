#include "pm.h"
//#include "lcdgfx.h"
#include "oled.h"

//HardwareSerial pmSerial(PM_SERIAL);
//SerialPM pms(PMS5003, pmSerial);
SerialPM pms(PMS5003, PM_RX, PM_TX);

void read_pm_task(void * parameter);
void print_pm_error(SerialPM::STATUS pms_status);

extern DisplaySSD1306_128x64_I2C display;

// initalise pm2.5 sensor
// using the pms5003
void init_pm(void){
  Serial.println("init pm... ");
  // initalise serial port for sensor
//  pmSerial.begin(9600, SERIAL_8N1, PM_RX, PM_TX);
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
//    void (*onStatusChange)(void) = (void (*)(void))parameter;
//    pms.pm25 = analogRead(34)/15;
//    Serial.printf("analogread: %d\n",pms.pm25);
    pms.read(); // read the PM s
    
//    display.printFixed(0, 32, "                     ", STYLE_NORMAL);//clear line
//    display.printFixed(0, 32, "PM 2.5: ", STYLE_NORMAL);
//    display.printFixed(8*6, 32, String(pms.pm25).c_str(), STYLE_NORMAL);
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
//    cal_AQ(pms.pm25, pms.pm10);
//    if(last_air_quality_status != air_quality_status){
//      last_air_quality_status = air_quality_status;
//      if(onStatusChange != NULL) onStatusChange();
//    }
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
