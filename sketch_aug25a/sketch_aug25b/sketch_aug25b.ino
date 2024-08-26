#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_VL53L0X.h>


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
        for (int i = 0; i < 10; i++)
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
        for (int i = 0; i < this->filterSize; i++)
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