#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            3

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      286
#define DASHWIDTH      0 
#define DASHFADE       30
#define DASHPOSITION   50

#define MAXBRIGHT      255

const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

int pos = 144;
bool fDir = true;

bool moving = true;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);

}

void loop() {


  for (int i = 0; i < DASHWIDTH; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(pos + i, pixels.Color(MAXBRIGHT, MAXBRIGHT, MAXBRIGHT)); // Moderately bright green color.

  }
  // Generates the fade
  for (int i = 0; i < DASHFADE + 1; i++) {

    int bright = (MAXBRIGHT / DASHFADE) * (DASHFADE - i);
    Serial.println(bright);
    //bright = pgm_read_byte(&gamma8[bright]);
    bright = gamma8[bright];
    Serial.println(bright);
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

  //delay(10);
}
