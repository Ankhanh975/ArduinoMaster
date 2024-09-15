#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// Create an object for the VL53L0X sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  // Initialize Serial communication at 115200 baud rate
  Serial.begin(115200);
  Serial.println("VL53L0X test");

  // Initialize I2C communication
  Wire.begin();

  // Initialize the VL53L0X sensor
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
}

void loop() {
  // Create a variable to hold the sensor data
  VL53L0X_RangingMeasurementData_t measure;

  // Read the distance from the sensor
  lox.rangingTest(&measure, false);

  // Print the distance to the Serial monitor
  if (measure.RangeStatus != 4) {  // if not out of range
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println("Out of range");
  }

  // Add a small delay for readability
  delay(100);
}