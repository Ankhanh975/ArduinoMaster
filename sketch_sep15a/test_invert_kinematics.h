#include <Servo.h>  // Library for controlling servos

// Constants for leg segment lengths (in mm)
const float L1 = 100.0;  // Length of upper leg
const float L2 = 100.0;  // Length of lower leg

// Servo objects for the hip and knee joints
Servo hipServo;
Servo kneeServo;

// Pins for the servos
const int hipPin = 9;
const int kneePin = 10;

// Variables for joint angles
float theta1 = 0;  // Hip joint angle (in degrees)
float theta2 = 0;  // Knee joint angle (in degrees)

// Predefined motion sequence (array of x, y coordinates for the foot)
float motionSequence[5][2] = { {100, 50}, {120, 60}, {140, 70}, {120, 60}, {100, 50} };
int sequenceLength = 5;  // Number of steps in the sequence
int currentStep = 0;     // Index for the current step

// Function to calculate joint angles for a desired (x, y) position
void inverseKinematics(float x, float y) {
  float r = sqrt(x * x + y * y);  // Distance from the origin to the target position

  // Check if the position is reachable
  if (r > (L1 + L2)) {
    Serial.println("Position out of reach.");
    return;
  }

  // Calculate the knee angle (theta2)
  float cosTheta2 = (r * r - L1 * L1 - L2 * L2) / (2 * L1 * L2);
  theta2 = acos(cosTheta2) * 180.0 / PI;  // Convert radians to degrees

  // Calculate the hip angle (theta1)
  float alpha = atan2(y, x) * 180.0 / PI;  // Convert radians to degrees
  float beta = acos((r * r + L1 * L1 - L2 * L2) / (2 * r * L1)) * 180.0 / PI;  // Convert radians to degrees
  theta1 = alpha - beta;

  // Apply motor constraints (limit angles)
  theta1 = constrain(theta1, 0, 180);  // Constrain hip angle to 0-180 degrees
  theta2 = constrain(theta2, 0, 180);  // Constrain knee angle to 0-180 degrees
}

// Function to move the servos to the desired joint angles
void moveServos() {
  hipServo.write(theta1);  // Set hip servo to the calculated angle
  kneeServo.write(180 - theta2);  // Set knee servo (reversed)

  // Print the angles for debugging
  Serial.print("Hip Angle: ");
  Serial.println(theta1);
  Serial.print("Knee Angle: ");
  Serial.println(theta2);
}

// Function to perform the predefined motion sequence
void performMotionSequence() {
  // Get the current (x, y) target from the sequence
  float x = motionSequence[currentStep][0];
  float y = motionSequence[currentStep][1];

  // Calculate the inverse kinematics for the current step
  inverseKinematics(x, y);

  // Move the servos to the calculated angles
  moveServos();

  // Move to the next step in the sequence
  currentStep++;
  if (currentStep >= sequenceLength) {
    currentStep = 0;  // Loop back to the start of the sequence
  }

  delay(1000);  // Wait for 1 second before moving to the next step
}

void setup() {
  Serial.begin(9600);  // Initialize serial communication for debugging

  // Attach the servos to their pins
  hipServo.attach(hipPin);
  kneeServo.attach(kneePin);

  // Set initial positions for the servos
  hipServo.write(90);  // Start the hip servo at 90 degrees
  kneeServo.write(90);  // Start the knee servo at 90 degrees
}

void loop() {
  // Perform the predefined motion sequence
  performMotionSequence();
}