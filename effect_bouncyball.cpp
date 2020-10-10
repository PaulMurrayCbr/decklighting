#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

class BouncyBallEffect  : public EffectImpl {
    int width;
    long speed;
  public:
    void setup(RoomState &r, Strip &s);
    void loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};

EffectImpl *bouncyBallEffect = new BouncyBallEffect();

void BouncyBallEffect::setup(RoomState &r, Strip &s) {
  
}

void BouncyBallEffect::loop(RoomState &r, Strip &s) {
  
}

void BouncyBallEffect::loadArgs() {
  
}

void BouncyBallEffect::serialize() {
  boppage();
  strcpy(pagep, "{}");
  pagep += 2;
}
