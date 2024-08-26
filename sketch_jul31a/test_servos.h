#include <Servo.h>


const int servoPins[] = {2, 3, 4, 5}; // Pins connected to the servos
const int numServos = 4;              // Number of servos

Servo servos[numServos]; // Create an array of Servo objects

// Global offset values for specific servos
// 107 107 85 90
const int offsetServo0 = +21;
const int offsetServo1 = +17;
const int offsetServo2 = 0;
const int offsetServo3 = -5;

void setup()
{
    Serial.begin(9600); // Initialize serial communication at 115200 baud rate
    Serial.println(100);

    // Attach each servo to its corresponding pin and set initial positions with offsets
    for (int i = 0; i < numServos; i++)
    {
        servos[i].attach(servoPins[i]);
        int initialPosition = 90; // Default initial position

        if (i == 0 || i == 1)
        {
            initialPosition = 180 - initialPosition;
        }
        // Apply offsets for specific servos
        if (i == 0)
        {
            initialPosition += offsetServo0;
        }
        else if (i == 1)
        {
            initialPosition += offsetServo1;
        }
        else if (i == 2)
        {
            initialPosition += offsetServo2;
        }
        else if (i == 3)
        {
            initialPosition += offsetServo3;
        }

        initialPosition = constrain(initialPosition, 0, 180); // Ensure position is within bounds

        // Flip the position for servo 1 and servo 3

        servos[i].write(initialPosition); // Set each servo to its corrected initial position
    }
}

void loop()
{
    if (Serial.available() > 0)
    {
        String input = Serial.readStringUntil('\n'); // Read input from serial monitor
        Serial.println(input);
        input.trim(); // Remove any leading or trailing whitespace
        processCommand(input);
    }
    delay(16);
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

    if (servoIndex >= 0 && servoIndex < numServos && angle >= 0 && angle <= 180)
    {
        // Flip the angle for servo 1 and servo 3
        if (servoIndex == 0 || servoIndex == 1)
        {
            angle = 180 - angle;
        }
        // Apply offsets for specific servos
        if (servoIndex == 0)
        {
            angle += offsetServo0; // Apply offset for servo 1
        }
        else if (servoIndex == 1)
        {
            angle += offsetServo1; // Apply offset for servo 2
        }
        else if (servoIndex == 2)
        {
            angle += offsetServo2; // Apply offset for servo 3
        }
        else if (servoIndex == 3)
        {
            angle += offsetServo3; // Apply offset for servo 4
        }

        angle = constrain(angle, 0, 180); // Ensure angle is within bounds

        servos[servoIndex].write(angle); // Set servo to the specified angle
        Serial.print("Servo ");
        Serial.print(servoIndex);
        Serial.print(" set to ");
        Serial.println(angle);
    }
    else
    {
        Serial.println("Invalid input");
    }
}