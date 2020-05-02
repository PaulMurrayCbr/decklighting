#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Esp.h>

#include "decklighting.h"
#include "webserver.h"
#include "passwords.h"
#include "clock.h"

ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80

void handleState();

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

void webserver_loop() {
    server.handleClient(); // Listen for HTTP requests from clients
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
