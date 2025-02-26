#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP32Servo.h> // Use ESP32Servo for ESP32

RF24 radio(4, 5); // CE, CSN pins

#include <stdint.h> // Include for fixed-width types

// Define a packed struct to hold joystick data
struct __attribute__((packed)) Data {
    int16_t bldc; // Joystick X-axis (BLDC throttle control)
    int16_t opp;  // Opposite servo control
    int16_t sm;   // Same servo control
};

Data joystickData;

Servo servo1;
Servo servo2;

// Define ESC pins
const int escPin1 = 12; // ESC 1 pin
const int escPin2 = 14; // ESC 2 pin

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

    servo1.attach(2);   // Attach servo1 to pin 2
    servo2.attach(13);  // Attach servo2 to pin 13

    servo1.write(90); 
    servo2.write(90);

    // Initialize both ESCs with minimum throttle
    sendThrottleSignal(escPin1, 1000);
    sendThrottleSignal(escPin2, 1000);
    delay(2000); // Wait for ESCs to initialize

    Serial.println("Setup complete. Waiting for data...");
}

void loop() {
    if (radio.available()) {
        radio.read(&joystickData, sizeof(joystickData));

        Serial.print("Received: ");
        Serial.print("BLDC: ");
        Serial.print(joystickData.bldc);
        Serial.print(", Opp: ");
        Serial.print(joystickData.opp);
        Serial.print(", Same: ");
        Serial.println(joystickData.sm);

        // Map joystick data to ESC throttle range
        int throttle1 = map(joystickData.bldc, 0, 1023, 1100, 2000); // Set minimum throttle to 1100
        int throttle2 = map(joystickData.bldc, 0, 1023, 1100, 2000); // Set minimum throttle to 1100

        // Send throttle signal to ESCs
        sendThrottleSignal(escPin1, throttle1);
        sendThrottleSignal(escPin2, throttle2);

        // Control servos
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

// Function to send PWM signal to ESC
void sendThrottleSignal(int pin, int pulse) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(pin, LOW);
    delayMicroseconds(20000 - pulse); // Maintain 50Hz (20ms period)
}
