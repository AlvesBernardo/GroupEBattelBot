#include <Adafruit_NeoPixel.h>

#define SERIAL_LOGS

// pins
const int NeoPixelPin = A0;
const int GripperPin = A1;
const int RightWheelSensorPin = 3; // R1 rotation sensor
const int LeftWheelSensorPin = 2;  // R2 rotation sensor
const int MotorFLPin = 5; // Corresponds to A2 
const int MotorFRPin = 6; // Corresponds to B1 
const int MotorBLPin = 10; // Corresponds to A1 
const int MotorBRPin = 9; // Corresponds to B2
const int SensorFrontTrigPin = 4;
const int SensorFrontEchoPin = 7;
const int SensorLeftTrigPin = 8;
const int SensorLeftEchoPin = 11;
const int SensorRightTrigPin = 12;
const int SensorRightEchoPin = 13;
// Constants
const int LineSensorCount = 6; // number of line sensors in use, technical characteristics allow up to 8
const int LineSensorArray[LineSensorCount] =  {A7, A6, A2, A3, A4, A5};
const int LineSensorValues[LineSensorCount] = {-5, -3, -1, 1, 3, 5}; // used in calculations to represent the location of the black line as a single integer sum
// NeoPixel-related constants
const int NeoPixelCount = 4; // number of NeoPixels on the robot
const int PixelBackLeft = 0;
const int PixelBackRight = 1;
const int PixelFrontRight = 2;
const int PixelFrontLeft = 3;
Adafruit_NeoPixel pixels(NeoPixelCount, NeoPixelPin, NEO_GRB + NEO_KHZ800);
// Colors
const uint32_t green = pixels.Color(255, 0, 0);
const uint32_t red = pixels.Color(0, 255, 0);
const uint32_t blue = pixels.Color(0, 0, 255);
const uint32_t yellow = pixels.Color(155, 255, 0);
const uint32_t cyan = pixels.Color(255, 0, 255);
const uint32_t magenta = pixels.Color(0, 255, 255);
const uint32_t orange = pixels.Color(51, 255, 0);
const uint32_t white = pixels.Color(255, 255, 255);
const uint32_t aqua = pixels.Color(200, 0, 50);

const char DirectionLeft = 'L';
const char DirectionRight = 'R';

const int DarknessThreshold = 750;     // determines the threshold value for the robot to consider a reading from the line sensor as "dark" 
const double SafeDistanceFront = 16;   // the robot will not go closer than this distance to an obstacle
const double SafeDistanceSide = 6;  // The robot will try to keep this distance to the wall
const double SafeDistanceMargin = 0.5; // The robot will adjust its course if it exceeds SafeDistanceSide +/- SafeDistanceMargin range

const int GripperPulseOpen = 1700;  // sending this value to the gripper via writeToGripper() forces it open
const int GripperPulseClose = 1050; // completely opposite effect: forces the gripper shut
const int GripperPulseCounter = 8;  // number of times the pulse is sent to "convince" the gripper to work

const int TimeOut = 2000; // if the robot does not move for this amount of milliseconds, then the program will force it into retreat

const int FullTurnPulses = 120; // 
const double CmPerPulse = 0.5;  // a single "tick" of a rotation sensor is approx. 0.5 cm in length. This is useful for distance calculations

const int PreviousUpdateInterval = 10; // all "prev" global variables will be updated once in this amount of iterations through loop or similar functions;
// global variables
long g_tick = 0; // the basic unit of time of the program. Measures the total amount of all iterations of all important loops; determines when "prev" variables are updated
volatile unsigned long g_lastLeftRotationUpdate = millis(); // used to check if the robot's left wheel is stuck
volatile unsigned long g_lastRightRotationUpdate = millis(); // used to check if the robot's right wheel is stuck
unsigned long g_lastWallTurn = millis(); // used to prevent the robot from being too eager to turn
unsigned long g_lastRetreat = millis();

bool g_isSwitchedOff = false; // forces an early exit from the loop() (unless a safeguard is activated) after it is set to true. Effectively terminates the program.
char g_followWall = DirectionRight; // must be 'U', 'L' or 'R'; determines which wall the robot will stay close to and, therefore which turns will it make

bool g_isLeftMotorFwd = false;
bool g_isRightMotorFwd = false;

volatile unsigned long g_rightWheelCounter = 0; // counts the rotation sensor values for the right wheel. Used by interrupts and "smart" motor functions
volatile unsigned long g_leftWheelCounter = 0;  // ditto, but left wheel

/*
 * Reading from sensors is quite time-consuming, so it would be a better use of resources to store the values inside a variable and update it at regular intervals
 * This does sacrifice some accuracy, but since the robot moves slowly and the code is not critically slow, this should not be a cause for concern here
 * The time saved from reading the sensors will instead be used by robot's internal logic
*/
double g_curLeftDistance  = 0; // latest reading from the left distance sensor
double g_curFrontDistance = 0; // latest reading from the front distance sensor
double g_curRightDistance = 0; // latest reading from the right distance sensor

double g_prevLeftDistance  = 0; // previous reading from the left distance sensor
double g_prevFrontDistance = 0; // previous reading from the front distance sensor
double g_prevRightDistance = 0; // previous reading from the right distance sensor

int g_curLineSensorValue = 0;  // latest line sensor value
int g_prevLineSensorValue = 0; // previous line sensor value

// functions

// generic gripper functions
void writeToGripper(int pulse) {
    for (int i = 0; i < GripperPulseCounter; i++) {
        digitalWrite(GripperPin, HIGH);
        delayMicroseconds(pulse);
        digitalWrite(GripperPin, LOW);
        delay(20);
    }
}
void openGripper() {
    writeToGripper(GripperPulseOpen);
}
void closeGripper() {
    writeToGripper(GripperPulseClose);
}

// interrupt-related functions
void updateRightWheelCounter() {
    noInterrupts(); // disables interrupts for the duration of this function: we do not want more than one interrupt at any time
    g_lastRightRotationUpdate = millis();
    g_rightWheelCounter++;
    interrupts();
}
void updateLeftWheelCounter() {
    noInterrupts();
    g_lastLeftRotationUpdate = millis();
    g_leftWheelCounter++;
    interrupts();
}
void resetWheelCounters(int leftWheel=0, int rightWheel=0) {
    noInterrupts();
    unsigned long curTime = millis();
    g_lastLeftRotationUpdate = curTime;
    g_lastRightRotationUpdate = curTime;
    g_leftWheelCounter = leftWheel;
    g_rightWheelCounter = rightWheel;
    interrupts();
}
bool isLeftWheelStuck(int timeOut = TimeOut) {
    return (g_isLeftMotorFwd && (millis() > g_lastLeftRotationUpdate + timeOut));
}

bool isRightWheelStuck(int timeOut = TimeOut) {
    return (g_isRightMotorFwd && (millis() > g_lastRightRotationUpdate + timeOut));
}

// basic motor controls - do not let them continue without the backing of sensors and logic
void writeToMotors(uint8_t forwardLeftSpeed, uint8_t forwardRightSpeed, uint8_t backwardLeftSpeed, uint8_t backwardRightSpeed) {
    if (forwardLeftSpeed > 0) {
        g_isLeftMotorFwd = true;
    }
    else {
        g_isLeftMotorFwd = false;
    }
    if (forwardRightSpeed > 0) {
        g_isRightMotorFwd = true;
    }
    else {
        g_isRightMotorFwd = false;
    }
    analogWrite(MotorFLPin, forwardLeftSpeed);
    analogWrite(MotorFRPin, forwardRightSpeed);
    analogWrite(MotorBLPin, backwardLeftSpeed);
    analogWrite(MotorBRPin, backwardRightSpeed);
}
void stopMotors() {
    writeToMotors(0, 0, 0, 0);
}
void moveForward() {
    writeToMotors(190, 210, 0, 0);
}
void moveBackward() {
    writeToMotors(0, 0, 190, 210);
}
void turnLeft() {
    writeToMotors(0, 250, 0, 0);
}
void turnRight() {
    writeToMotors(250, 0, 0, 0);
}
void moveForwardLeft() {
    writeToMotors(200, 250, 0, 0);
}
void moveForwardRight() {
    writeToMotors(250, 200, 0, 0);
}
void moveBackwardLeft() {
    writeToMotors(0, 0, 200, 250);
}
void moveBackwardRight() {
    writeToMotors(0, 0, 230, 210);
}
// NeoPixel Functions
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

double getDistanceGeneric(const int trigPin, const int echoPin, char sensorName='U') {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigPin, LOW);
    double duration = pulseIn(echoPin, HIGH);
    double distance = (duration/2) / 29.1;
    #ifdef SERIAL_LOGS
    Serial.print("HC-SR04 (");
    Serial.print(sensorName);
    Serial.print("): >>> ");
    Serial.print(distance);
    Serial.println(" cm.");
    #endif
    return distance;
}

double getLeftDistance() {
    return getDistanceGeneric(SensorLeftTrigPin, SensorLeftEchoPin, 'L');
}
double getFrontDistance() {
    return getDistanceGeneric(SensorFrontTrigPin, SensorFrontEchoPin, 'F');
}
double getRightDistance() {
    return getDistanceGeneric(SensorRightTrigPin, SensorRightEchoPin, 'R');
}


// Line Sensor Functions
int getDarkCount() {
    int counter = 0;
    for (int sensorPin : LineSensorArray) {
        if (analogRead(sensorPin) > DarknessThreshold) {
            counter += 1;
        }
    }
    return counter;
}
bool isAllDark() {
    return getDarkCount() == LineSensorCount;
}
bool isAllLight() {
    return getDarkCount() == 0;
}
int calcLinePosition() {
    int pos = 0;
    for (int index = 0; index < LineSensorCount; index++) {
        if (analogRead(LineSensorArray[index]) > DarknessThreshold) {
            pos += LineSensorValues[index];
        }
    }
    return pos;
}

void followLine() {
    while (!(isAllLight() || isAllDark())) {
        g_curLineSensorValue = calcLinePosition();
        if (g_prevLineSensorValue == g_curLineSensorValue) {
            // if the position hasn't changed, continue doing what it was doing
            continue;
        }
        stopMotors();
        if (g_curLineSensorValue == 0) {
            // result of 0 means that either everything is dark, everything is light or the line is right in front
            // the while loop condition ensures that the first two situations prompt an immediate exits 
            clearPixels();
            frontPixels(cyan);
            moveForward();
        }
        bool requiresExtremeTurn = abs(g_curLineSensorValue) > 5;
        auto funcLeft = requiresExtremeTurn ? []() -> void {turnLeft();} : []() -> void {moveForwardLeft();};
        auto funcRight = requiresExtremeTurn ? []() -> void {turnRight();} : []() -> void {moveForwardRight();};
        if (g_curLineSensorValue > 0) {
            clearPixels();
            rightPixels(cyan);
            funcRight();
        } else {
            clearPixels();
            leftPixels(cyan);
            funcLeft();
        }
        g_prevLineSensorValue = g_curLineSensorValue;
    }
    moveForward();
    fillPixels(magenta);
    stopMotors();
}

void updateCurDistances() {
    g_curLeftDistance = getLeftDistance();
    g_curFrontDistance = getFrontDistance();
    g_curRightDistance = getRightDistance();
}
void updatePrevDistances() {
    g_prevLeftDistance = g_curLeftDistance;
    g_prevFrontDistance = g_curFrontDistance;
    g_prevRightDistance = g_curRightDistance;
}
void tick() {
    if (g_tick % PreviousUpdateInterval == 0) {
        updatePrevDistances();
    }
    updateCurDistances();
    g_tick++;
}

bool hasToRetreat(int timeOut = TimeOut) {
    return (isLeftWheelStuck(timeOut) || isRightWheelStuck(timeOut));
}
void retreat(int retreatDuration=1000) {
    clearPixels();
    rearPixels(red);
    if (g_followWall == DirectionLeft) {
        moveBackwardLeft();
    } else {
        moveBackwardRight();
    }
    delay(retreatDuration);
    stopMotors();
    closeGripper();
    resetWheelCounters();
    clearPixels();
    g_lastRetreat = millis();
}

int degreesToTicks(int angle) {
    return 120 * angle / 360;
}
int ticksToAngles(int ticks) {
    return ticks * 360 / 120;
}

void moveDistance(int distance, uint32_t color = green) {
    stopMotors();
    clearPixels();
    frontPixels(color);
    int targetTicks = g_rightWheelCounter + (distance / CmPerPulse);
    while (g_rightWheelCounter < targetTicks) {
        if (hasToRetreat()) {
            retreat();
            return;
        }
        moveForward();
    }
    stopMotors();
    clearPixels();
}

void turnByAngle(int angle, char turnDirection, uint32_t color = orange) {
    long differenceInTicks = degreesToTicks(angle);
    double (*checkDistanceFunction)();
    volatile unsigned long *rotatingWheelCounter;
    volatile unsigned long *stoppedWheelCounter;
    stopMotors();
    clearPixels();
    switch (turnDirection) {
        case DirectionLeft:
            leftPixels(color);
            rotatingWheelCounter = &g_rightWheelCounter;
            stoppedWheelCounter = &g_leftWheelCounter;
            checkDistanceFunction = getLeftDistance;
            // this line of code may be duplicated in the next case, but we do not want to waste any time once the motors start turning
            differenceInTicks += *rotatingWheelCounter - *stoppedWheelCounter; 
            turnLeft();
            break;
        case DirectionRight:
            rightPixels(color);
            rotatingWheelCounter = &g_leftWheelCounter;
            stoppedWheelCounter = &g_rightWheelCounter;
            checkDistanceFunction = getRightDistance;
            differenceInTicks += *rotatingWheelCounter - *stoppedWheelCounter;
            turnRight();
            break;
    }
    while (*rotatingWheelCounter - *stoppedWheelCounter < differenceInTicks) {
        // some code to prevent the compiler from optimizing the loop away
        if (hasToRetreat()) {
            retreat();
            int remainingTicks = *rotatingWheelCounter - *stoppedWheelCounter;
            resetWheelCounters();
            turnByAngle(ticksToAngles(remainingTicks), turnDirection, color);
            return;
        }
        if (checkDistanceFunction() < SafeDistanceSide) {
            break;
        }
        Serial.print("");
    }
    stopMotors();
    updatePrevDistances();
    clearPixels();
}

void trackWall(char wallPosition, uint32_t color = green) {
    frontPixels(color);
    double *distance;
    void (*adjustTowardsFn)(); // f-n -> function; f-n does not mean fun
    void (*adjustAwayFn)();  
    switch (wallPosition) {
        case DirectionLeft:
            distance = &g_curLeftDistance;
            adjustTowardsFn = &moveForwardLeft;
            adjustAwayFn = &moveForwardRight;
            break;
        case DirectionRight:
            distance = &g_curRightDistance;
            adjustTowardsFn = &moveForwardRight;
            adjustAwayFn = &moveForwardLeft;
            break;
        default:
            return;
    }
    // using pointers allows us to use this beautiful logic with minimum repeats
    if (SafeDistanceSide + SafeDistanceMargin < *distance) {
        adjustTowardsFn();
        return;
    } else if (SafeDistanceSide - SafeDistanceMargin > *distance) {
        adjustAwayFn();
        return;
    }
    moveForward();
}

void awaitActivation() {
    do {
        g_curFrontDistance = getFrontDistance();
    } while (g_curFrontDistance > 25);
}
void start() {
    fillPixels(blue);
    openGripper();
    delay(500);
    awaitActivation();
    fillPixels(white);
    delay(3000);
    resetWheelCounters();
    moveDistance(25);
    followLine();
    closeGripper();
    turnByAngle(90, DirectionLeft);
    tick();
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting the program...");
    pinMode(LeftWheelSensorPin, INPUT);
    pinMode(RightWheelSensorPin, INPUT);
    pinMode(MotorFLPin, OUTPUT);
    pinMode(MotorFRPin, OUTPUT);
    pinMode(MotorBLPin, OUTPUT);
    pinMode(MotorBRPin, OUTPUT);
    pinMode(GripperPin, OUTPUT);
    pinMode(NeoPixelPin, OUTPUT);
    pinMode(SensorLeftTrigPin, OUTPUT);
    pinMode(SensorLeftEchoPin, INPUT);
    pinMode(SensorFrontTrigPin, OUTPUT);
    pinMode(SensorFrontEchoPin, INPUT);
    pinMode(SensorRightTrigPin, OUTPUT);
    pinMode(SensorRightEchoPin, INPUT);
    for (int sensorPin : LineSensorArray) {
        pinMode(sensorPin, INPUT);
    }
    attachInterrupt(digitalPinToInterrupt(RightWheelSensorPin), updateRightWheelCounter, CHANGE);
    attachInterrupt(digitalPinToInterrupt(LeftWheelSensorPin), updateLeftWheelCounter, CHANGE);
    pixels.begin();
    g_followWall = DirectionLeft;
    start();
}

void loop() {
    tick();
    if (g_isSwitchedOff) {
        if (g_curLeftDistance < 4 && g_curRightDistance < 4) {
            // failsafe, cover two sensors on the left and right to reactivate the robot without restarting the protocol
            g_isSwitchedOff = false;
            closeGripper();
        }
        return;
    }
    // if its dark, stop and drop
    if (isAllDark()) {
        g_isSwitchedOff = true;
        fillPixels(magenta);
        openGripper(); 
        moveBackward();
        delay(1000);
        stopMotors();
        clearPixels();
      return;
    }
    // sees a line: follow it
    if (calcLinePosition() && g_lastRetreat + TimeOut < millis()) {
        followLine();
        return;
    }
    if (g_followWall == DirectionRight && g_curRightDistance > 35) {
        rightPixels(blue);
        moveDistance(10);
        turnByAngle(90, DirectionRight, blue);
        return;
    }
    if (g_followWall == DirectionLeft && g_curLeftDistance > 35) {
        leftPixels(blue);
        moveDistance(10);
        turnByAngle(90, DirectionLeft, blue);
        return;
    }
    if (g_curFrontDistance <= SafeDistanceFront) {
        if (g_curLeftDistance < g_curRightDistance) {
            turnByAngle(90, DirectionRight);
        }
        else { 
            turnByAngle(90, DirectionLeft);
        }
        retreat(300);
        return;
    }
    if (hasToRetreat()) {
        retreat(700);
        return;
    }
    trackWall(g_followWall);
}
