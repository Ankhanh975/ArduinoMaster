#include "ServoMaster.h"
// #include "SensorSystem.h"

// Define the pin number for the LED
const int ledPin = 13;

// Create an instance of the SensorSystem class
// SensorSystem sensorSystem;
ServoMaster servoMaster;

long frameCount = 0;

void setup() {
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
  // sensorSystem.initialize();
  // sensorSystem.update();
}
String state = "running";
int level = 0;
int delayTime = 3300;

void loop() {
  delay(delayTime / 1000);
  delayMicroseconds(delayTime % 1000);

  // Update sensor data at 60 times per second
  // sensorSystem.update();
  servoMaster.update();

  frameCount += 1;

  // Blink the LED based on pitch, roll, or distance thresholds
  // digitalWrite(ledPin, (sensorSystem.isExceedingThreshold() || sensorSystem.isDistanceExceedingThreshold()) ? HIGH : LOW);

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // Read input from serial monitor
    input.trim();
    if (input.startsWith("/set")) {
      if (input.startsWith("/set state ")) {
        String set_state = input.substring(11);
        if (set_state == "start" || set_state == "debug" || set_state == "running" || set_state == "idle" || set_state == "jump") {
          state = set_state;
          delay(100);
          frameCount = 0;
        }
        Serial.print("State set to ");
        Serial.println(state);
      } else if (input.startsWith("/set angle ")) {
        String set_angle = input.substring(11);
        int servoNum = set_angle.substring(0, 1).toInt();
        int angle = set_angle.substring(2).toInt();
        servoMaster.setAngle(servoNum, angle);
        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" angle set to ");
        Serial.println(angle);
      } else if (input.startsWith("/set servoOffset ")) {
        String set_servoOffset = input.substring(17);
        int servoNum = set_servoOffset.substring(0, 1).toInt();
        int offset = set_servoOffset.substring(2).toInt();
        offset = servoMaster.addServoOffset(servoNum, offset);
        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" offset set to ");
        Serial.println(offset);
      } else if (input.startsWith("/set angleMultiplier ")) {
        String set_angleMultiplier = input.substring(21);
        int servoNum = set_angleMultiplier.substring(0, 1).toInt();
        int multiplier = set_angleMultiplier.substring(2).toInt();
        servoMaster.setAngleMultiplier(servoNum, multiplier);
        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" multiplier set to ");
        Serial.println(multiplier);
      } else if (input.startsWith("/set swapServoPin ")) {
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
      } else if (input.startsWith("/set delayTime ")) {
        String set_delayTime = input.substring(15);
        delayTime = set_delayTime.toInt();
        Serial.print("Delay time set to ");
        Serial.println(delayTime);
      }
    }
  }
  // and ((sensorSystem.isExceedingThreshold() || sensorSystem.isDistanceExceedingThreshold()) ? false : true)
  if (state == "running") {

    int level = (frameCount % 170);
    if (level == 0) {
      servoMaster.setAngle(0, 100);
      servoMaster.setAngle(3, 100);
    }
    if (level == 20) {
      servoMaster.setAngle(2, 125);
    }
    if (level == 30) {
      servoMaster.setAngle(0, 80);
      servoMaster.setAngle(3, 80);
    }
    if (level == 50) {
      servoMaster.setAngle(1, 125);
    }
    if (level == 60) {
      servoMaster.setAngle(0, 55);
      servoMaster.setAngle(3, 55);
    }
    if (level == 80) {
      servoMaster.setAngle(1, 100);
      servoMaster.setAngle(2, 100);
    }
    if (level == 100) {
      servoMaster.setAngle(3, 125);
    }
    if (level == 110) {
      servoMaster.setAngle(1, 80);
      servoMaster.setAngle(2, 80);
    }
    if (level == 130) {
      servoMaster.setAngle(0, 120);
    }
    if (level == 150) {
      servoMaster.setAngle(1, 55);
      servoMaster.setAngle(2, 55);
    }
  } else if (state == "debug") {
    servoMaster.setAngle(0, 90);
    servoMaster.setAngle(1, 90);
    servoMaster.setAngle(2, 90);
    servoMaster.setAngle(3, 90);
    state = "idle";
  } else if (state == "jump") {
    servoMaster.setAngle(0, 0);
    servoMaster.setAngle(1, 0);
    servoMaster.setAngle(2, 180);
    servoMaster.setAngle(3, 180);

    for (int8_t i; i < 100; i++) {
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
