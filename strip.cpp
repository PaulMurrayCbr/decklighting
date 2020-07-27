#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "decklighting.h"
#include "strip.h"
#include "rgb.h"

const int GAMEROOM_PIXELS = 421;
const int THEATRE_PIXELS = 706;
//const int GAMEROOM_PIXELS = 30;
//const int THEATRE_PIXELS = 75;

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

    Serial.print(" room "); Serial.println((int) &room);

    Serial.print(" 1r "); Serial.print(room.color1.r);
    Serial.print(" 1g "); Serial.print(room.color1.g);
    Serial.print(" 1b "); Serial.print(room.color1.b);

    Serial.print(" 2r "); Serial.print(room.color2.r);
    Serial.print(" 2g "); Serial.print(room.color2.g);
    Serial.print(" 2b "); Serial.println(room.color2.b);



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
