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

  // Setup sensor
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x24); // 0x24 = 36
  particleSensor.setPulseAmplitudeIR(0x24);  // 0x24 = 36

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
