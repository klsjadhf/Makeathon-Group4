#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
 Serial.begin(9600);

 display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3C (for the 128x32)
 
  display.display();
  delay(2000);

 // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);

}

void loop() {
  
  float sensor=analogRead(A0);
  display.setCursor(0, 0);
  display.print("Value: ");
  display.println(sensor);
  display.display(); //you have to tell the display to...display
  delay(2000);
  display.clearDisplay();
}
