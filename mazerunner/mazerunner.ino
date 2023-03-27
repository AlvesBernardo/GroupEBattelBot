// Warning! Achtung! Attention! Увага! Внимание! Atenţie! 
// DO NOT, UNDER ANY CIRCUMSTANCES, HAVE ANYTHING CONNECTED TO PINS 0 OR 1 WHEN THE PROGRAM UPLOADS
// DO NOT USE PIN 0 OR PIN 1 UNLESS SERIAL COMMUNICATION HAS BEEN DISABLED

// Neopixel libraries
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
// Servo library
#include <Servo.h>
// Line sensor library
#include <QTRSensors.h>
/*
 * Serial communication uses either pin 1 or 0, which we need for the bluetooth module, since we have ran out of all other pins.
 * bluetooth module must be disconnected when the program is being uploaded to the chip.
*/
#define MAZERUNNER_ENABLE_SERIAL
#define MAZERUNNER_ENABLE_BLUETOOTH
#undef MAZERUNNER_ENABLE_BLUETOOTH

/*  PINS: 2-13 are "normal pins", with the exception of 3, 5, 6, 9, 10, 11 are PWM pins
 *  however, the Servo library disables PWM mode on pins 9 and 10, so use them as normal pins
*/
const int NeoPixelPin = 2; // pin used by the NeoPixel library to control the NeoPixels
const int GripperPin = 9;  // controls the gripper
// motor pins
const int MotorFLPin = 3;  // left forward motion
const int MotorBLPin = 11; // left backward motion
const int MotorFRPin = 6;  // right forward motion
const int MotorBRPin = 5;  // right backward motion
// sensor pins
const int SensorLeftTrigPin = 13;  //  left distance sensor controls
const int SensorFrontTrigPin = 4;  // front distance sensor controls
const int SensorRightTrigPin = 8;  // right distance sensor controls
const int SensorLeftEchoPin = 12;  //  left distance sensor readings
const int SensorFrontEchoPin = 7;  // front distance sensor readings
const int SensorRightEchoPin = 10; // right distance sensor readings


const double ErrorMargin = 0.10;
bool isWithinPercRange(double value1, double value2, double percentageRange=ErrorMargin) {
    if (value2 > value1) {
        // swap two values;
        value2 = value2 + value1;
        value1 = value2 - value1;
        value2 = value2 - value1;
    }
    double percentageDiff = (value1 - value2) / value1;
    return percentageDiff <= percentageRange;  
}

// NeoPixel-related code
const int NumberOfNeoPixels = 4;
const int PixelBackLeft = 0;
const int PixelBackRight = 1;
const int PixelFrontRight = 2;
const int PixelFrontLeft = 3;

// the NeoPixel object we are going to use everytime we require to work with the pixels
Adafruit_NeoPixel pixels(NumberOfNeoPixels, NeoPixelPin, NEO_GRB + NEO_KHZ800);

// colors (require a fixed-size 4 byte int -> uint32_t in Green, Red, Blue format)
const uint32_t green = pixels.Color(255, 0, 0);
const uint32_t red = pixels.Color(0, 255, 0);
const uint32_t blue = pixels.Color(0, 0, 255);
const uint32_t yellow = pixels.Color(155, 255, 0);
const uint32_t cyan = pixels.Color(255, 0, 255);
const uint32_t magenta = pixels.Color(0, 255, 255);
const uint32_t orange = pixels.Color(51, 255, 0);
const uint32_t white = pixels.Color(255, 255, 255);
const uint32_t aqua = pixels.Color(200, 0, 50);

void clearPixels() {
    pixels.clear();
    pixels.show();
}
void fillPixels(uint32_t color) {
    pixels.setPixelColor(PixelFrontLeft, color);
    pixels.setPixelColor(PixelFrontRight, color);
    pixels.setPixelColor(PixelBackLeft, color);
    pixels.setPixelColor(PixelBackRight, color);
    pixels.show();
}
void frontPixels(uint32_t color) {
    pixels.setPixelColor(PixelFrontLeft, color);
    pixels.setPixelColor(PixelFrontRight, color);
    pixels.show();
}
void rearPixels(uint32_t color) {
    pixels.setPixelColor(PixelBackLeft, color);
    pixels.setPixelColor(PixelBackRight, color);
    pixels.show();
}
void leftPixels(uint32_t color) {
    pixels.setPixelColor(PixelFrontLeft, color);
    pixels.setPixelColor(PixelBackLeft, color);
    pixels.show();
}
void rightPixels(uint32_t color) {
    pixels.setPixelColor(PixelFrontRight, color);
    pixels.setPixelColor(PixelBackRight, color);
    pixels.show();
}

// motors: control the movement of the robot
const int MoveSpeedLeft = 255;
const int MoveSpeedRight = 255;
const int SlowSpeedLeft = 125;
const int SlowSpeedRight = 150;

void stopMotors() {
    analogWrite(MotorFLPin, 0);
    analogWrite(MotorFRPin, 0);
    analogWrite(MotorBLPin, 0);
    analogWrite(MotorBRPin, 0);
}

void moveForward(int speedLeft=MoveSpeedLeft, int speedRight=MoveSpeedRight) {
    analogWrite(MotorFLPin, speedLeft);
    analogWrite(MotorFRPin, speedRight);
    analogWrite(MotorBLPin, 0);
    analogWrite(MotorBRPin, 0);
}
void moveBackward(int speedLeft=MoveSpeedLeft, int speedRight=MoveSpeedRight) {
    analogWrite(MotorFLPin, 0);
    analogWrite(MotorFRPin, 0);
    analogWrite(MotorBLPin, speedLeft);
    analogWrite(MotorBRPin, speedRight);
}
void turnLeft(int speedLeft=MoveSpeedLeft, int speedRight=MoveSpeedRight) {
    analogWrite(MotorFLPin, 0);
    analogWrite(MotorFRPin, speedLeft);
    analogWrite(MotorBLPin, speedRight);
    analogWrite(MotorBRPin, 0);
}
void turnRight(int speedLeft=MoveSpeedLeft, int speedRight=MoveSpeedRight) {
    analogWrite(MotorFLPin, speedLeft);
    analogWrite(MotorFRPin, 0);
    analogWrite(MotorBLPin, 0);
    analogWrite(MotorBRPin, speedRight);
}
void turnLeftDelay(int mult = 1) {
    turnLeft();
    delay(600*mult);
    stopMotors();
}
void turnRightDelay(int mult = 1) {
    turnRight();
    delay(600*mult);
    stopMotors();
}

// distance Sensors
double getDistance(int trigPin, int echoPin, char sensorName='U') {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigPin, LOW);
    double duration = pulseIn(echoPin, HIGH);
    double distance = (duration/2) / 29.1;
    #ifdef MAZERUNNER_ENABLE_SERIAL
    Serial.print("HC-SR04 (");
    Serial.print(sensorName);
    Serial.print("): >>> ");
    Serial.print(distance);
    Serial.println(" cm.");
    #endif
    return distance;
}
double getLeftDistance() {
    return getDistance(SensorLeftTrigPin, SensorLeftEchoPin, 'L');
}
double getFrontDistance() {
    return getDistance(SensorFrontTrigPin, SensorFrontEchoPin, 'F');
}
double getRightDistance() {
    return getDistance(SensorRightTrigPin, SensorRightEchoPin, 'R');
}

//line sensor
QTRSensors lineSensor;

const uint8_t SensorCount = 8;
const uint8_t LineSensors[SensorCount] = {A6, A7, A1, A0, A2, A3, A4, A5};
uint16_t g_prevSensorReading[SensorCount];
uint16_t g_sensorReading[SensorCount];

void initLineSensors() {
    for (int pin : LineSensors)  {
        pinMode(pin, INPUT);
    }
    lineSensor.setTypeAnalog();
    lineSensor.setSensorPins(LineSensors, SensorCount);
}
void calibrateLineSensors() {
    fillPixels(cyan);
    #ifdef MAZERUNNER_ENABLE_SERIAL
    Serial.print("Calibrating the line sensors");
    #endif
    unsigned long timeAtStart = millis();
    moveForward();
    delay(50);
    moveForward(SlowSpeedLeft, SlowSpeedRight) ;
    for (int i = 0; i < 70; i++) {
        lineSensor.calibrate();
    }
    stopMotors();
    clearPixels();
}

// Gripper
bool g_isHoldingObject = false;
Servo gripper;

void openGripper() {
    gripper.write(120);
    g_isHoldingObject = false;
}

void closeGripper() {
    gripper.write(50);
    g_isHoldingObject = true;
}

/*  if bluetooth communication is enabled, the robot must wait until it receives a signal from another robot
 *  before starting the race
 *  otherwise, it should start running immediately
*/
#ifdef MAZERUNNER_ENABLE_BLUETOOTH
bool g_receivedActivationSignal = false;
#else
bool g_receivedActivationSignal = true;
#endif

int g_tick = 0;
double g_prevReadingLeft = 9999;
double g_prevReadingFront = 9999;
double g_prevReadingRight = 9999;

double g_distanceLeft = getLeftDistance();
double g_distanceFront = getFrontDistance();
double g_distanceRight = getRightDistance();

const double DistanceThreshold = 15;
const int DirectionLeft = 1;
const int DirectionRight = -1;
bool g_lookForNearestTurn = false;

void currentReadings() {
    g_distanceLeft = getLeftDistance();
    g_distanceFront = getFrontDistance();
    g_distanceRight = getRightDistance();
}

void updatePrevReadings() {
    g_prevReadingLeft = g_distanceLeft;
    g_prevReadingFront = g_distanceFront;
    g_prevReadingRight = g_distanceRight;
}

void turnToTarget(int turnDirection) {
    double *currentSide, *prevSide;
    updatePrevReadings();
    switch(turnDirection) {
    case DirectionLeft:
        Serial.println("Left");
        currentSide = &g_distanceLeft;
        prevSide = &g_prevReadingRight;
        turnLeft();
        break;
    case DirectionRight:
        Serial.println("Right");
        currentSide = &g_distanceRight;
        prevSide = &g_prevReadingLeft;
        turnRight();
        break;
    }
    bool allowExit = false;
    do {
        currentReadings();
        if (!allowExit && (!isWithinPercRange(*currentSide, g_prevReadingFront) || (!isWithinPercRange(*prevSide, g_distanceFront)))) {
            allowExit = true;
        }
        Serial.println(*currentSide);
        Serial.println(g_prevReadingFront);
        Serial.println(*prevSide);
        Serial.println(g_distanceFront);
        Serial.println(allowExit);
        Serial.println(isWithinPercRange(*currentSide, g_prevReadingFront));
        Serial.println(isWithinPercRange(*prevSide, g_distanceFront));
        
    } while (!allowExit || !isWithinPercRange(*currentSide, g_prevReadingFront) || (!isWithinPercRange(*prevSide, g_distanceFront)));
    stopMotors();
    clearPixels();
    delay(2000);
}

void tick() {
    if (g_tick % 100 == 0) {
        updatePrevReadings();
    }
    g_tick++;
}

void setup() {
    #ifdef MAZERUNNER_ENABLE_SERIAL
    Serial.begin(9600);
    
    #endif
    pinMode(NeoPixelPin, OUTPUT);
    pinMode(MotorFLPin, OUTPUT);
    pinMode(MotorFRPin, OUTPUT);
    pinMode(MotorBLPin, OUTPUT);
    pinMode(MotorBRPin, OUTPUT);
    pinMode(SensorLeftTrigPin, OUTPUT);
    pinMode(SensorLeftEchoPin, INPUT);
    pinMode(SensorFrontTrigPin, OUTPUT);
    pinMode(SensorFrontEchoPin, INPUT);
    pinMode(SensorRightTrigPin, OUTPUT);
    pinMode(SensorRightEchoPin, INPUT);
    pinMode(GripperPin, OUTPUT);
    initLineSensors();
    gripper.attach(GripperPin);
    openGripper();
    pixels.begin();
    pixels.setBrightness(255);
    fillPixels(white);
}

void loop() {
    moveForward();
    frontPixels(green);
    delay(1000);
    clearPixels();
    turnLeft();
    leftPixels(green);
    delay(1000);
    moveBackward();
    frontPixels(green);
    delay(1000);
    clearPixels();
    turnRight();
    rightPixels(green);
    delay(1000);
    stopMotors();
    openGripper();
    delay(1000);
    closeGripper();
    currentReadings();
}
