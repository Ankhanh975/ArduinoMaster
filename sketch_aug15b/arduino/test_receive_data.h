#include <Wire.h>
#include <Servo.h>

Servo myServo;
const int servoPin = 2;
const int ledPin = 13; // Built-in LED pin

unsigned long previousMillis = 0; // Stores the last time the LED was updated
const long interval = 500; // Interval at which to blink (milliseconds)
bool ledState = LOW; // Current state of the LED

void setup() {
  Wire.begin(8); // Join I2C bus with address #8
  Wire.onReceive(receiveEvent); // Register event
  myServo.attach(servoPin);
  pinMode(ledPin, OUTPUT); // Initialize the LED pin as an output
  Serial.begin(9600);
  Serial.println("Arduino Master Initialized");
}

void loop() {
  // Non-blocking LED blink
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState; // Toggle the LED state
    digitalWrite(ledPin, ledState); // Update the LED
  }
}

void receiveEvent(int howMany) {
  String command = "";
  while (Wire.available()) {
    char c = Wire.read(); // Receive byte as a character
    command += c;
  }
  Serial.println("Received command: " + command);

  if (command.startsWith("move servo to")) {
    int angle = command.substring(13).toInt();
    myServo.write(angle);
    Serial.println("Moving servo to " + String(angle));
  }
}
