/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Esp.h>
#include <Wire.h>

#include "passwords.h"
#include "RTC2.h"

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleState();

class State {
  public:
    int a;
}
state;

TwoWire softwire;
RTC2 rtc(softwire);

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

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
    server.send(200, "text/html", "<html><body><a href=\"/state\">State</a></body></html>");   // Send HTTP status 200 (Ok) and send some text to the browser/client
  });
  server.on("/state", handleState);
  server.onNotFound([]() {
    Serial.println("got not root");
    server.send(404, "text/plain", "404: Not found");
  });

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");

  softwire.begin(D4, D3);

  delay(3000);
  DateTime2 dt;
  rtc.get(dt);

  Serial.println(dt.sec);         /* seconds */
  Serial.println(dt.min);         /* minutes */
  Serial.println(dt.hour);        /* hours */
  Serial.println(dt.mday);        /* day of the month */
  Serial.println(dt.mon);         /* month */
  Serial.println(dt.year);        /* year */
  Serial.println(dt.wday);        /* day of the week */
  Serial.println(dt.yday);        /* day in the year */
  Serial.println(dt.isdst);       /* daylight saving time */
  Serial.println(dt.year_s);      /* year in short notation*/

  DateTime2 set;
  set.sec = 0;
  set.min = 19;
  set.hour = 13;
  set.mday = 25;
  set.mon = 4; // apr
  set.year = 2020;
  set.wday = 6; // today is saturday
  rtc.set(set);

  rtc.get(dt);

  Serial.println(dt.sec);         /* seconds */
  Serial.println(dt.min);         /* minutes */
  Serial.println(dt.hour);        /* hours */
  Serial.println(dt.mday);        /* day of the month */
  Serial.println(dt.mon);         /* month */
  Serial.println(dt.year);        /* year */
  Serial.println(dt.wday);        /* day of the week */
  Serial.println(dt.yday);        /* day in the year */
  Serial.println(dt.isdst);       /* daylight saving time */
  Serial.println(dt.year_s);      /* year in short notation*/

}

void loop(void) {
  server.handleClient();                    // Listen for HTTP requests from clients
  wdt_reset();
}

char json[256];

void handleState() {

  char json[256];

  DateTime2 dt;
  rtc.get(dt);

  snprintf(json, sizeof(json),
           "{\n\t\"a\": %d, \n\t\"millis\": %ld, \n\t\"dt\": { "
           "\n\t\t\"sec\": %d, "
           "\n\t\t\"min\": %d, "
           "\n\t\t\"hour\": %d, "
           "\n\t\t\"day\": %d, "
           "\n\t\t\"mday\": %d, "
           "\n\t\t\"mon\": %d, "
           "\n\t\t\"year\": %d, "
           "\n\t\t\"wday\": %d, "
           "\n\t\t\"isdst\": %d, "
           "\n\t\t\"year_s\": %d "
           "\n\t}\n}" ,
           state.a, millis(), //
           dt.sec ,        //* seconds */
           dt.min  ,      //* minutes */
           dt.hour  ,      //* hours */
           dt.mday  ,      //* day of the month */
           dt.mon    ,     //* month */
           dt.year    ,    //* year */
           dt.wday     ,   //* day of the week */
           dt.yday      ,  //* day in the year */
           dt.isdst      , //* daylight saving time */
           dt.year_s      //* year in short notation*/
          );

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.sendHeader("Access-Control-Max-Age", "86400");

  server.send(200, "application/json", json);   // Send HTTP status 200 (Ok) and send some text to the browser/client
}
