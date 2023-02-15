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

  int pinArr[]={A7, A6, A5, A4, A3, A2, A1, A0};

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
int sensRead[7];
void loop() {
  for(int i = 0; i<7; i++){
    if(analogRead(pinArr[i])>925){
      sensRead[i]=1;
    }
    else if(analogRead(pinArr[i])<400){
      sensRead[i]=0;
    }
  }
  for(int i=0; i<7; i++){
    Serial.print(sensRead[i]);
    Serial.print("-");
  }
  Serial.println("-");
  if(sensRead[1]==0){
    goForward();
  }
  else{
    stop();
  }
  Serial.println(conversion(sensRead));
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
int conversion(int reading[]){
  int position = 0;
  int relativePos = -4;
  if((reading[0]==1) && (reading[1]==0) && (reading[2]==0) && (reading[3]==0) && (reading[4]==0) && (reading[5]==0) && (reading[6]==0) && (reading[7]==0)){
      position = -6;
      return position;
    }
  if((reading[0]==0) && (reading[1]==0) && (reading[2]==0) && (reading[3]==0) && (reading[4]==0) && (reading[5]==0) && (reading[6]==0) && (reading[7]==1)){
    position = 6;
    return position;
  }
  for(int i = 0; i<7; i++){
    if(reading[i]==1){
      position=position+relativePos;
    }
    relativePos++;
    if(relativePos==0){
      relativePos++;
    }
  }
  return position;
}