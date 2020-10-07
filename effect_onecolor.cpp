#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

void OneColorEffect::setup(RoomState &room, Strip &strip) {
  const int len = strip.getLength();
  uint32_t cp = outputStrip.Color(room.color1.r, room.color1.g, room.color1.b);
  for (int i = 0; i < len; i++) {
    strip.set(i, cp);
  }
}
