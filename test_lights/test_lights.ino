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

  int pinArr[]={A7, A6, A5, A4, A3, A2, A1, A0};

//==================[ SETUP ]==================================
void setup() {
  Serial.begin(9600);
  pinMode(PinLight0, INPUT);
  pinMode(PinLight1, INPUT);
  pinMode(PinLight2, INPUT);
  pinMode(PinLight3, INPUT);
  pinMode(PinLight4, INPUT);
  pinMode(PinLight5, INPUT);
  pinMode(PinLight6, INPUT);
  pinMode(PinLight7, INPUT);

}
//=====================[ LOOP BEGINING ] =======================
int sensRead[7];
void loop() {
  delay(1000);
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

  //it goes from left to right. Pin 7 is the on the far left, pin 0 is on the far right.
  /*
  Serial.print(analogRead(PinLight7));
  Serial.print("-");  
  Serial.print(analogRead(PinLight6));
  Serial.print("-");
  Serial.print(analogRead(PinLight5));
  Serial.print("-");
  Serial.print(analogRead(PinLight4));
  Serial.print("-");
  Serial.print(analogRead(PinLight3));
  Serial.print("-");
  Serial.print(analogRead(PinLight2));
  Serial.print("-");
  Serial.print(analogRead(PinLight1));
  Serial.print("-");
  Serial.print(analogRead(PinLight0));
  Serial.println("-");
*/
}
//=========================[ END OF PROGGRAM]=======================



//========================[ FUNCTIONS ]===============================
void 
