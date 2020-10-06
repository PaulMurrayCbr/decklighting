#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "decklighting.h"
#include "strip.h"
#include "rgb.h"

const int CHUNKS = 5;

const int CHUNKSZ[CHUNKS] = {
  253, // entrance
  192, // game room
  205, // theatre
  249, // deck
  247 // back wall
};

const int STRIP_LENGTH = CHUNKSZ[0] + CHUNKSZ[1] + CHUNKSZ[2] + CHUNKSZ[3] + CHUNKSZ[4];

class Strip { // a pure virtual interface
  public:
    virtual int getLength() = 0;
    virtual void clear() = 0;
    virtual void set(int n, uint32_t c) = 0;
};


// these are the actual drawing routines
// maybe they should be elsewhere
void strip_static_effect(RoomState& room, Strip& strip);
void strip_alternate_effect(RoomState& room, Strip& strip);

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
          switch (state.room[i].effect) {
            case STATIC:
              strip_static_effect(state.room[i], incBlock[i]);
              break;
            case ALTERNATE:
              strip_alternate_effect(state.room[i], incBlock[i]);
              break;
          }
        }
      }
    }
  }
  outputStrip.show();
}

inline unsigned lim(double f) {
  if (f >= 255) return 255;
  if (f <= 0) return 0;
  return (unsigned) f;
}

void strip_static_effect(RoomState & room, Strip & strip) {

  const int len = strip.getLength();
  uint32_t c;

  if (room.interpolation == LINEAR) {


    double rstart = (unsigned)room.color1.r;
    double rrange = ((double)(unsigned)room.color2.r - (double)(unsigned)room.color1.r);
    double gstart = (unsigned)room.color1.g;
    double grange = ((double)(unsigned)room.color2.g - (double)(unsigned)room.color1.g);
    double bstart = (unsigned)room.color1.b;
    double brange = ((double)(unsigned)room.color2.b - (double)(unsigned)room.color1.g);

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

  else {
    rgb rgb1;
    rgb rgb2;
    hsv hsv1;
    hsv hsv2;

    rgb1.r = room.color1.r / 255.0;
    rgb1.g = room.color1.g / 255.0;
    rgb1.b = room.color1.b / 255.0;

    rgb2.r = room.color2.r / 255.0;
    rgb2.g = room.color2.g / 255.0;
    rgb2.b = room.color2.b / 255.0;

    hsv1 = rgb2hsv(rgb1);
    hsv2 = rgb2hsv(rgb2);

    hsv at = hsv1;
    hsv inc;

    inc.s = (hsv2.s - hsv1.s) / (double)(len - 1);
    inc.v = (hsv2.v - hsv1.v) / (double)(len - 1);

    double gap = (hsv2.h - hsv1.h);

    switch (room.interpolation) {
      case HUEUP:
        if (gap < 0) gap += 360;
        break;
      case HUEDOWN:
        if (gap > 0) gap -= 360;
        break;
      case HUENEAR:
        if (gap > 180) gap -= 360;
        else if (gap < -180) gap += 360;
        break;
      case HUEFAR:
        if (gap <= 0 && gap > -180)gap += 360;
        else if (gap > 0 && gap < 180)gap -= 360;
        break;
    }

    inc.h = gap / (double)(len - 1);

    for (int i = 0; i < len; i++) {
      rgb ccc = hsv2rgb(at);
      c = outputStrip.Color(
            lim(ccc.r * 256),
            lim(ccc.g * 256),
            lim(ccc.b * 256)
          );
      strip.set(i, c);

      at.h += inc.h;
      while (at.h < 0) at.h += 360;
      while (at.h > 360) at.h -= 360;
      at.s += inc.s;
      at.v += inc.v;
    }
  }
}

void strip_alternate_effect(RoomState & room, Strip & strip) {

  const int len = strip.getLength();
  uint32_t cp[2];
  cp[0] = outputStrip.Color(room.color1.r, room.color1.g, room.color1.b);
  cp[1] = outputStrip.Color(room.color2.r, room.color2.g, room.color2.b);

  for (int i = 0; i < len; i++) {
    strip.set(i, cp[i & 1]);
  }
}


// don't know why this needs to be here


inline double norm(double x) {
  return x <= 0.0 ? 0 : x >= 1.0 ? 1.0 : x;
}

hsv rgb2hsv(rgb in)
{
  hsv         out;
  double      min, max, delta;

  in.r = norm(in.r);
  in.g = norm(in.g);
  in.b = norm(in.b);

  min = in.r < in.g ? in.r : in.g;
  min = min  < in.b ? min  : in.b;

  max = in.r > in.g ? in.r : in.g;
  max = max  > in.b ? max  : in.b;

  out.v = max;                                // v
  delta = max - min;
  if (delta < 0.00001)
  {
    out.s = 0;
    out.h = 0; // undefined, maybe nan?
    return out;
  }
  if ( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
    out.s = (delta / max);                  // s
  } else {
    // if max is 0, then r = g = b = 0
    // s = 0, h is undefined
    out.s = 0.0;
    out.h = 0;                            // its now undefined
    return out;
  }
  if ( in.r >= max )                          // > is bogus, just keeps compilor happy
    out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
  else if ( in.g >= max )
    out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
  else
    out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

  out.h *= 60.0;                              // degrees

  if ( out.h < 0.0 )
    out.h += 360.0;

  return out;
}

rgb hsv2rgb(hsv in)
{
  while (in.h < 0) in.h += 360;
  while (in.h > 360) in.h -= 360;
  in.s = norm(in.s);
  in.v = norm(in.v);

  double      hh, p, q, t, ff;
  long        i;
  rgb         out;

  if (in.s <= 0.0) {      // < is bogus, just shuts up warnings
    out.r = in.v;
    out.g = in.v;
    out.b = in.v;
    return out;
  }
  hh = in.h;
  if (hh >= 360.0) hh = 0.0;
  hh /= 60.0;
  i = (long)hh;
  ff = hh - i;
  p = in.v * (1.0 - in.s);
  q = in.v * (1.0 - (in.s * ff));
  t = in.v * (1.0 - (in.s * (1.0 - ff)));

  switch (i) {
    case 0:
      out.r = in.v;
      out.g = t;
      out.b = p;
      break;
    case 1:
      out.r = q;
      out.g = in.v;
      out.b = p;
      break;
    case 2:
      out.r = p;
      out.g = in.v;
      out.b = t;
      break;

    case 3:
      out.r = p;
      out.g = q;
      out.b = in.v;
      break;
    case 4:
      out.r = t;
      out.g = p;
      out.b = in.v;
      break;
    case 5:
    default:
      out.r = in.v;
      out.g = p;
      out.b = q;
      break;
  }
  return out;
}
