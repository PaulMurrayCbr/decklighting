#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"
#include "rgb.h"

inline unsigned lim(double f) {
  if (f >= 255) return 255;
  if (f <= 0) return 0;
  return (unsigned) f;
}

void RainbowEffect::setup(RoomState & room, Strip & strip) {
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
      case HUERBOW:
        // gap is fine the way it is
        break;
      case HUEXRBOW:
        if (gap <= 0) gap += 360;
        else gap -= 360;
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
