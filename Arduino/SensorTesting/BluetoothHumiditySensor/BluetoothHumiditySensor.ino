#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 7       // DHT sensor connected to pin 7
#define DHTTYPE DHT11  // Using DHT11 sensor

SoftwareSerial BTSerial(2, 3); // RX, TX (Bluetooth module)
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);     // Serial Monitor for debugging
  BTSerial.begin(9600);   // Bluetooth module baud rate
  dht.begin();            // Initialize DHT sensor
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  String data = "Temp: " + String(temperature) + "C, Hum: " + String(humidity) + "%";
  Serial.println(data);      // Print data to Serial Monitor
  BTSerial.println(data);    // Send data via Bluetooth

  delay(2000);  // Send data every 2 seconds
}
