#include <Servo.h> // Include the Servo library

int sign(float number)
{
  return (number > 0) ? 1 : -1;
}
class ServoMaster
{
private:
  const int numServos = 4; // Number of servos

  int servoPins[4] = {12, 10, 11, 9}; // Pins connected to the servos

  float offsetServos[4] = {4.0, -7.0, -9.0, -6.0};

  float angle[4] = {90.0, 90.0, 90.0, 90.0};

  float angleMultiplier[4] = {0.55, 0.55, 0.55, 0.55};

  float set_angle[4] = {90.0, 90.0, 90.0, 90.0};

  Servo servos[4]; // Create an array of Servo objects

  void attach()
  {
    for (int i = 0; i < this->numServos; i++)
    {
      this->servos[i].attach(servoPins[i]);
      int initialPosition = 90;        // Default initial position
      this->write(i, initialPosition); // Set each servo to its corrected initial position
    }
  }
  void write(int servoNum, float initialPosition)
  {
    if (servoNum == 1 || servoNum == 3)
    {
      initialPosition = 180 - initialPosition;
    }
    initialPosition += offsetServos[servoNum];
    float _angle = constrain(initialPosition, 0, 180); // Ensure position is within bounds
    float final_pos;
    if (servoNum == 0)
    {
      final_pos = (_angle - 90) * this->angleMultiplier[servoNum] + 90;
    }
    this->servos[servoNum].write(final_pos);
  }

public:
  void init()
  {
    this->attach();
  }
  void update()
  {

    for (int ii = 0; ii < 4; ii++)
    {
      for (int i = 0; i < 2; i++)
      {
        if (set_angle[ii] != angle[ii])
        {
          angle[ii] += sign(set_angle[ii] - angle[ii]) * 1.25;
          write(ii, angle[ii]);
        }
        else
        {
          break;
        }
      }
    }
  }
  void setAngle(int servoNum, float angle)
  {

    int _angle = constrain(angle, 0, 180); // Ensure position is within bounds
    this->set_angle[servoNum] = _angle;
  }

  void setAngleImidiately(int servoNum, float angle)
  {
    this->write(servoNum, angle);
    this->angle[servoNum] = angle;
    this->set_angle[servoNum] = angle;
  }
  float get_angle(int num)
  {
    return angle[num];
  }
  float addServoOffset(int servoNum, float offset)
  {
    this->offsetServos[servoNum] += offset;
    return this->offsetServos[servoNum];
  }
  float setAngleMultiplier(int servoNum, float multiplier)
  {
    this->angleMultiplier[servoNum] = multiplier;
    return this->angleMultiplier[servoNum];
  }
  void setServoPin(int servoNum, int pin)
  {
    this->servoPins[servoNum] = pin;
    this->servos[servoNum].attach(pin);
  }
};

ServoMaster servoMaster;
long frameCount = 0;
void setup()
{
  servoMaster.init();
  Serial.begin(9600);
  Serial.println("Starting");

  if (Serial.available())
  {
    String input = Serial.readStringUntil('\n'); // Read input from serial monitor
  }

  delay(500);
  servoMaster.setAngleImidiately(0, 90);
  servoMaster.setAngleImidiately(1, 90);
  servoMaster.setAngleImidiately(2, 90);
  servoMaster.setAngleImidiately(3, 90);

  delay(8);

  servoMaster.setAngle(0, 120);
  servoMaster.setAngle(1, 60);
  servoMaster.setAngle(2, 60);
  servoMaster.setAngle(3, 120);

  for (int i; i < 15; i++)
  {
    delay(8);
    servoMaster.update();
  }
}
String state = "idle";
int level = 0;
int delayTime = 1750;

void loop()
{
  delay(int(delayTime / 1000));
  delayMicroseconds(delayTime % 1000);

  frameCount += 1;
  servoMaster.update();
  // Serial.print("frameCount: ");
  // Serial.println(frameCount);

  if (Serial.available() > 0)
  {
    String input = Serial.readStringUntil('\n'); // Read input from serial monitor
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
      else if (input.startsWith("/set servoPin "))
      {
        String set_servoPin = input.substring(14);
        int servoNum = set_servoPin.substring(0, 1).toInt();
        int pin = set_servoPin.substring(2).toInt();
        servoMaster.setServoPin(servoNum, pin);
        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" pin set to ");
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
  if (state == "running")
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
