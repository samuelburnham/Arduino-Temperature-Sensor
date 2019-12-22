#include <EEPROM.h>


void setup() {
  Serial.begin(9600);
  EEPROM.write(0,0);
  Serial.print("Current counter number: ");
  Serial.println(EEPROM.read(0));
}

void loop() {
  // put your main code here, to run repeatedly:

}
