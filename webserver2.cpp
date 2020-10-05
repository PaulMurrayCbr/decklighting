#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Esp.h>

#include "decklighting.h"
#include "webserver2.h"
#include "passwords.h"
#include "favicon.h"
#include "strip.h"

ESP8266WebServer server2(80); // Create a webserver object that listens for HTTP request on port 80

void ws2Home();

void ws2Status();
void ws2On();
void ws2Off();
void ws2Out();
void ws2Effect();
void ws2Color();
void ws2Interp();

void reply();
void replyRoom(RoomState &s);
void replyAll();

void webserver2_setup() {

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

  server2.on("/", ws2Home);
  server2.on("/index.html", ws2Home);
  server2.on("/status", ws2Status);
  server2.on("/on", ws2On);
  server2.on("/off", ws2Off);
  server2.on("/off", ws2Out);
  server2.on("/effect", ws2Effect);
  server2.on("/color", ws2Color);
  server2.on("/interp", ws2Interp);

  server2.onNotFound([]() {
    Serial.println("got not found");
    server2.send(404, "text/plain", "404: Not found");
  });

  server2.begin(); // Actually start the server
  Serial.println("HTTP server started");
}

void webserver2_loop() {
  server2.handleClient(); // Listen for HTTP requests from clients
}

void ws2Home() {
  server2.sendHeader("Location", "http://paulmurraycbr.github.io/decklighting/index.html");
  server2.send(303, "text/html", F("<html><body>Use <a href=\"http://paulmurraycbr.github.io/decklighting/index.html\">the new, you-beaut angular app at github</a>!</body></html>"));
  memset(msg, ' ', 120);
  msg[0] = '\0';
}

void ws2Status() {
  reply();
}

void ws2On() {
  reply();
}

void ws2Off() {
  reply();
}

void ws2Out() {
  reply();
}

void ws2Effect() {
  reply();
}

void ws2Color() {
  reply();
}

void ws2Interp() {
  reply();
}

void reply() {
  server2.sendHeader("Access-Control-Allow-Origin", "*");
  server2.sendHeader("Access-Control-Allow-Method", "GET, POST");

  if (server2.hasArg("room")) {
    String s = server2.arg("room");
    int n = atoi(s.c_str());
    if (n >= 1 && n <= 5) replyRoom(state.room[n-1]);
    else replyAll();
  }
  else {
nope:
    replyAll();
  }
}

void replyAll() {
  pagep = strcat(pagep, F("{ \"status\": \""));
  pagep = strcat(pagep, state.allOn ? F("on") : F("off"));
  pagep = strcat(pagep, F("\", \"brightness\": "));
  itoa(state.brightness, pagep, 10);
  boppage();
  pagep = strcat(pagep, F("}"));

  server2.send(200, "application/json", page);
}

void replyRoom(RoomState &s) {

  pagep = strcat(pagep, F("{ \"status\": \""));
  switch (s.onOffOut) {
    case ON: pagep = strcat(pagep, F("on")); break;
    case OFF: pagep = strcat(pagep, F("off")); break;
    case OUT: pagep = strcat(pagep, F("out")); break;
  }
  pagep = strcat(pagep, F("\", \"density\": "));
  itoa(s.density, pagep, 10);
  boppage();
  pagep = strcat(pagep, F(",\"c1\": \"#"));
  pagep = s.color1.write(pagep);
  pagep = strcat(pagep, F("\",\"c2\": \"#"));
  pagep = s.color2.write(pagep);
  pagep = strcat(pagep, F("\", \"interp\": \""));

  switch (s.interpolation) {
    case LINEAR: pagep = strcat(pagep, F("linear")); break;
    case HUEUP: pagep = strcat(pagep, F("hueup")); break;
    case HUEDOWN: pagep = strcat(pagep, F("huedown")); break;
    case HUENEAR: pagep = strcat(pagep, F("huenear")); break;
    case  HUEFAR: pagep = strcat(pagep, F("huefar")); break;
    case  HUERBOW: pagep = strcat(pagep, F("huerbow")); break;
    case  HUEXRBOW: pagep = strcat(pagep, F("huexbow")); break;
  }

  pagep = strcat(pagep, F("\", \"effect\": \""));
  switch (s.effect) {
    case STATIC: pagep = strcat(pagep, F("rainbow")); break;
    case ALTERNATE: pagep = strcat(pagep, F("theatre")); break;
    case PLASMA: pagep = strcat(pagep, F("plasma")); break;
  }
  pagep = strcat(pagep, F("\""));

  pagep = strcat(pagep, F(", \n\t\"effectData\": {}"));

  pagep = strcat(pagep, F("}"));

  server2.send(200, "application/json", page);

}
