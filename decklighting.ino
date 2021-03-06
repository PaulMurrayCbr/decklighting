#include "decklighting.h"
#include "webserver2.h"
#include "strip.h"
#include "persist.h"

State state;

char page[15000];
char *pagep;
char msg[130];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  persist_setup();
  webserver2_setup();
  strip_setup();
}

void loop(void) {
  persist_loop();
  webserver2_loop();
  strip_loop();
  wdt_reset();
}

char *strcat(char *b, const __FlashStringHelper *P) {
  char *p = (char *)(void *)P;
  while (*b) b++;
  while (*b++ = pgm_read_byte_near(p++));
  return b - 1;
}
