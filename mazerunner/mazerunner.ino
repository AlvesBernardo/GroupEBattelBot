// Neopixel library to simplify the use of Neopixel LEDs
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
// Servo library to make it easier to use the gripper
#include <Servo.h>

/*
 * While developing the software for the robot, we need to log crucial information from the 
 * sensors and code to know how it perceives and reacts to its environment.
 * However, the debugging tools use up time and available memory, so we need 
 * a way to disable them without introducing an additional run-time cost to the program.
 * Preprocessor commands remove code that does not pass the ifdef or ifndef from the file
 * therefore, minimizing memory usage when not in debug mode
 */
#define DEBUG
// TODO: remove all debug-related preprocessor commands and code before submitting the project

// pin constants
const int NeoPixelPin = 2;  // used by neopixel library to control the output on the pixels
const int SonarTrigPin = 4; // distance sensor requires an input pin to start sending pulses
const int SonarEchoPin = 7; // distance sensor uses this pin to output the signal
const int RotorPin = 8;     // rotates the distance sensor
const int GripperPin = 9;   // used by servo library to control the gripper
const int MotorFLPin = 10; // left motor forward motion
const int MotorBLPin = 11; // left motor backward motion
const int MotorFRPin = 6; // right motor forward motion
const int MotorBRPin = 5; // right motor backward motion
const int PinR2 = 12;
const int PinR1 = 13;

// Neopixels
const int totalPixels = 4;
const int pixelRearLeft = 0;
const int pixelRearRight = 1;
const int pixelFrontRight = 2;
const int pixelFrontLeft = 3;
Adafruit_NeoPixel pixels(totalPixels, NeoPixelPin, NEO_GRB + NEO_KHZ800);

// some colors to simplify the use of sensors
const uint32_t blue_green = pixels.Color(200, 0, 50);
const uint32_t green = pixels.Color(255, 0, 0);
const uint32_t red = pixels.Color(0, 255, 0);
const uint32_t blue = pixels.Color(0, 0, 255);
const uint32_t yellow = pixels.Color(155, 255, 0);
const uint32_t cyan = pixels.Color(255, 0, 255);
const uint32_t magenta = pixels.Color(0, 255, 255);
const uint32_t orange = pixels.Color(51, 255, 0);
const uint32_t white = pixels.Color(255, 255, 255);

Servo gripper;
Servo rotor;

const int NumLineSensors = 8;

const int STRAIGHT = 0;
const int SLEFT = 1;
const int LEFT = 2;
const int ELEFT = 3;
const int SRIGHT = 4;
const int RIGHT = 5;
const int ERIGHT = 6;

int rotorDirection = STRAIGHT;


// LINE SENSORS
int g_darkThreshold = 650;
int lineSensors[] = {A6, A7, A1, A0, A2, A3, A4, A5}; // pins must be in order from left to right
void initLineSensor() {
  // initialize all the pins before using them
  for (int pin : lineSensors) {
    pinMode(pin, INPUT);
  }
}
#ifdef DEBUG
void logLineSensors() {
  Serial.print("Line Sensor Readings: ");
  for (int pin : lineSensors) {
    Serial.print(analogRead(pin));
    Serial.print(", ");
  }
  Serial.print("\n");
}
#endif
boolean isAllDark() {
  int reading;
  for (int pin : lineSensors) {
    reading = analogRead(pin);
    if (reading < g_darkThreshold) {
      return false;
    }
  }
  return true;
}

double getDistanceCm() {
  digitalWrite(SonarTrigPin, LOW); // clean any pulses
  delayMicroseconds(5);
  digitalWrite(SonarTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(SonarTrigPin, LOW);
  double duration = pulseIn(SonarEchoPin, HIGH);
  double distance = (duration/2) / 29.1;
  digitalWrite(SonarTrigPin, LOW); // cleaning up the pulses
  #ifdef DEBUG
  Serial.print("Distance to object: ");
  Serial.print(distance);
  Serial.print("cm. \n");
  #endif
  return distance;
}

const int speed_left = 255;
const int speed_right = 255;

void releaseGripper() {
  gripper.write(120);
  #ifdef DEBUG
  Serial.print("releaseGripper()\n");
  #endif
}
void holdGripper() {
  gripper.write(50);
  #ifdef DEBUG
  Serial.print("holdGripper()\n");
  #endif
}


void readR1R2() {
  auto r1 = pulseIn(PinR1, HIGH);
  auto r2 = pulseIn(PinR2, HIGH);
  Serial.print("R1: ");
  Serial.print(r1);
  Serial.print("; R2: ");
  Serial.print(r2);
  Serial.print("\n");
}

void forward() {
  #ifdef DEBUG
  Serial.print("forward()\n");
  #endif
  analogWrite(MotorFLPin, speed_left);
  analogWrite(MotorFRPin, speed_right);
  analogWrite(MotorBLPin, 0);
  analogWrite(MotorBRPin, 0);
}
void turnLeft() {
  #ifdef DEBUG
  Serial.print("turnLeft()\n");
  #endif
  analogWrite(MotorFLPin, 0);
  analogWrite(MotorFRPin, speed_left);
  analogWrite(MotorBLPin, speed_right);
  analogWrite(MotorBRPin, 0);
} 
void turnRight() {
  #ifdef DEBUG
  Serial.print("turnRight()\n");
  #endif
  analogWrite(MotorFLPin, speed_left);
  analogWrite(MotorFRPin, 0);
  analogWrite(MotorBLPin, 0);
  analogWrite(MotorBRPin, speed_right);
}
void backward() {
  #ifdef DEBUG
  Serial.print("backward()\n");
  #endif
  analogWrite(MotorBLPin, speed_left);
  analogWrite(MotorBRPin, speed_right);
  analogWrite(MotorFLPin, 0);
  analogWrite(MotorFRPin, 0);
}
void halt() {
  #ifdef DEBUG
  Serial.print("halt()\n");
  #endif
  analogWrite(MotorBLPin, 0);
  analogWrite(MotorBRPin, 0);
  analogWrite(MotorFLPin, 0);
  analogWrite(MotorFRPin, 0);
}

void rotorExtremeLeft() {
  #ifdef DEBUG
  Serial.print("rotorExtremeLeft()\n");
  #endif
  rotorDirection = ELEFT;
  rotor.write(180);
}
void rotorLeft() {
  #ifdef DEBUG
  Serial.print("rotorLeft()\n");
  #endif
  rotorDirection = LEFT;
  rotor.write(135);
}
void rotorSlightLeft() {
  #ifdef DEBUG
  Serial.print("rotorSlightLeft()\n");
  #endif
  rotorDirection = SLEFT;
  rotor.write(105);
}
void rotorStraight() {
  #ifdef DEBUG
  Serial.print("rotorStraight()\n");
  #endif
  rotorDirection = STRAIGHT;
  rotor.write(90);
}
void rotorSlightRight() {
  #ifdef DEBUG
  Serial.print("rotorSlightRight()\n");
  #endif
  rotorDirection = SRIGHT;
  rotor.write(75);
}
void rotorRight() {
  #ifdef DEBUG
  Serial.print("rotorRight()\n");
  #endif
  rotorDirection = RIGHT;
  rotor.write(45);
}
void rotorExtremeRight() {
  #ifdef DEBUG
  Serial.print("rotorExtremeRight()\n");
  #endif
  rotorDirection = ERIGHT;
  rotor.write(0);
}
void frontLights(uint32_t color) {
  pixels.setPixelColor(pixelFrontLeft, color);
  pixels.setPixelColor(pixelFrontRight, color);
  pixels.show();
}
void rearLights(uint32_t color) {
  pixels.setPixelColor(pixelRearLeft, color);
  pixels.setPixelColor(pixelRearRight, color);
  pixels.show();
}
void allLights(uint32_t color) {
  pixels.setPixelColor(pixelFrontLeft, color);
  pixels.setPixelColor(pixelFrontRight, color);
  pixels.setPixelColor(pixelRearLeft, color);
  pixels.setPixelColor(pixelRearRight, color);
  pixels.show();
}
void leftLights(uint32_t color) {
  pixels.setPixelColor(pixelFrontLeft, color);
  pixels.setPixelColor(pixelRearLeft, color);
  pixels.show();
}
void rightLights(uint32_t color) {
  pixels.setPixelColor(pixelRearRight, color);
  pixels.setPixelColor(pixelFrontRight, color);
  pixels.show();
}
void clearLights() {
  pixels.clear();
  pixels.show();
}

void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  pinMode(SonarTrigPin, OUTPUT);
  pinMode(SonarEchoPin, INPUT);
  pinMode(NeoPixelPin, OUTPUT);
  pinMode(RotorPin, OUTPUT);
  pinMode(GripperPin, OUTPUT);
  pinMode(MotorFLPin, OUTPUT);
  pinMode(MotorBLPin, OUTPUT);
  pinMode(MotorFRPin, OUTPUT);
  pinMode(MotorBRPin, OUTPUT);
  pixels.begin();
  pixels.clear();
  allLights(white);
  gripper.attach(GripperPin);
  rotor.attach(RotorPin);
  holdGripper();
  rotorStraight();
  initLineSensor();
}

const int distanceThreshold = 15;
const int adjustmentInterval = 3;
auto lastAdjustment = millis();
const double adjustmentThreshold = 1.1;

void loop() {
  double distance = getDistanceCm();
  if (rotorDirection == STRAIGHT) {
    if (distance < distanceThreshold) {
      halt();
      frontLights(red);
      rotorSlightLeft();
      leftLights(yellow);
      delay(500);
    } else {
    forward();
    pixels.clear();
    pixels.show();
    }
  }
  else {
    if (distance > distanceThreshold) {
      switch (rotorDirection) {
        case SLEFT:
        case LEFT:
        case ELEFT:
          rotorStraight();
          delay(1000);
          while (getDistanceCm() < distanceThreshold) {
            turnLeft();
            leftLights(orange);
          }
          break;
        case SRIGHT:
        case RIGHT:
        case ERIGHT:
          rotorStraight();
          delay(1000);
          while (getDistanceCm() < distanceThreshold) {
            turnRight();
            rightLights(orange);
          }
          break;
      }
    } else {
      switch(rotorDirection) {
        case SLEFT:
          rotorLeft();
          leftLights(yellow);
          delay(500);
          break;
        case LEFT:
          rotorExtremeLeft();
          leftLights(yellow);
          delay(500);
          break;
        case ELEFT:
          rotorSlightRight();
          rightLights(yellow);
          delay(1500);
          break;
        case SRIGHT:
          rotorRight();
          rightLights(yellow);
          delay(500);
          break;
        case RIGHT:
          rotorExtremeRight();
          rightLights(yellow);
          delay(500);
        case ERIGHT:
          rotorStraight();
          frontLights(cyan);
          delay(500);
        case STRAIGHT:
          rotorSlightLeft();
          leftLights(yellow);
          backward();
          delay(500);
      }
    }
  }
  
}
