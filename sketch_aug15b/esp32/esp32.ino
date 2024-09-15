#include <BluetoothSerial.h">

// Create a BluetoothSerial object
BluetoothSerial SerialBT;

void setup() {
  SerialBT.begin("ESP32_BT2"); // Name of the Bluetooth device
  Serial.begin(9600); // Start the Serial communication with the Arduino Nano
}

void loop() {
  if (SerialBT.available()) {
    String message = SerialBT.readStringUntil('\n');
    Serial.print(message);
  }
  if (Serial.available()) {
    String response = Serial.readStringUntil('\n');
    SerialBT.print(response);
  }
  delay(1);
}