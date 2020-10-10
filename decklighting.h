#ifndef DECKLIGHTING_H
#define DECKLIGHTING_H

#include "rgb.h"

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

enum Effect { ONECOLOR=0, RAINBOW=1, THEATRE=2, PLASMA=3, BOUNCYBALL=4, FLICKER=5 };
extern const char *effectlabel[];
extern const int effecttypes;

// may want a pure virtual: setup, loop, derialise, readparams

class RoomState;

class EffectImpl {
  public:
    virtual void setup(RoomState &r, Strip &s) = 0;
    virtual void loop(RoomState &r, Strip &s) {}
    virtual void loadArgs() {}
    virtual void serialize() {
      boppage();
      strcpy(pagep, "null");
      pagep += 4;
    }
};

class OneColorEffect : public EffectImpl {
  public:
    void setup(RoomState &r, Strip &s);
};
class RainbowEffect  : public EffectImpl {
  public:
    void setup(RoomState &r, Strip &s);
};
class TheatreEffect  : public EffectImpl {
  public:
    void setup(RoomState &r, Strip &s);
};
class PlasmaEffect  : public EffectImpl {
  public:
    void setup(RoomState &r, Strip &s);
    void loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};
class BouncyBallEffect  : public EffectImpl {
    int width;
    long speed;
  public:
    void setup(RoomState &r, Strip &s);
    void loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};
class FlickerEffect  : public EffectImpl {
    int speed1;
    int speed2;
  public:
    void setup(RoomState &r, Strip &s);
    void loop(RoomState &r, Strip &s);
    void loadArgs();
    void serialize();
};

enum OnOffOut { ON, OFF, OUT };

enum Interpolation {
  LINEAR, HUEUP, HUEDOWN, HUENEAR, HUEFAR, HUERBOW, HUEXRBOW
};

class RoomState {
  public:
    RGB color1;
    RGB color2;
    OnOffOut onOffOut = ON;

    int density = 1;
    Effect effect = ONECOLOR;
    Interpolation interpolation = LINEAR;

    OneColorEffect oneColorEffect;
    RainbowEffect rainbowEffect;
    TheatreEffect theatreEffect;
    PlasmaEffect plasmaEffect;
    BouncyBallEffect bouncyBallEffect;
    FlickerEffect flickerEffect;

    EffectImpl& getEffect() {
      switch (effect)  {
        case ONECOLOR: return oneColorEffect;
        case RAINBOW: return rainbowEffect;
        case THEATRE: return theatreEffect;
        case PLASMA: return plasmaEffect;
        case BOUNCYBALL: return bouncyBallEffect;
        case FLICKER: return flickerEffect;
      }
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
