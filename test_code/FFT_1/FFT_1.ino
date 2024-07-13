#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <arduinoFFT.h>

MAX30105 particleSensor;
#define SAMPLES 64
#define SAMPLING_FREQUENCY 64 // 25Hz sampling frequency

ArduinoFFT<double> FFT = ArduinoFFT<double>();

double redBuffer[SAMPLES];
double irBuffer[SAMPLES];
double vReal[SAMPLES];
double vImag[SAMPLES];
uint16_t bufferLength = SAMPLES;
uint32_t lastSampleTime = 0;

void setup() {
  Serial.begin(115200);
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x24);
  particleSensor.setPulseAmplitudeIR(0x24);
}

void loop() {
  collectData();
  calculateFFT();
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

  // Calculate SpO2 using a simple ratio of red/ir (this is a very basic method, consider using more sophisticated algorithms)
  double irMean = mean(irBuffer, SAMPLES);
  double redMean = mean(redBuffer, SAMPLES);
  double ratio = (redMean / irMean);
  double spo2 = 110 - 25 * ratio; // Simplified estimation formula

  Serial.print("Estimated SpO2: ");
  Serial.print(spo2);
  Serial.println(" %");
}

double mean(double *array, int length) {
  double sum = 0;
  for (int i = 0; i < length; i++) {
    sum += array[i];
  }
  return sum / length;
}
