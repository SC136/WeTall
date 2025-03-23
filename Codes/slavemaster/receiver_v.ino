#include <Wire.h>
#include <ESP32Servo.h>

#define I2C_SLAVE_ADDRESS 0x08

// ESP32 pins for servos and ESC
#define SERVO1_PIN 2     // D2 on ESP32
#define SERVO2_PIN 13    // D13 on ESP32
#define ESC1_PIN 12      // D12 on ESP32 - BLDC ESC
#define ESC2_PIN 14      // D14 on ESP32 - BLDC ESC

// Create servo objects
Servo servo1;
Servo servo2;
Servo esc1;   // Using Servo library for ESC control
Servo esc2;

// Variables to store received raw values
int bldc_raw = 0;
int opp_raw = 0;
int sm_raw = 0;
int aux_raw = 0;

// Variables to store mapped values
int bldc_output = 0;
int opp_output = 0;
int sm_output = 0;
int aux_output = 0;

bool newData = false;

// Timing
unsigned long lastRequestTime = 0;
const int requestInterval = 50;  // Request data every 50ms

// For ESC throttle verification
const int MIN_THROTTLE = 1000;
const int MAX_THROTTLE = 2000;
const int THROTTLE_CUTOFF = 1050;  // Anything below this is considered zero throttle

void setup() {
  // Enable serial for debugging
  Serial.begin(115200);
  Serial.println("ESC Initialization Starting");
  
  // Initialize I2C as master
  Wire.begin();
  
  // ESP32 Servo initialization
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // Attach servos to pins
  servo1.setPeriodHertz(50);      // Standard 50Hz servo
  servo1.attach(SERVO1_PIN, 500, 2400);
  
  servo2.setPeriodHertz(50);      // Standard 50Hz servo
  servo2.attach(SERVO2_PIN, 500, 2400);
  
  Serial.println("Starting ESC calibration sequence...");
  
  // Create ESC objects but don't attach them yet
  esc1.setPeriodHertz(60);
  esc2.setPeriodHertz(60);
  
  // Standard ESC calibration sequence
  Serial.println("1. Setting ESCs to max throttle position");
  esc1.attach(ESC1_PIN, MIN_THROTTLE, MAX_THROTTLE);
  esc2.attach(ESC2_PIN, MIN_THROTTLE, MAX_THROTTLE);
  esc1.writeMicroseconds(MAX_THROTTLE);
  esc2.writeMicroseconds(MAX_THROTTLE);
  delay(2000);  // Some ESCs need to see max signal first
  
  Serial.println("2. Setting ESCs to min throttle position");
  esc1.writeMicroseconds(MIN_THROTTLE);
  esc2.writeMicroseconds(MIN_THROTTLE);
  delay(5000);  // Allow time for ESCs to recognize min throttle
  
  Serial.println("3. ESC calibration complete, ready to arm");
  
  // Set servos to center position
  servo1.write(90);
  servo2.write(90);
  
  Serial.println("Setup complete. ESCs should now respond to throttle input.");
}

void loop() {
  // Request data from Arduino at regular intervals
  if (millis() - lastRequestTime > requestInterval) {
    requestDataFromArduino();
    lastRequestTime = millis();
  }
  
  // If we have new data, update servos and ESCs
  if (newData) {
    // Map the values for servos
    opp_output = map(opp_raw, 0, 1023, 0, 180);
    sm_output = map(sm_raw, 0, 1023, 0, 180);
    
    // Map throttle values with a dead zone at the bottom
    // This ensures ESC gets a definitive off signal when joystick is at minimum
    bldc_output = map(bldc_raw, 0, 1023, MIN_THROTTLE, MAX_THROTTLE);
    
    // Add a throttle cutoff threshold for safety
    if (bldc_raw < 50) {  // If joystick is near minimum
      bldc_output = MIN_THROTTLE;
    }
    
    // Apply the values
    servo1.write(opp_output);
    servo2.write(sm_output);
    esc1.writeMicroseconds(bldc_output);  // First BLDC motor uses bldc_output
    esc2.writeMicroseconds(bldc_output);  // Second BLDC motor now also uses bldc_output
    
    // Debug output
    Serial.print("BLDC Raw: "); 
    Serial.print(bldc_raw);
    Serial.print(" -> Output: ");
    Serial.print(bldc_output);
    Serial.print(" | Both ESCs set to: ");
    Serial.println(bldc_output);
    
    newData = false;
  }
  
  delay(10);  // Small delay for stability
}

void requestDataFromArduino() {
  uint8_t buffer[10];
  uint8_t bytesReceived = 0;
  
  // Request 10 bytes from the Arduino
  Wire.requestFrom(I2C_SLAVE_ADDRESS, 10);
  
  // Read the bytes if they're available
  while (Wire.available() && bytesReceived < 10) {
    buffer[bytesReceived] = Wire.read();
    bytesReceived++;
  }
  
  // If we got all the bytes we expected
  if (bytesReceived == 10) {
    // Reconstruct values from pairs of bytes
    bldc_raw = (buffer[0] << 8) | buffer[1];
    opp_raw = (buffer[2] << 8) | buffer[3];
    sm_raw = (buffer[4] << 8) | buffer[5];
    aux_raw = (buffer[6] << 8) | buffer[7];
    
    // Check if it's new data
    if (buffer[8] == 1) {
      newData = true;
    }
  }
}
