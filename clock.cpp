#include <Arduino.h>
#include <Wire.h>

#include "decklighting.h"
#include "clock.h"
#include "RTC2.h"

TwoWire softwire;
RTC2 rtc(softwire);

void clock_setup() {
  softwire.begin(D4, D3);

  /*
    DateTime2 set;
    set.sec = 0;
    set.min = 19;
    set.hour = 13;
    set.mday = 25;
    set.mon = 4; // apr
    set.year = 2020;
    set.wday = 6; // today is saturday
    rtc.set(set);
  */

  DateTime2 dt;
  rtc.get(dt);
  toJson(dt);

  Serial.println(json);
}
