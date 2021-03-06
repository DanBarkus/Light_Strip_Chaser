#include <Adafruit_SSD1306.h>

#include <Encoder.h>
#include <SPI.h>
#include <Wire.h>

#include <OctoWS2811.h>
//
//#define OLED_RESET 4
//Adafruit_SSD1306 display(-1);

#ifdef __AVR__
#include <avr/power.h>
#endif


#define LB             3
#define CB             17
#define RB             1

// How many NeoPixels are attached to the Arduino?
const int ledsPerStrip = 286;


Encoder myEnc(22, 23);

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
float currBright = 150;
int fade = 20;
bool fDir = true;
bool AUTO = false;
bool RUNNING = false;
bool STIME = false;

int stage = 0;
int sPos = 0;
int ePos = 0;
int dur = 0;
int sTime = 0;
int eTime = 0;

int last = 0;

int oThresh = 50;
int thresh = oThresh;

bool moving = false;
int bright = 0;

bool lc = false;
bool cc = false;
bool rc = false;


DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);
void setup() {
  leds.begin(); // This initializes the NeoPixel library.
  leds.show();
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

  if (AUTO) {
    if (RUNNING) {
      Serial.println("Stage: ");
      //      if (stage === -1) {
      //        stage = 0;
      //      }
      if (stage == 0) {
        Serial.println("Choose Start Position");
        sPos = readEnc(sPos, 1);
        width = 2;
        fade = 0;
        pos = sPos;
      }
      else if (stage == 1) {
        Serial.println("Choose End Position");
        ePos = readEnc(ePos, 1);
      }
      else if (stage == 2) {
        Serial.println("Choose Width");
        pos = (sPos + (ePos - sPos) / 2) - (width / 2);
        width = readEnc(width, 1);
      }
      else if (stage == 3) {
        Serial.println("Choose Brightness");
        pos = (sPos + (ePos - sPos) / 2) - (width / 2);
        currBright = readEnc(currBright, 2);
      }
      else if (stage == 4) {
        Serial.println("Choose Fade");
        pos = (sPos + (ePos - sPos) / 2) - (width / 2);
        fade = readEnc(fade, 1);
      }
      else if (stage == 5) {
        sPos = sPos - (fade + width / 2);
        ePos = ePos + (fade + width / 2);
        Serial.println("Choose Timespan (Seconds)");
        pos = (sPos + (ePos - sPos) / 2) - (width / 2);
        dur = readEnc(dur, 4);
        width = dur / 10;
      }
      else if (stage == 6) {
        Serial.println("Start Show");
      }
      else if (stage == 7) {
        Serial.println("Running Show");
        if (STIME) {
          if (millis() < eTime) {
            pos = millis() * (ePos - sPos) / eTime;
          }
          else if (millis() > eTime) {
            Serial.println("Show Over");
            STIME = false;
            stage++;
          }
        }
        else {
          sTime = millis();
          eTime = sTime + dur * 1000;
          STIME = true;
        }
      }
      else if (stage == 8) {
        Serial.println("Back to Restart");
        Serial.println("Next to Exit");
      }
      else if (stage == 9) {
        stage = 0;
        RUNNING = false;
      }
      stage = stage + checkClick();


    }
    else {
      Serial.println("Setting up AUTO");
      stage = 0;
      sPos = 5;
      ePos = ledsPerStrip - 5;
      dur = 30;
      sTime = 0;
      eTime = 0;

      RUNNING = true;
      STIME = false;
      fade = 0;
      width = 2;
      bright = 200;
      RUNNING = true;
    }
  }

  else {
    int btn = checkClick();
    if (btn == 0) {
      pos = readEnc(pos, 2);
      if (pos < 0 - (width + fade)) {
        pos = 0 - (width + fade);
      }
      else if (pos > ledsPerStrip + fade) {
        pos = ledsPerStrip + fade;
      }
    }
    else if (btn == 1) {
      currBright = readEnc(currBright, 2);
      if (currBright < 0) {
        currBright = 1;
      }
      else if (currBright > 255) {
        currBright = 255;
      }
    }
    else if (btn == 2) {
      fade = readEnc(fade, 2);
      if (fade <= 0) {
        fade = 0;
      }
    }
    else if (btn == -1) {
      //Serial.println("Left");
      width = readEnc(width, 2);
      if (width < 1) {
        width = 2;
      }
    }
    else if (btn == -2) {
      pos = 50;
      width = 10;
      fade  = 20;
      currBright = 155;
    }
  }
  //-------------------------------
  // Actually does the lights stuff
  //-------------------------------
  for (int i = 0; i < ledsPerStrip; i++) {
    bright = 0;
    // clear all pixels by setting to black
    leds.setPixel(i, bright, bright, bright);
  }

  for (int i = 0; i < width; i++) {
    bright = gamma8[(int)currBright];
    // draws the main width bar
    leds.setPixel(pos + i, bright, bright, bright);

  }
  //Generates the fade
  for (int i = 0; i < fade + 1; i++) {

    bright = (currBright / fade) * (fade - i);
    if (i == 1) {
      Serial.println(currBright / fade);
    }
    //bright = pgm_read_byte(&gamma8[bright]);
    bright = gamma8[bright];
    //Serial.println(bright);
    // Back end of the fade
    leds.setPixel(pos - i, bright, bright, bright);
    // Leading end of the fade
    leds.setPixel(pos + width + i, bright, bright, bright);
  }

  leds.show(); // This sends the updated pixel color to the hardware.

  //  display.clearDisplay();
  //  display.setCursor(0,0);
  //  display.println(pos);
  //  display.println(width);
  //  display.println(fade);
  //  display.display();

  //delay(20);

}

int readEnc (int input, int mult) {
  int nMult = 4 / mult;
  if (input != last) {
    myEnc.write(input * nMult);
  }
  long newInput = (myEnc.read());
  if (newInput != input) {
    input = newInput / nMult;
    last = newInput / nMult;
    Serial.println(newInput);
    Serial.println(input);
  }
  return input;
}

int checkClick () {
  if (digitalRead(LB) == LOW && digitalRead(RB) == LOW && digitalRead(CB) == LOW) {
    if (lc || cc || rc) {
      return 0;
    }
    lc = false;
    cc = false;
    rc = false;
  }
  else if (digitalRead(LB) == HIGH && digitalRead(RB) == HIGH) {
    if (!lc || !rc) {
      Serial.println("Both");
      return 2;
    }
    lc = true;
    rc = true;
  }
  else if (digitalRead(LB) == HIGH) {
    if (!lc) {
      Serial.println("Left");
      return -1;
    }
    lc = true;
    cc = false;
    rc = false;
  }
  else if (digitalRead(RB) == HIGH) {
    if (!rc) {
      Serial.println("Right");
      return 1;
    }
    lc = false;
    cc = false;
    rc = true;
  }
  else if (digitalRead(CB) == HIGH) {
    if (!cc) {
      Serial.println("Center");
      return -2;
    }
    lc = false;
    cc = true;
    rc = false;
  }
  //if (digitalRead(LB) == LOW && digitalRead(RB) == LOW && digitalRead(CB) == LOW)
  //    else if (digitalRead(CB) == HIGH) {
  //      Serial.println("Center");
  //      pos = 50;
  //      width = 10;
  //      fade  = 20;
  //      currBright = 155;
  //      lc = true;
  //      cc = true;
  //      rc = true;
  //    }
}

