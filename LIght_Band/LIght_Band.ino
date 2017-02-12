#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      288
#define DASHWIDTH      0
#define DASHFADE       10
#define DASHPOSITION   250

#define MAXBRIGHT      200

int pos = 200;
bool fDir = true;

bool moving = true;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.

}

void loop() {


  for (int i = 0; i < DASHWIDTH; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(pos + i, pixels.Color(MAXBRIGHT, MAXBRIGHT, MAXBRIGHT)); // Moderately bright green color.

  }
  // Generates the fade
  for (int i = 0; i < DASHFADE + 1; i++) {

    int bright = (MAXBRIGHT / DASHFADE) * (DASHFADE - i);
    // Back end of the fade
    pixels.setPixelColor(pos - i, pixels.Color(bright, bright, bright));
    // Leading end of the fade
    pixels.setPixelColor(pos + DASHWIDTH + i, pixels.Color(bright, bright, bright));
  }

  if (pos > NUMPIXELS - DASHWIDTH - DASHFADE) {
    fDir = false;
  }
  else if (pos < DASHFADE) {
    fDir = true;
  }

  if (moving) {
    if (fDir) {
      pos++;
    }
    else {
      pos--;
    }
  }

  pixels.show(); // This sends the updated pixel color to the hardware.

  delay(50);
}
