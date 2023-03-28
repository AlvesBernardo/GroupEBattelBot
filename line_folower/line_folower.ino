/*************************
*    Line folower        *
*************************/
//==================[INCLUSION OF STUFF]======================

#include <Adafruit_NeoPixel.h>

//==================[ DEFINITION OF PINS ]====================

int length = 10;

const int trunRatio = 35;
const int baseSpeed = 250;

const int motorBPin2 = 6; //left wheel backward
const int motorBPin1 = 9; //left wheel forward
const int motorAPin2 = 10; //right wheel forward
const int motorAPin1 = 11; //right wheel backwards

int pinArr[]={A7, A6, A5, A4, A3, A2, A1, A0};//initialisation of pins
int posVal[]={-4,-3,-2,-1,1,2,3,4};//possible values

int position = 0;

const int maxIntercectionDuration = 10;//mesuring unit used to stop, the smaller it is the faster it stops. Make it too small it will stop at the intersections.
int curentIntercectionDuration = 0;

int neoPin = 7;//neopixels declaration
int NUMPIXELS = 4;
Adafruit_NeoPixel pixels(NUMPIXELS, neoPin, NEO_GRB + NEO_KHZ800);
const int GRBdef[] = {171, 140, 153};
const int GRBforward[] = {255, 0, 0};
const int GRBturn[] = {165, 255, 0};
const int GRBextremeTurn[] = {0, 255, 255};
const int GRBstop[] = {0, 255, 0};
const int GRBevsasion[] = {0, 0, 255};


const int trig = 4;
const int echo = 2;

const int gripperPin = 13;

long duration;
int distance;

bool isRaceStarted = false;//switchable for testing

//==================[ SETUP ]==================================
void setup() {
  Serial.begin(9600);
  
  pinMode(gripperPin, OUTPUT);
  
  pinMode(pinArr, INPUT);

  pinMode(motorBPin2, OUTPUT);
  pinMode(motorBPin1, OUTPUT);
  pinMode(motorAPin1, OUTPUT);
  pinMode(motorAPin2, OUTPUT);
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(gripperPin, OUTPUT);

  pinMode(neoPin,OUTPUT);
  pixels.begin();
  pixels.setBrightness(50);
  for(int i=0; i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(GRBdef[0], GRBdef[1], GRBdef[2]));
  }
  pixels.show();
}
//=====================[ LOOP BEGINING ] =======================




  
int sensRead[8];//empty array for reading results
void loop() {
  if(isRaceStarted){
    if(detectObject()){//evasion of objects
          evade();    
    }
    else{
      for(int i = 0; i<8; i++){
        if(analogRead(pinArr[i])>750){
          sensRead[i]=1;
        }
        else if(analogRead(pinArr[i])<750){
          sensRead[i]=0;
        }
      }//the sensors reading are stored in previously empty array
      
      int position = conversion(sensRead);//conversts reading to position

      if(isIntercection(sensRead)){
        curentIntercectionDuration++;
        //checks for the intersections
      }
      else{
        curentIntercectionDuration = 0;
      }
      if(curentIntercectionDuration>maxIntercectionDuration){//it will not stop at small intersections, but wull at the massive black cube at the end
        stop();
        raceEnd();
        while(true){
          delay(1);
          //end of the race
        }
      }
      //Serial.println(curentIntercectionDuration);
      //Serial.println(position);
      move(position, sensRead);//move
    }
  }
  else if(detectRobot()){//start sequence of the race, executed once, but since it is movement, it is in loop
    delay(300);
    startRace();
    isRaceStarted=true;
  }

}
//=========================[ END OF PROGGRAM]=======================



//========================[ FUNCTIONS ]===============================
void stop(){
  analogWrite(motorBPin1, 0);
  analogWrite(motorAPin2, 0);
  analogWrite(motorAPin1, 0);
  analogWrite(motorBPin2, 0);

  for(int i=0; i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(GRBstop[0], GRBstop[1], GRBstop[2]));
  }
  pixels.show();
}
int conversion(int reading[]){//first of all it checks for outliers such as 10000000 and 00000001;
  int position = 0;
  if(reading[0]==1 && reading[1]==0 && reading[2]==0 && reading[3]==0 && reading[4]==0 && reading[5]==0 && reading[6]==0 && reading[7]==0){
      position = -13;
      return position;
    }
  if(reading[0]==0 && reading[1]==0 && reading[2]==0 && reading[3]==0 && reading[4]==0 && reading[5]==0 && reading[6]==0 && reading[7]==1){
    position = 13;
    return position;
  }//if it isnt an outlier it goes here
  for(int i = 0; i<8; i++){
    if(reading[i]==1){
      position=position+posVal[i];//this array are all the values. If the reading is 1 it sums if it is 0, it does nothing that way even if it is 11110000 it will work
    }
  }
  return position;//returns position
}

void move(int position, int reading[]){//movement based on the reading of position
  if(reading[0]==0 && reading[1]==0 && reading[2]==0 && reading[3]==0 && reading[4]==0 && reading[5]==0 && reading[6]==0 && reading[7]==0){//failsave
    delay(100);
    return;
  }
  if(position == 0){
    analogWrite(motorAPin1, 0);
    analogWrite(motorBPin2, 0);
    analogWrite(motorBPin1, baseSpeed);
    analogWrite(motorAPin2, baseSpeed);

    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.setPixelColor(1, pixels.Color(0, 0, 0));
    pixels.setPixelColor(2, pixels.Color(GRBforward[0], GRBforward[1], GRBforward[2]));
    pixels.setPixelColor(3, pixels.Color(GRBforward[0], GRBforward[1], GRBforward[2]));
    pixels.show();

  }
  if(position>0 && position<12){
    if(trunRatio*position<255){
      analogWrite(motorAPin1, 0);
      analogWrite(motorBPin2, 0);
      analogWrite(motorAPin2, baseSpeed-(trunRatio*position));
      analogWrite(motorBPin1, baseSpeed);

      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.setPixelColor(1, pixels.Color(GRBturn[0], GRBturn[1], GRBturn[2]));
      pixels.setPixelColor(2, pixels.Color(GRBturn[0], GRBturn[1], GRBturn[2]));
      pixels.setPixelColor(3, pixels.Color(0, 0, 0));
      pixels.show();
    }
    else{
      analogWrite(motorAPin1, 0);
      analogWrite(motorBPin2, 0);
      analogWrite(motorAPin2, 0);
      analogWrite(motorBPin1, baseSpeed);

      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.setPixelColor(1, pixels.Color(GRBturn[0], GRBturn[1], GRBturn[2]));
      pixels.setPixelColor(2, pixels.Color(GRBturn[0], GRBturn[1], GRBturn[2]));
      pixels.setPixelColor(3, pixels.Color(0, 0, 0));
      pixels.show();
    }
  }
  if(position<0 && position>-12){
    if(trunRatio*position>-255){
      analogWrite(motorAPin1, 0);
      analogWrite(motorBPin2, 0);
      analogWrite(motorBPin1, baseSpeed+(trunRatio*position));
      analogWrite(motorAPin2, baseSpeed);

      pixels.setPixelColor(0, pixels.Color(GRBturn[0], GRBturn[1], GRBturn[2]));
      pixels.setPixelColor(1, pixels.Color(0,0,0));
      pixels.setPixelColor(2, pixels.Color(0,0,0));
      pixels.setPixelColor(3, pixels.Color(GRBturn[0], GRBturn[1], GRBturn[2]));
      pixels.show();
    }
    else{
      analogWrite(motorAPin1, 0);
      analogWrite(motorBPin2, 0);
      analogWrite(motorBPin1, 0);
      analogWrite(motorAPin2, baseSpeed);

      pixels.setPixelColor(0, pixels.Color(GRBturn[0], GRBturn[1], GRBturn[2]));
      pixels.setPixelColor(1, pixels.Color(0,0,0));
      pixels.setPixelColor(2, pixels.Color(0,0,0));
      pixels.setPixelColor(3, pixels.Color(GRBturn[0], GRBturn[1], GRBturn[2]));
      pixels.show();
    }
  }
  if(position>12){
    analogWrite(motorAPin1, baseSpeed);
    analogWrite(motorBPin2, 0);
    analogWrite(motorAPin2, 0);
    analogWrite(motorBPin1, baseSpeed);

    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.setPixelColor(1, pixels.Color(GRBextremeTurn[0], GRBextremeTurn[1], GRBextremeTurn[2]));
    pixels.setPixelColor(2, pixels.Color(GRBextremeTurn[0], GRBextremeTurn[1], GRBextremeTurn[2]));
    pixels.setPixelColor(3, pixels.Color(0, 0, 0));
    pixels.show();
  }
  if(position<-12){
    analogWrite(motorAPin1, 0);
    analogWrite(motorBPin2, baseSpeed);
    analogWrite(motorAPin2, baseSpeed);
    analogWrite(motorBPin1, 0);

    pixels.setPixelColor(0, pixels.Color(GRBextremeTurn[0], GRBextremeTurn[1], GRBextremeTurn[2]));
    pixels.setPixelColor(1, pixels.Color(0,0,0));
    pixels.setPixelColor(2, pixels.Color(0,0,0));
    pixels.setPixelColor(3, pixels.Color(GRBextremeTurn[0], GRBextremeTurn[1], GRBextremeTurn[2]));
    pixels.show();
  }
}

bool detectObject(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  //max 200cm. if returns more, it detected nothing.
  digitalWrite(trig, LOW);


  duration = pulseIn(echo, HIGH);
  distance = duration*0.0344/2;
  if(distance<20){
    return true;
  }
  return false;
}

void evade(){
  if(detectObject()){
    stop();

    pixels.setPixelColor(0, pixels.Color(GRBevsasion[0], GRBevsasion[1], GRBevsasion[2]));//
    pixels.setPixelColor(1, pixels.Color(GRBevsasion[0], GRBevsasion[1], GRBevsasion[2]));
    pixels.setPixelColor(2, pixels.Color(GRBevsasion[0], GRBevsasion[1], GRBevsasion[2]));
    pixels.setPixelColor(3, pixels.Color(GRBevsasion[0], GRBevsasion[1], GRBevsasion[2]));//
    pixels.show();
    
    analogWrite(motorBPin1, baseSpeed);
    delay(300);
    analogWrite(motorBPin1, baseSpeed);
    analogWrite(motorAPin2, baseSpeed);
    delay(800);
    stop();
    analogWrite(motorAPin2, baseSpeed);
    delay(450);
    stop();
    analogWrite(motorBPin1, baseSpeed);
    analogWrite(motorAPin2, baseSpeed);
    delay(800);
  }
}

void startRace(){
  analogWrite(motorBPin1, baseSpeed);
  analogWrite(motorAPin2, baseSpeed);
  delay(950);
  stop();
  closeGripper();
  analogWrite(motorAPin1, 0);
  analogWrite(motorBPin2, baseSpeed);//left turn start could be calibrated more precise.
  analogWrite(motorAPin2, baseSpeed);
  analogWrite(motorBPin1, 0);
  delay(415);
  stop();
}
bool isIntercection(int reading[]){
  if(reading[0]==1 && reading[1]==1 && reading[2]==1 && reading[3]==1 && reading[4]==1 && reading[5]==1 && reading[6]==1 && reading[7]==1){
    return true;
  }
  return false;
}

void openGripper(){
  for(int i = 0; i<10; i++){
    digitalWrite(gripperPin, HIGH);
    delayMicroseconds(1700);//pulse duration in microseconds
    digitalWrite(gripperPin, LOW);
    delay(20);
  }
}

void closeGripper(){
  for(int i = 0; i < 10; i++){
    digitalWrite(gripperPin, HIGH);
    delayMicroseconds(1000);//pulse duration in microseconds
    digitalWrite(gripperPin, LOW);
    delay(20);
  }
}

bool detectRobot(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  //max 200cm. if returns more, it detected nothing.
  digitalWrite(trig, LOW);


  duration = pulseIn(echo, HIGH);
  distance = duration*0.0344/2;
  if(distance<25){
    return true;
  }
  return false;
}
void raceEnd(){
  analogWrite(motorBPin2, baseSpeed);
  analogWrite(motorAPin1, baseSpeed);
  delay(100);
  openGripper();
  analogWrite(motorBPin2, baseSpeed);
  analogWrite(motorAPin1, baseSpeed);
  delay(300);
  stop();
}
