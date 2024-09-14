#define IN1 25  // Pin for motor A IN1
#define IN2 26  // Pin for motor A IN2
#define PWM_CHANNEL_A 0
#define PWM_FREQUENCY 1000
#define PWM_RESOLUTION 8

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);

  // Set up motor control pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Set up PWM channels
  ledcSetup(PWM_CHANNEL_A, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(IN1, PWM_CHANNEL_A);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Get command from command line
    command.trim(); // Remove any extra spaces or new lines

    // Example command: "fwd 100" (forward at 100% speed)
    if (command.startsWith("fwd")) {
      int speed = command.substring(4).toInt(); // Get speed value
      controlMotor(true, speed); // Forward with speed
    } else if (command.startsWith("rev")) {
      int speed = command.substring(4).toInt(); // Get speed value
      controlMotor(false, speed); // Reverse with speed
    } else if (command.equals("stop")) {
      controlMotor(false, 0); // Stop the motor
    }
  }
}

void controlMotor(bool forward, int speed) {
  int dutyCycle = map(speed, 0, 100, 0, 255); // Convert speed percentage to PWM duty cycle

  if (forward) {
    ledcWrite(PWM_CHANNEL_A, dutyCycle); // IN1 controls speed
    digitalWrite(IN2, LOW);               // IN2 set to LOW for forward
  } else {
    ledcWrite(PWM_CHANNEL_A, 0);          // IN1 off for reverse
    digitalWrite(IN2, HIGH);              // IN2 set to HIGH for reverse
  }
}