#include <Wire.h>
#include <ESP32Servo.h>
#include <MPU6050_tockn.h>  // Add MPU6050 library

#define I2C_SLAVE_ADDRESS 0x08
#define MPU_ADDR 0x68       // MPU6050 I2C address

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

// MPU6050 instance
MPU6050 mpu(Wire);

// Variables to store received raw values
int bldc_raw = 0;
int opp_raw = 0;  // This will now be our throttle from A1
int sm_raw = 0;
int aux_raw = 0;

// Variables to store mapped values
int throttle_output = 0;  // Renamed to clarify this is our throttle
int opp_output = 0;
int sm_output = 0;
int aux_output = 0;

// Motor output values after PID adjustment
int motor1_output = 0;
int motor2_output = 0;

bool newData = false;

// Timing
unsigned long lastRequestTime = 0;
unsigned long lastMpuReadTime = 0;
const int requestInterval = 50;  // Request data every 50ms
const int mpuInterval = 10;      // Read MPU data every 10ms

// For ESC throttle verification
const int MIN_THROTTLE = 1000;
const int MAX_THROTTLE = 2000;
const int THROTTLE_CUTOFF = 1050;  // Anything below this is considered zero throttle

// PID Constants
float Kp = 5.0;  // Proportional gain
float Ki = 0.1;  // Integral gain
float Kd = 1.0;  // Derivative gain

// PID Variables
float yError = 0;        // Current error in Y-axis rotation
float yPrevError = 0;    // Previous error for derivative calculation
float yIntegral = 0;     // Integral accumulator
float yDerivative = 0;   // Derivative term
float pidOutput = 0;     // PID controller output

// MPU data
float yAngle = 0;        // Y-axis angle from MPU
float targetAngle = 0;   // Desired angle (usually 0 for level)

void setup() {
  // Enable serial for debugging
  Serial.begin(115200);
  Serial.println("Bicopter PID Control Initialization Starting");
  
  // Initialize I2C as master
  Wire.begin();
  
  // Initialize MPU6050
  mpu.begin();
  mpu.calcGyroOffsets(true); // Calibrate gyro with output
  Serial.println("MPU6050 initialized");
  
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
  
  // Attach both ESCs simultaneously before sending any signals
  esc1.attach(ESC1_PIN, MIN_THROTTLE, MAX_THROTTLE);
  esc2.attach(ESC2_PIN, MIN_THROTTLE, MAX_THROTTLE);
  
  // Standard ESC calibration sequence - send commands to both ESCs at the same time
  Serial.println("1. Setting ESCs to max throttle position");
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
  
  // Read MPU data at regular intervals
  if (millis() - lastMpuReadTime > mpuInterval) {
    readMpuData();
    lastMpuReadTime = millis();
  }
  
  // If we have new data, update servos and ESCs
  if (newData) {
    // Map the values for servos
    sm_output = map(sm_raw, 0, 1023, 0, 180);
    
    // Map throttle values from A1 (opp_raw) instead of A0 (bldc_raw)
    // This ensures ESC gets a definitive off signal when joystick is at minimum
    throttle_output = map(opp_raw, 0, 1023, MIN_THROTTLE, MAX_THROTTLE);
    
    // Add a throttle cutoff threshold for safety
    if (opp_raw < 50) {  // If joystick is near minimum
      throttle_output = MIN_THROTTLE;
      // Reset PID integral term when throttle is off
      yIntegral = 0;
    }
    
    // Calculate PID adjustment if throttle is above minimum
    if (throttle_output > MIN_THROTTLE + 50) {
      calculatePID();
      
      // Apply PID corrections to motors
      // If bicopter tilts right (positive angle), increase left motor
      // If bicopter tilts left (negative angle), increase right motor
      motor1_output = constrain(throttle_output - pidOutput, MIN_THROTTLE, MAX_THROTTLE);
      motor2_output = constrain(throttle_output + pidOutput, MIN_THROTTLE, MAX_THROTTLE);
    } else {
      // Below cutoff threshold, both motors at minimum
      motor1_output = MIN_THROTTLE;
      motor2_output = MIN_THROTTLE;
    }
    
    // Apply the values
    servo1.write(90);  // Keep at center position since we're not using A0 controls
    servo2.write(sm_output);
    esc1.writeMicroseconds(motor1_output);
    esc2.writeMicroseconds(motor2_output);
    
    // Debug output
    Serial.print("Throttle (A1): "); 
    Serial.print(opp_raw);
    Serial.print(" -> ");
    Serial.print(throttle_output);
    Serial.print(" | Y Angle: ");
    Serial.print(yAngle);
    Serial.print(" | PID Out: ");
    Serial.print(pidOutput);
    Serial.print(" | Motors: L:");
    Serial.print(motor1_output);
    Serial.print(" R:");
    Serial.println(motor2_output);
    
    newData = false;
  }
  
  delay(10);  // Small delay for stability
}

void readMpuData() {
  mpu.update();
  
  // Get Y-axis angle (roll) from MPU
  yAngle = mpu.getAngleY();
  
  // Optional: apply complementary filter here if needed for smoother readings
}

void calculatePID() {
  // Calculate error (difference between desired angle and current angle)
  yError = targetAngle - yAngle;
  
  // Proportional term
  float pTerm = Kp * yError;
  
  // Integral term (with anti-windup)
  yIntegral += yError;
  yIntegral = constrain(yIntegral, -100, 100);  // Prevent integral windup
  float iTerm = Ki * yIntegral;
  
  // Derivative term
  yDerivative = yError - yPrevError;
  float dTerm = Kd * yDerivative;
  
  // Calculate total PID output
  pidOutput = pTerm + iTerm + dTerm;
  
  // Constrain PID output to reasonable values
  pidOutput = constrain(pidOutput, -500, 500);
  
  // Save current error for next iteration
  yPrevError = yError;
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