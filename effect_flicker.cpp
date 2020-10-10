#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

class FlickerEffect  : public EffectImpl {
    int speed1;
    int speed2;
  public:
    void setup(RoomState &r, Strip &s);
    void loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};

EffectImpl *flickerEffect = new FlickerEffect();

void FlickerEffect::setup(RoomState &r, Strip &s) {
  
}

void FlickerEffect::loop(RoomState &r, Strip &s) {
  
}

void FlickerEffect::loadArgs() {
  
}

void FlickerEffect::serialize() {
  boppage();
  strcpy(pagep, "{}");
  pagep += 2;
}
