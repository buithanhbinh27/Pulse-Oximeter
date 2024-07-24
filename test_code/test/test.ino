#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

uint32_t irBuffer[100]; // IR LED sensor data
uint32_t redBuffer[100];  // Red LED sensor data
int32_t bufferLength; // Number of samples in the buffer
int32_t spo2; // Calculated SpO2 value
int8_t validSPO2; // Indicator to show if the calculated SpO2 is valid
int32_t heartRate; // Calculated heart rate value
int8_t validHeartRate; // Indicator to show if the calculated heart rate is valid

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // Initialize I2C communication
  Wire.begin(); // Customize the I2C pins and speed as needed

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  Serial.println("MAX30105 initialized.");

  byte ledBrightness = 60; // Options: 0=Off to 255=50mA
  byte sampleAverage = 1; // Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 400; // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; // Options: 69, 118, 215, 411
  int adcRange = 8192; // Options: 2048, 4096, 8192, 16384
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings

  Serial.println("Setup complete.");
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

// const int ledPin = 2; // Chân GPIO2 trên ESP32 thường được sử dụng cho LED

// void setup() {
//   // Thiết lập chân LED là output
//   pinMode(ledPin, OUTPUT);

//   // Khởi động giao tiếp Serial với baud rate 115200
//   Serial.begin(115200);
// }

// void loop() {
//   // Bật LED
//   digitalWrite(ledPin, HIGH);
//   Serial.println("LED is ON");
//   delay(1000); // Đợi 1 giây

//   // Tắt LED
//   digitalWrite(ledPin, LOW);
//   Serial.println("LED is OFF");
//   delay(1000); // Đợi 1 giây
// }



