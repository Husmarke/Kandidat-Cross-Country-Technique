#include <SoftwareSerial.h>
#include "Adafruit_VL53L0X.h"

// ------------------ CONFIG ------------------
#define NUM_SOURCES 6
// for the loadcell pins
// black = first
// white = second
// silver = third

const int LoadcellPins[NUM_SOURCES] = { A5,A7,A6,A10,A9,A8};
const int IMUPins[NUM_SOURCES] = {A1,A2,A3};


#define TRIG_PIN 4 // remove when getdistance has been changed
#define ECHO_PIN 5

// LIDAR sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();


#define delayMS 5  // Lower delay for higher frequency

// Use SoftwareSerial only if you must – prefer HardwareSerial
//SoftwareSerial BTSerial(6, 7);  // RX, TX

// ------------------ STATE VARIABLES ------------------
int strainValues[NUM_SOURCES];
//float calibrationFactors[NUM_SOURCES] = {0};

// ---------------------- SETUP ------------------------
void setup() {
  Serial.begin(9600);    // Faster baud rate
  //BTSerial.begin(9600); /uncomment when BT sensor works and is connected

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if (!lox.begin()) { // boot check for LIDAR
    Serial.println(F("Failed to boot VL53L0X"));

  //calibrateSensors();  // uncomment when function is changed (see function)
  }
}

// ------------------ CALIBRATION --------------------
void calibrateSensors() {// TODO change calibration to use min/max normalization
  const int samples = 200;
  float sums[NUM_SOURCES] = {0};

  for (int j = 0; j < samples; j++) {
    for (int i = 0; i < NUM_SOURCES; i++) {
      sums[i] += analogRead(LoadcellPins[i]);
    }
    delay(5);  // Small delay to let ADC settle
  }
}

// ------------------ DISTANCE ------------------
float getDistance() { // TODO change to work with LIDAR
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    return measure.RangeMilliMeter;
  } else {
    return;
  }
}

// ------------------ MAIN LOOP ------------------
void loop() {
  // Read sensors
  for (int i = 0; i < NUM_SOURCES; i++) {
    strainValues[i] = analogRead(LoadcellPins[i]);
  }

  float distance = getDistance(); // getting distance in mm from LIDAR

  // Send data using raw print for speed
  Serial.print("L:");
  for (int i = 0; i < 3; i++) {
    Serial.print(strainValues[i]);
    Serial.print(",");
  }
  Serial.print("R:");
  for (int i = 3; i < 6; i++) {
    Serial.print(strainValues[i]);
    Serial.print(",");
  }
  Serial.print("D:");
  Serial.println(distance, 1);

  // BTSerial follows same structure
  //BTSerial.print("L:");
  //for (int i = 0; i < 3; i++) {
    //BTSerial.print(strainValues[i]);
    //BTSerial.print(",");
  //}
  //BTSerial.print("R:");
  //for (int i = 3; i < 6; i++) {
    //BTSerial.print(strainValues[i]);
    //BTSerial.print(",");
  //}
  //BTSerial.print("D:");
  //BTSerial.println(distance, 1);

  delay(delayMS);  // Tweak or remove to increase rate
}
