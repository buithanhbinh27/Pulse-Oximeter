#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// Khai báo đối tượng cảm biến
MAX30105 particleSensor;

// Định nghĩa các tham số
#define BUFFER_LENGTH 100

uint32_t irBuffer[BUFFER_LENGTH]; 
uint32_t redBuffer[BUFFER_LENGTH];
int32_t spo2; // Calculated SpO2 value
int8_t validSPO2; // Indicator to show if the calculated SpO2 is valid
int32_t heartRate; // Calculated heart rate value
int8_t validHeartRate; // Indicator to show if the calculated heart rate is valid

//------------------------------------------------------------------------------------------
void setup() {
  // Khởi tạo giao tiếp Serial
  Serial.begin(115200);

  // Khởi tạo cảm biến
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }

  // Cấu hình cảm biến
  byte ledBrightness = 64; // Độ sáng đèn LED (0-255)
  byte sampleAverage = 4; // Số mẫu trung bình (1, 2, 4, 8, 16, 32)
  byte ledMode = 2; // Chế độ LED (RED+IR)
  int sampleRate = 100; // Tốc độ lấy mẫu (50, 100, 167, 200, 400, 600, 800, 1000)
  int pulseWidth = 411; // Độ rộng xung (69, 118, 215, 411)
  int adcRange = 8192; // Phạm vi ADC (2048, 4096, 8192, 16384)

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}


void loop() {
  // Đọc dữ liệu từ cảm biến
  Serial.println("Start measure: ");

  for (int i = 0; i < BUFFER_LENGTH; i++) {
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }
  

  maxim_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_LENGTH, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

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
