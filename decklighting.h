#ifndef DECKLIGHTING_H
#define DECKLIGHTING_H

extern char page[];
extern char *pagep;
extern char msg[];
inline void boppage() {
      while(*pagep) pagep++;
}

class RGB {
  inline char * ww(unsigned char v, char *p) {
    v &= 0xF;
    while(*p)p++;
    if(v<=9) *p='0'+v; else *p='A'+v-10;
  }
  inline void w(unsigned char v, char *p) {
    ww(v>>4, p);
    ww(v, p+1);
  }
  public:
  unsigned char r,g,b;  
  RGB(int rr, int gg, int bb) : r(rr), g(gg), b(bb) {}
  RGB() : RGB(64,64,192) { }
  inline char *write(char *p) {
    w(r, p);
    w(g, p+2);
    w(b, p+4);
    p[6] = '\0';
    return p+6;
  }
};

enum Effect { STATIC, ALTERNATE, PLASMA };

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
  Effect effect = STATIC;
  Interpolation interpolation = LINEAR;
  
  RoomState(int r, int g, int b) : color1(r,g,b), color2(r,g,b) {}
};

class State {
  public:
    unsigned char brightness = 128;
    RoomState theatre;
    RoomState gameroom;

    boolean allOn = true;
    boolean gameroomOn = true;
    boolean theatreOn = true;
    boolean tworooms = true;

    RoomState room[5] = {
      RoomState( 64, 64,192),
      RoomState( 64,192,192),
      RoomState( 64,192, 64),
      RoomState(192,192, 64),
      RoomState(192, 64, 64)
    }
    ;

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

char *strcat(char *b, const __FlashStringHelper *P);


#endif
