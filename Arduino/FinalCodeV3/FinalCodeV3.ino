#include <SoftwareSerial.h>
#include <SoftwareWire.h>
#include <Wire.h>
#include "ICM20600.h"
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


//IMU
float ax, ay, az;

#define delayMS 5  // Lower delay for higher frequency

// Use SoftwareSerial only if you must – prefer HardwareSerial
//SoftwareSerial BTSerial(6, 7);  // RX, TX

// Define Software I2C connections (SDA, SCL)(IMU)
SoftwareWire wire1(A2, 21); // IMU 1: SDA = A2, SCL = D21
SoftwareWire wire2(A3, 21); // IMU 2: SDA = A3, SCL = D21
SoftwareWire wire3(A4, 21); // IMU 3: SDA = A4, SCL = D21

// Initialize IMU instances on each software I2C bus(IMU)
ICM20600 imu1(&wire1);
ICM20600 imu2(&wire2);
ICM20600 imu3(&wire3);


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
  wire1.begin();
  wire2.begin();
  wire3.begin();

  // Initialize each IMU
  imu1.initialize();
  imu2.initialize();
  imu3.initialize();
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
  }
}

// ------------------ IMU acceleration------------------
//void getAcceleration(){
//  float acc[9] = {imu1.getAccelerationX(), imu1.getAccelerationY(), imu1.getAccelerationZ(), imu2.getAccelerationX(), imu2.getAccelerationY(), imu2.getAccelerationZ(), imu3.getAccelerationX(), imu3.getAccelerationY() , imu3.getAccelerationZ()};
//  return acc;
//  }
void getAcceleration(float& ax, float& ay, float& az) {
  
  ax = imu1.getAccelerationX();
  ay = imu1.getAccelerationY();
  az = imu1.getAccelerationZ();
}


// ------------------ MAIN LOOP ------------------
void loop() {
  // Read sensors
  for (int i = 0; i < NUM_SOURCES; i++) {
    strainValues[i] = analogRead(LoadcellPins[i]);
  }

  float distance = getDistance(); // getting distance in mm from LIDAR
  getAcceleration(ax, ay, az);
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
  Serial.print(distance, 1);

  Serial.print("Acc 1x:");
  Serial.print(ax, 1);

  Serial.print("Acc 1y:");
  Serial.print(ay, 1);

  Serial.print("Acc 1z:");
  Serial.println(az, 1);


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
