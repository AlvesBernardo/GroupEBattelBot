  int PinLight0=A0;
  int PinLight1=A1;
  int PinLight2=A2;
  int PinLight3=A3;
  int PinLight4=A4;
  int PinLight5=A5;
  int PinLight6=A6;
  int PinLight7=A7;

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

void loop() {
  delay(1000);
  Serial.print(analogRead(A0));
  Serial.print("-");  
  Serial.print(analogRead(A1));
  Serial.print("-");
  Serial.print(analogRead(A2));
  Serial.print("-");
  Serial.print(analogRead(A3));
  Serial.print("-");
  Serial.print(analogRead(A4));
  Serial.print("-");
  Serial.print(analogRead(A5));
  Serial.print("-");
  Serial.print(analogRead(A6));
  Serial.print("-");
  Serial.print(analogRead(A7));
  Serial.println("-");

}
