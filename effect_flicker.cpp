#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"
#include "webserver2.h"

class FlickerEffect  : public EffectImpl {
    int c1Rate;
    int c2Rate;
  public:
    void setup(RoomState &r, Strip &s);
    boolean loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};

EffectImpl *newFlickerEffect() { return new FlickerEffect();}

void FlickerEffect::setup(RoomState &r, Strip &s) {
  
}

boolean FlickerEffect::loop(RoomState &r, Strip &s) {
  return false;
}

void FlickerEffect::loadArgs() {
    if (server2.hasArg("c1Rate")) {
      c1Rate = atoi(server2.arg("c1Rate").c_str());
    }
    if (server2.hasArg("c2Rate")) {
      c2Rate = atoi(server2.arg("c2Rate").c_str());
    }
}

void FlickerEffect::serialize() {
  boppage();
  pagep = strcat(pagep, F("{"));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"c1Rate\": "));
  itoa(c1Rate, pagep, 10);
  boppage();
  pagep = strcat(pagep, F(","));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"c2Rate\": "));
  itoa(c2Rate, pagep, 10);
  boppage();
  pagep = strcat(pagep, F("}"));
}
