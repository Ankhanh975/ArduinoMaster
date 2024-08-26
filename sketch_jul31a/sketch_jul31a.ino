#include "point_vs_plane.h"
#include "servo_class.h"

ServoMaster servoMaster;
void setup()
{
    Serial.begin(9600);
    servoMaster.init();
    Serial.println("Starting");

    servoMaster.setAngle(0, 130);
    servoMaster.setAngle(3, 130);
    servoMaster.setAngle(1, 60);
    servoMaster.setAngle(2, 60);
    for (int i = 0; i < 50; i++)
    {
        delay(16);
        servoMaster.update();
    }
}
void loop()
{
    delay(12);

    servoMaster.update();
    if (Serial.available() > 0)
    {
        String input = Serial.readStringUntil('\n'); // Read input from serial monitor
        Serial.println(input);
        if (input == "report")
        {
            // Print robot status
            float d = 0;
            int num = main_calculation(servoMaster.get_angle(0), servoMaster.get_angle(1), servoMaster.get_angle(2), servoMaster.get_angle(3), d);
            Serial.print("The robot is in num: ");
            Serial.println(num);
            Serial.print("The distance is ");
            Serial.println(d);
            Serial.print("The robot is in angles: ");
            Serial.print(servoMaster.get_angle(0));
            Serial.print(" ");
            Serial.print(servoMaster.get_angle(1));
            Serial.print(" ");
            Serial.print(servoMaster.get_angle(2));
            Serial.print(" ");
            Serial.println(servoMaster.get_angle(3));
        }
        input.trim(); // Remove any leading or trailing whitespace
        processCommand(input);
    }
  
    // if (servoMaster.get_angle(0) >= 120 && servoMaster.get_angle(3) >= 120)
    // {
    //     servoMaster.setAngle(0, 65);
    //     servoMaster.setAngle(3, 65);
    // }
    // if (servoMaster.get_angle(1) >= 120 && servoMaster.get_angle(2) >= 120)
    // {
    //     servoMaster.setAngle(1, 65);
    //     servoMaster.setAngle(2, 65);
    // }
}
void processCommand(String command)
{
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex == -1)
    {
        Serial.println("Invalid command format");
        return;
    }

    int servoIndex = command.substring(0, spaceIndex).toInt(); // Get servo index
    int angle = command.substring(spaceIndex + 1).toInt();     // Get angle
    servoMaster.setAngle(servoIndex, angle);
}
