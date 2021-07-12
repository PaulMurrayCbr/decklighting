#ifndef EFFECT_H
#define EFFECT_H

#include "config.h"

class RoomState;
class Strip;

enum Effect { ONECOLOR=0, RAINBOW=1, THEATRE=2, PLASMA=3, BOUNCYBALL=4, FLICKER=5 };
extern const char *effectlabel[];
extern const int effecttypes;

class EffectImpl {
  public:
    virtual void setup(RoomState &r, Strip &s) = 0;
    virtual boolean loop(RoomState &r, Strip &s) { return false;}
    virtual void loadArgs(RoomState &r) {}
    virtual void serialize(union ConfigUnion &cfg); // the default case is defined in the .cpp

    // user has selected a different effect. reset the config to sensible values
    // default is memset(0)
    virtual void resetConfig(union ConfigUnion &cfg);
    // we have restarted from a reset. Reload the old config.
    virtual void reloadConfig(union ConfigUnion &cfg) {}
};


EffectImpl *newEffectImpl(Effect e);


#endif
