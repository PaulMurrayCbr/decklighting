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
#include "RTC.h"

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleState();

class State {
  public:
    int a;
}
state;

TwoWire softwire;
RTC rtc(softwire);

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

  Serial.println("hey, I wonder if the twowire works?");
  delay(3000);
  rtc.begin();
  Serial.println(rtc.isrunning());
  DateTime dt = rtc.now();
  Serial.println(dt.year());
  Serial.println(dt.month());
  Serial.println(dt.day());
  Serial.println(dt.hour());
  Serial.println(dt.minute());
  Serial.println(dt.second());

}

void loop(void) {
  server.handleClient();                    // Listen for HTTP requests from clients
  wdt_reset();
}

char json[256];

void handleState() {

  char json[256];

  DateTime dt = rtc.now();

  snprintf(json, sizeof(json),
           "{ \"a\": %d, \"millis\": %ld, \"dt\": { "
           "\"y\": %d, \"m\": %d, \"d\": %d,"
           "\"hh\": %d, \"mm\": %d, \"ss\": %d"
           " } }" ,
           state.a, millis(),
           dt.year(), dt.month(), dt.day(),
           dt.hour(), dt.minute(), dt.second()
          );

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.sendHeader("Access-Control-Max-Age", "86400");

  server.send(200, "application/json", json);   // Send HTTP status 200 (Ok) and send some text to the browser/client
}
