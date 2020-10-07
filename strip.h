#ifndef DECKLIGHTING_STRIP_H
#define DECKLIGHTING_STRIP_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel outputStrip;

void strip_setup();
void strip_loop();
void strip_update();


#endif
