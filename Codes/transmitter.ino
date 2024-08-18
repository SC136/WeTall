#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// radio
RF24 radio(7, 8); // CE, CSN

// address for radio communication
const byte address[6] = "00001";

// joystick pins
const int joy1X = A0;
const int joy1Y = A1;
const int joy2X = A2;
const int joy2Y = A3;

struct Data_Package {
  int throttle;
  int servoX;
  int servoY;
};

Data_Package data;

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(joy1X, INPUT);
  pinMode(joy1Y, INPUT);
  pinMode(joy2X, INPUT);
  pinMode(joy2Y, INPUT);
}

void loop() {
  // read the joystick values
  data.throttle = analogRead(joy1Y); // for throttle control
  data.servoX = analogRead(joy2X);   // for servo X-axis
  data.servoY = analogRead(joy2Y);   // for servo Y-axis

  // send the data
  radio.write(&data, sizeof(Data_Package));

  delay(20);
}
