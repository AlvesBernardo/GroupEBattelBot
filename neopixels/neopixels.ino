#include <Adafruit_NeoPixel.h>
int neoPin = 7;

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
  pixels.setPixelColor(0 ,pixels.Color(200, 200, 0));
  pixels.show();
  delay(100);
  pixels.setPixelColor(0 ,pixels.Color(171, 140, 153));
  pixels.show();
  delay(100);
  // put your main code here, to run repeatedly:
}
