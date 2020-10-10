#ifndef INTERPOLATION_H
#define  INTERPOLATION_H

#include "rgb.h"

typedef void (*RgbCallback)(int, rgb);
typedef void (*InterpolationImpl)(rgb, rgb, int, RgbCallback);

enum Interpolation {
  LINEAR, HUEUP, HUEDOWN, HUENEAR, HUEFAR, HUERBOW, HUEXRBOW
};


InterpolationImpl getInterpolation(Interpolation i);


#endif
