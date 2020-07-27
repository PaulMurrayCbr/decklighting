#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "decklighting.h"
#include "strip.h"

const int GAMEROOM_PIXELS = 421;
const int THEATRE_PIXELS = 706;
//const int GAMEROOM_PIXELS = 50;
//const int THEATRE_PIXELS = 50;

void strip_chunk(RoomState& room, Strip& strip);
void strip_static_effect(RoomState& room, Strip& strip);

Adafruit_NeoPixel outputStrip = Adafruit_NeoPixel(GAMEROOM_PIXELS + THEATRE_PIXELS, D8, NEO_GRB + NEO_KHZ800);

class NeopixelStrip : public Strip {
    const int start;
    const int len;
  public:
    NeopixelStrip(int start, int len) : start(start), len(len) {

    }
    int getLength()  {
      return len;
    }
    virtual void clear() {
      for (int i = start; i < start + len; i++) {
        outputStrip.setPixelColor(i, 0);
      }
    }
    virtual void set(int n, uint32_t c) {
      outputStrip.setPixelColor(start + n, c);
    }
}
wholeroomStripz(0, GAMEROOM_PIXELS + THEATRE_PIXELS),
                theatreStripz(0, THEATRE_PIXELS),
                gameroomStripz(THEATRE_PIXELS, GAMEROOM_PIXELS)

                ;

class IncStrip : public Strip {
    Strip &strip;
    int inc = 1;
  public:
    IncStrip(Strip& strip) : strip(strip) {}

    void setInc(int n) {
      inc = n;
    }

    int getLength()  {
      return (strip.getLength() - 1) / inc + 1;
    }
    virtual void clear() {
      strip.clear();
    }
    virtual void set(int n, uint32_t c) {
      strip.set(n * inc, c);
    }
}
wholeroomStrip(wholeroomStripz),
               theatreStrip(theatreStripz),
               gameroomStrip(gameroomStripz);



uint32_t Wheel(byte WheelPos);

void strip_setup() {
  outputStrip.begin();
  strip_update();
}

void strip_loop() {
}

void strip_update() {
  if (!state.allOn) {
    outputStrip.setBrightness(0);
    wholeroomStrip.clear();
  }
  else {
    outputStrip.setBrightness(state.brightness);

    if (state.tworooms) {
      theatreStrip.setInc(state.theatre.density);
      gameroomStrip.setInc(state.gameroom.density);

      theatreStripz.clear();
      gameroomStripz.clear();

      if (state.theatreOn)
        strip_chunk(state.theatre, theatreStrip);

      if (state.gameroomOn)
        strip_chunk(state.gameroom, gameroomStrip);
    }
    else {
      wholeroomStrip.setInc(state.theatre.density);
      wholeroomStripz.clear();
      strip_chunk(state.theatre, wholeroomStrip);
    }
  }

  outputStrip.show();
}

inline unsigned lim(double f) {
  if (f >= 255) return 255;
  if (f <= 0) return 0;
  return (unsigned) f;
}

void strip_chunk(RoomState& room, Strip& strip) {
  switch (room.effect) {
    case STATIC:
      strip_static_effect(room, strip);
      break;
  }
}

void strip_static_effect(RoomState& room, Strip& strip) {

  uint32_t c;

  double rstart = (unsigned)room.color1.r;
  double rrange = ((double)(unsigned)room.color2.r - (double)(unsigned)room.color1.r);
  double gstart = (unsigned)room.color1.g;
  double grange = ((double)(unsigned)room.color2.g - (double)(unsigned)room.color1.g);
  double bstart = (unsigned)room.color1.b;
  double brange = ((double)(unsigned)room.color2.b - (double)(unsigned)room.color1.g);

  const int len = strip.getLength();
  for (int i = 0; i < len; i++) {
    double dd = (double)i / (double)(len - 1);
    c = outputStrip.Color(
          lim(rstart + rrange * dd),
          lim(gstart + grange * dd),
          lim(bstart + brange * dd)
        );
    strip.set(i, c);
  }

}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return outputStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return outputStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return outputStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
