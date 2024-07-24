#include <Wire.h>
#include "MAX30105.h"
#include "arduinoFFT.h"

MAX30105 particleSensor;
// Create an ExcelWriter object

#define SAMPLES 64
#define SAMPLING_FREQUENCY 64 // 64Hz sampling frequency

double redBuffer[SAMPLES];
double irBuffer[SAMPLES];
uint32_t lastSampleTime = 0;
bool shouldRun = false; // Flag to control data collection

void setup() {
  Serial.begin(115200);
  Wire.begin();


  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }
	byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 400; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 8192; //Options: 2048, 4096, 8192, 16384
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
}

void loop() {
  if (shouldRun) {
    collectData();
    exportDataToSerial();
  }

  // Check for serial input
  if (Serial.available()) {
    char inputChar = Serial.read();
    if (inputChar == 'a') {
      shouldRun = true; // Start data collection
    } else if (inputChar == 'b') {
      shouldRun = false; // Stop data collection
    }
  }


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

void exportDataToSerial() {
  for (int i = 0; i < SAMPLES; i++) {
    Serial.print(irBuffer[i]);
    Serial.print(",");
  }
  for (int i = 0; i < SAMPLES; i++) {
    Serial.print(redBuffer[i]);
    Serial.print(",");
  }
  Serial.println();  // End of line for one set of data

}
