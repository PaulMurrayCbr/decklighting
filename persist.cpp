#include <Arduino.h>
#include <EEPROM.h>
#include "decklighting.h"
#include "persist.h"

#define STATE_BYTES ((byte*)&state)

// if the lighting hasn't been dicked around with for 5 minutes, save the state to eeprom
const uint32_t CONFIG_CHANGE_TIMEOUT = 5UL * 60UL * 1000UL;

uint32_t most_recent_config_update = 0;
boolean config_dirty = false;

void persist_setup() {
  EEPROM.begin(sizeof(state)+1);
  Serial.print("EEPROM length ");  
  Serial.println(EEPROM.length());  
  byte v = EEPROM.read(0);
  if(v != State::VERSION) {
    for(int i = 0; i<sizeof(state.room)/sizeof(*state.room); i++) {
      state.room[i].getEffect().resetConfig(state.room[i].config);
    }
  }
  else {
    EEPROM.get(1, state);
    for(int i = 0; i<sizeof(state.room)/sizeof(*state.room); i++) {
      state.room[i].getEffect().reloadConfig(state.room[i].config);
    }
  }
  
}

void persist_loop() {
  if(!config_dirty || millis()-most_recent_config_update < CONFIG_CHANGE_TIMEOUT) {
    return;
  }

  // do this first in case the next bit fails or something
  config_dirty = false;

  EEPROM.write(0, State::VERSION);
  EEPROM.put(1, state);
  EEPROM.commit();
}

void configDirty() {
  config_dirty = true;
}
