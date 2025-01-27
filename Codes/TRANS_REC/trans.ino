#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN pins

#include <stdint.h> // Include for fixed-width types

// Define a packed struct to hold joystick data
struct __attribute__((packed)) Data {
    int16_t bldc; // Joystick X-axis (BLDC throttle control)
    int16_t opp;  // Joystick Y-axis (opposite servo control)
    int16_t sm;   // Joystick extra axis (same servo control)
};

// Add battery data struct definition
struct __attribute__((packed)) BatteryData {
    float voltage;
};

Data joystickData;
BatteryData batteryData;

// Define analog input pins for ESP32
const int PIN_BLDC = 34;  // GPIO34 for BLDC control
const int PIN_OPP = 35;   // GPIO35 for opposite servo control
const int PIN_SM = 32;    // GPIO32 for same servo control`

void setup() {
    Serial.begin(9600); // Start serial communication for debugging
    Serial.print("Size of Data struct: ");
    Serial.println(sizeof(Data)); // Print size of struct
    
    radio.begin();
    radio.openWritingPipe(0xF0F0F0F0E1LL); // Set address for writing
    radio.setPALevel(RF24_PA_HIGH);           
    radio.stopListening(); // Set to transmitter mode
    radio.enableAckPayload(); // Enable ACK payload
}

void loop() {
    // Read joystick values using ESP32 GPIO pins
    joystickData.bldc = analogRead(PIN_BLDC);
    joystickData.opp = analogRead(PIN_OPP);
    joystickData.sm = analogRead(PIN_SM);

    // Serial.print("Sending: ");
    // Serial.print("BLDC: ");
    // Serial.print(joystickData.bldc);
    // Serial.print(", Opp: ");
    // Serial.print(joystickData.opp);
    // Serial.print(", Same: ");
    // Serial.println(joystickData.sm);

    // Send data to receiver
    bool success = radio.write(&joystickData, sizeof(joystickData));
    
    if (success) {
        // Check if we received battery data in ACK payload
        if (radio.available()) {
            radio.read(&batteryData, sizeof(BatteryData));
            Serial.print("Battery Voltage: ");
            Serial.print(batteryData.voltage);
            Serial.println("V");
        }
    } else {
        // Serial.println("Failed to send data!");
    }

    delay(100); // Adjust delay as needed for stability
}
