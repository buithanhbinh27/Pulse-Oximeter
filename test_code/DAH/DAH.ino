#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// Khai báo đối tượng cảm biến
MAX30105 particleSensor;

// Định nghĩa các tham số
#define BUFFER_SIZE 100

// Khai báo các biến toàn cục
uint16_t irBuffer[BUFFER_SIZE]; // Buffer lưu dữ liệu IR
uint16_t redBuffer[BUFFER_SIZE]; // Buffer lưu dữ liệu Red
int32_t spo2; // Giá trị SpO2
int8_t spo2Valid; // Trạng thái hợp lệ của giá trị SpO2
int32_t heartRate; // Giá trị nhịp tim
int8_t heartRateValid; // Trạng thái hợp lệ của giá trị nhịp tim

void setup() {
  // Khởi tạo giao tiếp Serial
  Serial.begin(115200);

  // Khởi tạo cảm biến
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


void loop() {
  // Đọc dữ liệu từ cảm biến
  Serial.println("Bat dau doc: ");

  for (int i = 0; i < BUFFER_SIZE; i++) {
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
  }

  Serial.println("Buffer full, starting calculation...");

  // Gọi hàm tính toán nhịp tim và SpO2
  maxim_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_SIZE, redBuffer, &spo2, &spo2Valid, &heartRate, &heartRateValid);
  Serial.println("Calculate completed!");
  // Hiển thị kết quả
  /*
  Serial.print("Heart Rate: ");
  if (heartRateValid)
    Serial.println(heartRate);
  else
    Serial.println("Invalid");

  Serial.print("SpO2: ");
  if (spo2Valid)
    Serial.println(spo2);
  else
    Serial.println("Invalid");
  */
  // Đợi một lúc trước khi đọc dữ liệu lần tiếp theo
  delay(1000);}
