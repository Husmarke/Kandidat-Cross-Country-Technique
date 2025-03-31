/*
 *  sketch: SerialPassThrough_SoftwareSerial_Basic
 *  martyncurrey.com
 *   
 *  Use software serial to talk to serial/UART connected device
 *  What ever is entered in the serial monitor is sent to the connected device
 *  Anything received from the connected device is copied to the serial monitor
 * 
 *  Pins
 *  BT VCC to Arduino 5V out. 
 *  BT GND to GND
 *  Arduino D2 (Arduino RX) goes to module TX
 *  Arduino D3 (Arduino TX) goes to module RX. May need a voltage divider.
 * 
 *  Assumes a 5V Arduino is being used
 *  If the connected device is 3.3v add a voltage divider (5v to 3.3v) between Arduino TX and device RX
 *  Arduino RX to device TX does not need a voltage divider. The Arduino will see 3.3v as high
 * 
 */

#include <SoftwareSerial.h>
SoftwareSerial softSerial(7, 6); // RX, TX
 
char c=' ';

 
void setup() 
{
    Serial.begin(9600,SERIAL_8N1 );
   Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    softSerial.begin(9600);
    Serial.println("softSerial started at 9600");
    Serial.println("Ready");
}

 
void loop()
{
    // Read from the Serial Monitor and send to the UART module
    if (Serial.available())
    {
        c = Serial.read();
         softSerial.write(c);
    }

    // Read from the UART module and send to the Serial Monitor
    if (softSerial.available())
    {
        c = softSerial.read();
        Serial.write(c); 
    }
    
} // void loop()