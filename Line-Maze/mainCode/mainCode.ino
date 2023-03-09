// including library
#include <Adafruit_NeoPixel.h> //including file
#ifdef __AVR__
#include <avr/power.h>
#endif
//#include <Servo.h>

//defining pins for distance sensor
const int trigPin = 12;
const int echoPin = 13;
const int servoPin = 4;

//defining motors
const int motorPin = 11; //A1
const int motorPin2 = 10; //A2
const int motorPin3 = 6; //B1
const int motorPin4 = 5; //B2


//line senor pin
const int lineSenor1 = A6;
const int lineSenor2 = A0;
const int lineSenor3 = A1;
const int lineSenor4 = A2;
const int lineSenor5 = A3;
const int lineSenor6 = A4;

//left to right
int pinBlnWh[] = {A6, A0, A1, A2, A3, A4};
int positionVal[]={-3,-2,-1,1,2,3};
int reading[6]={};
int positionRobot= 0;

//speeds
const int turningRatio = 35;
const int basicSpeed = 200;
const int minumumSpeed = 75;


// defines variables
long duration;
int distance;
int neoPIN = 7; //NEO pixel datapin
int NUMPIXELS = 4;


Adafruit_NeoPixel pixels(NUMPIXELS, neoPIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

//create servo object
//Servo servo;
int angle = 10;


//bluetooth adapter
int state = 0;
char data = 0;
int rx =8; //reciver
int tx =  2; //transmiter
const int trig = 4;
const int echo = 2;


void setup() {
  //motors declaration 
  pinMode(motorPin2, OUTPUT);
  pinMode (motorPin4, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode (motorPin3, OUTPUT);
  Serial.begin(9600); // Starts the serial communication


  //declaring iuput for lineSenor
  pinMode(lineSenor1, INPUT);
  pinMode(lineSenor2, INPUT);
  pinMode(lineSenor3, INPUT);
  pinMode(lineSenor4, INPUT);
  pinMode(lineSenor5, INPUT);
  pinMode(lineSenor6, INPUT);



   //==================[ Leds ]====================
   pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  pinMode(neoPIN, OUTPUT); // neo pixel as output
  pixels.begin();         //begin it
  pixels.setBrightness(50);
  pixels.show(); // Initialize all pixels to 'off'


 //==================[ bluetoth ]====================
   pinMode(tx, OUTPUT);
   pinMode(rx, INPUT);

  Serial.begin(9600);
    pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  
  }


void loop() {
  
 


  showLineSensorReading();

  for (int i = 0; i < 6; i++) {
      if (analogRead(pinBlnWh[i]) > 750) {
        reading[i] = 1;
      }
      else if (analogRead(pinBlnWh[i]) < 750) {
        reading[i] = 0;
      }
    }//the sensors reading are stored in previously empty array
    int positionRobot = conversion(reading);//conversts reading to position
  Serial.println(positionRobot);
    moving(positionRobot);//move

/*
   if ((analogRead(lineSenor2)   >=  700) && (analogRead(lineSenor3)   >=  700)) {
    forward();
  }
  else if ((analogRead(lineSenor7)  >=  700) && (analogRead(lineSenor8)  >= 700 )) {
    turnRight();
  }
  else if ((analogRead(lineSenor5) >= 700) && (analogRead(lineSenor6)  >=  700)) {
    turnLeft();
  }
*/

  //bluetooth
  bluethootAdapter();
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
    state = Serial.read(); // Reads the data from the serial port
 }



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


  detectDistance(); //call funtion to detect distance to certain object
  
  
  }



  
/*    =======================*/
/*    ======[funtions] ========*/
/*    =======================*/

  void forward() {
  analogWrite(motorPin2, 150);
  analogWrite(motorPin, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, 150);
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

void Stop() {
  analogWrite(motorPin2, 0); //Right Motor forword Pin
  analogWrite(motorPin, 0); //Right Motor backword Pin
  analogWrite(motorPin3, 0); //Left Motor backword Pin
  analogWrite(motorPin4, 0); //Left Motor forward Pin

}



void showLineSensorReading() {

  Serial.print(analogRead(lineSenor1));
  Serial.print(",");
  Serial.print(analogRead(lineSenor2));
  Serial.print(",");
  Serial.print(analogRead(lineSenor3));
  Serial.print(",");
  Serial.print(analogRead(lineSenor4));
  Serial.print(",");
  Serial.print(analogRead(lineSenor5));
  Serial.print(",");
  Serial.print(analogRead(lineSenor6));
  Serial.println("-");
  delay(1000);

}

int conversion(int reading[]){//first of all it checks for outliers such as 10000000 and 00000001;
  
  if(reading[0]==1 && reading[1]==0 && reading[2]==0 && reading[3]==0 && reading[4]==0 && reading[5]==0 && reading[6]==0 && reading[7]==0){
      positionRobot = -7;
      return positionRobot;
    }
  if(reading[0]==0 && reading[1]==0 && reading[2]==0 && reading[3]==0 && reading[4]==0 && reading[5]==0 && reading[6]==0 && reading[7]==1){
    positionRobot = 7;
    return positionRobot;
  }//if it isnt an outlier it goes here
  for(int i = 0; i<6; i++){
    if(reading[i]==1){
      positionRobot=positionRobot+positionVal[i];//this array are all the values. If the reading is 1 it sums if it is 0, it does nothing that way even if it is 11110000 it will work
    }
  }
  return positionRobot;//returns position
}


void moving(int positionRobot){
  if (positionRobot == 0) {
    analogWrite(motorPin2,basicSpeed );
    analogWrite(motorPin4,basicSpeed );
  }
   if (positionRobot > 0  && positionRobot<12) {
     if(turningRatio*positionRobot<255){
    analogWrite(motorPin2, basicSpeed - (turningRatio * positionRobot));
    analogWrite(motorPin4, basicSpeed);
     }else {
    analogWrite(motorPin2, 0);
    analogWrite(motorPin4, basicSpeed);
      
      }
  }
  if (positionRobot < 0  && positionRobot >- 12) {
    if(turningRatio*positionRobot>-255){
    analogWrite(motorPin2, basicSpeed + (turningRatio * positionRobot));
    analogWrite(motorPin4, basicSpeed);
    }else{
     analogWrite(motorPin2, basicSpeed);
    analogWrite(motorPin4, 0);
      
      }
  }
   if(positionRobot>12){
    analogWrite(motorPin2, basicSpeed);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin2, basicSpeed);
  }
  if(positionRobot<-12){
    analogWrite(motorPin2, 0);
    analogWrite(motorPin4, basicSpeed);
    analogWrite(motorPin4, basicSpeed  );
    analogWrite(motorPin2, 0);
  }

  }


//for bluethoot
  
void bluethootAdapter(){

    if(Serial.available() > 0)       /*check for serial data availability*/
    {
        data = Serial.read();        /*read data coming from Bluetooth device*/
        Serial.print(data);          /*print values on serial monitor*/
        Serial.print("\n");          /*print new line*/
        if(data == '1')              /*check data value*/
            digitalWrite(3, HIGH);  /*Turn ON LED if serial data is 1*/
        else if(data == '0')         /*check data value*/
            digitalWrite(3, LOW);   /*Turn OFF LED if serial data is 0*/
    }                     

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
        analogWrite(motorPin2, 0); //Right Motor forword Pin
        analogWrite(motorPin4, 0); //Left Motor forward Pin
        pixels.show();

    

  }



}
