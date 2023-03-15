// Neopixel library to simplify the use of Neopixel LEDs
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// We use the Servo library to control the gripper
#include <Servo.h>

/*
 * We are using preprocessor commands to disable certain functionality that could impact performance during the race
 * enable it back to ensure that the serial monitor prints logs
*/
#define DEBUG

/*
 * PINS:
 * Available PWM pins: 3, 5, 6, 11. (9 and 10 are disabled by the Servo library) 
*/
const int NeoPixelPin = 2;                // NeoPixel library will control the output of all four NeoPixels
const int SensorFrontTrigPin = 4;         // Trig pin for the front distance sensor
const int SensorFrontEchoPin = 7;         // Front distance sensor output
const int SensorLeftTrigPin = 13;         // Trig pin for the left distance sensor
const int SensorLeftEchoPin = 12;         // Left distance sensor output
const int SensorRightTrigPin = 8;         // Trig pin for the right distance sensor
const int SensorRightEchoPin = 10;        // Right distance sensor output
const int GripperPin = 9;                 // Gripper servo library
const int MotorFLPin = 3;                 // PWM left motor forward motion
const int MotorBLPin = 11;                // PWM left motor backward motion
const int MotorFRPin = 6;                 // PWM right motor forward motion
const int MotorBRPin = 5;                 // PWM right motor backward motion

// useful functions for maths
bool approxComparison(double target, double value, double margin=0.05) {
    return (target * (1-margin) > value && target * (1+margin) < value);
}


// neopixels setup and functions that control the lights
const int NumberOfNeoPixels = 4;
const int pixelRearLeft = 0;
const int pixelRearRight = 1;
const int pixelFrontRight = 2;
const int pixelFrontLeft = 3;
Adafruit_NeoPixel pixels(NumberOfNeoPixels, NeoPixelPin, NEO_GRB + NEO_KHZ800);

void allPixels(uint32_t color) {
    pixels.setPixelColor(pixelFrontLeft, color);
    pixels.setPixelColor(pixelFrontRight, color);
    pixels.setPixelColor(pixelRearLeft, color);
    pixels.setPixelColor(pixelRearRight, color);
    pixels.show();
}
void frontPixels(uint32_t color) {
    pixels.setPixelColor(pixelFrontLeft, color);
    pixels.setPixelColor(pixelFrontRight, color);
    pixels.show();
}
void rearPixels(uint32_t color) {
    pixels.setPixelColor(pixelRearLeft, color);
    pixels.setPixelColor(pixelRearRight, color);
    pixels.show();
}
void leftPixels(uint32_t color) {
    pixels.setPixelColor(pixelFrontLeft, color);
    pixels.setPixelColor(pixelRearLeft, color);
    pixels.show();
}
void rightPixels(uint32_t color) {
    pixels.setPixelColor(pixelFrontRight, color);
    pixels.setPixelColor(pixelRearRight, color);
    pixels.show();
}
void clearPixels() {
    pixels.clear();
    pixels.show();
}

// colors constants
const uint32_t blue_green = pixels.Color(200, 0, 50);
const uint32_t green = pixels.Color(255, 0, 0);
const uint32_t red = pixels.Color(0, 255, 0);
const uint32_t blue = pixels.Color(0, 0, 255);
const uint32_t yellow = pixels.Color(155, 255, 0);
const uint32_t cyan = pixels.Color(255, 0, 255);
const uint32_t magenta = pixels.Color(0, 255, 255);
const uint32_t orange = pixels.Color(51, 255, 0);
const uint32_t white = pixels.Color(255, 255, 255);

// Line sensor
const int isDarkThreshold = 800;
const int numberLineSensors = 8;
int lineSensor[numberLineSensors] = {A6, A7, A1, A0, A2, A3, A4, A5}; // pins must be in the same order as their corresponding sensors are from left to right

void inline initLineSensor() {
    // run this function in setup() before using the linesensor
    for (int sensorPin : lineSensor) {
        pinMode(sensorPin, INPUT);
    }
}
#ifdef DEBUG
void logLineSensorReading() {
    Serial.print("Line Sensors (L->R): ");
    for (int sensorPin : lineSensor) {
        Serial.print(analogRead(sensorPin));
        Serial.print(", ");
    }
    Serial.print("\n");
}
#endif

int getLineSensorReading() {
    int result = 0;
    // -7 -5 -3 -1 1 3 5 7
    for (int i = 0; i < numberLineSensors; i++) {
        if (analogRead(lineSensor[i]) >= isDarkThreshold) {
            result += (-7 + i*2);
        }
    }
    #ifdef DEBUG
    logLineSensorReading();
    Serial.print("Line Sensor Reading: ");
    Serial.println(result);
    #endif
    return result;
}

bool isAllLight() {
    for (int sensorPin : lineSensor) {
        if (analogRead(sensorPin) >= isDarkThreshold) {
            return false;
        }
    }
    return true;
}

bool isAllDark() {
    for (int sensorPin : lineSensor) {
        if (analogRead(sensorPin) < isDarkThreshold) {
            return false;
        }
    }
    return true;
}

//Distance Sensor
class DistanceSensor {
    private:
    int m_trigPin;
    int m_echoPin;
    char m_pos; // 'L', 'F', 'R'
    public:
    DistanceSensor(char pos, int trigPin, int echoPin) {
        m_trigPin = trigPin;
        m_echoPin = echoPin;
        m_pos = pos;
        pinMode(m_trigPin, OUTPUT);
        pinMode(m_echoPin, INPUT);
    }
    double getDistance() {
        // clean up the pulse
        digitalWrite(m_trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(m_trigPin, HIGH);
        delayMicroseconds(5);
        digitalWrite(m_trigPin, LOW);
        delayMicroseconds(2);
        double duration = pulseIn(m_echoPin, HIGH);
        double distance = (duration/2) / 29.1;
        #ifdef DEBUG
        Serial.print("Distance Sensor (");
        Serial.print(m_pos);
        Serial.print(") Reading (cm): ");
        Serial.println(distance);
        #endif
        return distance;
    }
};

DistanceSensor leftDistanceSensor =  DistanceSensor('L', SensorLeftTrigPin, SensorLeftEchoPin);
DistanceSensor frontDistanceSensor = DistanceSensor('F', SensorFrontTrigPin, SensorFrontEchoPin);
DistanceSensor rightDistanceSensor = DistanceSensor('R', SensorRightTrigPin, SensorRightEchoPin);

// Motor functions
void moveForward(int speed_left=255, int speed_right=255) {
    #ifdef DEBUG
    Serial.print("forward()\n");
    #endif
    analogWrite(MotorFLPin, speed_left);
    analogWrite(MotorFRPin, speed_right);
    analogWrite(MotorBLPin, 0);
    analogWrite(MotorBRPin, 0);
}
void turnLeft(int speed_left=255, int speed_right=255) {
    #ifdef DEBUG
    Serial.print("turnLeft()\n");
    #endif
    analogWrite(MotorFLPin, 0);
    analogWrite(MotorFRPin, speed_left);
    analogWrite(MotorBLPin, speed_right);
    analogWrite(MotorBRPin, 0);
} 
void turnRight(int speed_left=255, int speed_right=255) {
    #ifdef DEBUG
    Serial.print("turnRight()\n");
    #endif
    analogWrite(MotorFLPin, speed_left);
    analogWrite(MotorFRPin, 0);
    analogWrite(MotorBLPin, 0);
    analogWrite(MotorBRPin, speed_right);
}
void moveBackward(int speed_left=255, int speed_right=255) {
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

// gripper
Servo gripper;
bool g_isGripperOpen = true;

void openGripper() {
    gripper.write(120);
    g_isGripperOpen = true;
    #ifdef DEBUG
    Serial.print("openGripper()\n");
    #endif
}
void closeGripper() {
    gripper.write(50);
    g_isGripperOpen = false;
    #ifdef DEBUG
    Serial.print("closeGripper()\n");
    #endif
}

void followLine() {
    int prevReading = 0;
    while (!isAllLight() || prevReading != 0) {
        int reading = getLineSensorReading();
        if (reading < -5 || reading > 5) {
          moveForward();
        }
        else if (reading < -1) {
            turnLeft();
            clearPixels();
            leftPixels(cyan);
        } else if (reading > 1) {
            turnRight();
            clearPixels();
            rightPixels(cyan);
        } else {
            moveForward();
            clearPixels();
            frontPixels(cyan);
        }
        prevReading = reading;
    }
    clearPixels();
    allPixels(green);
    halt();
}


const int distanceThreshold = 15;
const int dirLeft = 5;
const int dirStraight = 0;
const int dirRight = -5;

const double errorMargin = 0.05;

void measuredTurn(int turnDirection, double errorMargin=errorMargin) {
    double targetDistance;
    halt();
    clearPixels();
    switch(turnDirection) {
        case dirLeft:
            targetDistance = leftDistanceSensor.getDistance();
            leftPixels(cyan);
            turnLeft();
            break;
        case dirRight:
            targetDistance = rightDistanceSensor.getDistance();
            rightPixels(cyan);
            turnRight();
        default:
            return;
    }
    double currentDistance;
    do {
        currentDistance = frontDistanceSensor.getDistance();
    } while (currentDistance < targetDistance * (1-errorMargin) || currentDistance > targetDistance * (1 + errorMargin));
}
/*
void turnToTarget(int dir, double targetDistance) {
    targetDistance *= 0.95;
    rotateDistanceSensor(angleStraight);
    delay(200);
    clearPixels();
    switch (dir) {
        case dirLeft:
            leftPixels(green);
            turnLeft();
            break;
        case dirRight:
            rightPixels(green);
            turnRight();
            break;
        default:
            return;
    }
    double currentDistance;
    do {
        currentDistance = getDistanceSensorReading();
    } while (targetDistance > currentDistance);
    halt();
    clearPixels();
}
*/
void turn90(int dir) {
    switch (dir) {
        case dirLeft:
            turnLeft();
            break;
        case dirRight:
            turnRight();
            break;
    }
    delay(700);
    halt();
}


// flags
bool g_hasMovedGripper = false; // we use this to see if the robot is still in the same dark square as when it last moved the gripper
bool g_lookForEntranceMode = true;
bool g_lookForExitMode = false;
bool g_receivedActivationSignal = true;

void setup() {
    #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Setup()");
    Serial.println(0.1 == 0.1);
    Serial.println(0.1 + 0.1 + 0.1 == 0.3);
    Serial.println(approxComparison(0.1 + 0.1 + 0.1, 0.3));
    #endif
    pinMode(NeoPixelPin, OUTPUT);
    pinMode(GripperPin, OUTPUT);
    pinMode(MotorFLPin, OUTPUT);
    pinMode(MotorFRPin, OUTPUT);
    pinMode(MotorBLPin, OUTPUT);
    pinMode(MotorBRPin, OUTPUT);
    initLineSensor();
    gripper.attach(GripperPin);
    allPixels(white);    
    while (!g_receivedActivationSignal) {
    }
    closeGripper();
    turn90(dirLeft);
    delay(500);
    turn90(dirRight);
}

void loop() {
    double distanceLeft = leftDistanceSensor.getDistance();
    double distanceFront = frontDistanceSensor.getDistance();
    double distanceRight = rightDistanceSensor.getDistance();
    if (distanceLeft < 20) {
        leftPixels(red);
    }
    if (distanceFront < 20) {
        frontPixels(magenta);
    }
    if (distanceRight < 20) {
       rightPixels(red);
    }
    delay(500);
    clearPixels();
    /*
    double prevDistance = 200;
    while (isAllLight()) {
        double curDistance = getDistanceSensorReading();
        if (curDistance < prevDistance) {
            if (curDistance < distanceThreshold) {
                allPixels(red);
                halt();
                double maxDistance = 0;
                leftPixels(yellow);
                for (int turnAngle = angleStraight; turnAngle != angleExtremeLeft; turnAngle += dirLeft) {
                    rotateDistanceSensor(turnAngle);
                    double distance = getDistanceSensorReading();
                    if (distance > maxDistance) {
                        maxDistance = distance;
                    }
                }
                if (maxDistance > distanceThreshold) {
                    turnToTarget(dirLeft, maxDistance);
                    continue;
                }
                rightPixels(yellow);
                for (int turnAngle = angleStraight; turnAngle != angleExtremeRight; turnAngle += dirRight) {
                    rotateDistanceSensor(turnAngle);
                    double distance = getDistanceSensorReading();
                    if (distance > maxDistance) {
                        maxDistance = distance;
                    }
                }
                if (maxDistance > distanceThreshold) {
                    turnToTarget(dirRight, maxDistance);
                    continue;
                }
                // move backward and look to the right
                clearPixels();
                rearPixels(green);
                rotateDistanceSensor(angleExtremeRight);
                moveBackward();
                delay(500);
                do {
                    prevDistance = curDistance;
                    curDistance = getDistanceSensorReading();
                    delay(100);
                } while(prevDistance < curDistance);
                // move forward and look to the left 
            } else {
                moveForward();
                allPixels(green);
            }
            continue;
        }
        else {
            // course correction
        }
        prevDistance = curDistance;
    }
    halt();
    #ifdef DEBUG
    logLineSensorReading();
    #endif
    allPixels(cyan);
    */
    /*
    getLineSensorReading();
    if (!isAllLight()) {
        g_lookForExitMode = true;
    }
    if (g_lookForExitMode) {
        followLine();
    }
    */
    /*
    if (isAllDark()) {
        moveForward();
        clearPixels();
        frontPixels(blue_green);
        if (!g_hasMovedGripper) {
            if (g_isGripperOpen) {
              closeGripper();
            }
            else {
                openGripper();
            }
            g_hasMovedGripper = true;
        }
    } else if (!isAllLight()) {
        // line follow mode
        g_hasMovedGripper = false;
        int dir = getLineSensorReading();
        Serial.println(dir);
        if (dir < 0) {
            clearPixels();
            leftPixels(green);
            turnLeft();
        } else if (dir > 0) {
            clearPixels();
            rightPixels(green);
            turnRight();
        } else {
            clearPixels();
            frontPixels(green);
            moveForward();
        }
    }
    #ifdef DEBUG
    logLineSensorReading();
    #endif
    */
}
