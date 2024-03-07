#include <WiFi.h>

// Pins for the ultrasonic sensor
const int triggerPin = 5; // Example trigger pin
const int echoPin = 18;   // Example echo pin
long duration;
int distance;

void setup() {
  Serial.begin(115200);
  // Initialize ultrasonic sensor pins
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  unsigned long startMillis = millis(); // Start of measurement period
  bool shouldSleep = false;
  
  while (millis() - startMillis < 30000) { // Check for 30 seconds
    // Measure distance
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    
    if (distance > 50) { // If distance is greater than 50 cm
      shouldSleep = true;
    } else {
      shouldSleep = false;
      break; // Exit loop if distance is less than or equal to 50 cm
    }
    
    delay(1000); // Delay between measurements
  }
  
  if (shouldSleep) {
    Serial.println("Entering deep sleep for 30 seconds");
    esp_sleep_enable_timer_wakeup(30 * 1000000); // Time in microseconds
    esp_deep_sleep_start();
  }
}