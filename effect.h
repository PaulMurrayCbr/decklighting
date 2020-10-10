#ifndef EFFECT_H
#define EFFECT_H

class RoomState;
class Strip;

enum Effect { ONECOLOR=0, RAINBOW=1, THEATRE=2, PLASMA=3, BOUNCYBALL=4, FLICKER=5 };
extern const char *effectlabel[];
extern const int effecttypes;

class EffectImpl {
  public:
    virtual void setup(RoomState &r, Strip &s) = 0;
    virtual boolean loop(RoomState &r, Strip &s) { return false;}
    virtual void loadArgs() {}
    virtual void serialize(); // the default case is defined in the .cpp
};


EffectImpl *newEffectImpl(Effect e);



#endif
