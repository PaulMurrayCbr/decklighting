#ifndef DECKLIGHTING_H
#define DECKLIGHTING_H

#include "rgb.h"
#include "interpolation.h"
#include "effect.h"

extern char page[];
extern char *pagep;
extern char msg[];
inline void boppage() {
  while (*pagep) pagep++;
}

class Strip { // a pure virtual interface
  public:
    virtual int getLength() = 0;
    virtual void clear() = 0;
    virtual void set(int n, uint32_t c) = 0;
};


class RoomState;

enum OnOffOut { ON, OFF, OUT };

class RoomState {
  public:
    RGB color1;
    RGB color2;
    OnOffOut onOffOut = ON;

    int density = 1;
    Effect effect = ONECOLOR;
    Interpolation interpolation = LINEAR;

    EffectImpl& getEffect() {
      return getEffectImpl(effect);
    }

    RoomState(int r, int g, int b) : color1(r, g, b), color2(r, g, b) {}

    void setup(Strip &s) {
      getEffect().setup(*this, s); 
    }
    
    void loop(Strip &s) {
      getEffect().loop(*this, s); 
    }

    void loadArgs() {
      getEffect().loadArgs();
    }

    void serialize() {
      getEffect().serialize();
    }
};

class State {
  public:
    unsigned char brightness = 128;

    boolean allOn = true;

    RoomState room[5] = {
      RoomState( 64, 64, 192),
      RoomState( 192, 192, 64),
      RoomState( 64, 64, 192),
      RoomState(192, 192, 64),
      RoomState(64, 64, 192)
    }
    ;

    State() :
      brightness(128)
    {

    }
};

extern State state;

char *strcat(char *b, const __FlashStringHelper *P);


#endif
