const int gripperPin = 13;
int servoMinPulse = 500;
int servoCenterPulse = 1400; // 
int servoMaxPulse = 2400; //in microseconds

int gripper_open_pulse = 2100;
int gripper_close_pulse =  800; // 
int servoPulseRepeat = 10; // number of pulse send to servo

void setup() {
  pinMode(gripperPin, OUTPUT);
}

void loop() {
    //openGripper();
    //delay(10000);
    //closeGripper();
    //min pulse for closing

    //closeGripper();
    //delay(5000);
    //servo(gripperPin, servoMaxPulse);
}


void servo(int pin, int length) {
  digitalWrite(pin, HIGH);
  delayMicroseconds(length);//in microseconds
  digitalWrite(pin, LOW);
  delay(20); 
}

void openGripper(){
  servo(gripperPin, gripper_open_pulse);
}
  
void closeGripper(){
  servo(gripperPin, gripper_close_pulse);
}
