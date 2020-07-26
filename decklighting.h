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
  RGB color1;
  RGB color2;

  int density = 1;
  
  RoomState(int r, int g, int b) : color1(r,g,b), color2(r,g,b) {}
};

class State {
  public:
    unsigned char brightness;
    RoomState theatre;
    RoomState gameroom;

    boolean allOn = true;
    boolean gameroomOn = true;
    boolean theatreOn = true;
    boolean tworooms = true;

  State() :
    theatre(64,64,192),
    gameroom(64,192,64),
    brightness(128)
  {
        
  }
};

class Strip {
  public:
  virtual int getLength() = 0;
  virtual void clear() = 0;
  virtual void set(int n, uint32_t c) = 0;
};

extern State state;

#endif
