#include <Adafruit_NeoPixel.h>
#include <Servo.h> // messes up analogWrite on pins 9 and 10

#define SERIAL_LOGS
//#undef SERIAL_LOGS // remove this to make the program output more data to the serial monitor

// Pin definitions
const int NeoPixelPin = A0;
const int GripperPin = A1;

const int RightWheelSensorPin = 3; // R1 rotation sensor
const int LeftWheelSensorPin = 2;  // R2 rotation sensor

const int MotorFLPin = 5; // Corresponds to A2 
const int MotorFRPin = 6; // Corresponds to B1 
const int MotorBLPin = 8; // Corresponds to A1 
const int MotorBRPin = 9; // Corresponds to B2

const int SensorFrontTrigPin = 4;
const int SensorFrontEchoPin = 7;
const int SensorLeftTrigPin = 10;
const int SensorLeftEchoPin = 11;
const int SensorRightTrigPin = 12;
const int SensorRightEchoPin = 13;
// Line Sensor setup
const int DarkThreshold = 650;
const int SensorCount = 6;
const int LineSensorArray[SensorCount] = {A7, A6, A2, A3, A4, A5};
int g_prevSensorValues[SensorCount];
int g_sensorValues[SensorCount];

const char DirectionLeft = 'L';
const char DirectionRight = 'R';

bool g_isHoldingObject = false;
Servo gripper;

int g_rightWheelCounter = 0;
int g_leftWheelCounter = 0;

// used to make the intervals for refreshing "prev" values at a more reasonable rate
const int UpdateInterval = 20;
const double DistanceThreshold = 15;
int g_iterationsCounter = 0;

double g_prevFrontDistance = 0;
double g_prevLeftDistance = 0;
double g_prevRightDistance = 0;
double g_curFrontDistance = 0;
double g_curLeftDistance = 0;
double g_curRightDistance = 0;

const int NeoPixelCount = 4;
const int PixelBackLeft = 0;
const int PixelBackRight = 1;
const int PixelFrontRight = 2;
const int PixelFrontLeft = 3;

// the NeoPixel object we are going to use everytime we require to work with the pixels
Adafruit_NeoPixel pixels(NeoPixelCount, NeoPixelPin, NEO_GRB + NEO_KHZ800);

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

// functions
const double WheelRadius = 3.5;
const double DistanceBetweenWheels = 11;
const int TicksPerTurn = 40; // average

int degreesToTicks(int angle) {
    /*
    return static_cast<int>((TicksPerTurn * DistanceBetweenWheels * angle) / (360 * WheelRadius)); 
    */
    // Experiments have shown that a 90 degree turn requires approx 45 ticks
    return TicksPerTurn * angle / 90;
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

// Gripper Functions
void openGripper() {
    #ifdef SERIAL_LOGS
    Serial.println("openGripper()");
    #endif
    gripper.write(120);
    g_isHoldingObject = false;
}

void closeGripper() {
    #ifdef SERIAL_LOGS
    Serial.println("closeGripper()");
    #endif
    gripper.write(50);
    g_isHoldingObject = true;
}

// Rotation Sensor Functions
void updateRightWheelCounter() {
    noInterrupts();
    g_rightWheelCounter++;
    interrupts();
}
void updateLeftWheelCounter() {
    noInterrupts();
    g_leftWheelCounter++;
    interrupts();
}
void resetWheelCounters(int leftWheel=0, int rightWheel=0) {
    noInterrupts();
    g_leftWheelCounter = leftWheel;
    g_rightWheelCounter = rightWheel;
    interrupts();
}

// Motor Functions
void stopMotors() {
    #ifdef SERIAL_LOGS
    Serial.println("StopMotors()");
    #endif
    analogWrite(MotorFLPin, 0);
    analogWrite(MotorFRPin, 0);
    digitalWrite(MotorBLPin, LOW);
    digitalWrite(MotorBRPin, LOW);
}
void moveForward(int motorSpeed=255) {
    #ifdef SERIAL_LOGS
    Serial.print("moveForward(");
    Serial.print(motorSpeed);
    Serial.println(")");
    #endif
    analogWrite(MotorFLPin, motorSpeed);
    analogWrite(MotorFRPin, motorSpeed);
    digitalWrite(MotorBLPin, LOW);
    digitalWrite(MotorBRPin, LOW);
}
void moveBackward() {
    #ifdef SERIAL_LOGS
    Serial.println("moveBackward()");
    #endif
    analogWrite(MotorFLPin, 0);
    analogWrite(MotorFRPin, 0);
    digitalWrite(MotorBLPin, HIGH); 
    digitalWrite(MotorBRPin, HIGH);
}
void turnLeft() {
    #ifdef SERIAL_LOGS
    Serial.println("turnLeft()");
    #endif
    analogWrite(MotorFLPin, 0);
    analogWrite(MotorFRPin, 255);
    digitalWrite(MotorBLPin, LOW);
    digitalWrite(MotorBRPin, LOW);
}
void turnRight() {
    #ifdef SERIAL_LOGS
    Serial.println("turnRight()");
    #endif
    analogWrite(MotorFLPin, 255);
    analogWrite(MotorFRPin, 0);
    digitalWrite(MotorBLPin, LOW);
    digitalWrite(MotorBRPin, LOW);
}
void moveForwardLeft(int slowSpeed=160, int fastSpeed=255) {
    #ifdef SERIAL_LOGS
    Serial.println("moveForwardLeft()");
    #endif
    analogWrite(MotorFLPin, slowSpeed);
    analogWrite(MotorFRPin, fastSpeed);
    digitalWrite(MotorBLPin, LOW);
    digitalWrite(MotorBRPin, LOW);
}
void moveForwardRight(int slowSpeed=160, int fastSpeed=255) {
    #ifdef SERIAL_LOGS
    Serial.println("moveForwardRight()");
    #endif
    analogWrite(MotorFLPin, fastSpeed);
    analogWrite(MotorFRPin, slowSpeed);
    digitalWrite(MotorBLPin, LOW);
    digitalWrite(MotorBRPin, LOW);
}
void rotateByAngle(int angle, char turnDirection) {
    #ifdef SERIAL_LOGS
    Serial.print("rotateByAngle(");
    Serial.print(angle);
    Serial.print(", ");
    Serial.print(turnDirection);
    Serial.println(")");
    #endif
    stopMotors();
    int targetTicksDiff = degreesToTicks(angle);
    int * rotatingWheelCounter;
    int * stoppedWheelCounter;
    int i = 0; // this variable is used to fool the compiler that the while loop is doing something useful instead of just waiting for the ticks to elapse
    clearPixels();
    switch (turnDirection) {
        case DirectionLeft:
            leftPixels(orange);
            rotatingWheelCounter = &g_rightWheelCounter;
            stoppedWheelCounter = &g_leftWheelCounter;
            targetTicksDiff += *rotatingWheelCounter - *stoppedWheelCounter; // this line of code may be duplicated in the next case, 
            // but it's better to minimize the amount of calculations done once the robot starts rotating.
            turnLeft();
            break;
        case DirectionRight:
            rightPixels(orange);
            rotatingWheelCounter = &g_leftWheelCounter;
            stoppedWheelCounter = &g_rightWheelCounter;
            targetTicksDiff += *rotatingWheelCounter - *stoppedWheelCounter;
            turnRight();
            break;
    }
    Serial.print("Stopped, Rotating: ");
    Serial.print(*stoppedWheelCounter);
    Serial.print(", ");
    Serial.println(*rotatingWheelCounter);
    Serial.print("Target: ");
    Serial.println(targetTicksDiff);
    while (*rotatingWheelCounter - *stoppedWheelCounter < targetTicksDiff) {
        // some code to prevent the compiler from optimizing the loop away
        i++;
        Serial.print("");
    }
    stopMotors();
    clearPixels();
}
// Distance Sensor Functions
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
auto getLeftDistance  = [] () -> double {return getDistanceGeneric(SensorLeftTrigPin, SensorLeftEchoPin, 'L');};
auto getFrontDistance = [] () -> double {return getDistanceGeneric(SensorFrontTrigPin, SensorFrontEchoPin, 'F');};
auto getRightDistance = [] () -> double {return getDistanceGeneric(SensorRightTrigPin, SensorRightEchoPin, 'R');};

// Line Sensor Functions
void logLineSensorReadings() {
    Serial.print("Line Sensor (L->R): ");
    for (int sensorPin : LineSensorArray) {
        Serial.print(analogRead(sensorPin));
        Serial.print(", ");
    }
    Serial.println();
}

// other functions
void updateCurDistances() {
    // technically, there is a delay of around 10 ms in between each function call listed, but at the slow speeds the robot moves, this should not be a major problem
    // trying to optimize it, however, is dangerous, because the sound waves from one sensor can be picked up by the other, giving us a garbage result
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
    if (g_iterationsCounter % UpdateInterval == 0) {
        updatePrevDistances();
    }
    updateCurDistances();
    g_iterationsCounter++;
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
    gripper.attach(GripperPin);
    pixels.begin();
    stopMotors();
    fillPixels(white);
    openGripper();
}

bool g_lookForNearestExit = false;

void loop() {
    // Check if there is a need for a course adjustment
    // first, look at the difference in rotations between left and right wheels
    // if it is more than some arbitrary value (CourseCorrectionThreshold), then the offending wheel must be allowed to rotate
    tick();
    if (!g_lookForNearestExit) {
        if (g_curFrontDistance > DistanceThreshold) {
            fillPixels(green);
            moveForward();
        } else {
            stopMotors();
            if (g_curLeftDistance > DistanceThreshold) {
                clearPixels();
                rotateByAngle(90, DirectionLeft);
            } else if (g_curRightDistance > DistanceThreshold) {
                clearPixels();
                rotateByAngle(90, DirectionRight);
            } else {
                if (g_curRightDistance > g_curLeftDistance) {
                    clearPixels();
                    rotateByAngle(180, DirectionRight);
                } else {
                    clearPixels();
                    rotateByAngle(180, DirectionLeft);
                }
            }
        }
    } else {
    }
}
