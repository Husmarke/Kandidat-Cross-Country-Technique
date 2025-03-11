#include <SoftwareSerial.h>

int analogPin1 = A1; 
int analogPin2 = A2; 
int analogPin3 = A3; 

int val1 = 0;  // variable to store the value read
int val2 = 0;  // variable to store the value read
int val3 = 0;  // variable to store the value read

int zero1 = 0;
int zero2 = 0;
int zero3 = 0;

// Ultrasonic
const int trigPin = 2;  
const int echoPin = 3;
float dur, dist;
 
SoftwareSerial BTSerial(2, 3); // RX, TX (Bluetooth module)
  
  void setup() {
    Serial.begin(9600);           //  setup serial
    BTSerial.begin(9600);   // Bluetooth module baud rate

    zero1 = analogRead(analogPin1);  // read the input pin
    zero2 = analogRead(analogPin2);  // read the input pin
    zero3 = analogRead(analogPin3);  // read the input pin

    
    pinMode(trigPin, OUTPUT);  
    pinMode(echoPin, INPUT);  
    Serial.begin(9600);  

  }

  void loop() {
    val1 = analogRead(analogPin1)-zero1;  // read the input pin
    val2 = analogRead(analogPin2)-zero2;  // read the input pin
    val3 = analogRead(analogPin3)-zero3;  // read the input pin

    //Echo
    digitalWrite(trigPin, LOW);  
    delayMicroseconds(2);  
    digitalWrite(trigPin, HIGH);  
    delayMicroseconds(10);  
    digitalWrite(trigPin, LOW); 
    dur = pulseIn(echoPin, HIGH); //measure echo duration
    dist = (dur*.0343)/2; //calc distance
    Serial.print("Distance: ");  
    Serial.println(dist); 

    String data = "Loadcell_1 " + String(val1) + " Loadcell_2 " + String(val2) +" Loadcell_3 " + String(val3) + "Distance" + String(dist);
    Serial.println(data);          // debug value

    
    BTSerial.println(data);    // Send data via Bluetooth
    delay(50);
    
  }



  
