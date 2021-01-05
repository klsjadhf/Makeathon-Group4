#include "pm.h"

//HardwareSerial pmSerial(PM_SERIAL);
//SerialPM pms(PMS5003, pmSerial);
SerialPM pms(PMS5003, PM_RX, PM_TX);
String air_quality_str = "";
int air_quality_status = -1;
int last_air_quality_status = -1;

void read_pm_task(void * parameter);
void print_error(SerialPM::STATUS pms_status);
String cal_AQ(uint16_t pm25_reading, uint16_t pm10_reading);

// initalise pm2.5 sensor
// using the pms5003
void init_pm(void (*onChange)(void)){
  Serial.print("init pm... ");
  // initalise serial port for sensor
//  pmSerial.begin(9600, SERIAL_8N1, PM_RX, PM_TX);
//  pms.init();
  // create new task to continuously read the pm2.5 sensor and update struct
  xTaskCreate(
    read_pm_task,          /* Task function. */
    "read_pm_task",        /* String with name of task. */
    10000,                 /* Stack size in bytes. */
    (void*)onChange,      /* Parameter passed as input of the task */
    1,                     /* Priority of the task. */
    NULL                   /* Task handle. */
  );  
  Serial.println("done");
}

void read_pm_task(void * parameter){
  while(1){
    void (*onStatusChange)(void) = (void (*)(void))parameter;
    pms.pm25 = analogRead(34)/15;
    Serial.printf("analogread: %d\n",pms.pm25);
//    pms.read(); // read the PM s
    #if PM_DEBUG
      Serial.print("Reading sensor. Status: ");
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
        print_error(pms.status);
    #endif
    cal_AQ(pms.pm25, pms.pm10);
    if(last_air_quality_status != air_quality_status){
      last_air_quality_status = air_quality_status;
      if(onStatusChange != NULL) onStatusChange();
    }
    delay(PM_R_INT);
  }
  vTaskDelete(NULL);
}

void print_error(SerialPM::STATUS pms_status){
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

// calculates the air quality based on following link (need to do 24 hr avg?)
// https://www.haze.gov.sg/docs/default-source/faq/computation-of-the-pollutant-standards-index-%28psi%29.pdf
String cal_AQ(uint16_t pm25_reading, uint16_t pm10_reading){
  const int pm25[] = {0, 12, 55, 150, 250, 350, 500};
  const int pm10[] = {0, 50, 150, 350, 420, 500, 600};
  const int psi[] = {0, 50, 100, 200, 300, 400 ,500};
  int i,j;
  for(i=0; i<7; i++){
    if(pm25_reading < pm25[i])
      break;
  }
  int pm25_sub = round(((float)(psi[i]-psi[i-1])/(float)(pm25[i]-pm25[i-1]))*(float)(pm25_reading-pm25[i-1]))+psi[i-1];

  for(i=0; i<7; i++){
    if(pm10_reading < pm10[i])
      break;
  }
  int pm10_sub = round(((float)(psi[i]-psi[i-1])/(float)(pm10[i]-pm10[i-1]))*(float)(pm10_reading-pm10[i-1]))+psi[i-1];

  int PSI = max(pm25_sub, pm10_sub);
  
  if(PSI<psi[1]){
    air_quality_str = "Good";
    air_quality_status = GOOD;
  }
  else if(PSI<psi[2]){
    air_quality_str = "Moderate";
    air_quality_status = MODERATE;
  }
  else if(PSI<psi[3]){
    air_quality_str = "Unhealthy";
    air_quality_status = UNHEALTHY;
  }
  else if(PSI<psi[4]){
    air_quality_str = "Very Unhealthy";
    air_quality_status = V_UNHEALTHY;
  }
  else{
    air_quality_str = "Hazardous";
    air_quality_status = HAZARDOUS;
  }

  #if PM_DEBUG 
    Serial.printf("pm25_sub: %d, pm10_sub: %d, PSI: %d\n", pm25_sub, pm10_sub, PSI);
    Serial.printf("Air quality is %s\n", air_quality_str.c_str());
  #endif

  return air_quality_str;
}
