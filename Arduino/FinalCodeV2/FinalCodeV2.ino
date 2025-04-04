#include <SoftwareSerial.h>

// ------------------ CONFIG ------------------
#define NUM_SOURCES 6
const int sensorPins[NUM_SOURCES] = { A1, A0, A2, A6, A4, A5 };
const float loadcellFactors[NUM_SOURCES] = {2.835, 3.633, 2.056, 2.178, 1.639, 1.0};

#define TRIG_PIN 4
#define ECHO_PIN 5
#define delayMS 10  // Lower delay for higher frequency

// Use SoftwareSerial only if you must – prefer HardwareSerial
SoftwareSerial BTSerial(6, 7);  // RX, TX

// ------------------ STATE VARIABLES ------------------
int strainValues[NUM_SOURCES];
float calibrationFactors[NUM_SOURCES] = {0};

// ---------------------- SETUP ------------------------
void setup() {
  Serial.begin(115200);    // Faster baud rate
  BTSerial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  calibrateSensors();  // Only run once
}

// ------------------ CALIBRATION --------------------
void calibrateSensors() {
  const int samples = 200;
  float sums[NUM_SOURCES] = {0};

  for (int j = 0; j < samples; j++) {
    for (int i = 0; i < NUM_SOURCES; i++) {
      sums[i] += analogRead(sensorPins[i]) * loadcellFactors[i];
    }
    delay(5);  // Small delay to let ADC settle
  }

  for (int i = 0; i < NUM_SOURCES; i++) {
    calibrationFactors[i] = sums[i] / samples;
  }
}

// ------------------ DISTANCE ------------------
float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // timeout added
  float dist = duration * 0.034 / 2;
  if (dist > 70) dist = 70;
  return dist;
}

// ------------------ MAIN LOOP ------------------
void loop() {
  // Read sensors
  for (int i = 0; i < NUM_SOURCES; i++) {
    strainValues[i] = (analogRead(sensorPins[i]) * loadcellFactors[i]) - calibrationFactors[i];
  }

  float distance = getDistance();

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
  BTSerial.print("L:");
  for (int i = 0; i < 3; i++) {
    BTSerial.print(strainValues[i]);
    BTSerial.print(",");
  }
  BTSerial.print("R:");
  for (int i = 3; i < 6; i++) {
    BTSerial.print(strainValues[i]);
    BTSerial.print(",");
  }
  BTSerial.print("D:");
  BTSerial.println(distance, 1);

  delay(delayMS);  // Tweak or remove to increase rate
}
