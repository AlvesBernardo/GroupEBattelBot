int lineSensor1 = A0;
int test = 1;

void setup(){
    pinMode(lineSensor1, INPUT);
}

void loop(){
  test = 2;
}





/*
//defining motors and their speeds and array
const int motorPin = 11; //A1
const int motorPin2 = 10; //A2
const int motorPin3 = 6; //B1
const int motorPin4 = 5; //B2
const int motorR1 = 9  ;//R1
const int motorR2 = 3; //R2
const int turningRatio = 35;
const int basicSpeed = 250;
const int minumumSpeed = 75;
int posVal[] = { -4, -3, -2, -1, 1, 2, 3, 4}; //possible values
int positionOfRobot = 0;
int sensRead[6]={};
//line senor pin
int arrayForSensors[] = {A0, A1, A2, A3, A4, A6};
int lineSensor1 = A0;
int lineSensor2 = A1;
int lineSensor3 = A2;
int lineSensor4 = A3;
int lineSensor5 = A4;
int lineSensor6 = A5;
int lineSensor7 = A6;
int lineSensor8 = A7;




void setup(){
  
  //==================[ LineSensor ]====================
  pinMode(lineSensor1, INPUT);
  pinMode(lineSensor2, INPUT);
  pinMode(lineSensor3, INPUT);
  pinMode(lineSensor4, INPUT);
  pinMode(lineSensor5, INPUT);
  pinMode(lineSensor6, INPUT);
  pinMode(lineSensor7, INPUT);
  pinMode(lineSensor8, INPUT);


  //==================[ Motors]====================
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);

  
}

void loop() {
   //==================[ Moving ]====================
  if (detectDistance()) {
    stopCar();
  }
  else {
    for (int i = 0; i <6 ; i++) {
      if (analogRead(arrayForSensors[i]) > 800) {
        sensRead[i] = 1;
        Serial.println(1);
        delay(1000);
      }
      else if (analogRead(arrayForSensors[i]) < 480) {
        sensRead[i] = 0;
        Serial.println(0);
        delay(1000);
      }
    }//the sensors reading are stored in previously empty array
    int positionOfRobot = conversion(sensRead);//conversts reading to position
    Serial.println(positionOfRobot);
    loopForSensor(positionOfRobot);//move
  }


 
 }



 
int conversion(int reading[]) { //first of all it checks for outliers such as 10000000 and 00000001;
 
  if (reading[0] == 1 && reading[1] == 0 && reading[2] == 0 && reading[3] == 0 && reading[4] == 0 && reading[5] == 0) {
    positionOfRobot = -7;
    return positionOfRobot;
  }
  if (reading[0] == 0 && reading[1] == 0 && reading[2] == 0 && reading[3] == 0 && reading[4] == 0 && reading[5] == 0) {
    positionOfRobot = 7;
    return positionOfRobot;
  }//if it isnt an outlier it goes here
  for (int i = 0; i < 6; i++) {
    if (reading[i] == 1) {
      positionOfRobot = positionOfRobot + posVal[i]; //this array are all the values. If the reading is 1 it sums if it is 0, it does nothing that way even if it is 11110000 it will work
    }
  }
  return positionOfRobot;//returns position
}




//loop to calibrate our sensors

void loopForSensor(int positionOfRobot) {
  if (positionOfRobot == 0) {
    analogWrite(motorPin2, basicSpeed);
    analogWrite(motorPin4, basicSpeed);
  }
  if (positionOfRobot > 0) {
    analogWrite(motorPin2, basicSpeed - (turningRatio * positionOfRobot));
    analogWrite(motorPin4, basicSpeed);
  }
  if (positionOfRobot < 0) {
    analogWrite(motorPin2, basicSpeed + (turningRatio * positionOfRobot));
    analogWrite(motorPin4, basicSpeed);
  }



}
*/
