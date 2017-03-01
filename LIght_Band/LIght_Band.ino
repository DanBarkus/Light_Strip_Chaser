#include <Adafruit_SSD1306.h>

#include <Adafruit_NeoPixel.h>
#include <Encoder.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//
//#define OLED_RESET -1
//Adafruit_SSD1306 display(OLED_RESET);

#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            10
#define LB             7
#define CB             9
#define RB             8

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      286


Encoder myEnc(2, 3);

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
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};



int pos = 50;
int width = 30;
int currBright = 150;
int fade = 20;
bool fDir = true;

bool moving = false;
int bright = 0;

bool lc = false;
bool cc = false;
bool rc = false;


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//  display.clearDisplay();
//
//  display.setTextSize(1);
//  display.setTextColor(WHITE);

  pinMode(LB, INPUT);
  pinMode(CB, INPUT);
  pinMode(RB, INPUT);
}

void loop() {


  if (digitalRead(LB) == HIGH && digitalRead(RB) == HIGH) {
    if (!lc || !rc) {
      myEnc.write(fade * 2);
    }
    Serial.println("Both");
    long newFade = (myEnc.read());
    Serial.println(newFade);
    if (newFade != fade) {
      if (newFade < 0) {
        newFade = 0;
      }
      fade = newFade / 2;
      Serial.println(fade);
    }
    lc = false;
    rc = false;
  }
  else if (digitalRead(LB) == HIGH) {
    if (!lc) {
      myEnc.write(width * 2);
    }
    Serial.println("Left");
    long newWidth = (myEnc.read());
    Serial.println(newWidth);
    if (newWidth != width || newWidth != 2 * width) {
      if (newWidth < 4) {
        newWidth = 4;
      }
      width = newWidth / 2;
      Serial.println(width);
    }
    lc = true;
    cc = false;
    rc = false;
  }
  else if (digitalRead(CB) == HIGH) {
    Serial.println("Center");
    pos = 50;
    width = 10;
    fade  = 20;
    currBright = 155;
    lc = true;
    cc = true;
    rc = true;
  }
  else if (digitalRead(RB) == HIGH) {
    if (!rc) {
      myEnc.write(currBright);
    }
    Serial.println("Right");
    long newBright = (myEnc.read());
    Serial.println(newBright);
    if (newBright != currBright) {
      if (newBright < 0) {
        newBright = 0;
      }
      else if (newBright > 255) {
        newBright = 255;
      }
      currBright = newBright;
      Serial.println(currBright);
    }
    lc = false;
    cc = false;
    rc = true;
  }
  else {
    if (lc || cc || rc) {
      myEnc.write(pos * 2);
    }
    long newPosition = (myEnc.read());
    Serial.println(newPosition);
    if (newPosition != pos || newPosition != 2 * pos) {
      pos = newPosition / 2;
      Serial.println(pos);
    }
    lc = false;
    cc = false;
    rc = false;
  }


  for (int i = 0; i < NUMPIXELS; i++) {
    bright = 0;
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(bright, bright, bright)); // Moderately bright green color.

  }

  for (int i = 0; i < width; i++) {
    bright = gamma8[currBright];
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(pos + i, pixels.Color(bright, bright, bright)); // Moderately bright green color.

  }
  //Generates the fade
  for (int i = 0; i < fade + 1; i++) {

    bright = (currBright / fade) * (fade - i);
    //Serial.println(bright);
    //bright = pgm_read_byte(&gamma8[bright]);
    bright = gamma8[bright];
    //Serial.println(bright);
    // Back end of the fade
    pixels.setPixelColor(pos - i, pixels.Color(bright, bright, bright));
    // Leading end of the fade
    pixels.setPixelColor(pos + width + i, pixels.Color(bright, bright, bright));
  }

  if (pos > NUMPIXELS - width - fade) {
    fDir = false;
  }
  else if (pos < fade) {
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
  
//  display.clearDisplay();
//  display.setCursor(0,0);
//  display.println(pos);
//  display.println(width);
//  display.println(fade);
//  display.display();

  //delay(20);

}


