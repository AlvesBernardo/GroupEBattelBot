// including library
#include <Adafruit_NeoPixel.h> //including file
#ifdef __AVR__
#include <avr/power.h>
#endif
#include<SoftwareSerial.h>

//defining pins for distance sensor
const int trigPin = 12;
const int echoPin = 13;
const int servoPin = 4;

//defining motors
const int motorPin = 11; //A1
const int motorPin2 = 10; //A2
const int motorPin3 = 6; //B1
const int motorPin4 = 5; //B2

//rotation sensors
const int motorR1 = 3;
const int motorR2 = 2;

//gripper
int gripperPin = 13;

int countL = 0;
int countR = 0;

//line senor pin
const int lineSenor1 = A6;
const int lineSenor2 = A0;
const int lineSenor3 = A1;
const int lineSenor4 = A2;
const int lineSenor5 = A3;
const int lineSenor6 = A4;
const int lineSenor7 = A5;//right
const int lineSenor8 = A7;// left

//left to right
int pinBlnWh[] = {A7, A6, A0, A1, A2, A3, A4, A5}; //from left to right order
int posVal[] = { -4, -3, -2, -1, 1, 2, 3, 4}; //possible values
int reading[8] = {};
int positionRobot = 0;
int previousPosition = 0;
int currentPosition = 0;

//speeds
const int turningRatio = 35;
const int basicRight = 170;
const int basicLeft = 152;
const int minumumSpeed = 75;


// defines variables
long duration;
int distance;
int distanceToNextRobot;
int neoPIN = 4; //NEO pixel datapin
int NUMPIXELS = 7;


Adafruit_NeoPixel pixels(NUMPIXELS, neoPIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

//create servo object
//Servo servo;
int angle = 10;

boolean hasRacesStarted = false;
int blackLines = 0;
void setup() {
  //==================[ Motors ]====================
  //motors declaration
  pinMode(motorPin2, OUTPUT);
  pinMode (motorPin4, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode (motorPin3, OUTPUT);
  pinMode(motorR1, INPUT);
  pinMode(motorR2, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), rotationL, CHANGE);
  attachInterrupt(digitalPinToInterrupt(2), rotationR, CHANGE);
  Serial.begin(9600); // Starts the serial communication

  //==================[ Line sensor ]====================
  //declaring iuput for lineSenor
  pinMode(lineSenor1, INPUT);
  pinMode(lineSenor2, INPUT);
  pinMode(lineSenor3, INPUT);
  pinMode(lineSenor4, INPUT);
  pinMode(lineSenor5, INPUT);
  pinMode(lineSenor6, INPUT);
  pinMode(lineSenor7, INPUT);
  pinMode(lineSenor8, INPUT);



  //==================[ Leds and distance ]====================
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  pinMode(neoPIN, OUTPUT); // neo pixel as output
  pixels.begin();         //begin it
  pixels.setBrightness(50);
  pixels.show(); // Initialize all pixels to 'off'


  //==================[ bluetoth ]====================



}


void loop() {
  if (hasRacesStarted == false) {
    startRace();
  } else {


    // detectDistance();
    for (int i = 0; i < 8; i++) {
      if (analogRead(pinBlnWh[i]) > 750) {
        reading[i] = 1;
      }
      else if (analogRead(pinBlnWh[i]) < 750) {
        reading[i] = 0;
      }
      // showLineSensorReading();
    }//the sensors reading are stored in previously empty array
    int positionRobot = conversion(reading);//conversts reading to position
    //
    moving(positionRobot, reading); //move

    showLineSensorReading();



    //distance
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
    previousPosition = positionRobot;


  }

}




/*    =======================*/
/*    ======[funtions] ========*/
/*    =======================*/


void forward() {
  analogWrite(motorPin2, basicLeft);
  analogWrite(motorPin, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, basicRight);
}

void turnRight() {
  analogWrite(motorPin2, 0);  //Right Motor forword Pin
  analogWrite(motorPin, 150); //Right Motor backword Pin
  analogWrite(motorPin3, 0);  //Left Motor backword Pin
  analogWrite(motorPin4, 150); //Left Motor forword Pin
}

void turnLeft() {
  analogWrite(motorPin2, 150); //Right Motor forword Pin
  analogWrite(motorPin, 0);  //Right Motor backword Pin
  analogWrite(motorPin3, 150); //Left Motor backword Pin
  analogWrite(motorPin4, 0);  //Left Motor forword Pin
}

void stopRobot() {
  analogWrite(motorPin2, 0); //Right Motor forword Pin
  analogWrite(motorPin, 0); //Right Motor backword Pin
  analogWrite(motorPin3, 0); //Left Motor backword Pin
  analogWrite(motorPin4, 0); //Left Motor forward Pin

}



void showLineSensorReading() {

  Serial.print(reading[0]);
  Serial.print(",");
  Serial.print(reading[1]);
  Serial.print(",");
  Serial.print(reading[2]);
  Serial.print(",");
  Serial.print(reading[3]);
  Serial.print(",");
  Serial.print(reading[4]);
  Serial.print(",");
  Serial.print(reading[5]);
  Serial.print(",");
  Serial.print(reading[6]);
  Serial.print(",");
  Serial.print(reading[7]);

  Serial.println("-");
  delay(100);

}

int conversion(int reading[]) { //first of all it checks for outliers such as 10000000 and 00000001;


  int positionRobot = 0;
  if (reading[0] == 0 && reading[1] == 0 && reading[2] == 0 && reading[3] == 0 && reading[4] == 0 && reading[5] == 0 && reading[6] == 0 && reading[7] == 0) {
    positionRobot = -13;
    return positionRobot;
  }
  if (reading[0] == 0 && reading[1] == 0 && reading[2] == 0 && reading[3] == 0 && reading[4] == 0 && reading[5] == 0 && reading[6] == 0 && reading[7] == 1) {
    positionRobot = 13;
    return positionRobot;
  }
  if (reading[0] == 1 && reading[1] == 1 && reading[2] == 1 && reading[3] == 1 && reading[4] == 1 && reading[5] == 0 && reading[6] == 0 && reading[7] == 0) {
    positionRobot = 20;
    return positionRobot;
  }
  if (reading[0] == 0 && reading[1] == 0 && reading[2] == 0 && reading[3] == 1 && reading[4] == 1 && reading[5] == 1 && reading[6] == 1 && reading[7] == 1) {
    positionRobot = 30;
    return positionRobot;
  }

  if (reading[0] == 1 && reading[1] == 1 && reading[2] == 1 && reading[3] == 1 && reading[4] == 1 && reading[5] == 1 && reading[6] == 1 && reading[7] == 1) {
    positionRobot = 50;
    return positionRobot;
  }
  //if it isnt an outlier it goes here
  for (int i = 0; i < 7; i++) {
    if (reading[i] == 1) {
      positionRobot = positionRobot + posVal[i]; //this array are all the values. If the reading is 1 it sums if it is 0, it does nothing that way even if it is 11110000 it will work
    }
  }
  return positionRobot;//returns position
}


void moving(int positionRobot, int reading[]) {
  Serial.println("-");
  Serial.print("Poition");
  Serial.print(positionRobot);
  Serial.println("");

  if (positionRobot == 0 || previousPosition == 0) {
    analogWrite(motorPin2, basicLeft);
    analogWrite(motorPin4, basicRight);
    Serial.println("Straight");
  }

  if (positionRobot == 50 || previousPosition == 50) {
    analogWrite(motorPin2, 50);
    analogWrite(motorPin4, 160);
    Serial.println("Intersection");

  }
  if (positionRobot == 30 || previousPosition == 30) {
    analogWrite(motorPin2, 150);
    analogWrite(motorPin4, 80);
    Serial.println("right");
  }
  if (positionRobot == 20 || previousPosition == 20) {
    analogWrite(motorPin2, 150);
    analogWrite(motorPin4, 50);
    Serial.println("turn left");
  }

  if (positionRobot >= -7 && positionRobot < 0 ) { //TURN LEFT
    analogWrite(motorPin2, 80);//left weel
    analogWrite(motorPin4, 150);//right weel
    Serial.println("TURN LEFT");
  }
  if (positionRobot <= 7 && positionRobot > 0 ) {//TURN RIGHT
    analogWrite(motorPin2, 150);
    analogWrite(motorPin4, 80);
    Serial.println("TURN RIGHT");
  }
  
/*

  //if white so for 180 degree turn
  if (positionRobot == -13) {
    analogWrite(motorPin, basicLeft);
    analogWrite(motorPin3, basicRight);
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 255));
      pixels.show();
    }
    //make it make a u turn
    rotateLeft(200);
  }
*/
}








//distance
void detectDistance() {

  if (distance > 25) {
    //if distance above 10 neopixel off
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(137, 207, 240));
      pixels.show();
    }



  } else {
    //distance less then 10 neopixel on

    pixels.setPixelColor(3, pixels.Color(140, 255, 0 ));
    pixels.setPixelColor(0, pixels.Color(140, 255, 0 ));
    pixels.setPixelColor(1, pixels.Color(137, 207, 240));
    pixels.setPixelColor(2, pixels.Color(137, 207, 240));
    pixels.show();



  }



}


//posible start of Race needs to be checked out.
void detectRellayOfPin() {
  //go pick up pin and start the race
  if (distanceToNextRobot >= 25 || distanceToNextRobot <= 35) {
    forward();
    int countLines = 0;
    if (positionRobot = 50) {
      forward();
      countLines += 1;
    }

    if (countLines == 3) {
      delay(200);
      closeGripper();
    }
  }

}


void openGripper() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(gripperPin, HIGH);
    delayMicroseconds(1700);//pulse duration in microseconds
    digitalWrite(gripperPin, LOW);
    delay(20);
  }
}

void closeGripper() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(gripperPin, HIGH);
    delayMicroseconds(1000);//pulse duration in microseconds
    digitalWrite(gripperPin, LOW);
    delay(20);
  }
}

//endOfRace
void relayPin() {
}


// funtion to make it turn 180 degrees
void rotationL() {
  countL++;
}

void rotationR() {
  countR++;
}

void rotateLeft(int cycle) {
  if (countL < cycle) {

    analogWrite(motorPin2, 150);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin, 0);
    analogWrite(motorPin3, 0);
    countL++;

  } else {
    analogWrite(motorPin2, 0);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin, 0);
    analogWrite(motorPin3, 0);
    countL = 0;
    delay(800);
    

  }
}

void startRace() {
  analogWrite(motorPin2, basicLeft);
  analogWrite(motorPin4, basicRight);
  delay(1350);
  stopRobot();
  closeGripper();
  analogWrite(motorPin2, 0);
  analogWrite(motorPin4, basicRight);//left turn start could be calibrated more precise.
  analogWrite(motorPin4, basicRight);
  analogWrite(motorPin2, 0);
  delay(800);
  stopRobot();
  hasRacesStarted = true;

}
