#include "HX711.h"

// Define HX711 amplifier
#define DT_PIN 6  // Data pin
#define SCK_PIN 5 // Clock pin

HX711 scale;

void setup() {
    Serial.begin(9600);
    scale.begin(DT_PIN, SCK_PIN);
    
    Serial.println("HX711 Initialization...");
    
    // Tare the scale (set current reading to zero)
    Serial.println("Taring scale...");
    scale.tare();
    
    Serial.println("Ready!");
}

void loop() {
    // Read weight (raw units)
    float weight = scale.get_units(10); // Average over 10 readings
    
    Serial.print("Weight: ");
    Serial.print(weight, 10);
    Serial.println(" kg");

    delay(500); // Read every 500ms
}
