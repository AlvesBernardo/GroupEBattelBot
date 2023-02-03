

/*-----------------------------
 * ***   Making it blink
 ---------------------------*/

#include <Adafruit_NeoPixel.h> //including file
#ifdef __AVR__
  #include <avr/power.h>
#endif
const int trigPin = 9;
const int echoPin = 10;
int motorPin = 6;
int motorPin2 = 5;
int motorPin3 = 3;
int motorPin4 = 2;
// defines variables
long duration;
int distance;
int PIN = 7;
int NUMPIXELS = 4;

//motors
  ;  //initializing pin 2 as pwm
const int in_A0 = A0 ;
const int in_A1 = A1 ;
const int in_A2 = A2 ;
const int in_A3 = A4 ;



Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500 

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  pinMode(PIN, OUTPUT);
   pixels.begin();
  pixels.setBrightness(50);
  pixels.show(); // Initialize all pixels to 'off'
  pinMode(in_A1,OUTPUT);
  pinMode(in_A3,OUTPUT);
  
 
 
}




void loop() {

    
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
  digitalWrite(motorPin, HIGH);


 
if (distance > 10){

     pixels.begin();
    pixels.setBrightness(50);
    pixels.show(); // Initialize all pixels to 'off'
    Serial.print("no hand");
    analogWrite(in_A1,255);
     analogWrite(in_A3,255);
    }
  
if (distance < 10){
        
      for(int i=0; i<NUMPIXELS; i++) {
          pixels.setPixelColor(i,pixels.Color(0, 0, 255));
            pixels.begin();
             Serial.print("hand");
           analogWrite(in_A1,0);
            analogWrite(in_A3,0);
      }
  }



    //motors

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
