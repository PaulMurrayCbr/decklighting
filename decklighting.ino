#include "decklighting.h"
#include "webserver.h"
#include "clock.h"
#include "strip.h"

State state;

char json[256];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  webserver_setup();
  clock_setup();
  strip_setup();
}

void loop(void) {
  webserver_loop();
  strip_loop();
  wdt_reset();
}


void toJson(DateTime2 & dt) {
  snprintf(json, sizeof(json),
           "{\n\t\"a\": %d, \n\t\"millis\": %ld, \n\t\"dt\": { "
           "\n\t\t\"sec\": %d, "
           "\n\t\t\"min\": %d, "
           "\n\t\t\"hour\": %d, "
           "\n\t\t\"mday\": %d, "
           "\n\t\t\"mon\": %d, "
           "\n\t\t\"year\": %d, "
           "\n\t\t\"wday\": %d, "
           "\n\t\t\"yday\": %d, "
           "\n\t\t\"isdst\": %d, "
           "\n\t\t\"year_s\": %d "
           "\n\t}\n}",
           0, millis(), //
           dt.sec, //* seconds */
           dt.min, //* minutes */
           dt.hour, //* hours */
           dt.mday, //* day of the month */
           dt.mon, //* month */
           dt.year, //* year */
           dt.wday, //* day of the week */
           dt.yday, //* day in the year */
           dt.isdst, //* daylight saving time */
           dt.year_s //* year in short notation*/
          );
  json[sizeof(json) - 1] = '\0'; // just in case.
}
