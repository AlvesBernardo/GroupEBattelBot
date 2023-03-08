/*************************
*    Line folower        *
*************************/
  
//==================[ DEFINITION OF PINS ]====================
const int trunRatio = 35;
const int baseSpeed = 250;

const int motorBPin2 = 6; //left wheel backward
const int motorBPin1 = 9; //left wheel forward
const int motorAPin2 = 10; //right wheel forward
const int motorAPin1 = 11; //right wheel backwards

int pinArr[]={A7, A6, A5, A4, A3, A2, A1, A0};//initialisation of pins
int posVal[]={-4,-3,-2,-1,1,2,3,4};//possible values

int position = 0;



const int trig = 4;
const int echo = 2;

long duration;
int distance;

//==================[ SETUP ]==================================
void setup() {
  Serial.begin(9600);
  pinMode(pinArr, INPUT);

  pinMode(motorBPin2, OUTPUT);
  pinMode(motorBPin1, OUTPUT);
  pinMode(motorAPin1, OUTPUT);
  pinMode(motorAPin2, OUTPUT);
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}
//=====================[ LOOP BEGINING ] =======================
int sensRead[8];//empty array for reading results
void loop() {
  if(detectObject()){
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
    Serial.println(position);
    move(position);//move
  }
}
//=========================[ END OF PROGGRAM]=======================



//========================[ FUNCTIONS ]===============================
void stop(){
  analogWrite(motorBPin1, 0);
  analogWrite(motorAPin2, 0);
  analogWrite(motorAPin1, 0);
  analogWrite(motorBPin2, 0);
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

void move(int position){//movement based on the reading of position
  if(position == 0){
    analogWrite(motorAPin1, 0);
    analogWrite(motorBPin2, 0);
    analogWrite(motorBPin1, baseSpeed);
    analogWrite(motorAPin2, baseSpeed);    
  }
  if(position>0 && position<12){
    if(trunRatio*position<255){
      analogWrite(motorAPin1, 0);
      analogWrite(motorBPin2, 0);
      analogWrite(motorAPin2, baseSpeed-(trunRatio*position));
      analogWrite(motorBPin1, baseSpeed);
    }
    else{
      analogWrite(motorAPin1, 0);
      analogWrite(motorBPin2, 0);
      analogWrite(motorAPin2, 0);
      analogWrite(motorBPin1, baseSpeed);
    }
  }
  if(position<0 && position>-12){
    if(trunRatio*position>-255){
      analogWrite(motorAPin1, 0);
      analogWrite(motorBPin2, 0);
      analogWrite(motorBPin1, baseSpeed+(trunRatio*position));
      analogWrite(motorAPin2, baseSpeed);
    }
    else{
      analogWrite(motorAPin1, 0);
      analogWrite(motorBPin2, 0);
      analogWrite(motorBPin1, 0);
      analogWrite(motorAPin2, baseSpeed);
    }
  }
  if(position>12){
    analogWrite(motorAPin1, baseSpeed);
    analogWrite(motorBPin2, 0);
    analogWrite(motorAPin2, 0);
    analogWrite(motorBPin1, baseSpeed);
  }
  if(position<-12){
    analogWrite(motorAPin1, 0);
    analogWrite(motorBPin2, baseSpeed);
    analogWrite(motorAPin2, baseSpeed);
    analogWrite(motorBPin1, 0);
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
    stop();
  }
}