#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// radio
RF24 radio(7, 8); // CE, CSN

// address for the radio communication
const byte address[6] = "00001";

// motor control
const int motorPin = 3;
int throttleValue = 0;

// servo control
Servo servoX;
Servo servoY;

struct Data_Package {
  int throttle;
  int servoX;
  int servoY;
};

Data_Package data;

void setup() {
  Serial.begin(9600); // initialize serial monitor
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  pinMode(motorPin, OUTPUT);

  // attach servos to pins
  servoX.attach(9);
  servoY.attach(10);
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));

    // map and control the motor speed (throttle)
    throttleValue = map(data.throttle, 0, 1023, 0, 255);
    analogWrite(motorPin, throttleValue);

    // map and control the servos
    int servoXVal = map(data.servoX, 0, 1023, 0, 180);
    int servoYVal = map(data.servoY, 0, 1023, 0, 180);

    servoX.write(servoXVal);
    servoY.write(servoYVal);

    // log data to serial monitor
    Serial.print("Throttle: ");
    Serial.print(throttleValue);
    Serial.print(" | ServoX: ");
    Serial.print(servoXVal);
    Serial.print(" | ServoY: ");
    Serial.println(servoYVal);
  }

  delay(20);
}
