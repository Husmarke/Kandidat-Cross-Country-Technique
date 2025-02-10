#include <SoftwareSerial.h>

#define TRIG_PIN 9
#define ECHO_PIN 10

SoftwareSerial BTSerial(2, 3); // RX, TX (Bluetooth module)

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;  // Convert to cm

  String data = "Distance: " + String(distance) + " cm";
  Serial.println(data);
  BTSerial.println(data);

  delay(100);  // Send data every .1 second
}
