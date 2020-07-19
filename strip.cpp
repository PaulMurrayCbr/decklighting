#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "decklighting.h"
#include "clock.h"
#include "strip.h"

const int GAMEROOM_PIXELS = 421;
const int THEATRE_PIXELS = 706;

void strip_chunk(RoomState& room, int start, int len);


Adafruit_NeoPixel strip = Adafruit_NeoPixel(GAMEROOM_PIXELS + THEATRE_PIXELS, D8, NEO_GRB + NEO_KHZ800);

uint32_t Wheel(byte WheelPos);

void strip_setup() {
  strip.begin();
  strip_update();
}

void strip_loop() {
}

void strip_update() {
  strip.setBrightness(state.brightness);

//  uint32_t c;
//
//  c = strip.Color(state.theatre.color.r, state.theatre.color.g, state.theatre.color.b);
//  for(int i = 0; i<THEATRE_PIXELS; i++) {
//    strip.setPixelColor(i, c);
//  }
//  
//  c = strip.Color(state.gameroom.color.r, state.gameroom.color.g, state.gameroom.color.b);
//  for(int i = 0; i<GAMEROOM_PIXELS; i++) {
//    strip.setPixelColor(THEATRE_PIXELS+i, c);
//  }

  strip_chunk(state.theatre, 0, THEATRE_PIXELS);
  strip_chunk(state.gameroom, THEATRE_PIXELS, GAMEROOM_PIXELS);

  strip.show();
  
}

void strip_chunk(RoomState& room, int start, int len) {
  uint32_t c;

  c = strip.Color(room.color.r, room.color.g, room.color.b);
  for(int i = 0; i<len; i++) {
    strip.setPixelColor(start+i, c);
  }
  
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
