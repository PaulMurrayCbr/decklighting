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
    EffectImpl *effectImpl[10];
    Interpolation interpolation = LINEAR;

    EffectImpl& getEffect() {
      return *effectImpl[effect];
    }

    RoomState(int r, int g, int b) : color1(r, g, b), color2(r, g, b) {
      for(int i =0 ;i<effecttypes; i++) {
        effectImpl[i] = newEffectImpl((Effect)i);
      }
    }

    void setup(Strip &s) {
      getEffect().setup(*this, s); 
    }
    
    boolean loop(Strip &s) {
      return getEffect().loop(*this, s); 
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
    // change this every time the structure of this class is changed.
    static const byte VERSION = 1;
    
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
