#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// Khởi tạo cảm biến MAX30102
MAX30105 particleSensor;

// Biến để lưu dữ liệu từ cảm biến
uint32_t irBuffer[100]; // Dữ liệu hồng ngoại từ cảm biến
uint32_t redBuffer[100]; // Dữ liệu đỏ từ cảm biến
int32_t bufferLength; // Độ dài buffer
int32_t spo2; // Biến để lưu kết quả đo SpO2
int8_t validSPO2; // Biến để kiểm tra tính hợp lệ của SpO2
int32_t heartRate; // Biến để lưu kết quả đo nhịp tim
int8_t validHeartRate; // Biến để kiểm tra tính hợp lệ của nhịp tim

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Khởi tạo cảm biến MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Cảm biến MAX30102 không được tìm thấy. Hãy kiểm tra kết nối.");
    while (1);
  }

    // Setup to sense a nice looking saw tooth on the plotter
    byte ledBrightness = 64; // Options: 0=Off to 255=50mA
    byte sampleAverage = 4; // Options: 1, 2, 4, 8, 16
    byte ledMode = 2; // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 100; // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411; // Options: 69, 118, 215, 411
    int adcRange = 8192; // Options: 2048, 4096, 8192, 16384
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop() {
  // Đọc dữ liệu từ cảm biến
  bufferLength = 100; // Số lượng mẫu cần lấy
  for (int i = 0; i < bufferLength; i++) {
    while (!particleSensor.available()) ; // Chờ cho cảm biến có dữ liệu mới
    redBuffer[i] = particleSensor.getRed(); // Đọc giá trị đỏ
    irBuffer[i] = particleSensor.getIR(); // Đọc giá trị hồng ngoại
    particleSensor.nextSample(); // Di chuyển đến mẫu tiếp theo
  }

  // Gọi hàm tính toán nhịp tim và SpO2
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Hiển thị kết quả
  if (validHeartRate) {
    Serial.print("Nhịp tim: ");
    Serial.println(heartRate);
  } else {
    Serial.println("Không thể đo nhịp tim");
  }

  if (validSPO2) {
    Serial.print("SpO2: ");
    Serial.println(spo2);
  } else {
    Serial.println("Không thể đo SpO2");
  }

  delay(1000); // Đợi 1 giây trước khi lấy mẫu tiếp theo
}
