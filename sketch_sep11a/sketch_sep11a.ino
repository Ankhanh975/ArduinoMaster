#include <Wire.h>

void setup() {
    Serial.begin(115200);
    Wire.begin(); // Initialize I2C bus

    Serial.println("I2C Scanner");
    int nDevices = 0;

    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.print(address, HEX);
            Serial.println(" !");
            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        }
    }

    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    } else {
        Serial.print("Number of I2C devices found: ");
        Serial.println(nDevices);
    }
}

void loop() {
    // Do nothing in loop
}