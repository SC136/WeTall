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

Data joystickData;

void setup() {
    Serial.begin(9600); // Start serial communication for debugging
    Serial.print("Size of Data struct: ");
    Serial.println(sizeof(Data)); // Print size of struct
    
    radio.begin();
    radio.openWritingPipe(0xF0F0F0F0E1LL); // Set address for writing
    radio.setPALevel(RF24_PA_HIGH);           
    radio.stopListening(); // Set to transmitter mode
}

void loop() {
    // Read joystick values
    joystickData.bldc = analogRead(A0); // Read BLDC throttle control
    joystickData.opp = analogRead(A3);   // Read opposite servo control
    joystickData.sm = analogRead(A2);    // Read same servo control

    Serial.print("Sending: ");
    Serial.print("BLDC: ");
    Serial.print(joystickData.bldc);
    Serial.print(", Opp: ");
    Serial.print(joystickData.opp);
    Serial.print(", Same: ");
    Serial.println(joystickData.sm);

    // Send data to receiver
    bool success = radio.write(&joystickData, sizeof(joystickData));
    
    if (!success) {
        Serial.println("Failed to send data!");
    }

    delay(100); // Adjust delay as needed for stability
}
