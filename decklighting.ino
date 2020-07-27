#include "decklighting.h"
#include "webserver.h"
#include "strip.h"

State state;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  webserver_setup();
  strip_setup();
}

void loop(void) {
  webserver_loop();
  strip_loop();
  wdt_reset();
}
