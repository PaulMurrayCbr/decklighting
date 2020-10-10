#include <Arduino.h>
#include "effect.h"

const int effecttypes = 6;
const char *effectlabel[] = {"onecolor", "rainbow", "theatre", "plasma", "bouncyball", "flicker"};

extern char page[];
extern char *pagep;

EffectImpl *newOneColorEffect();
EffectImpl *newRainbowEffect();
EffectImpl *newTheatreEffect();
EffectImpl *newPlasmaEffect();
EffectImpl *newBouncyBallEffect();
EffectImpl *newFlickerEffect();

EffectImpl *newEffectImpl(Effect e) {
  switch (e) {
    default:
    case ONECOLOR: return newOneColorEffect();
    case RAINBOW: return newRainbowEffect();
    case THEATRE: return newTheatreEffect();
    case PLASMA: return newPlasmaEffect();
    case BOUNCYBALL: return newBouncyBallEffect();
    case FLICKER: return newFlickerEffect();
  }
}

void EffectImpl::serialize() {
  while (*pagep) pagep++;
  strcpy(pagep, "null");
  pagep += 4;
}
