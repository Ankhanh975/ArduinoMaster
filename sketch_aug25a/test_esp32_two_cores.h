#include <Arduino.h>

// Define the LED pin (assuming GPIO 2 for the onboard LED)
const int ledPin = 2;

// Task to blink the onboard LED
void task1(void *pvParameter) {
    pinMode(ledPin, OUTPUT); // Set LED pin as output
    while (1) {
        digitalWrite(ledPin, HIGH); // Turn the LED on
        delay(500);                 // Wait for 500 ms
        digitalWrite(ledPin, LOW);  // Turn the LED off
        delay(500);                 // Wait for 500 ms
    }
}

// Task to print "Hello World" every 500 ms
void task2(void *pvParameter) {
    while (1) {
        Serial.println("Hello World");
        delay(500); // Wait for 500 ms
    }
}

void app_main() {
    // Initialize serial communication
    Serial.begin(115200);

    // Create tasks on different cores
    xTaskCreatePinnedToCore(
        task1,         // Task function
        "Task 1",      // Task name
        2048,          // Stack size
        NULL,          // Task parameters
        1,             // Priority
        NULL,          // Task handle
        0              // Core 0
    );

    xTaskCreatePinnedToCore(
        task2,         // Task function
        "Task 2",      // Task name
        2048,          // Stack size
        NULL,          // Task parameters
        1,             // Priority
        NULL,          // Task handle
        1              // Core 1
    );
}