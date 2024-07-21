/*
  MAX30105 Breakout: Output all the raw Red/IR/Green readings
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 2nd, 2016
  https://github.com/sparkfun/MAX30105_Breakout

  Outputs all Red/IR/Green values.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).
*/

#include <Wire.h>
#include "MAX30105.h"

MAX30105 particleSensor;

bool shouldRun = false; // Flag to control data collection
unsigned long startTime = 0; // Variable to store the start time
unsigned long count = 0;

void setup()
{
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

void loop()
{
  if (shouldRun) {
    // Check if 5 seconds have passed
    if (millis() - startTime < 4000) {
      Serial.print(count);
      Serial.print("  ");
      Serial.print(particleSensor.getRed());
      Serial.print("  ");
      Serial.print(particleSensor.getIR());
      Serial.print('\n');
      count++;
    } else {
      shouldRun = false; // Stop data collection after 5 seconds
      count = 0;
    }
  }

  if (Serial.available()) {
    char inputChar = Serial.read();
    if (inputChar == 'a') {
      shouldRun = true; // Start data collection
      startTime = millis(); // Record the start time
    } else if (inputChar == 'b') {
      shouldRun = false; // Stop data collection
    }
  }
}

