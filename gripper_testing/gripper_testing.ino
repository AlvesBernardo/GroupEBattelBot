const int gripperPin = 13;

void setup() {
  pinMode(gripperPin, OUTPUT);
}

void loop() {
  closeGripper();
  delay(2000);
  openGripper();
  delay(2000);
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
  for(int i = 0; i<10; i++){
    digitalWrite(gripperPin, HIGH);
    delayMicroseconds(1000);//pulse duration in microseconds
    digitalWrite(gripperPin, LOW);
    delay(20);
  }
}