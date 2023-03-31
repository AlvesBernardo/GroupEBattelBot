// including library
#include <Adafruit_NeoPixel.h> //including file
#ifdef __AVR__
#include <avr/power.h>
#endif
#include<SoftwareSerial.h>

//defining pins for distance sensor
const int trigPin = 12; //distance sensor
const int echoPin = 13;//distance sensor
const int servoPin = 4; //not used

//defining motors
const int motorPin = 11; //A1
const int motorPin2 = 10; //A2
const int motorPin3 = 6; //B1
const int motorPin4 = 5; //B2

//rotation sensors
const int motorR1 = 3; //rotation sensor
const int motorR2 = 2; //rotation sensor

//gripper
int gripperPin = 8; 

//is intersectionDone
boolean isIttIntersection = false;
//var to count rotation
volatile int countL = 0;
volatile int countR = 0;

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
const int basicRight = 200;
const int basicLeft = 160;
const int minumumSpeed = 75;


// defines variables
long duration;
int distance;
int distanceToNextRobot;
int neoPIN = 7; //NEO pixel datapin
int NUMPIXELS = 4; //amount of numpixels


boolean test = false;


Adafruit_NeoPixel pixels(NUMPIXELS, neoPIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

//create servo object
//Servo servo;
int angle = 10;

boolean hasRacesStarted = false;
boolean isTurningRight = false;

boolean isIntersection = false;
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


  //==================[ gripper ]====================
  openGripper();


}


void loop() {
  //==================[ Start of race ]====================
  if (hasRacesStarted == false) {
    startRace();
  } else {

    //==================[ loop to convert values to 0 and 1 and converting lines ]====================
    detectDistance();
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






    //distance
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    //delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    //delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2;
    // Prints the distance on the Serial Monitor
    //Serial.print("Distance: ");
    //Serial.println(distance);
    previousPosition = positionRobot;
    moving(positionRobot, reading); //move

  }

}




/*    =======================*/
/*    ======[funtions] ========*/
/*    =======================*/

//==================[ show line readings ]====================

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


//==================[ depening on certain positions we want the robot to have a certain value ]====================
int conversion(int reading[]) { //first of all it checks for outliers such as 10000000 and 00000001;


  int positionRobot = 0;
  if (reading[0] == 0 && reading[1] == 0 && reading[2] == 0 && reading[3] == 0 && reading[4] == 0 && reading[5] == 0 && reading[6] == 0 && reading[7] == 0) {
    positionRobot = -13;
    return positionRobot;
  }
  if (reading[0] == 0 && reading[1] == 0 && reading[2] == 0 && reading[3] == 1 && reading[4] == 1 && reading[5] == 0 && reading[6] == 0 && reading[7] == 0) {
    positionRobot = 100;
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
    isIntersection = true;

  }
  //if it isnt an outlier it goes here
  for (int i = 0; i < 7; i++) {
    if (reading[i] == 1) {
      positionRobot = positionRobot + posVal[i]; //this array are all the values. If the reading is 1 it sums if it is 0, it does nothing that way even if it is 11110000 it will work
    }
  }
  return positionRobot;//returns position
}


//==================[ reading the position then making a certain movemen ]====================

void moving(int positionRobot, int reading[]) {
  //showLineSensorReading();

  if (reading[3] == 1 && reading[4]   ==  1) {
    forward();

  }
  /*
    if ( positionRobot == 50) {
      analogWrite(motorPin2, basicLeft);//left weel
      analogWrite(motorPin4, basicRight);//right weel
      intersectionRightTurning(38);
    }

  */


  if ((reading[3] == 1 || reading[4] ==  1) && reading[5] ==  1  && reading[6] == 1  && reading[7] == 1 || isIntersection == true) {
    stopRobot();
    isIntersection = true;
  }
  if (isIntersection == true)
  {
    intersectionRightTurning(25);

  }


  if (reading[0] == 0 && reading[1]   ==  0 && reading[2] == 0 && reading[3] ==  0  && reading[4] == 0 && reading[5]   ==  0  && reading[6] == 0 && reading[7]  ==  0 && previousPosition != 50 && isIntersection == false) {
    if (positionRobot != 100) {
      if (isIntersection == false)
      {
        rotateLeft(25);
      }
    }
  }


  if ( isIntersection == false ) {
    if (positionRobot >= -7 && positionRobot < 0 ) { //TURN LEFT
      analogWrite(motorPin2, 100);//left weel
      analogWrite(motorPin4, 104);//right weel
      Serial.println("TURN LEFT");
    }
    if (positionRobot <= 7 && positionRobot > 0 ) {//TURN RIGHT

      analogWrite(motorPin2, 140);
      analogWrite(motorPin4, 100);
      Serial.println("TURN RIGHT");
    }

  }
  /*
    //intersection Turn left
    if (reading[0] == 1 && reading[1] ==  1  && reading[2] == 1  && reading[3] == 1 || reading[4] ==  1 && reading[5] == 0 && reading[6] == 0  && reading[7] == 0) {
    rotateLeftTurn(55);
    Serial.println("ohh i need to turn left");
    }

  */
  //turn right


  /*
    if (isIntersection == false) {
      //if white go for 180 degree turn
      if (positionRobot == -13 && previousPosition != 50  && positionRobot != 0 ) {
        for (int i = 0; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 255));
          pixels.show();
        }
        //make it make a u turn
        rotateLeft(150);

      }
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




//==================[ rotation sensors ]====================
// funtion to make it turn 180 degrees
void rotationL() {
  countL++;
}

void rotationR() {
  countR++;
}

void rotateIntersection(int cycle) {
  if (countL < cycle) {
    analogWrite(motorPin2, HIGH);
    analogWrite(motorPin3, HIGH);
    analogWrite(motorPin2, 200);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin, 0);
    analogWrite(motorPin3, 200);
    delay(100);
    countL++;
  } else {
    analogWrite(motorPin2, 200);
    analogWrite(motorPin4, 200);
    analogWrite(motorPin, 0);
    analogWrite(motorPin3, 0);
    countL = 0;
    delay(800);
    isIttIntersection = false;


  }
}

void rotateLeftTurn(int cycle) {
  if (countL < cycle) {
    analogWrite(motorPin, HIGH);
    analogWrite(motorPin4, HIGH);
    analogWrite(motorPin4, 180);
    analogWrite(motorPin, 180);
    analogWrite(motorPin3, 0);
    analogWrite(motorPin2, 0);
    delay(100);
    countL++;

  } else {

    analogWrite(motorPin2, 0);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin, 0);
    analogWrite(motorPin3, 0);
    countL = 0;
    delay(300);


  }

}

void rotateLeft(int cycle) {
  if (positionRobot != 50) {
    if (positionRobot != 100) {
      if (countL < cycle) {
        analogWrite(motorPin2, 0);
        analogWrite(motorPin4, 180);
        analogWrite(motorPin, 180);
        analogWrite(motorPin3, 0);
        delay(250);
        countL++;

      } else {
        analogWrite(motorPin2, 180);
        analogWrite(motorPin4, 180);
        analogWrite(motorPin, 0);
        analogWrite(motorPin3, 0);
        countL = 0;
        delay(800);


      }
    }
  }
}

void rotateRight (int cycle) {
  if (countL < cycle) {
    analogWrite(motorPin2, HIGH);
    analogWrite(motorPin3, HIGH);
    analogWrite(motorPin2, 180);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin, 0);
    analogWrite(motorPin3, 180);
    delay(100);
    countL++;
  } else {
    analogWrite(motorPin2, 0);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin, 0);
    analogWrite(motorPin3, 0);
    countL = 0;
    delay(800);
    isTurningRight = false;


  }
}


//was suppose to be left turning but do to the rest of the code this will be right turning now on intersections
void  intersectionRightTurning (int cycle) {
  stopRobot();
  if (countL < cycle) {
    analogWrite(motorPin4, 0);
    analogWrite(motorPin, 0);
    analogWrite(motorPin2, 180);
    analogWrite(motorPin3, 0);
    delay(150);
    countL++;
    isIntersection = true;

  } else {
    analogWrite(motorPin2, 0);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin, 0);
    analogWrite(motorPin3, 0);
    countL = 0;
    isIntersection = false;


  }
}




//==================[ start and ending of the race ]====================
void startRace() {

  analogWrite(motorPin2, basicLeft);
  analogWrite(motorPin4, basicRight);
  delay(1400);
  stopRobot();
  analogWrite(motorPin2, 0);
  delay(150);
  closeGripper();
  analogWrite(motorPin4, basicRight);//left turn start could be calibrated more precise.
  analogWrite(motorPin4, basicRight);
  analogWrite(motorPin2, 0);
  delay(800);
  stopRobot();
  hasRacesStarted = true;
}
//funtion for the ending of the race
void endRace() {
  openGripper();
  analogWrite(motorPin, basicLeft);
  analogWrite(motorPin3, basicRight);
  delay(300);
  stopRobot();
}


void waitForRobot(){
  }

void openGripper() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(gripperPin, HIGH);
    delayMicroseconds(1700);//pulse duration in microseconds
    digitalWrite(gripperPin, LOW);
    delay(20);
  }
}

void closeGripper() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(gripperPin, HIGH);
    delayMicroseconds(1000);//pulse duration in microseconds
    digitalWrite(gripperPin, LOW);
    delay(20);
  }
}

/*
  void detectRobotCatchPin(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  //max 200cm. if returns more, it detected nothing.
  digitalWrite(trigPin, LOW);


  duration = pulseIn(echo, HIGH);
  distance = duration*0.0344/2;
  if(distance<35){
    return true;
  }
  return false;


  }
*/


//==================[ 4 movement funtions could be usefull ]====================
void forward() {
  analogWrite(motorPin, 255);
  analogWrite(motorPin4, 255);
  analogWrite(motorPin2, basicLeft);
  analogWrite(motorPin, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, basicRight);
}

void turnRight() {
  analogWrite(motorPin2, 180);  //Right Motor forword Pin
  analogWrite(motorPin, 0); //Right Motor backword Pin
  analogWrite(motorPin3, 0);  //Left Motor backword Pin
  analogWrite(motorPin4, 150); //Left Motor forword Pin
}

void turnLeft() {
  analogWrite(motorPin2, 150); //Right Motor forword Pin
  analogWrite(motorPin, 0);  //Right Motor backword Pin
  analogWrite(motorPin3, 0 ); //Left Motor backword Pin
  analogWrite(motorPin4, 180);  //Left Motor forword Pin
}

void stopRobot() {
  analogWrite(motorPin2, 0); //Right Motor forword Pin
  analogWrite(motorPin, 0); //Right Motor backword Pin
  analogWrite(motorPin3, 0); //Left Motor backword Pin
  analogWrite(motorPin4, 0); //Left Motor forward Pin


}
