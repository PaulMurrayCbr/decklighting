#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

class PlasmaEffect  : public EffectImpl {
  public:
    void setup(RoomState &r, Strip &s);
    boolean loop(RoomState &r, Strip &s);
    void loadArgs(RoomState &r);
    void serialize();
};

EffectImpl *newPlasmaEffect() { return new PlasmaEffect();}

void PlasmaEffect::setup(RoomState &r, Strip &s) {
  
}

boolean PlasmaEffect::loop (RoomState &r, Strip &s) {
  return false;
}

void PlasmaEffect::loadArgs(RoomState &r) {
  
}

void PlasmaEffect::serialize() {
  boppage();
  strcpy(pagep, "{}");
  pagep += 2;
}
