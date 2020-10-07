#ifndef DECKLIGHTING_H
#define DECKLIGHTING_H

extern char page[];
extern char *pagep;
extern char msg[];
inline void boppage() {
  while (*pagep) pagep++;
}

class RGB {
    inline void ww(unsigned char v, char *p) {
      v &= 0xF;
      if (v <= 9) *p = '0' + v; else *p = 'A' + v - 10;
    }
    inline void w(unsigned char v, char *p) {
      ww(v >> 4, p);
      ww(v, p + 1);
    }
    inline unsigned int h(char c) {
      if (c >= '0' && c <= '9') return c - '0';
      if (c >= 'a' && c <= 'f') return c - 'a' + 10;
      if (c >= 'A' && c <= 'F') return c - 'A' + 10;
      return 0;
    }
  public:
    unsigned char r, g, b;
    RGB(int rr, int gg, int bb) : r(rr), g(gg), b(bb) {}
    RGB() : RGB(64, 64, 192) { }
    inline char *write(char *p) {
      while (*p) p++;
      w(r, p);
      w(g, p + 2);
      w(b, p + 4);
      p[6] = '\0';
      return p + 6;
    }

    void read(const String &s) {
      const char *p = s.c_str();
      p++; // skip the hash character
      r = (h(p[0]) << 4) | (h(p[1]));
      g = (h(p[2]) << 4) | (h(p[3]));
      b = (h(p[4]) << 4) | (h(p[5]));
    }
};

class Strip { // a pure virtual interface
  public:
    virtual int getLength() = 0;
    virtual void clear() = 0;
    virtual void set(int n, uint32_t c) = 0;
};


enum Effect { ONECOLOR, RAINBOW, THEATRE, PLASMA, BOUNCYBALL, FLICKER };

// may want a pure virtual: setup, loop, derialise, readparams

class RoomState;

class EffectImpl {
  public:
    virtual void setup(RoomState &r, Strip &s) = 0;
    virtual void loop(RoomState &r, Strip &s) {}
    virtual void loadArgs() {}
    virtual void serialize() {
      boppage();
      strcpy(pagep, "null;");
      pagep += 5;
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
