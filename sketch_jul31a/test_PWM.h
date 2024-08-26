const int ledPin = 9;  // PWM-capable pin for the LED
const int checkPin = 10;  // Pin to check if it is high
const int delayTime = 50;  // Increased delay time in milliseconds

int highCounter = 0;  // Counter for consecutive HIGH readings
int brightness = 0;  // Initial brightness
int fadeAmount = 5;  // Amount to change the brightness each step

void setup() {
  pinMode(ledPin, OUTPUT);  // Set the LED pin as an output
}

void loop() {

  // Set the LED brightness
  analogWrite(ledPin, 20);

  // Change the brightness for next time through the loop
  brightness = brightness + fadeAmount;

  // Reverse the direction of the fading at the ends of the fade
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }

  // Wait for a longer period to slow down the cycle
  delay(delayTime);
}