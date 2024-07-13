#include <Wire.h>
#include "MAX30105.h"
#include "arduinoFFT.h"

MAX30105 particleSensor;

#define SAMPLES 64
#define SAMPLING_FREQUENCY 64 // 64Hz sampling frequency

ArduinoFFT<double> FFT = ArduinoFFT<double>();

double redBuffer[SAMPLES];
double irBuffer[SAMPLES];
double vReal[SAMPLES];
double vImag[SAMPLES];
uint16_t bufferLength = SAMPLES;
uint32_t lastSampleTime = 0;

double thresholdIR = 0.0;
double thresholdRed = 0.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }

  // Setup sensor
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x24); // 0x24 = 36
  particleSensor.setPulseAmplitudeIR(0x24);  // 0x24 = 36
}

void loop() {
  collectData();
  applyFilters();
  calculateFFT();
  double spo2 = calculateSpO2();
  Serial.print("Estimated SpO2: ");
  Serial.print(spo2);
  Serial.println(" %");
  delay(1000);
}

void collectData() {
  uint16_t numSamples = 0;
  while (numSamples < SAMPLES) {
    if (millis() - lastSampleTime > (1000 / SAMPLING_FREQUENCY)) {
      lastSampleTime = millis();
      redBuffer[numSamples] = particleSensor.getRed();
      irBuffer[numSamples] = particleSensor.getIR();
      numSamples++;
    }
  }

  // Cập nhật giá trị ngưỡng
  thresholdIR = calculateThreshold(irBuffer, SAMPLES);
  thresholdRed = calculateThreshold(redBuffer, SAMPLES);
}

void applyFilters() {
  // Lọc thông thấp - Trung bình trượt
  for (int i = 1; i < SAMPLES; i++) {
    irBuffer[i] = (irBuffer[i - 1] + irBuffer[i]) / 2.0;
    redBuffer[i] = (redBuffer[i - 1] + redBuffer[i]) / 2.0;
  }

  // Lọc thông cao - Bộ lọc thông qua giá trị ngưỡng
  for (int i = 0; i < SAMPLES; i++) {
    if (irBuffer[i] < thresholdIR) irBuffer[i] = 0;
    if (redBuffer[i] < thresholdRed) redBuffer[i] = 0;
  }
}

double calculateThreshold(double *buffer, int length) {
  double sum = 0;
  for (int i = 0; i < length; i++) {
    sum += buffer[i];
  }
  return sum / length * 0.5; // Ngưỡng được đặt bằng 50% giá trị trung bình
}

void calculateFFT() {
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = irBuffer[i];
    vImag[i] = 0;
  }

  FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, SAMPLES);

  double peakFrequency = FFT.majorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  double heartRate = peakFrequency * 60;

  Serial.print("Heart Rate: ");
  Serial.print(heartRate);
  Serial.println(" BPM");
}

double calculateSpO2() {
  double irMean = mean(irBuffer, SAMPLES);
  double redMean = mean(redBuffer, SAMPLES);

  double acIR = acComponent(irBuffer, irMean);
  double acRed = acComponent(redBuffer, redMean);

  double ratio = (acRed / redMean) / (acIR / irMean);
  double spo2 = 110 - 25 * ratio; // Simplified estimation formula

  return spo2;
}

double mean(double *array, int length) {
  double sum = 0;
  for (int i = 0; i < length; i++) {
    sum += array[i];
  }
  return sum / length;
}

double acComponent(double *array, double mean) {
  double maxVal = 0;
  double minVal = 1e12;
  for (int i = 0; i < SAMPLES; i++) {
    if (array[i] > maxVal) {
      maxVal = array[i];
    }
    if (array[i] < minVal) {
      minVal = array[i];
    }
  }
  return maxVal - minVal;
}
