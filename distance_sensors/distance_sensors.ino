const int trig = 4;
const int echo = 2;

long duration;

int distance;

void setup() {
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  delay(1000);

  digitalWrite(trig, LOW);
  delayMicroseconds(2);


  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
//max 200 cm

  digitalWrite(trig, LOW);


  duration = pulseIn(echo, HIGH);
  distance = duration*0.0344/2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(100);
}
