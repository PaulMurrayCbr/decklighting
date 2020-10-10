#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

class PlasmaEffect  : public EffectImpl {
  public:
    void setup(RoomState &r, Strip &s);
    void loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};

EffectImpl *plasmaEffect = new PlasmaEffect();

void PlasmaEffect::setup(RoomState &r, Strip &s) {
  
}

void PlasmaEffect::loop (RoomState &r, Strip &s) {
  
}

void PlasmaEffect::loadArgs() {
  
}

void PlasmaEffect::serialize() {
  boppage();
  strcpy(pagep, "{}");
  pagep += 2;
}
