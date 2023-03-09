
// including library

#include <Adafruit_NeoPixel.h> //including file
#ifdef __AVR__
#include <avr/power.h>
#endif
//#include <Servo.h>


//defining pins
const int trigPin = 12;
const int echoPin = 13;
const int servoPin = 4;



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
//Servo servo;
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



  //button declaration
  pinMode(buttonPin, INPUT);


  //==================[ Grippers ]====================
//  servo.attach(4);
  //servo.write(angle);



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
//  servo.write(100); //Sends a signal for the servo to go to the 0 degrees position
//  delay(3000);
 // servo.write(45); //Sends a signal for the servo to go to the 180 degrees position
  //delay(3000);


  //bluetooth
  bluethootAdapter();
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
    state = Serial.read(); // Reads the data from the serial port
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





/*
void gripper() {

  if (distance > 25) {
    for (angle = 10; angle <= 180; angle += 1) {
      servo.write(angle);
      delay(15);
    }
  }

}


*/
/*
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

*/




/*-------*/
/*bluetooth*/
/*-------*/
