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
int posVal[]={-3,-2,-1,1,2,3};
int reading[6]={};
int positionRobot= 0;

//speeds
const int turningRatio = 35;
const int basicSpeed = 250;
const int minumumSpeed = 75;


void setup() {

  pinMode(motorPin2, OUTPUT);
  pinMode (motorPin4, OUTPUT);
  Serial.begin(9600); // Starts the serial communication


  //declaring iuput for lineSenor
  pinMode(lineSenor1, INPUT);
  pinMode(lineSenor2, INPUT);
  pinMode(lineSenor3, INPUT);
  pinMode(lineSenor4, INPUT);
  pinMode(lineSenor5, INPUT);
  pinMode(lineSenor6, INPUT);

  
  }


void loop() {
  
  for(int i = 0; i<6; i++){
    if(analogRead(pinBlnWh[i])>700){
      reading[i] = 1;
    }
    else{
      reading[i] = 0;
    }
  }

  for(int i = 0; i<6; i++){
    Serial.print(reading[i]);
    Serial.print("-");
  }
  Serial.print("-");


  showLineSensorReading();

  for (int i = 0; i < 6; i++) {
      if (analogRead(pinBlnWh[i]) > 800) {
        reading[i] = 1;
      }
      else if (analogRead(pinBlnWh[i]) < 550) {
        reading[i] = 0;
      }
    }//the sensors reading are stored in previously empty array
    int positionRobot = conversion(reading);//conversts reading to position

    moving(positionRobot);//move
 

  
  
  }

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
   Serial.print(",--,");
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
  for(int i = 0; i<8; i++){
    if(reading[i]==1){
      positionRobot=positionRobot+posVal[i];//this array are all the values. If the reading is 1 it sums if it is 0, it does nothing that way even if it is 11110000 it will work
    }
  }
  return positionRobot;//returns position
   Serial.print(positionRobot);
  ^
  
}


void moving(int positionRobot){
   Serial.print(positionRobot);
   Serial.println("end");
  if (positionRobot == 0) {
    analogWrite(motorPin2,150 );
    analogWrite(motorPin4,150 );
   
  }
   if (positionRobot > 0) {
    analogWrite(motorPin2, basicSpeed - (turningRatio * positionRobot));
    analogWrite(motorPin4, basicSpeed);
  }
  if (positionRobot < 0) {
    analogWrite(motorPin2, basicSpeed + (turningRatio * positionRobot));
    analogWrite(motorPin4, basicSpeed);
  }

  
  }
