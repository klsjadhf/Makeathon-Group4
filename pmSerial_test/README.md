using analog input to test code.  
for actual sensor, uncomment lines 20, 39 and comment lines 37, 38 in pm.cpp  
using uart1, tx -> pin 18, rx -> pin19  
pin connection can be changed in pm.h  
in pm.h, put `#define PM_DEBUG 1` to enable debug messages to serial  

2 ways to use. async method and polling method.  

async method  
in `setup()` put `init_pm(function)` where `function` is the function to be called when the air quality status changes.  
time between each reading can be changed in pm.h. default is `#define PM_R_INT 500` (500 milliseconds)  

polling method  
in `setup()` put `init_pm(NULL)`  
the following variables will automatically updated every interval defined in pm.h  
`pms.pm01`, `pms.pm25`, `pms.pm10`  
`pms.n0p3`, `pms.n0p5`, `pms.n1p0`, `pms.n2p5`, `pms.n5p0`, `pms.n10p0`  
`air_quality_str`, `air_quality_status`  
continuously check values of these variables in `loop()`  

pms. something -> see [PMSerial](https://github.com/avaldebe/PMserial#decoded-measurements)  

`air_quality_str` can have the values  
`"Good"`, `"Moderate"`, `"Unhealthy"`, `"Very Unhealthy"`, `"Hazardous"`  

`air_quality_status` is enum of  
`GOOD`, `MODERATE`, `UNHEALTHY`, `V_UNHEALTHY`, `HAZARDOUS`


air quality formula from  
https://www.haze.gov.sg/docs/default-source/faq/computation-of-the-pollutant-standards-index-%28psi%29.pdf


example code depends on  
[Universal Telegram Bot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)  
[LCDGFX](https://github.com/lexus2k/lcdgfx)  

lib depends on  
[PMSerial](https://github.com/avaldebe/PMserial)
