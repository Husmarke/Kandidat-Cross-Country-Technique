#include "HX711.h"

// Define HX711 objects for each load cell
HX711 scale1, scale2, scale3, scale4, scale5, scale6;

// Define Data and Clock pins
//const int DT_PINS[] = {6, 7, 8, 9, 10, 11};  // Different DT pins
const int DT_PINS[] = {6}; //Test on one loadcell
const int SCK_PIN = 5;  // Shared SCK pin

void setup() {
    Serial.begin(9600);
    
    // Initialize all HX711 modules test
    scale1.begin(DT_PINS[0], SCK_PIN);
 //   scale2.begin(DT_PINS[1], SCK_PIN);
 //   scale3.begin(DT_PINS[2], SCK_PIN);
 //   scale4.begin(DT_PINS[3], SCK_PIN);
 //   scale5.begin(DT_PINS[4], SCK_PIN);
 //   scale6.begin(DT_PINS[5], SCK_PIN);
}

void loop() {
    // Read data from each load cell
    Serial.print("1: ");
    Serial.print(scale1.get_units()/420, 2);
    Serial.print(" kg\t");
/*
    Serial.print("2: ");
    Serial.print(scale2.get_units()/420, 2);
    Serial.print(" kg\t");

    Serial.print("3: ");
    Serial.print(scale3.get_units()/420, 2);
    Serial.print(" kg\t");

    Serial.print("4: ");
    Serial.print(scale4.get_units()/420, 2);
    Serial.println(" kg");

    Serial.print("5: ");
    Serial.print(scale5.get_units()/420, 2);
    Serial.println(" kg");

    Serial.print("6: ");
    Serial.print(scale6.get_units()/420, 2);
    Serial.println(" kg");
*/
    delay(1000);
}
