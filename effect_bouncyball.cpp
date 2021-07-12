#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"
#include "webserver2.h"

class BouncyBallEffect  : public EffectImpl {    
    double speedPerMs;
    double location = 0;
    boolean needToWriteTail = true;

    uint32_t tailThing[50];

    const double minspeed = .0001;
    const double maxspeed = 1;

    int fold(int i, int len) {
      while (i < 0) i += len * 2;
      i = i % (len * 2);
      if (i >= len) {
        i = len * 2 - i;
      }
      return i;
    }

    void writeTail(RoomState &r);
    void recalcDerivedValues(union ConfigUnion &cfg);

  public:
    void setup(RoomState &r, Strip &s);
    boolean loop(RoomState &r, Strip &s);
    void loadArgs(RoomState &r);
    void serialize(union ConfigUnion &cfg);
    void resetConfig(union ConfigUnion &cfg);
    void reloadConfig(union ConfigUnion &cfg);
};

EffectImpl *newBouncyBallEffect() {
  return new BouncyBallEffect();
}

void BouncyBallEffect::setup(RoomState &r, Strip &s) {
  writeTail(r);
}

void BouncyBallEffect::writeTail(RoomState &r) {
  if (r.config.bouncyball.tail < 0) r.config.bouncyball.tail = 0;
  if (r.config.bouncyball.tail > 50) r.config.bouncyball.tail = 50;

  static uint32_t *tt = tailThing;
  getInterpolation(r.interpolation)(r.color1.torgb(), r.color2.torgb(), r.config.bouncyball.tail, [](int i, rgb out) {
    uint32_t c = outputStrip.Color(out.r, out.g, out.b);
    tt[i] = c;
  });
  needToWriteTail = false;
}

boolean BouncyBallEffect::loop(RoomState &r, Strip &s) {
  if (needToWriteTail == true) {
    writeTail(r);
    needToWriteTail = false;
  }

  static long zzz = 0;
  if (millis() - zzz < 10) return false;

  int len = s.getLength();
  int len2 = len * 2;

  location += (millis() - zzz) * speedPerMs;
  zzz = millis();

  while (location >= len2) location -= len2;

  const int headat = (int) location;

  uint32_t c1 = outputStrip.Color(r.color1.r, r.color1.g, r.color1.b);
  uint32_t c2 = outputStrip.Color(r.color2.r, r.color2.g, r.color2.b);

  for (int i = 0; i < len; i++) s.set(i, c2);

  // then the tail
  for (int i = 0; i < r.config.bouncyball.tail; i++) {
    s.set(fold(headat + i, len), tailThing[r.config.bouncyball.tail - 1 - i]);
  }

  // and overwrte the top with head
  for (int i = 0 ; i < r.config.bouncyball.head; i++) {
    s.set(fold(headat + r.config.bouncyball.tail + i, len), c1);
  }

  return true;
}

void BouncyBallEffect::recalcDerivedValues(union ConfigUnion &cfg) {
  speedPerMs = exp( log(minspeed) + (log(maxspeed) - log(minspeed)) * (cfg.bouncyball.speed / 1000.0));

  if (cfg.bouncyball.tail < 0) cfg.bouncyball.tail = 0;
  if (cfg.bouncyball.tail > 50) cfg.bouncyball.tail = 50;

  needToWriteTail = true;
}

void BouncyBallEffect::resetConfig(union ConfigUnion &cfg) {
  EffectImpl::resetConfig(cfg);
  recalcDerivedValues(cfg);
}

void BouncyBallEffect::reloadConfig(union ConfigUnion &cfg) {
  EffectImpl::reloadConfig(cfg);
  recalcDerivedValues(cfg);
}

void BouncyBallEffect::loadArgs(RoomState &r) {
  if (server2.hasArg("head")) {
    r.config.bouncyball.head = atoi(server2.arg("head").c_str());
  }
  if (server2.hasArg("tail")) {
    r.config.bouncyball.tail = atoi(server2.arg("tail").c_str());
  }
  if (server2.hasArg("speed")) {
    r.config.bouncyball.speed = atol(server2.arg("speed").c_str());
  }

  recalcDerivedValues(r.config);
}

void BouncyBallEffect::serialize(union ConfigUnion &cfg) {
  boppage();
  pagep = strcat(pagep, F("{"));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"head\": "));
  itoa(cfg.bouncyball.head, pagep, 10);
  boppage();
  pagep = strcat(pagep, F(","));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"tail\": "));
  itoa(cfg.bouncyball.tail, pagep, 10);
  boppage();
  pagep = strcat(pagep, F(","));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"speed\": "));
  itoa(cfg.bouncyball.speed, pagep, 10);
  boppage();
  pagep = strcat(pagep, F("}"));
}
