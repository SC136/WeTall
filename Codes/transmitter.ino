#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeOut = 0xE8E8F0F0E1LL;

RF24 radio(7, 8);

struct DataToBeSent {
  byte ch1_throttle;
  byte ch2_servo;
};

DataToBeSent data;

void ResetData() {
  data.ch1_throttle = 0;
  data.ch2_servo = 0;
}

void setup() {
  radio.begin();
  radio.stopListening();
  radio.openWritingPipe(pipeOut);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);

  ResetData();
}

void loop() {
  data.ch1_throttle = map(analogRead(A0), 0, 1024, 0, 255);
  data.ch2_servo = map(analogRead(A1), 0, 1024, 0, 255);

  radio.write(&data, sizeof(DataToBeSent));
}