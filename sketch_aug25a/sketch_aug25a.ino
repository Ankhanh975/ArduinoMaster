#include "ServoMaster.h"
#include "SensorSystem.h"

ServoMaster servoMaster;
SensorSystem sensorSystem;

long frameCount = 0;
String state = "running";
int level = 0;
int delayTime = 2250;

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting");
  servoMaster.init();

  servoMaster.setAngleImidiately(0, 90);
  servoMaster.setAngleImidiately(1, 90);
  servoMaster.setAngleImidiately(2, 90);
  servoMaster.setAngleImidiately(3, 90);

  delay(250);

  sensorSystem.initialize();
}

void loop()
{
  delay(delayTime / 1000);
  delayMicroseconds(delayTime % 1000);

  frameCount += 1;

  servoMaster.update();
  sensorSystem.update();

  // Serial.print("frameCount: ");
  // Serial.println(frameCount);

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

  state = "running";
  delay(500);
  bool isExceeding = false;

  isExceeding = sensorSystem.isExceedingThreshold() ? true : (sensorSystem.isExceedingThreshold() ? true : false);

  Serial.print(sensorSystem.isExceedingThreshold());
  Serial.print(" ");
  Serial.print(sensorSystem.isDistanceExceedingThreshold());
  Serial.print(" ");
  Serial.print(level);
  Serial.print(" ");
  Serial.print(isExceeding);
  Serial.print(" ");
  Serial.println(state);

  if (state == "running" and !isExceeding)
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

    for (int i; i < 100; i++)
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
