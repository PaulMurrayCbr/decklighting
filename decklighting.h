#ifndef DECKLIGHTING_H
#define DECKLIGHTING_H

#include "RTC2.h"

void toJson(DateTime2 & dt);
extern char json[];


class State {
  public:
    int a;
};


extern State state;




#endif
