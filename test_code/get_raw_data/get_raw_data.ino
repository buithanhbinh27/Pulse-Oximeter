#include <Wire.h>
#include "MAX30105.h"

MAX30105 particleSensor;

#define BUFFER_SIZE 100
uint16_t irBuffer[BUFFER_SIZE]; // Buffer lưu dữ liệu IR
uint16_t redBuffer[BUFFER_SIZE]; // Buffer lưu dữ liệu Red
long startTime;
void setup()
{
  Serial.begin(115200);
  Wire.begin();

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }

  // Cấu hình cảm biến
  byte ledBrightness = 63; // Độ sáng đèn LED (0-255)
  byte sampleAverage = 4; // Số mẫu trung bình (1, 2, 4, 8, 16, 32)
  byte ledMode = 2; // Chế độ LED (RED+IR)
  int sampleRate = 100; // Tốc độ lấy mẫu (50, 100, 167, 200, 400, 600, 800, 1000)
  int pulseWidth = 411; // Độ rộng xung (69, 118, 215, 411)
  int adcRange = 8192; // Phạm vi ADC (2048, 4096, 8192, 16384)

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop()
{ 
    for (int i = 0; i < BUFFER_SIZE; i++) {
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
    }

    for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(irBuffer[i]);
    Serial.print(",");
    }
  
    for (int i = 0; i < BUFFER_SIZE; i++) {
      Serial.print(redBuffer[i]);
      Serial.print(",");
    }
    Serial.println();
}

