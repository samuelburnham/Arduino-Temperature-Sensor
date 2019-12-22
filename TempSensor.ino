#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <EEPROM.h>

#define ONE_WIRE_BUS 2
#define EEPROM_ADDR 0
#define BUZZER_PIN 8

DeviceAddress thermAddress;
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
OneWire oneWire(ONE_WIRE_BUS); // create a oneWire instance to communicate with temperature IC
DallasTemperature tempSensor(&oneWire);  // pass the oneWire reference to Dallas Temperature

char cycles_used = 0;
char cycles_remaining;
unsigned long start_time = 0;  //time in milliseconds
unsigned long current_time = 0;
float temp;
bool hot = false;

void setup() {
  Serial.begin(9600);
  cycles_used = (char)EEPROM.read(EEPROM_ADDR);
  cycles_remaining = 40 - (char)cycles_used;

  pinMode(BUZZER_PIN, OUTPUT);
  //Debug w/o LCD
  Serial.println("Start!");
  //Power on sequence for new use
  //Output Welcome Message, Number of Cycles remaining to LCD
  display.begin();
  display.setContrast(57); //Edit as needed
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Hello!");
  display.print("# of Cycles Remaining: ");
  display.println(cycles_remaining, DEC);
  display.display();
  delay(5000);
  display.clearDisplay();

  tempSensor.begin(); // initialize the temp sensor
  if (!tempSensor.getAddress(thermAddress, 0))
    Serial.println("Unable to find Device.");
  else {
    Serial.print("Device 0 Address: ");
    printAddress(thermAddress);
    Serial.println();
  }
  tempSensor.setResolution(thermAddress, 11);      // set the temperature resolution (9-12)

}

void loop() {
  tempSensor.requestTemperatures();
  temp = displayTemp(tempSensor.getTempC(thermAddress));
  //Once boiling point reached, wait one minute to alert
  //Error checks for false read
  if (temp >= 100 && hot == false) {
    hot = true;
    start_time = millis();
  }
  else if (temp >= 100 && hot == true) {
    current_time = millis();
    if (current_time-start_time >= 60000) {
      displayAlert();
    }
  }
  else {
    hot = false;
  }
  Serial.print("Current time: ");
  Serial.println(current_time-start_time);
}

float displayTemp(float tempReading) {             // temperature comes in as a float with 2 decimal place  
  // show temperature °C
  display.clearDisplay();
  display.print(tempReading, 2);  // rounded to 2 decimal places
  display.print((char)247);              // degree symbol
  display.println("C");
  Serial.print(tempReading);      // serial debug output
  Serial.print("°");
  Serial.println("C  ");

  display.display();                    // update the LCD display with all the new text
  return tempReading;
}

//Output LCD Message saying water's ready, buzzer sounds, and eeprom updates
//This continues until pushbutton pressed (careful of debounce!)
void displayAlert() {  
  cycles_used++;
  EEPROM.write(EEPROM_ADDR, cycles_used);
  display.clearDisplay();
  display.println("Water's Ready!");
  display.display();
  Serial.println("Water's Ready!");
  while (true) {
    tone(BUZZER_PIN, 700, 1000);
    delay(2000);
  }
}

// print device address from the address array
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
