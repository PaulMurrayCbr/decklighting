#include <Arduino.h>
#include <EEPROM.h>
#include "decklighting.h"
#include "persist.h"

#define STATE_BYTES ((byte*)&state)

// if the lighting hasn't been dicked around with for 5 minutes, save the state to eeprom
const uint32_t CONFIG_CHANGE_TIMEOUT = 5UL * 60UL * 1000UL;

uint32_t most_recent_config_update = 0;
boolean config_dirty = false;
boolean dont_use_eeprom;

void persist_setup() {
  dont_use_eeprom = (sizeof(state)+1) > EEPROM.length();
  if(dont_use_eeprom) {
    Serial.println();
    Serial.print("sizeof state is ");
    Serial.print(sizeof(State));
    Serial.print(", eeprom length is ");
    Serial.print(EEPROM.length());
    Serial.println(", not enough eeprom for state + version byte");
    return;
  }
  byte v = EEPROM.read(0);
  if(v != State::VERSION) return;
  for(int i = 0; i<sizeof(state);i++) {
    STATE_BYTES[i] = EEPROM.read(i+1);
  }
}

void persist_loop() {
  if(dont_use_eeprom || !config_dirty) { // || millis()-most_recent_config_update < CONFIG_CHANGE_TIMEOUT) {
    return;
  }

  // do this first in case the next bit fails or something
  config_dirty = false;

  // read first to reduce eeprom cycles
  if(State::VERSION != EEPROM.read(0)) {
    EEPROM.write(0, State::VERSION);
  }
  for(int i = 0; i<sizeof(state);i++) {
    if(STATE_BYTES[i] != EEPROM.read(i+1)) {
      EEPROM.write(i+1, STATE_BYTES[i]);
    }
  }  
}

void configDirty() {
  config_dirty = true;
}
