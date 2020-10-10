#include <Arduino.h>
#include "interpolation.h"
#include "rgb.h"

void interp_LINEAR(rgb a, rgb b, int len, RgbCallback callback);
void interp_HUEUP(rgb a, rgb b, int len, RgbCallback callback);
void interp_HUEDOWN(rgb a, rgb b, int len, RgbCallback callback);
void interp_HUENEAR(rgb a, rgb b, int len, RgbCallback callback);
void interp_HUEFAR(rgb a, rgb b, int len, RgbCallback callback);
void interp_HUERBOW(rgb a, rgb b, int len, RgbCallback callback);
void interp_HUEXRBOW(rgb a, rgb b, int len, RgbCallback callback);

void compute_hue_interp(rgb a, rgb b, int len, RgbCallback callback, int (*adjustgap)(int) );


InterpolationImpl getInterpolation(Interpolation i) {
  switch (i) {
    case LINEAR: return interp_LINEAR;
    case HUEUP: return interp_HUEUP;
    case HUEDOWN: return interp_HUEDOWN;
    case HUENEAR: return interp_HUENEAR;
    case HUEFAR: return interp_HUEFAR;
    case HUERBOW: return interp_HUERBOW;
    case HUEXRBOW : return interp_HUEXRBOW;
  }
}

inline unsigned lim(double f) {
  if (f >= 255) return 255;
  if (f <= 0) return 0;
  return (unsigned) f;
}

void interp_LINEAR(rgb a, rgb b, int len, RgbCallback callback) {
  double rstart = (unsigned)a.r;
  double rrange = ((double)(unsigned)b.r - (double)(unsigned)a.r);
  double gstart = (unsigned)a.g;
  double grange = ((double)(unsigned)b.g - (double)(unsigned)a.g);
  double bstart = (unsigned)a.b;
  double brange = ((double)(unsigned)b.b - (double)(unsigned)a.g);

  rgb out;

  for (int i = 0; i < len; i++) {
    double dd = (double)i / (double)(len - 1);
    out.r = lim(rstart + rrange * dd);
    out.g = lim(gstart + grange * dd);
    out.b = lim(bstart + brange * dd);
    callback(i, out);
  }
}

void interp_HUEUP(rgb a, rgb b, int len, RgbCallback callback) {
  compute_hue_interp(a, b, len, callback, [](int gap) {
    if (gap < 0) gap += 360;
    return gap;
  });
}
void interp_HUEDOWN(rgb a, rgb b, int len, RgbCallback callback) {
  compute_hue_interp(a, b, len, callback, [](int gap) {
    if (gap > 0) gap -= 360;
    return gap;
  });
}
void interp_HUENEAR(rgb a, rgb b, int len, RgbCallback callback) {
  compute_hue_interp(a, b, len, callback, [](int gap) {
    if (gap > 180) gap -= 360;
    else if (gap < -180) gap += 360;
    return gap;
  });
}
void interp_HUEFAR(rgb a, rgb b, int len, RgbCallback callback) {
  compute_hue_interp(a, b, len, callback, [](int gap) {
    if (gap <= 0 && gap > -180)gap += 360;
    else if (gap > 0 && gap < 180)gap -= 360;    return gap;
  });
}
void interp_HUERBOW(rgb a, rgb b, int len, RgbCallback callback) {
  compute_hue_interp(a, b, len, callback, [](int gap) {
    return gap;
  });
}
void interp_HUEXRBOW(rgb a, rgb b, int len, RgbCallback callback) {
  compute_hue_interp(a, b, len, callback, [](int gap) {
    if (gap <= 0) gap += 360;
    else gap -= 360;
    return gap;
  });
}


void compute_hue_interp(rgb a, rgb b, int len, RgbCallback callback, int (*adjustgap)(int) ) {
  rgb rgb1;
  rgb rgb2;
  hsv hsv1;
  hsv hsv2;

  rgb1.r = a.r / 255.0;
  rgb1.g = a.g / 255.0;
  rgb1.b = a.b / 255.0;

  rgb2.r = b.r / 255.0;
  rgb2.g = b.g / 255.0;
  rgb2.b = b.b / 255.0;

  hsv1 = rgb2hsv(rgb1);
  hsv2 = rgb2hsv(rgb2);

  hsv at = hsv1;
  hsv inc;

  inc.s = (hsv2.s - hsv1.s) / (double)(len - 1);
  inc.v = (hsv2.v - hsv1.v) / (double)(len - 1);

  double gap = (hsv2.h - hsv1.h);
  gap = adjustgap(gap);

  inc.h = gap / (double)(len - 1);

  rgb out;
  for (int i = 0; i < len; i++) {
    rgb ccc = hsv2rgb(at);
    out.r = lim(ccc.r * 256);
    out.g = lim(ccc.g * 256);
    out.b = lim(ccc.b * 256);
    callback(i, out);

    at.h += inc.h;
    while (at.h < 0) at.h += 360;
    while (at.h > 360) at.h -= 360;
    at.s += inc.s;
    at.v += inc.v;
  }
}
