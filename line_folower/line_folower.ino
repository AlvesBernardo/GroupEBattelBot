/*************************
*    Line folower        *
*************************/
  
//==================[ DEFINITION OF PINS ]====================
  const int PinLight0=A0;
  const int PinLight1=A1;
  const int PinLight2=A2;
  const int PinLight3=A3;
  const int PinLight4=A4;
  const int PinLight5=A5;
  const int PinLight6=A6;
  const int PinLight7=A7;

  int motorBPin2 = 6; //left wheel backward
  int motorBPin1 = 9; //left wheel forward
  int motorAPin2 = 10; //right wheel forward
  int motorAPin1 = 11; //right wheel backwards

  int pinArr[]={A7, A6, A5, A4, A3, A2, A1, A0};//initialisation of pins
  int posVal[]={-4,-3,-2,-1,1,2,3,4};//possible values

  int position = 0;

//==================[ SETUP ]==================================
void setup() {
  Serial.begin(9600);
  pinMode(pinArr, INPUT);

  pinMode(motorBPin2, OUTPUT);
  pinMode(motorBPin1, OUTPUT);
  pinMode(motorAPin1, OUTPUT);
  pinMode(motorAPin2, OUTPUT);

}
//=====================[ LOOP BEGINING ] =======================
int sensRead[8];//empty array for reading results
void loop() {
  delay(1000);
  for(int i = 0; i<8; i++){
    if(analogRead(pinArr[i])>750){
      sensRead[i]=1;
    }
    else if(analogRead(pinArr[i])<500){
      sensRead[i]=0;
    }
  }//the sensors reading are stored in previously empty array
  /*
  for(int i=0; i<8; i++){
    Serial.print(sensRead[i]);
    Serial.print("-");
  }
  Serial.println("-");
  /*
  Serial.println("-");
  if(sensRead[1]==0){
    goForward();
  }
  else{
    stop();
  }*/

  int position = conversion(sensRead);//conversts reading to position
  Serial.println(position);
  move(position);//move
}
//=========================[ END OF PROGGRAM]=======================



//========================[ FUNCTIONS ]===============================
void goForward(){
  analogWrite(motorBPin1, 150);
  analogWrite(motorAPin2, 150);
}
void stop(){
  analogWrite(motorBPin1, 0);
  analogWrite(motorAPin2, 0);
}
int conversion(int reading[]){//first of all it checks for outliers such as 10000000 and 00000001;
  int position = 0;
  if(reading[0]==1 && reading[1]==0 && reading[2]==0 && reading[3]==0 && reading[4]==0 && reading[5]==0 && reading[6]==0 && reading[7]==0){
      position = -7;
      return position;
    }
  if(reading[0]==0 && reading[1]==0 && reading[2]==0 && reading[3]==0 && reading[4]==0 && reading[5]==0 && reading[6]==0 && reading[7]==1){
    position = 7;
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
    analogWrite(motorBPin1, 250);
    analogWrite(motorAPin2, 250);    
  }
  if(position>0){
    analogWrite(motorBPin1, 250-(16*position));
  }
  if(position<0){
    analogWrite(motorAPin2, 250+(16*position));    
  }
  if(position>6){
    analogWrite(motorBPin1, 138);
    analogWrite(motorAPin2, 250);
  }
  if(position<(-7)){
    analogWrite(motorBPin1, 250);
    analogWrite(motorAPin2, 138);
  }
}