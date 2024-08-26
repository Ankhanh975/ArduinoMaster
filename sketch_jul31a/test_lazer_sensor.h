#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>

Servo myServo;  // Create a Servo object
VL53L0X sensor;

void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 bits per second
  Wire.begin();  // Initialize I2C communication

  sensor.init();
  sensor.setTimeout(50);  // Set a timeout for the sensor to 50 milliseconds
  sensor.startContinuous();

  myServo.attach(2);  // Attach the servo to pin 2
  pinMode(13, OUTPUT);  // Set the built-in LED pin as an output
}

void loop() {
  uint16_t distance = sensor.readRangeContinuousMillimeters();  // Read the distance in millimeters

  if (sensor.timeoutOccurred()) {
    Serial.println("Timeout! Reinitializing sensor...");
    sensor.init();  // Reinitialize the sensor
    sensor.setTimeout(50);  // Set the timeout again
    sensor.startContinuous();  // Restart continuous mode
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" mm");

    // Map the distance to a servo position (0 to 180 degrees)
    int servoPos = map(distance, 50, 1200, 0, 180);  // Adjust the range as needed
    servoPos = constrain(servoPos, 0, 180);  // Ensure the position is within bounds

    myServo.write(servoPos);  // Move the servo to the mapped position
  }

  delay(100);  // Wait for 100 milliseconds before taking another reading
}


  // Adjust the timing budget and inter-measurement period for higher resolution
  sensor.setMeasurementTimingBudget(200000);  // Set timing budget to 200 ms (200000 microseconds)
  sensor.setInterMeasurementPeriod(250);  // Set inter-measurement period to 250 ms
