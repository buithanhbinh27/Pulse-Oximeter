#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

// Khai báo các biến cần thiết
#define RATE_SIZE 4 // Kích thước của mảng lưu trữ nhịp tim
byte rates[RATE_SIZE]; // Mảng để lưu trữ nhịp tim
byte rateSpot = 0;
long lastBeat = 0; // Để lưu thời điểm nhịp đập cuối cùng
float beatsPerMinute;
int beatAvg;

const int BUFFER_SIZE = 100; // Kích thước của buffer
int32_t irBuffer[BUFFER_SIZE]; // Buffer lưu trữ dữ liệu IR
int32_t redBuffer[BUFFER_SIZE]; // Buffer lưu trữ dữ liệu Red

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize the MAX30102 sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found. Please check wiring/power.");
    while (1);
  }

  Serial.println("Place your finger on the sensor.");
  particleSensor.setup(); // Configure sensor. Use default settings.
  particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0x0A); // Turn off Green LED

  delay(1000); // Wait for initialization
}

void loop() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    // We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
      rateSpot %= RATE_SIZE; // Wrap variable

      // Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  // SpO2 calculation
  uint32_t redValue = particleSensor.getRed();
  irValue = particleSensor.getIR(); // Sử dụng lại biến irValue đã khai báo trước đó
  int32_t spo2; // Raw SpO2 value
  int8_t validSPO2; // Indicator to show if the SPO2 calculation is valid
  int32_t heartRate; // Raw heart rate value
  int8_t validHeartRate; // Indicator to show if the heart rate calculation is valid

  // Gọi hàm tính toán nhịp tim và SpO2
  calculate_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_SIZE, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  Serial.print(", SpO2=");
  Serial.print(spo2);
  Serial.print(", ValidSpO2=");
  Serial.println(validSPO2);

  delay(1000); // Delay in milliseconds
}

// Hàm tính toán nhịp tim và SpO2
void calculate_heart_rate_and_oxygen_saturation(int32_t *irBuffer, int bufferLength, int32_t *redBuffer, int32_t *spo2, int8_t *validSPO2, int32_t *heartRate, int8_t *validHeartRate) {
  // Dummy implementation for illustration purposes
  // Replace this with the actual algorithm

  *spo2 = 100; // Dummy SpO2 value
  *validSPO2 = 1; // Valid SpO2
  *heartRate = 70; // Dummy heart rate
  *validHeartRate = 1; // Valid heart rate
}
