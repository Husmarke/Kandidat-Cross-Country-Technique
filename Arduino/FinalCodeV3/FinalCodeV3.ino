#include <SoftwareSerial.h>
#include <Wire.h>
#include "ICM20600.h"
#include "Adafruit_VL53L0X.h"

// ------------------ CONFIG ------------------
#define NUM_SOURCES 6
// for the loadcell pins
// black = first
// white = second
// silver = third



const int LoadcellPins[NUM_SOURCES] = {A5, A12, A6, A10, A9, A8};

// LIDAR sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();


//IMU axis(1 for first IMU, 2 for second IMU)
float ax1, ay1, az1;
float ax2, ay2, az2;

#define delayMS 5  // Delay for program loop (not currently needed)

SoftwareSerial BTSerial(3, 2);  // RX, TX

// Initialize IMU instances on each software I2C bus(IMU)
ICM20600 imu1(true); // default
ICM20600 imu2(false); // changed adress

// ------------------ STATE VARIABLES ------------------
int strainValues[NUM_SOURCES];

// ---------------------------------------------VARIABLES FOR CALIBRATION---------------------------------------------------
float sumReadings[NUM_SOURCES] = { 0 };       // Sum of all readings per source
int numReadings[NUM_SOURCES] = { 0 };           // Number of readings per source
float calibrationFactors[NUM_SOURCES] = { 0 };  // Final calibration factors

// for minmax calibration (not currently in use)
//float max_val[NUM_SOURCES] = {0};
//float min_val[NUM_SOURCES] = {10000};

// ---------------------- SETUP ------------------------
void setup() {
  Serial.begin(9600);    // Faster baud rate
  BTSerial.begin(9600); //uncomment when BT sensor works and is connected

  delay(1000);
  if (!lox.begin()) { // boot check for LIDAR
    Serial.println(F("Failed to boot VL53L0X"));
  }
  
  calibrateSensors();
  Wire.begin();
  Wire.begin();

  // Initialize each IMU
  imu1.initialize();
  imu2.initialize();


}

// ------------------ CALIBRATION --------------------
/*
void calibrateSensorsMinMax() {// TODO change calibration to use min/max normalization
  //BTSerial.print("Calibration loadcells, move and jump");
  Serial.print("Calibration loadcells, move and jump");
  
  const int samples = 500;
  float cur_val;
  
  for (int j = 0; j < samples; j++) {
    for (int i = 0; i < NUM_SOURCES; i++) {
      cur_val = analogRead(LoadcellPins[i]);
      if (cur_val > max_val[i]){
        max_val[i] = cur_val;
      }
      else if (cur_val < min_val[i])
        min_val[i] = cur_val;
    }
    delay(5);  // Small delay to let ADC settle
  }
  BTSerial.print("Calabration completed, lets go skiing!");
  Serial.print("Calibration completed, lets go skiing!");
}
*/
void calibrateSensors() {
  const int samples = 500;
  for (int j = 0; j < samples; j++) {
    for (int i = 0; i < NUM_SOURCES; i++) {
      sumReadings[i] += analogRead(LoadcellPins[i]);  // read the input pin
      numReadings[i]++;
    }
  }
  
  for (int i = 0; i < NUM_SOURCES; i++) {
    if (numReadings[i] > 0) {
      calibrationFactors[i] = sumReadings[i] / numReadings[i];
    } else {
      Serial.print("A");
      Serial.print(i);
      Serial.println(" - No data collected.");
    }
  }
}

// ------------------ DISTANCE ------------------
float getDistance() { // TODO change to work with LIDAR
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    return measure.RangeMilliMeter;
  }
}

// ------------------ IMU acceleration------------------
void getAcceleration(float& ax1, float& ay1, float& az1, float& ax2, float& ay2, float& az2) {
 
  ax1 = imu1.getAccelerationX();
  ay1 = imu1.getAccelerationY();
  az1 = imu1.getAccelerationZ();
  
  ax2 = imu2.getAccelerationX();
  ay2 = imu2.getAccelerationY();
  az2 = imu2.getAccelerationZ();
}
  
float gx;
// ------------------ MAIN LOOP ------------------
void loop() {
  // Read sensors
  for (int i = 0; i < NUM_SOURCES; i++) {
    strainValues[i] = analogRead(LoadcellPins[i])-calibrationFactors[i];
  }

  float distance = getDistance(); // getting distance in mm from LIDAR
  getAcceleration(ax1, ay1, az1, ax2, ay2, az2); // getting acceleration from both IMUs
  // Send data using raw print for speed


 Serial.print("L: ");
 BTSerial.print("L: ");
  for (int i = 0; i < 3; i++) {
    //Serial.print(strainValues[i]);
    //Serial.print(" ");
    BTSerial.print(strainValues[i]);
    BTSerial.print(" ");
  }
  
  Serial.print("R: ");
  BTSerial.print("R: ");
  for (int i = 3; i < 6; i++) {
    //Serial.print(strainValues[i]);
    //Serial.print(" ");
    BTSerial.print(strainValues[i]);
    BTSerial.print(" ");
  }
/*
  Serial.print("D: ");
  Serial.print(distance, 1);

  Serial.print(" Acc 1:");
  Serial.print(" x: ");
  Serial.print(ax1, 1);

  Serial.print(" y: ");
  Serial.print(ay1, 1);

  Serial.print(" z: ");
  Serial.print(az1, 1);

  Serial.print(" Acc 2:");
  Serial.print(" x: ");
  Serial.print(ax2, 1);

  Serial.print(" y: ");
  Serial.print(ay2, 1);

  Serial.print(" z: ");
  Serial.println(az2, 1);

*/
  // BTSerial not needed since BT mmodule connected to hardware serial
  // BTSerial follows same structure
  BTSerial.print("D: ");
  BTSerial.print(distance, 1);

  BTSerial.print(" Acc 1:");
  BTSerial.print(" x: ");
  BTSerial.print(ax1, 1);

  BTSerial.print(" y: ");
  BTSerial.print(ay1, 1);

  BTSerial.print(" z: ");
  BTSerial.print(az1, 1);

  BTSerial.print(" Acc 2:");
  BTSerial.print(" x: ");
  BTSerial.print(ax2, 1);

  BTSerial.print(" y: ");
  BTSerial.print(ay2, 1);

  BTSerial.print(" z: ");
  BTSerial.println(az2, 1);
}
