#include <SoftwareSerial.h>

//foot 1
int analogPin1 = A1; 
int analogPin2 = A2; 
int analogPin3 = A3; 

int val1 = 0;  // variable to store the value read
int val2 = 0;  // variable to store the value read
int val3 = 0;  // variable to store the value read

//foot 2
int analogPin4 = A4;
int analogPin5 = A5;
int analogPin6 = A6;

int val4 = 0;  // variable to store the value read
int val5 = 0;  // variable to store the value read
int val6 = 0;  // variable to store the value read


// Var for first sensor reading
int zero1 = 0;
int zero2 = 0;
int zero3 = 0;

int zero4 = 0;
int zero5 = 0;
int zero6 = 0;

// Ultrasonic
const int trigPin = 2;  
const int echoPin = 3;
float dur, dist;

SoftwareSerial BTSerial(2, 3); // RX, TX (Bluetooth module)
  
  void setup() {
    Serial.begin(9600);           //  setup serial
    BTSerial.begin(9600);   // Bluetooth module baud rate

    // read first sensor input
    zero1 = analogRead(analogPin1);
    zero2 = analogRead(analogPin2);  
    zero3 = analogRead(analogPin3);  

    zero4 = analogRead(analogPin4);  
    zero5 = analogRead(analogPin5);  
    zero6 = analogRead(analogPin6);  
    
    pinMode(trigPin, OUTPUT);  
    pinMode(echoPin, INPUT);  
    Serial.begin(9600);  

  }
  
  void loop() {
    val1 = (analogRead(analogPin1)-zero1) *4;  // read the input pin
    val2 = (analogRead(analogPin2)-zero2)*4;  // read the input pin
    val3 = (analogRead(analogPin3)-zero3) * 4;  // read the input pin

    val4 = analogRead(analogPin4)-zero4;  // read the input pin
    val5 = analogRead(analogPin5)-zero5;  // read the input pin
    val6 = analogRead(analogPin6)-zero6;  // read the input pin
    
    //Read distance
    digitalWrite(trigPin, LOW);  
    delayMicroseconds(2);  
    digitalWrite(trigPin, HIGH);  
    delayMicroseconds(10);  
    digitalWrite(trigPin, LOW); 
    dur = pulseIn(echoPin, HIGH); //measure echo duration
    dist = (dur*.0343)/2; //calc distance

    String data ="Foot_1:\tRight" + String(val1) + " \tHeel" + String(val2) +"\tLeft" + String(val3) ;//+ "\nFoot_2:\tLoadcell_1 " + String(val4) + "\tLoadcell_2 " + String(val5) +"\tLoadcell_3 " + String(val6) + "\nDistance\t" + String(dist);
    Serial.println(data);          // debug value
    
    //"Distance" + String(dist);
    BTSerial.println(data);    // Send data via Bluetooth
    delay(50);
    
  }



  
