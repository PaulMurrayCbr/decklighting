#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"
#include "webserver2.h"

class BouncyBallEffect  : public EffectImpl {
    int head;
    int tail;
    long speed;
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

  public:
    void setup(RoomState &r, Strip &s);
    boolean loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};

EffectImpl *newBouncyBallEffect() {
  return new BouncyBallEffect();
}

void BouncyBallEffect::setup(RoomState &r, Strip &s) {
  writeTail(r);
}

void BouncyBallEffect::writeTail(RoomState &r) {
  if (tail < 0) tail = 0;
  if (tail > 50) tail = 50;

  static uint32_t *tt = tailThing;
  getInterpolation(r.interpolation)(r.color1.torgb(), r.color2.torgb(), tail, [](int i, rgb out) {
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
  for (int i = 0; i < tail; i++) {
    s.set(fold(headat + i, len), tailThing[tail - 1 - i]);
  }

  // and overwrte the top with head
  for (int i = 0 ; i < head; i++) {
    s.set(fold(headat + tail + i, len), c1);
  }

  return true;
}

void BouncyBallEffect::loadArgs() {
  if (server2.hasArg("head")) {
    head = atoi(server2.arg("head").c_str());
  }
  if (server2.hasArg("tail")) {
    tail = atoi(server2.arg("tail").c_str());
  }
  if (server2.hasArg("speed")) {
    speed = atol(server2.arg("speed").c_str());
  }

  speedPerMs = exp( log(minspeed) + (log(maxspeed) - log(minspeed)) * (speed / 1000.0));

  if (tail < 0) tail = 0;
  if (tail > 50) tail = 50;

  needToWriteTail = true;
}

void BouncyBallEffect::serialize() {
  boppage();
  pagep = strcat(pagep, F("{"));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"head\": "));
  itoa(head, pagep, 10);
  boppage();
  pagep = strcat(pagep, F(","));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"tail\": "));
  itoa(tail, pagep, 10);
  boppage();
  pagep = strcat(pagep, F(","));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"speed\": "));
  itoa(speed, pagep, 10);
  boppage();
  pagep = strcat(pagep, F("}"));
}
