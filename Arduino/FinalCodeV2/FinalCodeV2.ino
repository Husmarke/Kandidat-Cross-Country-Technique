
// --------------------------------------------------LIBRARIES-------------------------------------------------------------
#include <SoftwareSerial.h>

// ------------------------------------------------DEFINING PINS-----------------------------------------------------------

#define NUM_SOURCES 6                                            // Number of strain guage pins
const int sensorPins[NUM_SOURCES] = { A1, A0, A2, A6, A4, A5 };  // Strain gauge pin numbers in order INSIDE/OUTSIDE/HEEL (LEFT FOOT) INSIDE/OUTSIDE/HEEL (RIGHT FOOT)

#define TRIG_PIN 4  // Ultrasound sensor pins
#define ECHO_PIN 5

#define delayMS 50  // defined delay between measurments

SoftwareSerial BTSerial(6, 7);  // RX, TX (Bluetooth module)

// ------------------------------------------------DEFINING VALUES----------------------------------------------------------
int strainValues[6] = { 0 };  // variables to store the read values
int counter = 0;

// ------------------------------------------DEFINING MODES FOR THE PROGRAM-------------------------------------------------
// 🔹 Declare the enum and variable globally so it's accessible in `loop()`
enum Mode { CALIBRATION, OPERATION };
Mode currentMode = OPERATION;  // Default mode

// ---------------------------------------------VARIABLES FOR CALIBRATION---------------------------------------------------
float sumReadings[NUM_SOURCES] = { 0 };       // Sum of all readings per source
int numReadings[NUM_SOURCES] = { 0 };           // Number of readings per source
float calibrationFactors[NUM_SOURCES] = { 0 };  // Final calibration factors

// ---------------------------------------------------------SETUP-----------------------------------------------------------
void setup() {
  Serial.begin(9600);    //  setup serial
  BTSerial.begin(9600);  // Bluetooth module baud rate

  // -----------------------------------------------ULTRASOUND SETUP-------------------------------------------------------
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("\nStarting program in Calibration Mode");
  Serial.println("Calibrating...");
}

// ---------------------------------------------COLLECT CALIBRATION VALUES----------------------------------------------------
void calibrationMode() {
  for (int i = 0; i < NUM_SOURCES; i++) {
    sumReadings[i] += analogRead(sensorPins[i]);  // read the input pin
    numReadings[i]++;
  }
  delay(delayMS);
}
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

void operationMode() {
  // --------------------------------------------------LOADCELLS-----------------------------------------------------------
  for (int i = 0; i < NUM_SOURCES; i++) {
    strainValues[i] = (analogRead(sensorPins[i])) - calibrationFactors[i];  // read the input pin
  }

  // --------------------------------------------------ULTRASOUND-----------------------------------------------------------
  // send ultrasound pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);  // get pulse time
  float distance = duration * 0.034 / 2;    // Convert to cm

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

  }
