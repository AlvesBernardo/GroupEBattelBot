#include <Adafruit_NeoPixel.h>
int neoPin = 9;

int NUMPIXELS = 4;
Adafruit_NeoPixel pixels(NUMPIXELS, neoPin, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  pinMode(neoPin,OUTPUT);
  pixels.begin();
  pixels.setBrightness(50);
  for(int i=0; i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(171, 140, 153)); //GRB not RGB !
  }
  pixels.show();
}

void loop() {
  // put your main code here, to run repeatedly:
}
