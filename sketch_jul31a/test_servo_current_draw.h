#include <Servo.h>

Servo myServo;  // Create a Servo object

const int servoPin = 2;  // Define the pin for the servo
const int analogPin = A0;  // Define the pin for the analog input
const int fps = 60;  // Frames per second
const int delayTime = 1000 / fps;  // Calculate delay time for 60 fps
const int servoUpdateInterval = 10;  // Update servo position every 10 iterations

int pos = 0;  // Current position of the servo
int direction = 1;  // Direction of servo movement (1 for increasing, -1 for decreasing)
const int bufferSize5 = 5;  // Size of the rolling buffer for 5 points
const int bufferSize20 = 20;  // Size of the rolling buffer for 20 points
int analogBuffer5[bufferSize5] = {0};  // Rolling buffer for 5 analog values
int analogBuffer20[bufferSize20] = {0};  // Rolling buffer for 20 analog values
int bufferIndex5 = 0;  // Current index in the 5-point buffer
int bufferIndex20 = 0;  // Current index in the 20-point buffer
int loopCounter = 0;  // Counter to control servo update frequency

void setup() {
  myServo.attach(servoPin);  // Attach the servo to pin 2
  Serial.begin(115200);  // Initialize Serial communication at 9600 baud
}

void loop() {
  int analogValue = analogRead(analogPin);  // Read the analog value from pin A0

  // Update the 5-point rolling buffer
  analogBuffer5[bufferIndex5] = analogValue;
  bufferIndex5 = (bufferIndex5 + 1) % bufferSize5;

  // Update the 20-point rolling buffer
  analogBuffer20[bufferIndex20] = analogValue;
  bufferIndex20 = (bufferIndex20 + 1) % bufferSize20;

  // Calculate the average of the values in the 5-point buffer
  int sum5 = 0;
  for (int i = 0; i < bufferSize5; i++) {
    sum5 += analogBuffer5[i];
  }
  int averageValue5 = sum5 / bufferSize5;

  // Calculate the average of the values in the 20-point buffer
  int sum20 = 0;
  for (int i = 0; i < bufferSize20; i++) {
    sum20 += analogBuffer20[i];
  }
  int averageValue20 = sum20 / bufferSize20;

  // Send the analog value, 0, 50, servo position, 5-point average, and 20-point average to the Serial Monitor
  Serial.print(0);
  Serial.print(", ");
  Serial.print(50);
  Serial.print(", ");
  Serial.print(pos);
  Serial.print(", ");
  Serial.print(averageValue5);
  Serial.print(", ");
  Serial.println(averageValue20);

  // Update the servo position less frequently
  if (loopCounter % servoUpdateInterval == 0 || 1) {
    myServo.write(pos);  // Set the servo position

    // Update the servo position
    pos += direction;
    if (pos >= 180 || pos <= 0) {
      direction = -direction;  // Reverse direction at the ends
    }
  }

  loopCounter++;  // Increment the loop counter

  delay(delayTime);  // Wait for the next frame
}