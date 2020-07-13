#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "decklighting.h"
#include "clock.h"

const int PIXELS = 110;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, D8, NEO_GRB + NEO_KHZ800);

uint32_t Wheel(byte WheelPos);

#define MAXHOT 35

void strip_setup() {
  strip.begin();
  strip.setBrightness(128);
}

#define UPDATESPEED 50

void strip_loop() {
  static long lastUpdate;

  if (millis() - lastUpdate < UPDATESPEED) return;
  lastUpdate = millis();

  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i+lastUpdate/500))%256);
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
