#include <Wire.h>
#include "MAX30105.h" 
#include "spo2_algorithm.h" 

MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

uint16_t irBuffer[100]; // IR LED sensor data
uint16_t redBuffer[100];  // Red LED sensor data
int32_t bufferLength; // Number of samples in the buffer
int32_t spo2; // Calculated SpO2 value
int8_t validSPO2; // Indicator to show if the calculated SpO2 is valid
int32_t heartRate; // Calculated heart rate value
int8_t validHeartRate; // Indicator to show if the calculated heart rate is valid

void setup() {
  Serial.begin(115200);
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }
  
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); // Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(MAX_BRIGHTNESS); // Max LED brightness
  particleSensor.setPulseAmplitudeIR(MAX_BRIGHTNESS);
}

void loop() {
  // Read 100 samples from the sensor
  bufferLength = 100; // Buffer length
  for (int i = 0; i < bufferLength; i++) {
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); 
  }

  // Calculate heart rate and SpO2
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Print results
  Serial.print("Heart Rate: ");
  
  if (validHeartRate)
    Serial.print(heartRate);
  else
    Serial.print("Invalid");

  Serial.print(" bpm, SpO2: ");
  if (validSPO2)
    Serial.print(spo2);
  else
    Serial.print("Invalid");

  Serial.println(" %");

  delay(1000); // Wait for a second before the next reading
}
