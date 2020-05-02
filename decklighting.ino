/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Esp.h>

#include <Adafruit_NeoPixel.h>

#include "decklighting.h"
#include "passwords.h"
#include "webserver.h"
#include "clock.h"

State state;

ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80

void handleState();

char json[256];

const int PIXELS = 110;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, D8, NEO_BGR + NEO_KHZ800);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  setup_server();
  setup_clock();
  setup_strip();
}

void loop(void) {
  server.handleClient(); // Listen for HTTP requests from clients
  updateStrip();
  wdt_reset();
}


void setup_server() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int nl = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++nl >= 30) {
      Serial.println();
      nl = 0;
    }
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Setup done");

  server.on("/", []() {
    Serial.println("got root");
    server.send(200, "text/html", "<html><body><a href=\"/state\">State</a></body></html>"); // Send HTTP status 200 (Ok) and send some text to the browser/client
  });
  server.on("/state", handleState);
  server.onNotFound([]() {
    Serial.println("got not root");
    server.send(404, "text/plain", "404: Not found");
  });

  server.begin(); // Actually start the server
  Serial.println("HTTP server started");
}


void setup_strip() {
  strip.begin();
  strip.setBrightness(128);
}

void handleState() {
  DateTime2 dt;
  rtc.get(dt);
  toJson(dt);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.sendHeader("Access-Control-Max-Age", "86400");

  server.send(200, "application/json", json); // Send HTTP status 200 (Ok) and send some text to the browser/client
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
           state.a, millis(), //
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

void updateStrip() {
  static long lastUpdate;

  if(millis()-lastUpdate < 1000) return;
  lastUpdate = millis();
  
  int j = (int)(millis()/100L);
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i-j) & 255));
  }
  strip.show();
  
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
