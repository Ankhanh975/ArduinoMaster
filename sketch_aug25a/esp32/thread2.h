// #include <BluetoothSerial.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Define the LED pin (assuming GPIO 2 for the onboard LED)
const int ledPin = 8;

// Create a BluetoothSerial object
// BluetoothSerial SerialBT;

void thread2_setup()
{
    // SerialBT.begin("ESP32_BT2"); // Name of the Bluetooth device
    Serial.println("Starting thread 2");
    pinMode(ledPin, OUTPUT);     // Set LED pin as output
}

void thread2_loop()
{
    // if (SerialBT.available())
    // {
    //     String message = SerialBT.readStringUntil('\n');
    //     Serial.print(message);
    // }
    // if (Serial.available())
    // {
    //     String response = Serial.readStringUntil('\n');
    //     SerialBT.print(response);
    // }

    // Blink the onboard LED every 500ms
    static unsigned long lastBlinkTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= 500)
    {
        digitalWrite(ledPin, !digitalRead(ledPin)); // Toggle the LED state
        lastBlinkTime = currentTime;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); // Delay for 100 ms
}