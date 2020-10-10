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

int room;

int parseIntArg(char *name) {
  if (server2.hasArg(name)) {
    String s = server2.arg(name);
    return atoi(s.c_str());
  }
  else {
    return 0;
  }
}

void parseRoom() {
  room = parseIntArg("room");
  if (room < 1 || room > 5) room = 0;
}

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
  server2.on("/out", ws2Out);

  server2.onNotFound([]() {
    Serial.println("got not found");
    server2.send(404, "text/plain", "404: Not found");
  });

  server2.begin(); // Actually start the server
  Serial.println("HTTP server started");
}

void webserver2_loop() {
  pagep = page;
  page[0] = '\0';
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
  parseRoom();

  if (room == 0) {
    state.allOn = true;

    if (server2.hasArg("brightness")) {
      state.brightness = parseIntArg("brightness");
      if (state.brightness < 0 || state.brightness > 192) {
        state.brightness = 64;
      }
    }
  }
  else {
    RoomState &r = state.room[room - 1];

    r.onOffOut = ON;

    if (server2.hasArg("density")) {
      r.density = parseIntArg("density");
    }
    if (server2.hasArg("effect")) {
      for(int i = 0; i<effecttypes; i++) {
        if(server2.arg("effect") == effectlabel[i]) {
          r.effect = (Effect) i;
        }
      }
    }
    if (server2.hasArg("c1")) {
      Serial.println("got argument c1");
      Serial.println(server2.arg("c1").c_str());
      r.color1.read(server2.arg("c1"));
    }
    if (server2.hasArg("c2")) {
      Serial.println("got argument c2");
      Serial.println(server2.arg("c2").c_str());
      r.color2.read(server2.arg("c2"));
    }
    if (server2.hasArg("interp")) {
      if (server2.arg("interp") == "linear") r.interpolation = LINEAR;
      if (server2.arg("interp") == "huenear") r.interpolation = HUENEAR;
      if (server2.arg("interp") == "huefar") r.interpolation = HUEFAR;
      if (server2.arg("interp") == "huerbow") r.interpolation = HUERBOW;
      if (server2.arg("interp") == "huexrbow") r.interpolation = HUEXRBOW;
      if (server2.arg("interp") == "hueup") r.interpolation = HUEUP;
      if (server2.arg("interp") == "huedown") r.interpolation = HUEDOWN;
    }

  }

  strip_update();
  reply();
}

void ws2Off() {
  parseRoom();

  if (room == 0) {
    state.allOn = false;
  }
  else {
    RoomState &r = state.room[room - 1];
    r.onOffOut = OFF;
  }
  strip_update();
  reply();
}

void ws2Out() {
  parseRoom();

  if (room == 0) {
    state.allOn = false;
  }
  else {
    RoomState &r = state.room[room - 1];
    r.onOffOut = OUT;
  }
  strip_update();
  reply();
}

void reply() {
  server2.sendHeader("Access-Control-Allow-Origin", "*");
  server2.sendHeader("Access-Control-Allow-Method", "GET, POST");

  parseRoom();

  if (room != 0) {
    replyRoom(state.room[room - 1]);
  }
  else {
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
    case  HUEXRBOW: pagep = strcat(pagep, F("huexrbow")); break;
  }

  pagep = strcat(pagep, F("\", \"effect\": \""));
  pagep = strcat(pagep, effectlabel[s.effect]);
  pagep = strcat(pagep, F("\""));

  pagep = strcat(pagep, F(", \n\t\"effectData\": { \n\t\t\""));
  pagep = strcat(pagep, effectlabel[s.effect]);
  pagep = strcat(pagep, F("\": "));
  s.getEffect().serialize();
  pagep = strcat(pagep, F("}"));
  
  pagep = strcat(pagep, F("}"));

  server2.send(200, "application/json", page);

}
