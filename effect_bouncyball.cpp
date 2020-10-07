#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

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
