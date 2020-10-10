#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"

class TheatreEffect  : public EffectImpl {
  public:
    void setup(RoomState &r, Strip &s);
};

EffectImpl *newTheatreEffect() { return new TheatreEffect();}

void TheatreEffect::setup(RoomState &room, Strip &strip) {
  const int len = strip.getLength();
  uint32_t cp[2];
  cp[0] = outputStrip.Color(room.color1.r, room.color1.g, room.color1.b);
  cp[1] = outputStrip.Color(room.color2.r, room.color2.g, room.color2.b);

  for (int i = 0; i < len; i++) {
    strip.set(i, cp[i & 1]);
  }
  
}
