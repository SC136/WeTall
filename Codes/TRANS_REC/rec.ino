#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP32Servo.h> // Use ESP32Servo for ESP32
// #include <Servo.h>

RF24 radio(4, 5); // CE, CSN pins

#include <stdint.h> // Include for fixed-width types

// Define a packed struct to hold joystick data
struct __attribute__((packed)) Data {
    int16_t bldc; // Joystick X-axis (BLDC throttle control)
    int16_t opp;  // Opposite servo control
    int16_t sm;   // Same servo control
};

// New struct for battery data
struct __attribute__((packed)) BatteryData {
    float voltage;
};

Data joystickData;
BatteryData batteryData;

Servo servo1;
Servo servo2;

// Define ESC pins
const int escPin1 = 12; // ESC 1 pin
const int escPin2 = 14; // ESC 2 pin

// Add battery monitoring constants
const int batteryPin = 27;  // ESP32 analog pin for battery monitoring
const float vPow = 3.3;     // Reference voltage for ESP32 (3.3V)
const float r1 = 30000;     // Resistor 1 value in ohms (30 kΩ)
const float r2 = 7500;      // Resistor 2 value in ohms (7.5 kΩ)

void setup() {
    Serial.begin(9600);
    
    Serial.print("Size of Data struct: ");
    Serial.println(sizeof(Data)); // Print size of struct
    
    pinMode(escPin1, OUTPUT);
    pinMode(escPin2, OUTPUT);

    radio.begin();
    radio.openReadingPipe(1, 0xF0F0F0F0E1LL); // Set address for reading
    radio.setPALevel(RF24_PA_HIGH);
    radio.startListening(); // Set to receiver mode
    radio.enableAckPayload(); // Enable ACK payload

    servo1.attach(2);   // Attach servo1 to pin 2
    servo2.attach(13);  // Attach servo2 to pin 13

    servo1.write(90); 
    servo2.write(90);

    // Add analog pin setup for battery monitoring
    pinMode(batteryPin, INPUT);
    
    Serial.println("Setup complete. Waiting for data...");
}

void loop() {
    // Measure battery voltage
    int sensorValue = analogRead(batteryPin);
    float vOut = (sensorValue * vPow) / 4095.0;
    batteryData.voltage = vOut * (r1 + r2) / r2;

    if (radio.available()) {
        // Read incoming data
        radio.read(&joystickData, sizeof(joystickData));
        
        // Send battery voltage as ACK payload
        radio.writeAckPayload(1, &batteryData, sizeof(BatteryData));

        Serial.print("Received: ");
        Serial.print("BLDC: ");
        Serial.print(joystickData.bldc);
        Serial.print(", Opp: ");
        Serial.print(joystickData.opp);
        Serial.print(", Same: ");
        Serial.println(joystickData.sm);

       int servosame1 = map(joystickData.sm, 0, 1023, 0, 180);
       int servosame2 = map(joystickData.sm, 0, 1023, 180, 0);

        if (servosame1 >= 0 && servosame1 <= 127) {
            servo1.write(servosame1);
        }

        if (servosame2 >= 53 && servosame2 <= 180) {
            servo2.write(servosame2);
        }

       int servoopp1 = map(joystickData.opp, 0, 1023, 180, 0);
       int servoopp2 = map(joystickData.opp, 0, 1023, 180, 0);

        if (servoopp1 >= 53 && servoopp1 <= 135) {
            servo1.write(servoopp1);
        }

        if (servoopp2 >= 53 && servoopp2 <= 135) {
            servo2.write(servoopp2);
        }
        
        delay(100);
    }
}
