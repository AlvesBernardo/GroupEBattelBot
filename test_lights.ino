  int TestPin=A5;

void setup() {
  Serial.begin(9600);
  pinMode(TestPin, INPUT);
  pinMode(A6, INPUT);

}

void loop() {
  delay(1000);
  int incomingByte = analogRead(A5);
  Serial.println(incomingByte);
  

}
