// PID constants (unchanging)
const float kp = 2.0;
const float ki = 0.1;
const float kd = 0.05;

// Motor control pins for DRV8833
const int motorPWMPin = 9;    // PWM pin for motor control
const int motorDirPin = 8;    // Direction pin for motor

// Encoder pins
const int encoderPinA = 2;    // Encoder pin A
const int encoderPinB = 3;    // Encoder pin B

// Motor and encoder properties
const int ticksPerRevolution = 20; // Number of encoder ticks per motor revolution

// Desired position (setpoint) in degrees
const float setpoint = 360.0;  // Target position (e.g., one full rotation)

// Variables that change
float currentPosition = 0.0;  // Current position (in degrees)
float error = 0;
float prevError = 0;
float integral = 0;
float derivative = 0;
float pidOutput = 0;

// Encoder tick count
volatile long encoderTicks = 0;

void setup() {
  Serial.begin(9600);

  // Motor control pin setup
  pinMode(motorPWMPin, OUTPUT);
  pinMode(motorDirPin, OUTPUT);

  // Encoder setup
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), encoderTick, RISING);  // Count encoder ticks
}

void loop() {
  // Calculate current position in degrees
  currentPosition = (encoderTicks / (float)ticksPerRevolution) * 360.0;

  // PID calculations
  error = setpoint - currentPosition;
  integral += error;
  derivative = error - prevError;
  pidOutput = kp * error + ki * integral + kd * derivative;
  prevError = error;

  // Constrain the PID output to valid motor control range (-255 to 255)
  pidOutput = constrain(pidOutput, -255, 255);

  // Control motor using DRV8833
  if (pidOutput > 0) {
    // Move forward
    digitalWrite(motorDirPin, HIGH);
    analogWrite(motorPWMPin, pidOutput);
  } else {
    // Move backward
    digitalWrite(motorDirPin, LOW);
    analogWrite(motorPWMPin, -pidOutput);  // PWM can't be negative, so invert
  }

  // Debugging info
  Serial.print("Position: ");
  Serial.print(currentPosition);
  Serial.print(" degrees, PID Output: ");
  Serial.println(pidOutput);

  delay(100);  // Small delay for loop stability
}

// Encoder tick interrupt
void encoderTick() {
  if (digitalRead(encoderPinB) == HIGH) {
    encoderTicks++;
  } else {
    encoderTicks--;
  }
}