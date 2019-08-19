#include "RF24.h"
#include "printf.h"

RF24 radio(9, 10);
const int vlx = 0;
const int vly = 1;
const int vrx = 2;
const byte data_pipe[6] = "00001";


typedef struct controller{
  uint8_t lx;
  uint8_t ly;
  uint8_t rx;  
  }controller;

void setup() {
  Serial.begin(9600);
  printf_begin();
  
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(data_pipe);
  radio.stopListening();
  radio.printDetails();
}

controller inputs;
void loop() {
  inputs.lx = map(analogRead(vlx), 0, 1024, 0, 255);
  inputs.ly = map(analogRead(vly), 0, 1024, 0, 255);
  inputs.rx = map(analogRead(vrx), 0, 1024, 0, 255);
  
  radio.write(&inputs, sizeof(controller));
  printf("sending : lx: %d ly: %d rx: %d\r\n", inputs.lx, inputs.ly, inputs.rx);
  delay(1);
}
