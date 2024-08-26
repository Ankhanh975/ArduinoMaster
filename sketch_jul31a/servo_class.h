#include <Servo.h> // Include the Servo library

int sign(float number);
class ServoMaster
{
private:
    int servoPins[4];  // Pins connected to the servos
    int numServos = 4; // Number of servos
    Servo servos[4];   // Create an array of Servo objects

    int offsetServo0 = 4; //81
    int offsetServo1 = -7; // 93
    int offsetServo2 = -9;
    int offsetServo3 = -6;

    int angle1 = 90;
    int angle2 = 90;
    int angle3 = 90;
    int angle4 = 90;

    int set_angle1 = 90;
    int set_angle2 = 90;
    int set_angle3 = 90;
    int set_angle4 = 90;

    void attach()
    {
        for (int i = 0; i < this->numServos; i++)
        {
            servos[i].attach(servoPins[i]);
            int initialPosition = 90;        // Default initial position
            this->write(i, initialPosition); // Set each servo to its corrected initial position
        }
    }
    void write(int servoNum, int initialPosition)
    {
        if (servoNum == 1 || servoNum == 3)
        {
            initialPosition = 180 - initialPosition;
        }
        int _angle;
        if (servoNum == 0)
        {
            initialPosition += offsetServo0;
        }
        else if (servoNum == 1)
        {
            initialPosition += offsetServo1;
        }
        else if (servoNum == 2)
        {
            initialPosition += offsetServo2;
        }
        else if (servoNum == 3)
        {
            initialPosition += offsetServo3;
        }

        _angle = constrain(initialPosition, 0, 180); // Ensure position is within bounds
        this->servos[servoNum].write(_angle);
    }

public:
    void init()
    {
        this->servoPins[0] = 12;
        this->servoPins[1] = 10;
        this->servoPins[2] = 11;
        this->servoPins[3] = 9;

        this->attach();
    }
    void update()
    {
        for (int i = 0; i < 3; i++)
        {
            if (set_angle1 != angle1)
            {

                Serial.print("Servo 1: ");
                Serial.println(angle1);
                Serial.print("Set Servo 1: ");
                Serial.println(set_angle1);

                angle1 += sign(set_angle1 - angle1);
                write(0, angle1);
            }
            else
            {
                break;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if (set_angle2 != angle2)
            {
                Serial.print("Servo 2: ");
                Serial.println(angle2);
                Serial.print("Set Servo 2: ");
                Serial.println(set_angle2);

                angle2 += sign(set_angle2 - angle2);
                write(1, angle2);
            }
            else
            {
                break;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if (set_angle3 != angle3)
            {
                Serial.print("Servo 3: ");
                Serial.println(angle3);
                Serial.print("Set Servo 3: ");
                Serial.println(set_angle3);

                angle3 += sign(set_angle3 - angle3);
                write(2, angle3);
            }
            else
            {
                break;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if (set_angle4 != angle4)
            {
                Serial.print("Servo 4: ");
                Serial.println(angle4);
                Serial.print("Set Servo 4: ");
                Serial.println(set_angle4);

                angle4 += sign(set_angle4 - angle4);
                write(3, angle4);
            }
            else
            {
                break;
            }
        }
    }
    void setAngle(int servoNum, int angle)
    {
        Serial.print("Servo ");
        Serial.print(servoNum);
        Serial.print(" set to ");
        Serial.println(angle);

        int _angle = constrain(angle, 0, 180); // Ensure position is within bounds
        // Apply offsets for specific servos
        if (servoNum == 0)
        {
            set_angle1 = _angle;
        }
        else if (servoNum == 1)
        {
            set_angle2 = _angle;
        }
        else if (servoNum == 2)
        {
            set_angle3 = _angle;
        }
        else if (servoNum == 3)
        {
            set_angle4 = _angle;
        }
    }

    void setAngleImidiately(int servoNum, int angle)
    {
        this->write(servoNum, angle);
        if (servoNum == 0)
        {
            angle1 = angle;
            set_angle1 = angle;
        }
        else if (servoNum == 1)
        {
            angle2 = angle;
            set_angle2 = angle;
        }
        else if (servoNum == 2)
        {
            angle3 = angle;
            set_angle3 = angle;
        }
        else if (servoNum == 3)
        {
            angle4 = angle;
            set_angle4 = angle;
        }
    }
    int get_angle(int num)
    {
        if (num == 0)
        {
            return angle1;
        }
        else if (num == 1)
        {
            return angle2;
        }
        else if (num == 2)
        {
            return angle3;
        }
        else if (num == 3)
        {
            return angle4;
        }
    }
};
