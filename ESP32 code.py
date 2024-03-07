#include <Arduino.h>

#define ANALOG_PIN D1

// Function declarations
// int myFunction(int, int);

void setup() {
  Serial.begin(115200); // Start the serial communication
  pinMode(ANALOG_PIN, INPUT);
}

void loop() {
  int analogValue = analogRead(ANALOG_PIN); // Read the analog value
  float voltage = analogValue * (5.0 / 4095.0);
  
  Serial.print("ADC Value: ");
  Serial.print(analogValue);
  Serial.print(" Voltage: ");
  Serial.println(voltage); // 'println' adds a new line at the end
  
  delay(1000); // Wait for a second
}