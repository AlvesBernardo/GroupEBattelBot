#define servoPin 8
#define gripperPin 13
#define servoMinPulse 500
#define servoCenterPulse 1400 // 
#define servoMaxPulse 2400 //in microseconds

#define gripper_open_pulse 1600
#define gripper_close_pulse  971 // 
#define servoPulseRepeat 10 // number of pulse send to servo

const int a1 = 12; // Pin for motor 1 (Right), direction 1 (clockwise)
const int a2 = 11; // Pin for motor 1 (Right), direction 2 (anti-clockwise)
const int b1 = 7; // Pin for motor 2 (left), direction 1 (clockwise)
const int b2 = 5; // Pin for motor 2 (left), direction 2 (anti-clockwise)
const int speed = 255; //Setting speed to maximum
const int distance;
const int distanceToLeft;
const int distanceToRight;
const int distanceForward;


const int trigPin = 9; // ultrasonic trigger pin
const int echoPin = 10; // ultrasonic echo pin 

void setup() {
  Serial.begin(9600);
  pinMode(servoPin, OUTPUT);
  pinMode(gripperPin, OUTPUT);
  // put your setup code here, to run once:
  digitalWrite(servoPin, LOW);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(a1, OUTPUT);
  pinMode(a2, OUTPUT);
  pinMode(b1, OUTPUT);
  pinMode(b2, OUTPUT);

}

int rightDistance() {
  lookRight();
  long duration, distance, distanceToRight;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  distanceToRight = distance;
  Serial.print("the Right Distance is: ");
  Serial.println(distanceToRight);
  delay(300);
}

int leftDistance() {
  lookLeft();
  long duration, distance, distanceToLeft;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  delay(300);
  distanceToLeft = distance;
  Serial.print("the Left Distance is: ");
  Serial.println(distanceToLeft);
  delay(300);
}

int forwardDistance() {
  lookForward();
  long duration, distance, distanceForward;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  delay(300);
  distanceForward = distance;
  Serial.print("the Forward Distance is: ");
  Serial.println(distanceForward);
  delay(300);
}

// a servo needs at least a minimum of 2 pulses but generally 10 pulses.
void loop() {

  forwardDistance();
  goForward();
  
  if (distanceForward < 10) {
    Serial.println("I am here");
    halt();
    leftDistance();
  }
//  
//  if (distanceForward < 10) {
//    halt();
//    delay(500);
//    if(distanceToLeft > distanceToRight) {
//      goLeft();
//      delay(500);
//    } else {
//      goRight();
//      delay(500);
//    } 
//  } else if (distanceToLeft > 10) {
//    goRight();
//    delay(500);
//  } else {
//    goForward();
//    delay(500);
//  }


  }

  void servo(int pin, int length) {
        digitalWrite(pin, HIGH);
        delayMicroseconds(length);//in microseconds
        digitalWrite(pin, LOW);
        delay(20); 
      }
      

void lookRight() {
  servo(servoPin, servoMinPulse);
}

void lookLeft() {
  servo(servoPin, servoMaxPulse);
}

void lookForward() {
  servo(servoPin, servoCenterPulse);
}

void openGripper() {
  servo(gripperPin, gripper_open_pulse);
}

void closeGripper(){
  servo(gripperPin, gripper_close_pulse);
}

int getDistance() {
long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
}

void goForward() {
  // Set motor to move the car forward
  analogWrite(a1, LOW);
  analogWrite(a2, speed);
  analogWrite(b1, speed);
  analogWrite(b2, LOW);
}

void goLeft () {
// Set motor to move the car left
  analogWrite(a1, speed);
  analogWrite(a2, LOW);
  analogWrite(b1, speed);
  analogWrite(b2, LOW);
}

void goBackward() {
  // Set motor to move the car backwards
  analogWrite(a1, speed);
  analogWrite(a2, LOW);
  analogWrite(b1, LOW);
  analogWrite(b2, speed);
}
   
void goRight () {
  //Set motor to move the car right
  analogWrite(a1, LOW);
  analogWrite(a2, speed);
  analogWrite(b1, LOW);
  analogWrite(b2, speed);
}

void halt() {
  //Set motor to stop the car
  analogWrite(a1, LOW);
  analogWrite(a2, LOW);
  analogWrite(b1, LOW);
  analogWrite(b2, LOW);
}
 
