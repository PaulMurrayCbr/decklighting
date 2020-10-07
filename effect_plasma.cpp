#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

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
