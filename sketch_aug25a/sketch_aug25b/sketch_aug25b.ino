#include <Servo.h> // Include the Servo library

int sign(float number)
{
  return (number > 0) ? 1 : -1;
}
float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
class ServoMaster
{
private:
  const int numServos = 4; // Number of servos

  Servo servos[4]; // Create an array of Servo objects

  float offsetServos[4] = {4.0, -7.0, -9.0, -6.0};

  float offsetServosGlobal = +0.0;

  float angle[4] = {90.0, 90.0, 90.0, 90.0};

  float angleMultiplier[4] = {1.50, 1.50, 1.50, 1.50};

  float set_angle[4] = {90.0, 90.0, 90.0, 90.0};

  const float different_between_end_speed_and_center_speed = 0.25;

  const float distance_from_end_to_center = 30;

  bool servo_reverse[4] = {false, true, false, true};

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
    if (this->servo_reverse[servoNum] == true)
    {
      initialPosition = 180 - initialPosition;
    }
    initialPosition += offsetServosGlobal + offsetServos[servoNum];

    float _angle = constrain(initialPosition, 0, 180); // Ensure position is within bounds
    float final_pos;
    final_pos = (_angle - 90) * this->angleMultiplier[servoNum] + 90;
    this->servos[servoNum].write(final_pos);
  }

public:
  int servoPins[4] = {5, 4, 3, 2}; // Pins connected to the servos

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
          float speed_multiplier = 1.0;
          float angle_different = abs(90 - angle[ii]);
          speed_multiplier = speed_multiplier * mapf(angle_different, 0, this->distance_from_end_to_center, 1, 1 + this->different_between_end_speed_and_center_speed);
          angle[ii] += sign(set_angle[ii] - angle[ii]) * speed_multiplier;
          this->write(ii, angle[ii]);
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
};

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

}
String state = "running";
int level = 0;
int delayTime = 2250;

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
