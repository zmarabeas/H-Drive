#include "RF24.h"
#include "printf.h"
#include <Servo.h>

Servo fl;
Servo fr;
Servo bl;
Servo br;

RF24 radio(9, 10);
const byte data_pipe[6] = "00001";


typedef struct controller{
  uint8_t lx;
  uint8_t ly;
  uint8_t rx;  
  }controller;

void setup() {
  Serial.begin(9600);
  printf_begin();

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  fl.attach(5);
  fr.attach(6);
  br.attach(7);
  bl.attach(8);
  fl.write(90);
  fr.write(90);
  bl.write(90);
  br.write(90);
  delay(2000);
  
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, data_pipe);
  radio.startListening();
  radio.printDetails();
}

void loop() {
  if (radio.available())
  {
    int payload_size = radio.getDynamicPayloadSize();
    if (payload_size > 1)
    {
      controller inputs;
      radio.read(&inputs, payload_size);
      int lx = map(inputs.lx, 0, 255, 0, 180);
      //91 is 0 after mapping
      if(lx == 91){
          lx = 90;
        }
      int ly = map(inputs.ly, 0, 255, 0, 180);
      if(ly == 91){
          ly = 90;
        }
      int rx = map(inputs.rx, 0, 255, 0, 180);
      if(rx == 91){
          rx = 90;
        }

      //have to make negative inputs for algorithm to work
      lx -= 90;
      ly -= 90;
      rx -= 90;

      //fl = -ly - lx -rx
      //fr = ly - lx - rx
      //br = ly + lx - rx
      //bl = -ly + lx - rx
      int drive_fl = -ly - lx - rx;
      drive_fl = constrain(drive_fl, -90, 90);
      int drive_fr = ly - lx - rx;
      drive_fr = constrain(drive_fr, -90, 90);
      int drive_br = ly + lx - rx;
      drive_br = constrain(drive_br, -90, 90);
      int drive_bl = -ly + lx - rx;
      drive_bl = constrain(drive_bl, -90, 90);

      //make positive again
      drive_fl += 90;
      drive_fr += 90;
      drive_br += 90;
      drive_bl += 90;

      //fl and fr, 0 is forward
      //br and bl, 180 is forward
      //invert fr and bl
      drive_fr = map(drive_fr, 0, 180, 180, 0);
      drive_bl = map(drive_bl, 0, 180, 180, 0);

      //30 and 160 is max power for some reason
      drive_fl = constrain(drive_fl, 30,160);
      drive_fr = constrain(drive_fr, 30,160);
      drive_bl = constrain(drive_bl, 30,160);
      drive_br = constrain(drive_br, 30,160);

      printf("fl: %d, fr: %d, br: %d, bl: %d\n", drive_fl, drive_fr, drive_br, drive_bl);
      
      fl.write(drive_fl);
      fr.write(drive_fr);
      bl.write(drive_bl);
      br.write(drive_br);

      
    }
  }
  
}
