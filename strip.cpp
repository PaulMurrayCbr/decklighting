#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "decklighting.h"
#include "strip.h"

const int CHUNKS = 5;

const int CHUNKSZ[CHUNKS] = {
  253, // entrance
  192, // game room
  205, // theatre
  249, // deck
  247 // back wall
};

const int STRIP_LENGTH = CHUNKSZ[0] + CHUNKSZ[1] + CHUNKSZ[2] + CHUNKSZ[3] + CHUNKSZ[4];

Adafruit_NeoPixel outputStrip = Adafruit_NeoPixel(STRIP_LENGTH, D8, NEO_GRB + NEO_KHZ800);

class NeopixelStrip : public Strip {
    const int len;
  public:
    NeopixelStrip(int len) : len(len) {

    }
    int getLength()  {
      return len;
    }
    virtual void clear() {
      for (int i = 0; i < len; i++) {
        outputStrip.setPixelColor(i, 0);
      }
    }
    // this *reverses* out neopixels,
    // because they are physically right-to-left in the room
    // and I want to treat them as if they were left-to-right
    virtual void set(int n, uint32_t c) {
      outputStrip.setPixelColor(len - 1 - n, c);
    }
}
whole_room(STRIP_LENGTH);

class AdjustableChunk : public Strip {
    Strip &strip;
  public:
    int start;
    int len;
    AdjustableChunk(Strip& strip) : strip(strip) {}
    int getLength()  {
      return len;
    }
    virtual void clear() {
      for (int i = start; i < start + len; i++)
        strip.set(i, 0);
    }
    virtual void set(int n, uint32_t c) {
      strip.set(start + n, c);
    }
}
aBlock[] = {
  AdjustableChunk(whole_room),
  AdjustableChunk(whole_room),
  AdjustableChunk(whole_room),
  AdjustableChunk(whole_room),
  AdjustableChunk(whole_room)
};

class IncBlock : public Strip {
    Strip &strip;
  public:
    int inc = 1;

    IncBlock(Strip & strip) : strip(strip) {}

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
incBlock[] = {
  IncBlock(aBlock[0]),
  IncBlock(aBlock[1]),
  IncBlock(aBlock[2]),
  IncBlock(aBlock[3]),
  IncBlock(aBlock[4])
};

void strip_setup() {
  outputStrip.begin();
  strip_update();
}

void strip_loop() {
}

void strip_update() {
  if (!state.allOn) {
    outputStrip.setBrightness(0);
    whole_room.clear();
  }
  else {
    outputStrip.setBrightness(state.brightness);

    aBlock[0].start = 0;
    aBlock[0].len = CHUNKSZ[0];

    // initialize chuks
    for (int i = 1; i < CHUNKS; i++) {
      aBlock[i].start = aBlock[i - 1].start + aBlock[i - 1].len;
      aBlock[i].len = CHUNKSZ[i];
    }

    // merge 'out' chunks
    for (int i = CHUNKS - 1; i > 0; i--) {
      if (state.room[i].onOffOut == OUT) {
        aBlock[i - 1].len += aBlock[i].len;
      }
    }

    // paint chunks
    for (int i = 0; i < CHUNKS; i++) {
      if (i != 0 && state.room[i].onOffOut == OUT) {
        // skip
      }
      else {
        aBlock[i].clear();
        if (state.room[i].onOffOut == ON) {
          incBlock[i].inc = state.room[i].density;
          if (incBlock[i].inc < 1) incBlock[i].inc = 1;
          state.room[i].setup(incBlock[i]); // initial paint of the room
        }
      }
    }
  }
  outputStrip.show();
}
