
// including library

#include <Adafruit_NeoPixel.h> //including file
#ifdef __AVR__
#include <avr/power.h>
#endif
#include <Servo.h>


//defining pins
const int trigPin = 12;
const int echoPin = 13;
const int servoPin = 4;


//defining motors and their speeds and array
const int motorPin = 11; //A1
const int motorPin2 = 10; //A2
const int motorPin3 = 6; //B1
const int motorPin4 = 5; //B2
const int motorR1 = 9  ;//R1
const int motorR2 = 3; //R2
const int turningRatio = 35;
const int basicSpeed = 250;
const int minumumSpeed = 75;
int posVal[] = { -4, -3, -2, -1, 1, 2, 3, 4}; //possible values
int positionOfRobot = 0;
//line senor pin
int arrayForSensors[] = {A0, A1, A2, A3, A4, A5, A6, A7};

const int trig = 4;
const int echo = 2;




// defines variables
long duration;
int distance;
int neoPIN = 7; //NEO pixel datapin
int NUMPIXELS = 4;


//optional buttons to simulate a car starting
const int buttonPin = 2;
int buttonState = 0;






//varaible for full power at the begininng
int period = 1000;
unsigned long time_now = 0;

Adafruit_NeoPixel pixels(NUMPIXELS, neoPIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

//create servo object
Servo servo;
int angle = 10;

//bluetooth adapter
int state = 0;
char data = 0;
int rx =8; //reciver
int tx =  2; //transmiter



void setup() {

  //==================[ Leds ]====================
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  pinMode(neoPIN, OUTPUT); // neo pixel as output
  pixels.begin();         //begin it
  pixels.setBrightness(50);
  pixels.show(); // Initialize all pixels to 'off'


  //==================[ Motors]====================
  pinMode(motorPin2, OUTPUT);
  pinMode (motorPin4, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode (motorPin3, OUTPUT);
  pinMode (motorR1, OUTPUT);
  pinMode (motorR2, OUTPUT);


  //button declaration
  pinMode(buttonPin, INPUT);


  //==================[ Grippers ]====================
  servo.attach(4);
  servo.write(angle);


  //==================[ LineSensor ]====================
  //pinMode(arrayForSensors, INPUT);



  /*
     servo.attach(4);
     for (angle = 180; angle >= 0; angle -= 1){
      servo.write(angle);
      delay(15);
      }


     gripper();

  */
  
 //==================[ bluetoth ]====================
   pinMode(tx, OUTPUT);
   pinMode(rx, INPUT);

  Serial.begin(9600);


}



int sensRead[8];//empty array for reading results
void loop() {

  buttonState = digitalRead(buttonPin);


  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);



 



  //==================[ Grippers ]====================
  servo.write(100); //Sends a signal for the servo to go to the 0 degrees position
  delay(3000);
 // servo.write(45); //Sends a signal for the servo to go to the 180 degrees position
  //delay(3000);


  //bluetooth
  bluethootAdapter();
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
    state = Serial.read(); // Reads the data from the serial port
 }

  //==================[ Moving ]====================
  if (detectDistance()) {
    stopCar();
  }
  else {
    for (int i = 0; i < 8; i++) {
      if (analogRead(arrayForSensors[i]) > 800) {
        sensRead[i] = 1;
      }
      else if (analogRead(arrayForSensors[i]) < 550) {
        sensRead[i] = 0;
      }
    }//the sensors reading are stored in previously empty array
    int positionOfRobot = conversion(sensRead);//conversts reading to position
    Serial.println(positionOfRobot);
    loopForSensor(positionOfRobot);//move
  }

}


/*    =======================*/
/*    ======[funtions] ========*/
/*    =======================*/

  void bluethootAdapter(){

    if(Serial.available() > 0)       /*check for serial data availability*/
    {
        data = Serial.read();        /*read data coming from Bluetooth device*/
        Serial.print(data);          /*print values on serial monitor*/
        Serial.print("\n");          /*print new line*/
        if(data == '1')              /*check data value*/
            digitalWrite(3, HIGH);  /*Turn ON LED if serial data is 1*/
        else if(data == '0')         /*check data value*/
            digitalWrite(3, LOW);   /*Turn OFF LED if serial data is 0*/
    }                     

  }

int conversion(int reading[]) { //first of all it checks for outliers such as 10000000 and 00000001;
 
  if (reading[0] == 1 && reading[1] == 0 && reading[2] == 0 && reading[3] == 0 && reading[4] == 0 && reading[5] == 0 && reading[6] == 0 && reading[7] == 0) {
    positionOfRobot = -7;
    return positionOfRobot;
  }
  if (reading[0] == 0 && reading[1] == 0 && reading[2] == 0 && reading[3] == 0 && reading[4] == 0 && reading[5] == 0 && reading[6] == 0 && reading[7] == 1) {
    positionOfRobot = 7;
    return positionOfRobot;
  }//if it isnt an outlier it goes here
  for (int i = 0; i < 8; i++) {
    if (reading[i] == 1) {
      positionOfRobot = positionOfRobot + posVal[i]; //this array are all the values. If the reading is 1 it sums if it is 0, it does nothing that way even if it is 11110000 it will work
    }
  }
  return positionOfRobot;//returns position
}



bool detectDistance() {

  if (distance > 6) {
    //if distance above 10 neopixel off
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(137, 207, 240));
      pixels.show();
    }

  } else {
    //distance less then 10 neopixel on

    pixels.setPixelColor(3, pixels.Color(140, 255, 0 ));
    pixels.setPixelColor(0, pixels.Color(140, 255, 0 ));
    pixels.setPixelColor(1, pixels.Color(137, 207, 240));
    pixels.setPixelColor(2, pixels.Color(137, 207, 240));
    //analogWrite(motorPin2, 0); //Right Motor forword Pin
    //analogWrite(motorPin4, 0); //Left Motor forward Pin
    pixels.show();
    stopCar();




  }



}

//loop to calibrate our sensors

void loopForSensor(int positionOfRobot) {
  if (positionOfRobot == 0) {
    analogWrite(motorPin2, basicSpeed);
    analogWrite(motorPin4, basicSpeed);
  }
  if (positionOfRobot > 0) {
    analogWrite(motorPin2, basicSpeed - (turningRatio * positionOfRobot));
    analogWrite(motorPin4, basicSpeed);
  }
  if (positionOfRobot < 0) {
    analogWrite(motorPin2, basicSpeed + (turningRatio * positionOfRobot));
    analogWrite(motorPin4, basicSpeed);
  }



}





void gripper() {

  if (distance > 25) {
    for (angle = 10; angle <= 180; angle += 1) {
      servo.write(angle);
      delay(15);
    }
  }

}




void forword() {
  analogWrite(motorPin2, 180);
  analogWrite(motorPin, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, 180);
}

void turnRight() {
  analogWrite(motorPin2, 0);  //Right Motor forword Pin
  analogWrite(motorPin, 150); //Right Motor backword Pin
  analogWrite(motorPin3, 0);  //Left Motor backword Pin
  analogWrite(motorPin4, 150); //Left Motor forword Pin
}

void turnLeft() {
  analogWrite(motorPin2, 150); //Right Motor forword Pin
  analogWrite(motorPin, 0);  //Right Motor backword Pin
  analogWrite(motorPin3, 150); //Left Motor backword Pin
  analogWrite(motorPin4, 0);  //Left Motor forword Pin
}

void stopCar() {
  analogWrite(motorPin2, 0); //Right Motor forword Pin
  analogWrite(motorPin, 0); //Right Motor backword Pin
  analogWrite(motorPin3, 0); //Left Motor backword Pin
  analogWrite(motorPin4, 0); //Left Motor forward Pin

}


void startingPower() {
  time_now = millis();



  while (millis() < time_now + period) {
    analogWrite(motorPin2, 255);
    analogWrite(motorPin4, 255);
  }
}






/*-------*/
/*bluetooth*/
/*-------*/
