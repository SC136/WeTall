#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin not used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define joystick pins
#define JOYSTICK1_X_PIN A1  // First joystick X-axis
#define JOYSTICK1_Y_PIN A0  // First joystick Y-axis
#define JOYSTICK2_X_PIN A2  // Second joystick X-axis
#define JOYSTICK2_Y_PIN A3  // Second joystick Y-axis

RF24 radio(7, 8); // CE, CSN pins

// Expanded data structure to include both joysticks
struct Data {
  int bldc; // From first joystick X-axis (A0)
  int opp;  // From first joystick Y-axis (A1)
  int sm;   // From second joystick X-axis (A2)
  int aux;  // From second joystick Y-axis (A3)
};

Data joystickData;
unsigned long packetCount = 0;
unsigned long successCount = 0;
unsigned long lastDisplayUpdate = 0;

void setup() {
  Serial.begin(9600);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("NRF24 Transmitter");
  display.println("Initializing...");
  display.display();
  
  // Initialize radio
  radio.begin();
  radio.openWritingPipe(0xF0F0F0F0E1LL); // Same address as receiver
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening(); // Set to transmitter mode
  
  delay(1000);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("NRF24 Transmitter");
  display.println("Ready!");
  display.display();
  
  Serial.println("nRF24L01 Dual Joystick Transmitter Initialized");
}

void loop() {
  // Read values from both joysticks
  joystickData.bldc = analogRead(JOYSTICK1_X_PIN); // First joystick X
  joystickData.opp = analogRead(JOYSTICK1_Y_PIN);  // First joystick Y
  joystickData.sm = analogRead(JOYSTICK2_X_PIN);   // Second joystick X
  joystickData.aux = analogRead(JOYSTICK2_Y_PIN);  // Second joystick Y
  
  // Send data
  packetCount++;
  bool report = radio.write(&joystickData, sizeof(joystickData));
  if (report) {
    successCount++;
  }
  
  // Print values for debugging via Serial
  Serial.print("Sending: J1-X: ");
  Serial.print(joystickData.bldc);
  Serial.print(", J1-Y: ");
  Serial.print(joystickData.opp);
  Serial.print(", J2-X: ");
  Serial.print(joystickData.sm);
  Serial.print(", J2-Y: ");
  Serial.print(joystickData.aux);
  Serial.print(" | Transmission: ");
  Serial.println(report ? "Success" : "Failed");
  
  // Update OLED display approximately every 200ms
  if (millis() - lastDisplayUpdate > 200) {
    updateDisplay(report);
    lastDisplayUpdate = millis();
  }
  
  delay(100); // Match the delay in receiver for stability
}

void updateDisplay(bool lastTransmissionSuccess) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("NRF24 Transmitter");
  
  // Joystick values
  display.print("J1: ");
  display.print(joystickData.bldc);
  display.print(",");
  display.println(joystickData.opp);
  
  display.print("J2: ");
  display.print(joystickData.sm);
  display.print(",");
  display.println(joystickData.aux);
  
  // Transmission status
  display.print("Status: ");
  if (lastTransmissionSuccess) {
    display.println("SUCCESS");
  } else {
    display.println("FAILED");
  }
  
  // Packet statistics
  display.print("Packets: ");
  display.println(packetCount);
  
  display.print("Success Rate: ");
  if (packetCount > 0) {
    display.print((successCount * 100) / packetCount);
    display.println("%");
  } else {
    display.println("0%");
  }
  
  display.display();
}