#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Esp.h>

#include "decklighting.h"
#include "webserver.h"
#include "passwords.h"
#include "clock.h"
#include "favicon.h"

ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80

void handleState();
void handleRoot();
void handleFavicon();

void webserver_setup() {
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

  server.on("/", handleRoot);
  server.on("/state", handleState);
  server.on("/favicon.ico", handleFavicon);
  server.on("favicon.ico", handleFavicon);
  server.onNotFound([]() {
    Serial.println("got not found");
    server.send(404, "text/plain", "404: Not found");
  });

  server.begin(); // Actually start the server
  Serial.println("HTTP server started");
}

void webserver_loop() {
    server.handleClient(); // Listen for HTTP requests from clients
}


void handleState() {
  DateTime2 dt;
  rtc.get(dt);
  toJson(dt);

  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
  server.sendHeader(F("Access-Control-Allow-Methods"), F("POST, GET, OPTIONS"));
  server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
  server.sendHeader(F("Access-Control-Max-Age"), F("86400"));

  server.send(200, "application/json", json); // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleRoot() {
    Serial.println("got root");
    server.send(200, F("text/html"), F("<html><body><a href=\"/state\">State</a></body></html>")); // Send HTTP status 200 (Ok) and send some text to the browser/client

}

void handleFavicon() {
  server.send_P(200, (PGM_P) F("image/x-icon"), (PGM_P) favicon, favicon_len);  
}
