#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

#define I2C_SLAVE_ADDRESS 0x08

RF24 radio(7, 8); // CE, CSN pins

// Match the data structure with the transmitter
struct Data {
  int bldc; // From first joystick X-axis (A0)
  int opp;  // From first joystick Y-axis (A1)
  int sm;   // From second joystick X-axis (A2)
  int aux;  // From second joystick Y-axis (A3)
};

Data joystickData;

// Flag to track if new data has been received
bool newDataReceived = false;

void setup() {
  // Initialize I2C as slave
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  
  // Initialize radio
  radio.begin();
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL); // Set address for reading
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening(); // Set to receiver mode
}

void loop() {
  if (radio.available()) {
    // Read the incoming data
    radio.read(&joystickData, sizeof(joystickData));
    newDataReceived = true;
  }
  
  delay(10); // Small delay for stability
}

// Function that executes whenever data is requested by master
void requestEvent() {
  // Create a data packet with our raw values
  uint8_t buffer[10];
  
  // Pack the values - sending raw values now for ESP32 to map
  buffer[0] = (joystickData.bldc >> 8) & 0xFF;  // High byte
  buffer[1] = joystickData.bldc & 0xFF;         // Low byte
  
  buffer[2] = (joystickData.opp >> 8) & 0xFF;   // High byte
  buffer[3] = joystickData.opp & 0xFF;          // Low byte
  
  buffer[4] = (joystickData.sm >> 8) & 0xFF;    // High byte
  buffer[5] = joystickData.sm & 0xFF;           // Low byte
  
  buffer[6] = (joystickData.aux >> 8) & 0xFF;   // High byte
  buffer[7] = joystickData.aux & 0xFF;          // Low byte
  
  buffer[8] = newDataReceived ? 1 : 0;          // New data flag
  buffer[9] = 0;                                // Reserved for future use
  
  // Send the data
  Wire.write(buffer, 10);
  
  // Reset the new data flag
  newDataReceived = false;
}