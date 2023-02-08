int motorBPin2 = 6; //left wheel backward
int motorBPin1 = 9; //left wheel forward
int motorAPin2 = 10; //right wheel forward
int motorAPin1 = 11; //right wheel backwards

void setup() {
  pinMode(motorBPin2, OUTPUT);
  pinMode(motorBPin1, OUTPUT);
  pinMode(motorAPin1, OUTPUT);
  pinMode(motorAPin2, OUTPUT);


}

void loop() {
  analogWrite(motorBPin1, 200);
  analogWrite(motorAPin2, 200);
}
