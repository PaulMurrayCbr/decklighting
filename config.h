#ifndef CONFIG_H
#define CONFIG_H

#include "effect.h"

/**
 * This file defines the persistable configuration blocks for all of the effects. Both the effect implementations
 * and the decklighting state depend on these types
 */

// enum Effect { ONECOLOR=0, RAINBOW=1, THEATRE=2, PLASMA=3, BOUNCYBALL=4, FLICKER=5 };

struct OnecolorConfig {};

struct RainbowConfig {};

struct TheatreConfig {};

struct PlasmaConfig {};

struct BouncyballConfig {
    int head;
    int tail;
    long speed;
};

struct FlickerConfig {};

union ConfigUnion {
  struct OnecolorConfig oneColor;
  struct RainbowConfig rainbow;
  struct TheatreConfig theatre;
  struct PlasmaConfig plasma;
  struct BouncyballConfig bouncyball;
  struct FlickerConfig flicker;
};


#endif
