#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"
#include "rgb.h"
#include "interpolation.h"

class RainbowEffect  : public EffectImpl {
  public:
    void setup(RoomState &r, Strip &s);
};

EffectImpl *newRainbowEffect() { return new RainbowEffect();}

inline unsigned lim(double f) {
  if (f >= 255) return 255;
  if (f <= 0) return 0;
  return (unsigned) f;
}

void RainbowEffect::setup(RoomState & room, Strip & strip) {
  const int len = strip.getLength();

  static Strip& ssss = strip;

  getInterpolation(room.interpolation)(room.color1.torgb(), room.color2.torgb(), len, [](int i, rgb out) {
    uint32_t c = outputStrip.Color(out.r, out.g, out.b);
    ssss.set(i, c);
  });
}
