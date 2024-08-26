#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>
#include <math.h>

MPU6050 mpu;
const int numReadings = 5;
float accelXReadings[numReadings] = {0};
float accelYReadings[numReadings] = {0};
float accelZReadings[numReadings] = {0};
int readIndex = 0;

float filteredAccelX = 0;
float filteredAccelY = 0;
float filteredAccelZ = 0;
const float alpha = 0.1; // Smoothing factor for the low-pass filter

// Calibration offsets
const float accelXOffset = 0.0;
const float accelYOffset = 0.0;
const float accelZOffset = 0.0;

void setup() {
  Serial.begin(115200); // Start the serial communication
  Wire.begin();         // Initialize I2C communication
  mpu.initialize();     // Initialize the MPU6050 sensor

  // Check if the MPU6050 is connected
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }
}

void loop() {
  int16_t ax, ay, az;

  // Read raw accelerometer data
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert raw values to meaningful units and apply calibration offsets
  float accelX = (ax / 16384.0) - accelXOffset; // assuming +/- 2g range
  float accelY = (ay / 16384.0) - accelYOffset;
  float accelZ = (az / 16384.0) - accelZOffset;

  // Apply low-pass filter
  filteredAccelX = alpha * accelX + (1 - alpha) * filteredAccelX;
  filteredAccelY = alpha * accelY + (1 - alpha) * filteredAccelY;
  filteredAccelZ = alpha * accelZ + (1 - alpha) * filteredAccelZ;

  // Store the filtered readings in the arrays
  accelXReadings[readIndex] = filteredAccelX;
  accelYReadings[readIndex] = filteredAccelY;
  accelZReadings[readIndex] = filteredAccelZ;

  // Calculate the average of the last 5 filtered readings
  float avgAccelX = 0, avgAccelY = 0, avgAccelZ = 0;
  for (int i = 0; i < numReadings; i++) {
    avgAccelX += accelXReadings[i];
    avgAccelY += accelYReadings[i];
    avgAccelZ += accelZReadings[i];
  }
  avgAccelX /= numReadings;
  avgAccelY /= numReadings;
  avgAccelZ /= numReadings;

  // Calculate pitch and roll
  float pitch = atan2(avgAccelY, sqrt(avgAccelX * avgAccelX + avgAccelZ * avgAccelZ)) * 180.0 / PI;
  float roll = atan2(-avgAccelX, avgAccelZ) * 180.0 / PI;

  // Print the pitch and roll to the Serial Plotter
  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print(" Roll: ");
  Serial.println(roll);

  // Update the index
  readIndex = (readIndex + 1) % numReadings;

  delay(50);
}