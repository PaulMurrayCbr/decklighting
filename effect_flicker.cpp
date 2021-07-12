#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"
#include "webserver2.h"
#include "rgb.h"

class FlickerEffect  : public EffectImpl {
    uint32_t lastUpdate;

    double avgdurationms1=100;
    double avgdurationms2=100;
    void recalcDerivedValues(union ConfigUnion &cfg);
    
  public:
    void setup(RoomState &r, Strip &s);
    boolean loop(RoomState &r, Strip &s);
    void loadArgs(RoomState &r);
    void serialize(union ConfigUnion &cfg);
    void resetConfig(union ConfigUnion &cfg);
    void reloadConfig(union ConfigUnion &cfg);
};

EffectImpl *newFlickerEffect() { return new FlickerEffect();}

void FlickerEffect::setup(RoomState &r, Strip &s) {
  
}

boolean FlickerEffect::loop(RoomState &r, Strip &s) {
  if(millis()-lastUpdate < 25) return false;
  
  double ms = millis()-lastUpdate;
  lastUpdate = millis();

  double probofzero1 = exp (-ms/avgdurationms1);
  double probofzero2 = exp (-ms/avgdurationms2);

  long probofzero1long = 0x7FFFFFFF * probofzero1;
  long probofzero2long = 0x7FFFFFFF * probofzero2;
  
  boolean needupdate=false;
  int len = s.getLength();
  uint32_t c1 = outputStrip.Color(r.color1.r, r.color1.g, r.color1.b);
  uint32_t c2 = outputStrip.Color(r.color2.r, r.color2.g, r.color2.b);

  for(int i = 0; i<len; i++) {
    if(random(0x7FFFFFFF) > probofzero1long) {
      s.set(i, c1);
      needupdate = true;
    }
    if(random(0x7FFFFFFF) > probofzero2long) {
      s.set(i, c2);
      needupdate = true;
    }
  }
  
  return needupdate;
}

void FlickerEffect::recalcDerivedValues(union ConfigUnion &cfg) {
    avgdurationms1 = exp(log(100) + (log(60000.0)-log(100))*(cfg.flicker.c1Rate/1000.0));
    avgdurationms2 = exp(log(100) + (log(60000.0)-log(100))*(cfg.flicker.c2Rate/1000.0));
}

void FlickerEffect::resetConfig(union ConfigUnion &cfg) {
  EffectImpl::resetConfig(cfg);
  recalcDerivedValues(cfg);
}

void FlickerEffect::reloadConfig(union ConfigUnion &cfg) {
  EffectImpl::reloadConfig(cfg);
  recalcDerivedValues(cfg);
}


void FlickerEffect::loadArgs(RoomState &r) {
    if (server2.hasArg("c1Rate")) {
      r.config.flicker.c1Rate = atoi(server2.arg("c1Rate").c_str());
    }
    if (server2.hasArg("c2Rate")) {
      r.config.flicker.c2Rate = atoi(server2.arg("c2Rate").c_str());
    }
    recalcDerivedValues(r.config);
}

void FlickerEffect::serialize(union ConfigUnion &cfg) {
  boppage();
  pagep = strcat(pagep, F("{"));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"c1Rate\": "));
  itoa(cfg.flicker.c1Rate, pagep, 10);
  boppage();
  pagep = strcat(pagep, F(","));
  pagep = pagep = strcat(pagep, F("\n\t\t\t\"c2Rate\": "));
  itoa(cfg.flicker.c2Rate, pagep, 10);
  boppage();
  pagep = strcat(pagep, F("}"));
}
