#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "decklighting.h"
#include "clock.h"
#include "kelvin.h"

const int PIXELS = 110;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, D8, NEO_GRB + NEO_KHZ800);

uint32_t Wheel(byte WheelPos);

uint8_t cur[PIXELS];
uint8_t next[PIXELS];

#define MAXHOT 35



void strip_setup() {
  strip.begin();
  strip.setBrightness(128);
}

#define SPREADCOOL 4
#define SPREADSLOW 20
#define COOLSLOW 3
#define UPDATESPEED 50
#define HOTPER10SEC 50

#define UPDATESPER10SEC (1000/UPDATESPEED*10)
#define HOTCHANCE (UPDATESPER10SEC/HOTPER10SEC)

void strip_loop() {
  static long lastUpdate;

  if (millis() - lastUpdate < UPDATESPEED) return;
  lastUpdate = millis();


  memset(next, 0, PIXELS);
  for(int i = 0; i<PIXELS; i++) {
    if(cur[i]>0 && random(COOLSLOW)==0) cur[i]--;  
  }

  for(int i = 0; i<PIXELS;i++) {
    next[i] = cur[i];
    if(i>0 && cur[i-1]>SPREADCOOL && cur[i-1]-SPREADCOOL > next[i] && random(SPREADSLOW)==0) {
      next[i] = cur[i-1]-SPREADCOOL;
    }
    if(i<PIXELS-1 && cur[i+1]>SPREADCOOL && cur[i+1]-SPREADCOOL > next[i] && random(SPREADSLOW)==0) {
      next[i] = cur[i+1]-SPREADCOOL;
    }
  }
  
  memcpy(cur, next, PIXELS);

  if(random(HOTCHANCE)==0) {
    cur[random(PIXELS)] = MAXHOT;
  }

  for (int i = 0; i < strip.numPixels(); i++) {
    int idx = cur[i];

    strip.setPixelColor(i, kelvin[idx].r & 255, kelvin[idx].g & 255, kelvin[idx].b & 255);

  }
  strip.show();

}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
