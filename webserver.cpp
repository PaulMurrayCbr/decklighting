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
#include "strip.h"

ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80

//void handleState();
void handleRoot();
void writeColor(RoomState &room, char* p);
void handleGameroom();
void handleTheatre();
void updateRoom(RoomState& room);
void handleGlobal();
void handleFavicon();
void redirectToRoot();

char content[] =
  R"zzzz(<html><body>
<div>
 <form action="http://192.168.0.14/gameroom">
  <div><label for="gameroom-color">Game room <input type="color" id="gameroom-color" name="color" value="#*AAAAA"></label><input type="submit"></div>
 </form>
</div>
<div>
 <form action="http://192.168.0.14/theatre">
  <div><label for="theatre-color">Theatre <input type="color" id="theatre-color" name="color" value="#*BBBBB"></label><input type="submit"></div>
 </form>
</div>
<div>
 <form action="http://192.168.0.14/global">
  <div><label for="brightness">Brightness<input type="number" id="brightness" name="brightness" value="*55"></label><input type="submit"></div>
 </form>
</div>
</body></html>
)zzzz";

char *gameroomP, *theatreP, *brightnessP;

char hex[] = "0123456789ABCDEF";

inline char encodehex(unsigned n) { return hex[n&15]; }
inline int decodehex(char c) { if(c>='a') c-='a'-'A';return  strchr(hex,c)-hex; }

void webserver_setup() {
  gameroomP = strchr(content, '*');
  theatreP = strchr(gameroomP+1, '*');
  brightnessP = strchr(theatreP+1, '*');

  Serial.println(content);
  Serial.println(gameroomP);
  Serial.println(theatreP);
  Serial.println(brightnessP);
  
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
//  server.on("/state", handleState);
  server.on("/favicon.ico", handleFavicon);
  server.on("favicon.ico", handleFavicon);
  server.on("/gameroom", handleGameroom);
  server.on("/theatre", handleTheatre);
  server.on("/global", handleGlobal);
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


//void handleState() {
//  DateTime2 dt;
//  rtc.get(dt);
//  toJson(dt);
//
//  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
//  server.sendHeader(F("Access-Control-Allow-Methods"), F("POST, GET, OPTIONS"));
//  server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
//  server.sendHeader(F("Access-Control-Max-Age"), F("86400"));
//
//  server.send(200, "application/json", json); // Send HTTP status 200 (Ok) and send some text to the browser/client
//}

void handleRoot() {
  writeColor(state.gameroom, gameroomP);
  writeColor(state.theatre, theatreP);

  brightnessP[0] = (state.brightness / 100) % 10 + '0';
  brightnessP[1] = (state.brightness / 10) % 10 + '0';
  brightnessP[2] = (state.brightness / 1) % 10 + '0';
    
  server.send(200, "text/html", content); // Send HTTP status 200 (Ok) and send some text to the browser/client

}

void writeColor(RoomState &room, char* p) {
    p[0] = encodehex(room.color.r >> 4);
    p[1] = encodehex(room.color.r);
    p[2] = encodehex(room.color.g >> 4);
    p[3] = encodehex(room.color.g);
    p[4] = encodehex(room.color.b >> 4);
    p[5] = encodehex(room.color.b);
  
}

void handleGameroom() {
    updateRoom(state.gameroom);
    redirectToRoot();
}

void handleTheatre() {
    updateRoom(state.theatre);
    redirectToRoot();
}

void updateRoom(RoomState& room) {
    boolean update = false;
    for(int i = 0; i< server.args(); i++) {
      const char *arg = server.arg(i).c_str();
      const char *argName = server.argName(i).c_str();
      if(strcmp(argName, "color")==0) {
        arg++;
        room.color.r = (decodehex(arg[0]) << 4) | decodehex(arg[1]);
        room.color.g = (decodehex(arg[2]) << 4) | decodehex(arg[3]);
        room.color.b = (decodehex(arg[4]) << 4) | decodehex(arg[5]);
        update = true;
      }
    }

    if(update) strip_update();  
}

void handleGlobal() {
    boolean update = false;
  
    for(int i = 0; i< server.args(); i++) {
      const char *arg = server.arg(i).c_str();
      const char *argName = server.argName(i).c_str();
      if(strcmp(argName, "brightness")==0) {
        state.brightness = atoi(arg);
        update = true;
      }
    }

    if(update) strip_update();

    redirectToRoot();
}

void redirectToRoot() {
  server.sendHeader(F("Location"), F("/"));

  server.send(303, F("text/plain"), F("redirect")); // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleFavicon() {
  server.send_P(200, (PGM_P) F("image/x-icon"), (PGM_P) favicon, favicon_len);  
}
