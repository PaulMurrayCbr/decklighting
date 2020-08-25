#include "decklighting.h"
#include "webserver.h"
#include "strip.h"

State state;

char page[15000];
char *pagep;
char msg[130];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  webserver_setup();
  strip_setup();
}

void loop(void) {
  pagep = page;
  webserver_loop();
  strip_loop();
  wdt_reset();
}
