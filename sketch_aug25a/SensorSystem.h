#include <MPU6050.h>
#include <VL53L0X.h>

class SensorSystem {
public:
  SensorSystem()
    : filterIndex(0), lastUpdateTime(0), exceedsThreshold(false), distanceExceedsThreshold(false) {}

  void initialize() {
    // Initialize MPU6050
    Wire.begin();
    Wire.setClock(400000);  // Set I2C clock speed to 400 kHz

    this->mpu.initialize();
    this->mpu.setDLPFMode(0x06);
    if (this->mpu.testConnection()) {
      Serial.println("MPU6050 connection successful");
    } else {
      Serial.println("MPU6050 connection failed");
    }

    // Initialize VL53L0X
    if (!this->lox.init()) {
      Serial.println(F("Failed to boot VL53L0X"));
      while (1)
        ;
    }
    Serial.println(F("VL53L0X ready"));

    // Set the timing budget to a lower value for high-speed mode (e.g., 20 ms)
    this->lox.setMeasurementTimingBudget(5000);  // 2 ms timing budget
    this->lox.startContinuous(0);
    this->lox.setSignalRateLimit(0.4);

    // Calibrate MPU6050
    this->calibrateMPU6050();
  }

  void update() {
    unsigned long currentTime = millis();
    if (currentTime - this->lastUpdateTime >= this->updateInterval) {
      this->lastUpdateTime = currentTime;
      this->readSensors();
      this->checkThreshold();
      this->checkDistanceThreshold();
    }
  }

  void getCalibratedData(int16_t &ax, int16_t &ay, int16_t &az) {
    ax = this->calibratedAx;
    ay = this->calibratedAy;
    az = this->calibratedAz;
  }

  uint16_t readDistance() {
    return this->filteredDistance;
  }

  void calculatePitchRoll(float &pitch, float &roll) {
    pitch = atan2(this->calibratedAy, sqrt(this->calibratedAx * this->calibratedAx + this->calibratedAz * this->calibratedAz)) * 180.0 / PI;
    roll = atan2(-this->calibratedAx, this->calibratedAz) * 180.0 / PI;
  }
  bool isExceedingThreshold() {
    return this->exceedsThreshold;
  }

  bool isDistanceExceedingThreshold() {
    return this->distanceExceedsThreshold;
  }

private:
  MPU6050 mpu;
  VL53L0X lox;

  float baselineAx = 0, baselineAy = 0, baselineAz = 0;
  float calibratedAx, calibratedAy, calibratedAz;

  static const int filterSize = 3;
  uint16_t distanceReadings[filterSize];
  int filterIndex;
  uint16_t filteredDistance;

  unsigned long lastUpdateTime;
  static const unsigned long updateInterval = 1000 / 60;  // 60 updates per second

  bool exceedsThreshold;
  bool distanceExceedsThreshold;

  void calibrateMPU6050() {
    // int16_t ax, ay, az, gx, gy, gz;
    // for (int8_t i = 0; i < 10; i++)
    // {
    //   this->mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    //   this->baselineAx += ax / 16384.0;
    //   this->baselineAy += ay / 16384.0;
    //   this->baselineAz += -2 + az / 16384.0;
    //   delay(10); // Small delay between measurements
    // }
    // this->baselineAx /= 10;
    // this->baselineAy /= 10;
    // this->baselineAz /= 10;

    // 19:24:34.883 Baseline aX = -0.71
    // 19:24:34.883 -> Baseline aY = -0.10
    // 19:24:34.883 -> Baseline aZ = -1.24

    this->baselineAx = -0.71;
    this->baselineAy = -0.10;
    this->baselineAz = -1.24;

    Serial.print("Baseline aX = ");
    Serial.println(this->baselineAx);
    Serial.print("Baseline aY = ");
    Serial.println(this->baselineAy);
    Serial.print("Baseline aZ = ");
    Serial.println(this->baselineAz);
  }

  void readMPU6050() {
    int16_t ax, ay, az;
    this->mpu.getAcceleration(&ax, &ay, &az);
    this->calibratedAx = ax / 16384.0 - this->baselineAx;
    this->calibratedAy = ay / 16384.0 - this->baselineAy;
    this->calibratedAz = az / 16384.0 - this->baselineAz;
  }

  void readSensors() {
    this->readMPU6050();
    uint16_t distance = this->lox.readRangeContinuousMillimeters();
    this->filteredDistance = this->filterDistance(distance);
  }

  uint16_t filterDistance(uint16_t newDistance) {
    this->distanceReadings[this->filterIndex] = newDistance;
    this->filterIndex = (this->filterIndex + 1) % this->filterSize;

    uint32_t sum = 0;
    for (int8_t i = 0; i < this->filterSize; i++) {
      sum += this->distanceReadings[i];
    }
    return sum / this->filterSize;
  }

  void checkThreshold() {
    float pitch, roll;
    this->calculatePitchRoll(pitch, roll);
    this->exceedsThreshold = (abs(pitch) > 13 || abs(roll) > 13);
  }

  void checkDistanceThreshold() {
    this->distanceExceedsThreshold = (this->filteredDistance > 100);
  }
};
