#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

float accelXOffset = 0.0;
float accelYOffset = 0.0;
float accelZOffset = 0.0;

void calibrateAccelerometer();
void setup();
void loop();

void setup() {
  Serial.begin(115200); // Start the serial communication
  Wire.begin();         // Initialize I2C communication
  mpu.initialize();     // Initialize the MPU6050 sensor

  // Check if the MPU6050 is connected
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }

  // Calibrate the accelerometer
  calibrateAccelerometer();
}

void loop() {
  int16_t ax, ay, az;

  // Read raw accelerometer data
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert raw values to meaningful units and apply calibration offsets
  float accelX = (ax / 16384.0) - accelXOffset; // assuming +/- 2g range
  float accelY = (ay / 16384.0) - accelYOffset;
  float accelZ = (az / 16384.0) - accelZOffset;

  // Print the calibrated accelerometer values
  Serial.print("Calibrated AccelX: ");
  Serial.print(accelX);
  Serial.print(" Calibrated AccelY: ");
  Serial.print(accelY);
  Serial.print(" Calibrated AccelZ: ");
  Serial.println(accelZ);

  delay(500);
}

void calibrateAccelerometer() {
  const int numSamples = 1000;
  long sumX = 0, sumY = 0, sumZ = 0;

  for (int i = 0; i < numSamples; i++) {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    sumX += ax;
    sumY += ay;
    sumZ += az;

    delay(3); // Small delay between samples
  }

  // Calculate the average raw readings
  float avgX = sumX / (float)numSamples;
  float avgY = sumY / (float)numSamples;
  float avgZ = sumZ / (float)numSamples;

  // Calculate offsets
  accelXOffset = avgX / 16384.0; // assuming +/- 2g range
  accelYOffset = avgY / 16384.0;
  accelZOffset = (avgZ / 16384.0) - 1.0; // Z should be 1g when level

  Serial.println("Calibration complete");
  Serial.print("AccelX Offset: ");
  Serial.println(accelXOffset);
  Serial.print("AccelY Offset: ");
  Serial.println(accelYOffset);
  Serial.print("AccelZ Offset: ");
  Serial.println(accelZOffset);
}