#include <Arduino.h>
#include "decklighting.h"
#include "strip.h"
#include "webserver2.h"
#include "rgb.h"

class FlickerEffect  : public EffectImpl {
    int c1Rate=0;
    int c2Rate=0;
    uint32_t lastUpdate;

    double avgdurationms1=100;
    double avgdurationms2=100;
    
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

void FlickerEffect::loadArgs() {
    if (server2.hasArg("c1Rate")) {
      c1Rate = atoi(server2.arg("c1Rate").c_str());
    }
    if (server2.hasArg("c2Rate")) {
      c2Rate = atoi(server2.arg("c2Rate").c_str());
    }

    avgdurationms1 = exp(log(100) + (log(60000.0)-log(100))*(c1Rate/1000.0));
    avgdurationms2 = exp(log(100) + (log(60000.0)-log(100))*(c2Rate/1000.0));
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
