// Can't use Servo.h because it's hardware optimized and doesn't work at all when included with the Wire.h (i2c) library
// And that hardware optimization on Servo.h is not needed (small harm to the project even)
#include <Servo.h>

int sign(float number)
{
    return (number > 0) ? 1 : -1;
}
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
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

    float angleMultiplier[4] = {0.7, 0.7, 0.7, 0.7};

    float set_angle[4] = {90.0, 90.0, 90.0, 90.0};

    const float different_between_end_speed_and_center_speed = 0.25;

    const float distance_from_end_to_center = 90;

    bool servo_reverse[4] = {false, true, false, true};

    void attach()
    {
        for (int8_t i = 0; i < this->numServos; i++)
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

        for (int8_t ii = 0; ii < 4; ii++)
        {
            if (set_angle[ii] != angle[ii])
            {
                for (int8_t i = 0; i < 1; i++)
                {
                    float speed_multiplier = 1.0;
                    float angle_different = abs(90 - angle[ii]);
                    speed_multiplier = speed_multiplier * mapf(angle_different, 0, this->distance_from_end_to_center, 1, 1 + this->different_between_end_speed_and_center_speed);
                    angle[ii] += sign(set_angle[ii] - angle[ii]) * speed_multiplier;
                    this->write(ii, angle[ii]);
                    if (set_angle[ii] == angle[ii])
                    {
                        break;
                    }
                }
            }
        }
    }
    void setAngle(int servoNum, float angle)
    {
        float _angle = constrain(angle, 0, 180); // Ensure position is within bounds
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
