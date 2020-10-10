#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

class BouncyBallEffect  : public EffectImpl {
    int width;
    long speed;
  public:
    void setup(RoomState &r, Strip &s);
    boolean loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};

EffectImpl *newBouncyBallEffect() {return new BouncyBallEffect();}

void BouncyBallEffect::setup(RoomState &r, Strip &s) {
  
}

boolean BouncyBallEffect::loop(RoomState &r, Strip &s) {
  return false;
}

void BouncyBallEffect::loadArgs() {
  
}

void BouncyBallEffect::serialize() {
  boppage();
  strcpy(pagep, "{}");
  pagep += 2;
}
