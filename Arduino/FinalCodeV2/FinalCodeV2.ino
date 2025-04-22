

// --------------------------------------------------LIBRARIES-------------------------------------------------------------
#include <SoftwareSerial.h>
#include <SoftwareWire.h>
#include <Wire.h>
#include "ICM20600.h"

// ------------------------------------------------DEFINING PINS-----------------------------------------------------------

#define NUM_SOURCES 6                                            // Number of strain guage pins
const int sensorPins[NUM_SOURCES] = { A1, A0, A2, A6, A4, A5 };  // Strain gauge pin numbers in order INSIDE/OUTSIDE/HEEL (LEFT FOOT) INSIDE/OUTSIDE/HEEL (RIGHT FOOT)
<<<<<<< Updated upstream
const float loadcellFactors[NUM_SOURCES] = {2.835, 3.633, 2.056, 2.178, 1.639, 1.0};
#define TRIG_PIN 4  // Ultrasound sensor pins
#define ECHO_PIN 5

#define delayMS 50  // defined delay between measurments

SoftwareSerial BTSerial(6, 7);  // RX, TX (Bluetooth module)

=======
//const float loadcellFactors[NUM_SOURCES] = {3.9325, 3.6649, 3.1180 , 3.1963, 1.7241, 0.7430}; //all sensors get k-value 10
float loadcellFactors[NUM_SOURCES] = {0, 0, 0 , 0, 0 , 0};
#define TRIG_PIN 4  // Ultrasound sensor pins
#define ECHO_PIN 5

#define delayMS 100  // defined delay between measurments

SoftwareSerial BTSerial(6, 7);  // RX, TX (Bluetooth module)
// Define Software I2C connections (SDA, SCL)
SoftwareWire wire1(A2, 22); // IMU 1: SDA = A2, SCL = D22
SoftwareWire wire2(A3, 22); // IMU 2: SDA = A3, SCL = D22
SoftwareWire wire3(A4, 22); // IMU 3: SDA = A4, SCL = D22

// Initialize IMU objects
ICM20600 imu1(&wire1);
ICM20600 imu2(&wire2);
ICM20600 imu3(&wire3);
>>>>>>> Stashed changes
// ------------------------------------------------DEFINING VALUES----------------------------------------------------------
int strainValues[6] = { 0 };  // variables to store the read values
int counter = 0;

// ------------------------------------------DEFINING MODES FOR THE PROGRAM-------------------------------------------------
// 🔹 Declare the enum and variable globally so it's accessible in `loop()`
enum Mode { CALIBRATION, OPERATION };
Mode currentMode = OPERATION;  // Default mode

// ---------------------------------------------VARIABLES FOR CALIBRATION---------------------------------------------------
float sumReadings[NUM_SOURCES] = { 0 };       // Sum of all readings per source
<<<<<<< Updated upstream
int numReadings[NUM_SOURCES] = { 0 };           // Number of readings per source
float calibrationFactors[NUM_SOURCES] = { 0 };  // Final calibration factors

// ---------------------------------------------------------SETUP-----------------------------------------------------------
=======
float linearSumReadings[NUM_SOURCES] = { 0 };       // Sum of all readings per source(used for linear test)
int numReadings[NUM_SOURCES] = { 0 };           // Number of readings per source
int linearNumReadings[NUM_SOURCES] = { 0 };
float calibrationFactors[NUM_SOURCES] = { 0 };  // Final calibration factors

// ---------------------------------------------------------SETUP-----------------------------------------------------------  
>>>>>>> Stashed changes
void setup() {
  Serial.begin(9600);    //  setup serial
  BTSerial.begin(9600);  // Bluetooth module baud rate

<<<<<<< Updated upstream
  // -----------------------------------------------ULTRASOUND SETUP-------------------------------------------------------
=======
// -----------------------------------------------ULTRASOUND SETUP-------------------------------------------------------
>>>>>>> Stashed changes
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("\nStarting program in Calibration Mode");
  Serial.println("Calibrating...");
<<<<<<< Updated upstream
}

// ---------------------------------------------COLLECT CALIBRATION VALUES----------------------------------------------------
void calibrationMode() {
  for (int i = 0; i < NUM_SOURCES; i++) {
    sumReadings[i] += analogRead(sensorPins[i])*loadcellFactors[i];  // read the input pin
    numReadings[i]++;
  }
  delay(delayMS);
}
=======

// -----------------------------------------------Gyro SETUP-------------------------------------------------------
// Start each software I2C bus
  wire1.begin();
  wire2.begin();
  wire3.begin();

// Initialize each IMU
  imu1.initialize();
  imu2.initialize();
  imu3.initialize();

  Serial.println("All 3 IMUs initialized using SoftwareWire.");
}



// ---------------------------------------------COLLECT CALIBRATION VALUES----------------------------------------------------
void calibrationMode() {
  for (int i = 0; i < NUM_SOURCES; i++) {
    sumReadings[i] += analogRead(sensorPins[i]);  // read the input pin
    numReadings[i]++;
  }
    delay(delayMS);
}
void linearCalibrationMode(){
  for (int i = 0; i < NUM_SOURCES; i++){
    linearSumReadings[i] += ((analogRead(sensorPins[i])) - calibrationFactors[i]);
    linearNumReadings[i]++;
    }
    delay(delayMS);
  }

  
>>>>>>> Stashed changes
// --------------------------------------------COMPUTE CALIBRATION VALUES----------------------------------------------------
void computeAverages() {
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

<<<<<<< Updated upstream
void operationMode() {
  // --------------------------------------------------LOADCELLS-----------------------------------------------------------
  for (int i = 0; i < NUM_SOURCES; i++) {
    strainValues[i] = (analogRead(sensorPins[i])*loadcellFactors[i]) - calibrationFactors[i];  // read the input pin
  }

  // --------------------------------------------------ULTRASOUND-----------------------------------------------------------
=======
void computeLinearAverages() {
  for (int i = 0; i < NUM_SOURCES; i++) {
    if (numReadings[i] > 0) {
      loadcellFactors[i] = linearSumReadings[i] / linearNumReadings[i];
    } else {
      Serial.print("A");
      Serial.print(i);
      Serial.println(" - No data collected.");
    }
  }
}

 // --------------------------------------------------COMPUTE IMBALANCE-----------------------------------------------------------
float leftInbalance, rightInbalance;
float getInbalance(){
  leftInbalance = strainValues[1] - strainValues[0]; //Delta between outer and inner sensor, negativ number = more force inward
  rightInbalance = strainValues[4] - strainValues[3]; //Delta between outer and inner sensor, negativ number = more force inward
  
return leftInbalance, rightInbalance;
}

  // --------------------------------------------------COMPUTE MAXFORCE-----------------------------------------------------------
  float rightMaxForce;
  float leftMaxForce;
  float getMaxForce(){
    leftMaxForce = strainValues[0] + strainValues[1] + strainValues[2];
    rightMaxForce = strainValues[3] + strainValues[4] + strainValues[5];
    
    return leftMaxForce,rightMaxForce;
    }
  

  // --------------------------------------------------COMPUTE DISTANCE-----------------------------------------------------------
float distance;
float getDistance(){
>>>>>>> Stashed changes
  // send ultrasound pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);  // get pulse time
  float distance = duration * 0.034 / 2;    // Convert to cm
<<<<<<< Updated upstream

  // --------------------------------------------------SENDING DATA------------------------------------------------------------
  String data = "LeftFoot: " + String(strainValues[0]) + " " + String(strainValues[1]) + " " + String(strainValues[2]) + " RightFoot: " + String(strainValues[3]) + " " + String(strainValues[4]) + " " + String(strainValues[5]) + " Distance: " + String(distance);

  //Serial.println(data);      // debug value
  BTSerial.println(data);  // Send data via Bluetooth
  delay(delayMS);
}

// -----------------------------------------------BT COMMANDS MODE SWITCH----------------------------------------------------
void loop() {
  if (counter < 100) {
    calibrationMode();
  } 
  else if (counter == 100){
    computeAverages();  // Compute final calibration factors when switching

    for (int i = 0; i < NUM_SOURCES; i++) {
      Serial.println("Calibration factor " + String(i) + ": " + String(calibrationFactors[i]));  // read the input pin
    }
    Serial.println("Switched to Operation Mode");
  }
  else {
      operationMode();
  }
  counter = counter + 1;

=======
  //Spike block
  if (distance > 70) {
    distance = 70;
    }
  
  return distance;
}

void operationMode() {
  // --------------------------------------------------LOADCELLS-----------------------------------------------------------
  for (int i = 0; i < NUM_SOURCES; i++) {
    strainValues[i] = ((analogRead(sensorPins[i])) - calibrationFactors[i]) / loadcellFactors[i] * 100;  // read the input pin
  }

  // --------------------------------------------------DISTANCE-----------------------------------------------------------
  distance = getDistance();
  
  // --------------------------------------------------INBALANCE-----------------------------------------------------------
  //leftInbalance, rightInbalance = getInbalance();
  //String data =  "LeftFoot Inbalance: " + String(leftInbalance) + " RightFoot Inbalance: " + String(rightInbalance);

  // --------------------------------------------------COMPUTE MAXFORCE-----------------------------------------------------------
// leftMaxForce, rightMaxForce = getMaxForce(); 
  //String data =  "LeftFoot MaxForce: " + String(leftMaxForce) + " RightFoot MaxForce: " + String(rightMaxForce) + " Distance: " + String(distance);
  
  // --------------------------------------------------SENDING DATA----------- -------------------------------------------------
  int16_t ax1 = imu1.getAccelerationX();
  int16_t ay1 = imu1.getAccelerationY();
  int16_t az1 = imu1.getAccelerationZ();

  int16_t ax2 = imu2.getAccelerationX();
  int16_t ay2 = imu2.getAccelerationY();
  int16_t az2 = imu2.getAccelerationZ();

  int16_t ax3 = imu3.getAccelerationX();
  int16_t ay3 = imu3.getAccelerationY();
  int16_t az3 = imu3.getAccelerationZ();
  
  String data = "LeftFoot: " + String(strainValues[0]) + " " + String(strainValues[1]) + " " + String(strainValues[2]) + " RightFoot: " + String(strainValues[3]) + " " + String(strainValues[4]) + " " + String(strainValues[5]) + " Distance: " + String(distance) + "Accel X: " + String(ax1) + " " + " | Y: " + String(ay1) + " " + " | Z: " + String(az1);
  
  Serial.println(data);      // debug value
  //BTSerial.println(data);  // Send data via Bluetooth
  delay(delayMS);

  
}




bool mCalibration = true;
// -----------------------------------------------BT COMMANDS MODE SWITCH----------------------------------------------------
void loop() {
  if (counter < 100 and mCalibration) {
    calibrationMode();
  } 
  
  else if (counter == 100 and mCalibration){
    computeAverages();  // Compute final calibration factors when switching
    mCalibration = false;
    counter = 0;
    for (int i = 0; i < NUM_SOURCES; i++) {
      Serial.println("Calibration factor " + String(i) + ": " + String(calibrationFactors[i]));  // read the input pin
    }
     Serial.println("Get on load cells");
  delay(10000);
    Serial.println("Starting linear calibration");
  }

  else if (counter < 100 and mCalibration){
    linearCalibrationMode();
    }
  else if (counter == 100 and mCalibration){
    computeLinearAverages();
     Serial.println("Switched to Operation Mode");
    }
  else {
      operationMode();
  }
  counter = counter + 1;
  
>>>>>>> Stashed changes
  }
