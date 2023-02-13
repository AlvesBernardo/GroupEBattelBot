// Neopixel library to simplify the use of Neopixel LEDs
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
// Servo library to make it easier to use the gripper
#include <Servo.h>

/*
 * While developing the robot, we need to log crucial information from the 
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
const int SonarTrigPin = 3; // sonar/distance sensor requires an input pin to start sending pulses
const int SonarEchoPin = 7; // sonar uses this pin to output the signal
const int GripperPin = 9;   // used by servo library to control the gripper
const int MotorFLPin = 10; // left motor forward motion
const int MotorBLPin = 11; // left motor backward motion
const int MotorFRPin = 6; // right motor forward motion
const int MotorBRPin = 5; // right motor backward motion

// Neopixels
const int totalPixels = 4;
const int pixelRearLeft = 0;
const int pixelRearRight = 1;
const int pixelFrontRight = 2;
const int pixelFrontLeft = 3;
Adafruit_NeoPixel pixels(totalPixels, NeoPixelPin, NEO_GRB + NEO_KHZ800);

// some colors to simplify the use of sensors
const uint32_t green = pixels.Color(255, 0, 0);
const uint32_t red = pixels.Color(0, 255, 0);
const uint32_t blue = pixels.Color(0, 0, 255);
const uint32_t yellow = pixels.Color(155, 255, 0);
const uint32_t cyan = pixels.Color(255, 0, 255);
const uint32_t magenta = pixels.Color(0, 255, 255);
const uint32_t orange = pixels.Color(51, 255, 0);
const uint32_t white = pixels.Color(255, 255, 255);

Servo gripper;
Servo sensorRotor;

#ifdef DEBUG
void logValfromFunc(char* funcName, double value) {
  
}
void logValfromFunc(char* funcName, int value) {
  
}
#endif

const int NumLineSensors = 8;

class LineSensor {
private:
  int m_sensors[NumLineSensors] {};
  int m_darkThreshold = 650;
  // TODO: remove m_strictness from code before submitting the project
  double m_strictness = 1.0; // strictness determines how great a % score should be in order for the function to return true
  // values below 1 make the program less reliant on every sensor working perfectly, so the robot might work even if damaged
  #ifdef DEBUG
  // even in debug mode, we might want to switch off the output log to make sure the other logs are clearly visible
  bool m_logOutput = true;
  #endif
public:
  LineSensor(int left4, int left3, int left2, int left1, int right1, int right2, int right3, int right4) {
    int tempArray[] = {left4, left3, left2, left1, right1, right2, right3, right4};
    for (int i = 0; i >= NumLineSensors; i++) {
      m_sensors[i] = tempArray[i];
      pinMode(tempArray[i], INPUT);
    }
  }
  #ifdef DEBUG
  void setLogOutput(bool value) {
    m_logOutput = value;
  }
  #endif
  void setDarkThreshold(int threshold) {
    m_darkThreshold = threshold;
  }
  void setStrictness(double strictness) {
    m_strictness = strictness;
  }
  // not implemented
  int getFirstDarkRegion() {
    int result = -1;
    int reading;
    for (int pin : m_sensors) {
      reading = analogRead(pin);
      if (reading > m_darkThreshold) {
        result = pin;
        break;
      }
    }
    #ifdef DEBUG
    if (m_logOutput) {
      Serial.print("LineSensor.getFirstDarkRegion(): pin ");
      Serial.print(result);
      Serial.print(" has value of ");
      Serial.print(result!=-1? reading : 0);
      Serial.print(".\n");
    }
    #endif
    return result;
  }
  int getCountDarkRegions() {
    double count = 0;
    #ifdef DEBUG
    if (m_logOutput) {Serial.print("Line Sensor Output (left to right): ");}
    #endif
    for (int pin : m_sensors) {
      auto reading = analogRead(pin);
      if (reading > m_darkThreshold) {
        count += 1;
      }
      #ifdef DEBUG
      if (m_logOutput) {Serial.print(reading);Serial.print("; ");}
      #endif
    }
    #ifdef DEBUG
    if (m_logOutput) {Serial.print("total dark regions: ");Serial.print(count); Serial.print(".\n");}
    #endif
    return count;
  }
  bool isDark() {
    double count = 0;
    #ifdef DEBUG
    if (m_logOutput) {Serial.print("Line Sensor Output (left to right): ");}
    #endif
    for (int pin : m_sensors) {
      auto reading = analogRead(pin);
      if (reading > m_darkThreshold) {
        count += 1;
      }
      #ifdef DEBUG
      if (m_logOutput) {Serial.print(reading);Serial.print(", ");}
      #endif
    }
    #ifdef DEBUG
    if (m_logOutput) {Serial.print("\n");}
    #endif
    return (count / NumLineSensors) >= m_strictness;
  }
};
auto lineSensors = new LineSensor(A6, A7, A1, A0, A2, A3, A4, A5);

double getDistanceCm() {
  digitalWrite(SonarTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(SonarTrigPin, LOW); // cleaning up the pulses
  double duration = pulseIn(SonarEchoPin, HIGH);
  double distance = (duration / 2) / 29.1;
  #ifdef DEBUG
  Serial.print("Distance to object: ");
  Serial.print(distance);
  Serial.print("cm. \n");
  #endif
  return distance;
}

const int slow_speed_left = 150;
const int slow_speed_right = 150;

void forward() {
  #ifdef DEBUG
  Serial.print("forward()\n");
  #endif
  analogWrite(MotorFLPin, slow_speed_left);
  analogWrite(MotorFRPin, slow_speed_right);
  analogWrite(MotorBLPin, 0);
  analogWrite(MotorBRPin, 0);
}
void turnLeft() {
  #ifdef DEBUG
  Serial.print("turnLeft()\n");
  #endif
  analogWrite(MotorFLPin, slow_speed_left);
  analogWrite(MotorFRPin, 0);
  analogWrite(MotorBLPin, 0);
  analogWrite(MotorBRPin, slow_speed_right);
} 
void turnRight() {
  #ifdef DEBUG
  Serial.print("turnRight()\n");
  #endif
  analogWrite(MotorFLPin, 0);
  analogWrite(MotorFRPin, slow_speed_left);
  analogWrite(MotorBLPin, slow_speed_right);
  analogWrite(MotorBRPin, 0);
}
void backward() {
  #ifdef DEBUG
  Serial.print("backward()\n");
  #endif
  analogWrite(MotorBLPin, slow_speed_left);
  analogWrite(MotorBRPin, slow_speed_right);
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

void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  pinMode(SonarTrigPin, OUTPUT);
  pinMode(SonarEchoPin, INPUT);
  pinMode(NeoPixelPin, OUTPUT);
  pinMode(GripperPin, OUTPUT);
  pinMode(MotorFLPin, OUTPUT);
  pinMode(MotorBLPin, OUTPUT);
  pinMode(MotorFRPin, OUTPUT);
  pinMode(MotorBRPin, OUTPUT);
  pixels.begin();
  pixels.clear();
  pixels.setPixelColor(pixelRearLeft, red);
  pixels.setPixelColor(pixelFrontLeft, red);
  pixels.setPixelColor(pixelRearRight, green);
  pixels.setPixelColor(pixelFrontRight, green);
  pixels.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  forward();
}
