#ifndef DECKLIGHTING_H
#define DECKLIGHTING_H

#include "RTC2.h"

void toJson(DateTime2 & dt);
extern char json[];

class RGB {
  public:
  unsigned char r,g,b;  
  RGB(int rr, int gg, int bb) : r(rr), g(gg), b(bb) {}
  RGB() : RGB(64,64,192) { }
};

class RoomState {
  public:
  RGB color;
  RoomState(int r, int g, int b) : color(r,g,b) {}
};

class State {
  public:
    unsigned char brightness;
    RoomState theatre;
    RoomState gameroom;

  State() :
    theatre(64,64,192),
    gameroom(64,192,64),
    brightness(128)
  {
        
  }
};

extern State state;

#endif
