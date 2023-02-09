#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>


//defining pins
const int trig = 12;
const int echo = 13;

//motor pins
const int motorPin1 = 6;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;

// line sensor pins
const int lineSensor1 = A0;
const int lineSensor2 = A1;
const int lineSensor3 = A2;
const int lineSensor4 = A3;
const int lineSensor5 = A4;
const int lineSensor6 = A5;
const int lineSensor7 = A6;
const int lineSensor8 = A7;

// other pins
const int gripper = 8;
const int neoPix = 7;

// create servo object

Servo servo;
int angle = 10;

void setup() {
  // put your setup code here, to run once:
pinMode(gripper,OUTPUT);

//attach servo object

servo.attach(gripper);
servo.write(angle);

}

void loop() {
  // put your main code here, to run repeatedly:
  
 // scan from 0 to 180 degrees

  for (angle = 10; angle < 180; angle++)
  {
    servo.write(angle);
    delay(15);
  }
  
}
