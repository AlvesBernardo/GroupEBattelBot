

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





}



/*    ======[funtions] ========*/



void detectDistance() {

  if (distance > 25) {
    //if distance above 10 neopixel off
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(137, 207, 240));
      pixels.show();
    }
      analogWrite(motorPin2, 255);
      analogWrite(motorPin4, 255);


  } else {
    //distance less then 10 neopixel on
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(8, 136,8 ));
      pixels.show();
      
    }
    analogWrite(motorPin2, 0);
      analogWrite(motorPin4, 0);
  }



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
