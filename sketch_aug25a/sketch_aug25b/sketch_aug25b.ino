#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_VL53L0X.h>



// Define the pin number for the LED
const int ledPin = 13;

// Create an instance of the SensorSystem class
SensorSystem sensorSystem;

void setup()
{
    // Initialize serial communication
    Serial.begin(9600);

    // Initialize the LED pin as an output
    pinMode(ledPin, OUTPUT);

    // Initialize the sensor system
    sensorSystem.initialize();
}

void loop()
{
    // Update sensor data at 60 times per second
    sensorSystem.update();

    // Get calibrated MPU6050 data
    int16_t ax, ay, az, gx, gy, gz;
    sensorSystem.getCalibratedData(ax, ay, az, gx, gy, gz);

    // Get filtered distance
    uint16_t distance = sensorSystem.readDistance();

    // Calculate pitch and roll
    float pitch, roll;
    sensorSystem.calculatePitchRoll(pitch, roll);

    // print pitch, roll;
    Serial.print("Pitch: ");
    Serial.println(pitch);
    Serial.print("Roll: ");
    Serial.println(roll);

    // Check if pitch or roll exceeds 30 degrees and print a message
    if (sensorSystem.isExceedingThreshold())
    {
        Serial.println("Pitch or Roll exceeds 10 degrees!");
    }

    // Check if distance exceeds 50 cm and print a message
    if (sensorSystem.isDistanceExceedingThreshold())
    {
        Serial.println("Distance exceeds 10 cm!");
    }

    // Blink the LED based on pitch, roll, or distance thresholds
    digitalWrite(ledPin, (sensorSystem.isExceedingThreshold() || sensorSystem.isDistanceExceedingThreshold()) ? HIGH : LOW);

    delay(14);
}