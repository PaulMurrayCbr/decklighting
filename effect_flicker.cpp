#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

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
