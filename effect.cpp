#include <Arduino.h>
#include "effect.h"

const int effecttypes = 6;
const char *effectlabel[] = {"onecolor", "rainbow", "theatre", "plasma", "bouncyball", "flicker"};

extern char page[];
extern char *pagep;

extern EffectImpl *oneColorEffect;
extern EffectImpl *rainbowEffect;
extern EffectImpl *theatreEffect;
extern EffectImpl *plasmaEffect;
extern EffectImpl *bouncyBallEffect;
extern EffectImpl *flickerEffect;

EffectImpl &getEffectImpl(Effect e) {
  switch (e) {
    default:
    case ONECOLOR: return *oneColorEffect;
    case RAINBOW: return *rainbowEffect;
    case THEATRE: return *theatreEffect;
    case PLASMA: return *plasmaEffect;
    case BOUNCYBALL: return *bouncyBallEffect;
    case FLICKER: return *flickerEffect;
  }
}

void EffectImpl::serialize() {
  while (*pagep) pagep++;
  strcpy(pagep, "null");
  pagep += 4;
}
