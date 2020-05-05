#ifndef KELVIN_H
#define KELVIN_H

/* With grateful thanks to https://andi-siess.de/rgb-to-color-temperature/ */

#include <stdint.h>

#define K_LO_TEMP 1000
#define K_HI_TEMP 12000
#define K_STEP 100

#define KELVIN_SZ ((K_HI_TEMP-K_LO_TEMP)/K_STEP+1)

class Kelvin {
  public:
  const int16_t k;
  const uint8_t  r,g,b;

  Kelvin(int16_t k, uint8_t r, uint8_t g, uint8_t b) : k(k), r(r), g(g), b(b) {}
  
};

extern Kelvin kelvin[];

#endif
