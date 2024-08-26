#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_VL53L0X.h>
#include "ServoMaster.h"

class SensorSystem
{
public:
  SensorSystem()
      : filterIndex(0), lastUpdateTime(0), exceedsThreshold(false), distanceExceedsThreshold(false) {}

  void initialize()
  {
    // Initialize MPU6050
    Wire.begin();
    this->mpu.initialize();
    if (this->mpu.testConnection())
    {
      Serial.println("MPU6050 connection successful");
    }
    else
    {
      Serial.println("MPU6050 connection failed");
    }

    // Initialize VL53L0X
    if (!this->lox.begin())
    {
      Serial.println(F("Failed to boot VL53L0X"));
      while (1)
        ;
    }
    Serial.println(F("VL53L0X ready"));

    // Set the timing budget to a lower value for high-speed mode (e.g., 20 ms)
    this->lox.setMeasurementTimingBudgetMicroSeconds(20000); // 20 ms timing budget
    this->lox.startRangeContinuous(20);

    // Calibrate MPU6050
    this->calibrateMPU6050();
  }

  void update()
  {
    unsigned long currentTime = millis();
    if (currentTime - this->lastUpdateTime >= this->updateInterval)
    {
      this->lastUpdateTime = currentTime;
      this->readSensors();
      this->checkThreshold();
      this->checkDistanceThreshold();
    }
  }

  void getCalibratedData(int16_t &ax, int16_t &ay, int16_t &az, int16_t &gx, int16_t &gy, int16_t &gz)
  {
    ax = this->calibratedAx;
    ay = this->calibratedAy;
    az = this->calibratedAz;
  }

  uint16_t readDistance()
  {
    return this->filteredDistance;
  }

  void calculatePitchRoll(float &pitch, float &roll)
  {
    pitch = atan2(this->calibratedAy, sqrt(this->calibratedAx * this->calibratedAx + this->calibratedAz * this->calibratedAz)) * 180.0 / PI;
    roll = atan2(-this->calibratedAx, this->calibratedAz) * 180.0 / PI;
  }
  bool isExceedingThreshold()
  {
    return this->exceedsThreshold;
  }

  bool isDistanceExceedingThreshold()
  {
    return this->distanceExceedsThreshold;
  }

private:
  MPU6050 mpu;
  Adafruit_VL53L0X lox = Adafruit_VL53L0X();

  float baselineAx = 0, baselineAy = 0, baselineAz = 0;
  float calibratedAx, calibratedAy, calibratedAz;

  static const int filterSize = 3;
  uint16_t distanceReadings[filterSize];
  int filterIndex;
  uint16_t filteredDistance;

  unsigned long lastUpdateTime;
  static const unsigned long updateInterval = 1000 / 60; // 60 updates per second

  bool exceedsThreshold;
  bool distanceExceedsThreshold;

  void calibrateMPU6050()
  {
    int16_t ax, ay, az, gx, gy, gz;
    for (int8_t i = 0; i < 10; i++)
    {
      this->mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      this->baselineAx += ax / 16384.0;
      this->baselineAy += ay / 16384.0;
      this->baselineAz += -2 + az / 16384.0;
      delay(10); // Small delay between measurements
    }
    this->baselineAx /= 10;
    this->baselineAy /= 10;
    this->baselineAz /= 10;

    Serial.print("Baseline aX = ");
    Serial.println(this->baselineAx);
    Serial.print("Baseline aY = ");
    Serial.println(this->baselineAy);
    Serial.print("Baseline aZ = ");
    Serial.println(this->baselineAz);
  }

  void readMPU6050()
  {
    int16_t ax, ay, az, gx, gy, gz;
    this->mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    this->calibratedAx = ax / 16384.0 - this->baselineAx;
    this->calibratedAy = ay / 16384.0 - this->baselineAy;
    this->calibratedAz = az / 16384.0 - this->baselineAz;
  }

  void readSensors()
  {
    this->readMPU6050();
    uint16_t distance = this->lox.readRange();
    this->filteredDistance = this->filterDistance(distance);
  }

  uint16_t filterDistance(uint16_t newDistance)
  {
    this->distanceReadings[this->filterIndex] = newDistance;
    this->filterIndex = (this->filterIndex + 1) % this->filterSize;

    uint32_t sum = 0;
    for (int8_t i = 0; i < this->filterSize; i++)
    {
      sum += this->distanceReadings[i];
    }
    return sum / this->filterSize;
  }

  void checkThreshold()
  {
    float pitch, roll;
    this->calculatePitchRoll(pitch, roll);
    this->exceedsThreshold = (abs(pitch) > 10 || abs(roll) > 10);
  }

  void checkDistanceThreshold()
  {
    this->distanceExceedsThreshold = (this->filteredDistance > 100);
  }
};

// Define the pin number for the LED
const int ledPin = 13;

// Create an instance of the SensorSystem class
SensorSystem sensorSystem;
ServoMaster servoMaster;

long frameCount = 0;

void setup()
{
  servoMaster.init();
  Serial.begin(9600);
  Serial.println("Starting");

  servoMaster.setAngleImidiately(0, 90);
  servoMaster.setAngleImidiately(1, 90);
  servoMaster.setAngleImidiately(2, 90);
  servoMaster.setAngleImidiately(3, 90);

  delay(8);

  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Initialize the sensor system
  sensorSystem.initialize();
  sensorSystem.update();
}
String state = "running";
int level = 0;
int delayTime = 2250;

void loop()
{
  delay(delayTime / 1000);
  delayMicroseconds(delayTime % 1000);

  // Update sensor data at 60 times per second
  sensorSystem.update();
  servoMaster.update();

  frameCount += 1;
  
  // plot pitch and roll
  float pitch, roll;
  sensorSystem.calculatePitchRoll(pitch, roll);
  Serial.print("");
  Serial.print(pitch);
  Serial.print(", ");
  Serial.println(roll);

  // Check if pitch or roll exceeds 30 degrees and print a message
  if (sensorSystem.isExceedingThreshold())
  {
    // Serial.println("Pitch or Roll exceeds 10 degrees!");
  }

  // Check if distance exceeds 50 cm and print a message
  if (sensorSystem.isDistanceExceedingThreshold())
  {
    // Serial.println("Distance exceeds 10 cm!");
  }

  // Blink the LED based on pitch, roll, or distance thresholds
  digitalWrite(ledPin, (sensorSystem.isExceedingThreshold() || sensorSystem.isDistanceExceedingThreshold()) ? HIGH : LOW);

  if (Serial.available() > 0)
  {
    String input = Serial.readStringUntil('\n'); // Read input from serial monitor
    input.trim();
    if (input.startsWith("/set"))
    {
      if (input.startsWith("/set state "))
      {
        String set_state = input.substring(11);
        if (set_state == "start" || set_state == "debug" || set_state == "running" || set_state == "idle" || set_state == "jump")
        {
          state = set_state;
          delay(100);
          frameCount = 0;
        }
        Serial.print("State set to ");
        Serial.println(state);
      }
      else if (input.startsWith("/set angle "))
      {
        String set_angle = input.substring(11);
        int servoNum = set_angle.substring(0, 1).toInt();
        int angle = set_angle.substring(2).toInt();
        servoMaster.setAngle(servoNum, angle);
        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" angle set to ");
        Serial.println(angle);
      }
      else if (input.startsWith("/set servoOffset "))
      {
        String set_servoOffset = input.substring(17);
        int servoNum = set_servoOffset.substring(0, 1).toInt();
        int offset = set_servoOffset.substring(2).toInt();
        offset = servoMaster.addServoOffset(servoNum, offset);
        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" offset set to ");
        Serial.println(offset);
      }
      else if (input.startsWith("/set angleMultiplier "))
      {
        String set_angleMultiplier = input.substring(21);
        int servoNum = set_angleMultiplier.substring(0, 1).toInt();
        int multiplier = set_angleMultiplier.substring(2).toInt();
        servoMaster.setAngleMultiplier(servoNum, multiplier);
        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" multiplier set to ");
        Serial.println(multiplier);
      }
      else if (input.startsWith("/set swapServoPin "))
      {
        String set_servoPin = input.substring(14);
        int servoNum = set_servoPin.substring(0, 1).toInt();
        int pin = set_servoPin.substring(2).toInt();

        // Swap the servo pin
        int temp = servoMaster.servoPins[servoNum];
        servoMaster.servoPins[servoNum] = pin;
        pin = temp;

        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" swapped with servo ");
        Serial.println(pin);
      }
      else if (input.startsWith("/set delayTime "))
      {
        String set_delayTime = input.substring(15);
        delayTime = set_delayTime.toInt();
        Serial.print("Delay time set to ");
        Serial.println(delayTime);
      }
    }
  }
  if (state == "running" and (sensorSystem.isExceedingThreshold() || sensorSystem.isDistanceExceedingThreshold()) ? true : false)
  {

    int level = (frameCount % 170);
    if (level == 0)
    {
      servoMaster.setAngle(0, 100);
      servoMaster.setAngle(3, 100);
    }
    if (level == 20)
    {
      servoMaster.setAngle(2, 125);
    }
    if (level == 30)
    {
      servoMaster.setAngle(0, 80);
      servoMaster.setAngle(3, 80);
    }
    if (level == 50)
    {
      servoMaster.setAngle(1, 125);
    }
    if (level == 60)
    {
      servoMaster.setAngle(0, 55);
      servoMaster.setAngle(3, 55);
    }
    if (level == 80)
    {
      servoMaster.setAngle(1, 100);
      servoMaster.setAngle(2, 100);
    }
    if (level == 100)
    {
      servoMaster.setAngle(3, 125);
    }
    if (level == 110)
    {
      servoMaster.setAngle(1, 80);
      servoMaster.setAngle(2, 80);
    }
    if (level == 130)
    {
      servoMaster.setAngle(0, 120);
    }
    if (level == 150)
    {
      servoMaster.setAngle(1, 55);
      servoMaster.setAngle(2, 55);
    }
  }
  else if (state == "debug")
  {
    servoMaster.setAngle(0, 90);
    servoMaster.setAngle(1, 90);
    servoMaster.setAngle(2, 90);
    servoMaster.setAngle(3, 90);
    state = "idle";
  }
  else if (state == "jump")
  {
    servoMaster.setAngle(0, 0);
    servoMaster.setAngle(1, 0);
    servoMaster.setAngle(2, 180);
    servoMaster.setAngle(3, 180);

    for (int8_t i; i < 100; i++)
    {
      delay(5);
      servoMaster.update();
    }
    servoMaster.setAngleImidiately(0, 150);
    servoMaster.setAngleImidiately(1, 150);
    servoMaster.setAngleImidiately(2, 30);
    servoMaster.setAngleImidiately(3, 30);
    delay(2000);
    state = "idle";
  }
}
