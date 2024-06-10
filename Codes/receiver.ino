
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

const uint64_t pipeIn = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

struct ReceivedData {
    byte ch1_throttle;
    byte ch2_servo;
}

ReceivedData data;

Servo ch1;
Servo ch2;

int ch1_value = 0;
int ch2_value = 0;

void ResetData()
{
    data.ch1_throttle = 0;
    data.ch2_servo = 0;
}

void setup()
{
    radio.begin();
    radio.openReadingPipe (1,pipeIn);
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.startListening();
    
    ResetData();
    
    ch1.attach(2);
    ch2.attach(3);
}

unsigned long lastRecvTime = 0;

void ReceiveData()
{
    while ( radio.available() ) {
        radio.read(&data, sizeof(ReceivedData));
        lastRecvTime = millis();
    }
}

void loop()
{
    ReceiveData();
    unsigned long now = millis();
    if ( now - lastRecvTime > 1000 ) {
        ResetData();
    }
    
    ch1_value = map(data.ch1_throttle, 0, 255, 1000, 2000);
    ch2_value = map(data.ch2_servo, 0, 255, 0, 180);

    ch1.writeMicroseconds(ch1_value);
    ch2.write(ch2_value);
}
