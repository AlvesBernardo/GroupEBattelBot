

// including library

#include <Adafruit_NeoPixel.h> //including file
#ifdef __AVR__
#include <avr/power.h>
#endif


//defining pins
const int trigPin = 12;
const int echoPin = 13;
//defining motors
const int motorPin = 11;
const int motorPin2 = 10;
const int motorPin3 = 6;
const int motorPin4 = 5;

// defines variables
long duration;
int distance;
int neoPIN = 7; //NEO pixel datapin
int NUMPIXELS = 4;


//optional buttons to simulate a car starting
const int buttonPin = 2;
int buttonState = 0;



//line senor pin
const int lineSenor1 = A0;
const int lineSenor2 = A1;
const int lineSenor3 = A2;
const int lineSenor4 = A3;
const int lineSenor5 = A4;
const int lineSenor6 = A5;
const int lineSenor7 = A6;
const int lineSenor8 = A7;



//varaible for full power at the begininng
int period = 1000;
unsigned long time_now = 0;

Adafruit_NeoPixel pixels(NUMPIXELS, neoPIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  pinMode(neoPIN, OUTPUT); // neo pixel as output
  pixels.begin();         //begin it
  pixels.setBrightness(50);
  pixels.show(); // Initialize all pixels to 'off'
  pinMode(motorPin2, OUTPUT);
  pinMode (motorPin4, OUTPUT);


  //button declaration
  pinMode(buttonPin, INPUT);




  //declaring iuput for lineSenor
  pinMode(lineSenor1, INPUT);
  pinMode(lineSenor2, INPUT);
  pinMode(lineSenor3, INPUT);
  pinMode(lineSenor4, INPUT);
  pinMode(lineSenor5, INPUT);
  pinMode(lineSenor6, INPUT);
  pinMode(lineSenor7, INPUT);
  pinMode(lineSenor8, INPUT);





}




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




  detectDistance(); //call funtion to detect distance to certain object



  //line sensor if statements
    if((analogRead(lineSenor2)   >=  700)&&(analogRead(lineSenor3)   >=  700)){forward();}
    else if((analogRead(lineSenor7)  >=  700)&&(analogRead(lineSenor8)  >= 700 )){turnRight();}
    else if((analogRead(lineSenor5) >= 700)&&(analogRead(lineSenor6)  >=  700)){turnLeft();}
    //if((digitalRead(distanc) == 1)&&(digitalRead(lineSensorLeft) == 1)){Stop();}
  


  showLineSensorReading();
  //startingPower();


}



/*    ======[funtions] ========*/



void detectDistance() {

  if (distance > 25) {
    //if distance above 10 neopixel off
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(137, 207, 240));
      pixels.show();
    }



  } else {
    //distance less then 10 neopixel on
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(8, 136, 8 ));
      pixels.show();

    }

  }



}


void forward() {
  analogWrite(motorPin2, 150);
  analogWrite(motorPin, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, 150);
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

void Stop() {
  analogWrite(motorPin2, 0); //Right Motor forword Pin
  analogWrite(motorPin, 0); //Right Motor backword Pin
  analogWrite(motorPin3, 0); //Left Motor backword Pin
  analogWrite(motorPin4, 0); //Left Motor forward Pin

}


void startingPower(){
     time_now = millis();
   
   
   
    while(millis() < time_now + period){
          analogWrite(motorPin2, 255);
            analogWrite(motorPin4, 255);
    }
  }



void showLineSensorReading() {

  Serial.print(analogRead(lineSenor1));
  Serial.print(",");
  Serial.print(analogRead(lineSenor2));
  Serial.print(",");
  Serial.print(analogRead(lineSenor3));
  Serial.print(",");
  Serial.print(analogRead(lineSenor4));
  Serial.print(",");
  Serial.print(analogRead(lineSenor5));// pins right side
  Serial.print(",");
  Serial.print(analogRead(lineSenor6));//pins right side
  Serial.print(",");
  Serial.print(analogRead(lineSenor7));//pins left side
  Serial.print(",");
  Serial.print(analogRead(lineSenor8)); //pins left side
  Serial.println(",");
  delay(50);

}
/*

  void setup() {
    pinMode(ledRed, OUTPUT);
  }

  void loop(){
    digitalWrite(ledRed, LOW);
    delay(500);//wait 500mil
    digitalWrite(ledRed,HIGH);
    delay(500);
  }*/
