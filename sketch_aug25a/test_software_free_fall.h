#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }
}

void loop() {
  // Read accelerometer data
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert to g-forces
  float ax_g = ax / 16384.0;  // Adjust according to your sensitivity scale
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;

  // Calculate total acceleration magnitude
  float accel_magnitude = sqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);

  // Freefall threshold: close to 1 g (Earth's gravity), can be adjusted
  float freefall_threshold = 0.2; // Adjust based on sensitivity required

  if (accel_magnitude < freefall_threshold) {
    Serial.println("Freefall detected!");
  } else {
    Serial.println("No freefall.");
  }

  delay(100); // Adjust the delay as needed
}