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
void writeColor(RGB &color, char* p);
void handleGameroom();
void handleTheatre();
void updateRoom(RoomState& room);
void handleGlobal();
void handleFavicon();
void redirectToRoot();

void handleAllOnOff();
void handleGameroomOnOff();
void handleTheatreOnOff();
void handleTworoomsOnOff();

char content[] =
  R"zzzz(<html><head>
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
  <style>
  @viewport { width: device-width; zoom: 1.0;} 
  .main { border: thin solid gray; margin: 2px; padding: .5em;} 
  </style>
  </head><body>
012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789  
<div class="main">
 <form action="http://192.168.0.14/allonoff"><input type="submit" value="On/Off"></form>
 <form action="http://192.168.0.14/theatreonoff"><input type="submit" value="Theatre On/Off"></form>
 <form action="http://192.168.0.14/gameroomonoff"><input type="submit" value="Game Room On/Off"></form>
   <form action="http://192.168.0.14/tworoomsonoff"><input type="submit" value="One Room/Two Rooms"></form>
</div>
<div class="main">
 Theatre
 <form action="http://192.168.0.14/theatre">
  <input type="hidden" name="color1" value="#FFF0E0">
  <input type="hidden" name="color2" value="#FFF0E0">
  <input type="submit" value="ALL WHITE">
 </form>
 <form action="http://192.168.0.14/theatre">
  <div><label for="theatre-color1">Colour 1 <input type="color" id="theatre-color1" name="color1" value="#theac1"></label></div>
  <div><label for="theatre-color2">Colour 2 <input type="color" id="theatre-color2" name="color2" value="#theac2"></label></div>
  <div><input type="submit" value="Set"></div>
 </form>
</div>
<div class="main">
  Game Room
 <form action="http://192.168.0.14/gameroom">
  <input type="hidden" name="color1" value="#FFF0E0">
  <input type="hidden" name="color2" value="#FFF0E0">
  <input type="submit" value="ALL WHITE">
 </form>
 <form action="http://192.168.0.14/gameroom">
  <div><label for="gameroom-color1">Colour 1 <input type="color" id="gameroom-color1" name="color1" value="#grooc1"></label></div>
  <div><label for="gameroom-color2">Colour 2 <input type="color" id="gameroom-color2" name="color2" value="#grooc2"></label></div>
  <div><input type="submit" value="Set"></div>
 </form>
</div>
<div class="main">
 <form action="http://192.168.0.14/global">
  <div><label for="brightness">Brightness <input type="number" id="brightness" name="brightness" value="*BR"></label><input type="submit" value="Set"></div>
 </form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="192"><input type="submit" value="192"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="128"><input type="submit" value="128"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="64"><input type="submit" value="64"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="32"><input type="submit" value="32"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="16"><input type="submit" value="16"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="8"><input type="submit" value="8"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="5"><input type="submit" value="5"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="4"><input type="submit" value="4"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="3"><input type="submit" value="3"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="2"><input type="submit" value="2"></form>
 <form action="http://192.168.0.14/global" style="display: inline-block;"><input type="hidden" name="brightness" value="1"><input type="submit" value="1"></form>
</div>
</body></html>
)zzzz";

char hex[] = "0123456789ABCDEF";

inline char encodehex(unsigned n) { return hex[n&15]; }
inline int decodehex(char c) { if(c>='a') c-='a'-'A';return  strchr(hex,c)-hex; }

class RoomStateHandler {
  public:
  RoomState& rs;
  char *color1;
  char *color2;

  RoomStateHandler(RoomState& rs) : rs(rs) {}

  void writeState() {
    writeColor(rs.color1, color1);
    writeColor(rs.color2, color2);
  }

  void readHttp()  {
    boolean update = false;
    for(int i = 0; i< server.args(); i++) {
      const char *arg = server.arg(i).c_str();
      const char *argName = server.argName(i).c_str();
      if(strcmp(argName, "color1")==0) {
        arg++;
        rs.color1.r = (decodehex(arg[0]) << 4) | decodehex(arg[1]);
        rs.color1.g = (decodehex(arg[2]) << 4) | decodehex(arg[3]);
        rs.color1.b = (decodehex(arg[4]) << 4) | decodehex(arg[5]);
        update = true;
      }
      if(strcmp(argName, "color2")==0) {
        arg++;
        rs.color2.r = (decodehex(arg[0]) << 4) | decodehex(arg[1]);
        rs.color2.g = (decodehex(arg[2]) << 4) | decodehex(arg[3]);
        rs.color2.b = (decodehex(arg[4]) << 4) | decodehex(arg[5]);
        update = true;
      }
    }

    if(update) strip_update();  
  }

} gameroom(state.gameroom), theatre(state.theatre);


char *brightnessP;
char *msg;


void webserver_setup() {

  gameroom.color1 = strstr(content, "grooc1");
  gameroom.color2 = strstr(content, "grooc2");
  theatre.color1 = strstr(content, "theac1");
  theatre.color2 = strstr(content, "theac2");

  brightnessP = strstr(content, "*BR");
  msg = strstr(content, "0123456789");
  memset(msg, ' ', 120);
  
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

  server.on("/allonoff", handleAllOnOff);
  server.on("/gameroomonoff", handleGameroomOnOff);
  server.on("/theatreonoff", handleTheatreOnOff);
  server.on("/tworoomsonoff", handleTworoomsOnOff);
  
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

void handleRoot() {
  gameroom.writeState();
  theatre.writeState();

  brightnessP[0] = (state.brightness / 100) % 10 + '0';
  brightnessP[1] = (state.brightness / 10) % 10 + '0';
  brightnessP[2] = (state.brightness / 1) % 10 + '0';
    
  server.send(200, "text/html", content); // Send HTTP status 200 (Ok) and send some text to the browser/client

  memset(msg, ' ', 120);
}

void writeColor(RGB& color, char* p) {
    p[0] = encodehex(color.r >> 4);
    p[1] = encodehex(color.r);
    p[2] = encodehex(color.g >> 4);
    p[3] = encodehex(color.g);
    p[4] = encodehex(color.b >> 4);
    p[5] = encodehex(color.b);
  
}

void handleGameroom() {
    gameroom.readHttp();
    memset(msg, ' ', 120);
    sprintf(msg, "gameroom updated to %d/%d/%d - %d/%d/%d",
     state.gameroom.color1.r, state.gameroom.color1.g, state.gameroom.color1.b,
     state.gameroom.color2.r, state.gameroom.color2.g, state.gameroom.color2.b
    );
    msg[strlen(msg)] = ' ';
    
    redirectToRoot();
}

void handleTheatre() {
    theatre.readHttp();
    memset(msg, ' ', 120);
    sprintf(msg, "theatre updated to %d/%d/%d - %d/%d/%d",
     state.theatre.color1.r, state.theatre.color1.g, state.theatre.color1.b,
     state.theatre.color2.r, state.theatre.color2.g, state.theatre.color2.b
    );
    msg[strlen(msg)] = ' ';
    redirectToRoot();
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

    memset(msg, ' ', 120);
    sprintf(msg, "Brightness updated to %d", state.brightness);
    msg[strlen(msg)] = ' ';

    redirectToRoot();
}

void handleAllOnOff() {
  state.allOn = !state.allOn;
  strip_update();

    memset(msg, ' ', 120);
  sprintf(msg, 
  "<B>%s</B>, %s, %s, %s.",
  state.allOn ? "All On" : "All Off",
  state.theatreOn ? "Theatre On" : "Theatre Off",
  state.gameroomOn ? "Gameroom On" : "Gameroom Off",
  state.tworooms ? "Separate Rooms" : "One Room");
  msg[strlen(msg)] = ' ';
  
  redirectToRoot();
}

void handleTheatreOnOff() {
  state.theatreOn = !state.theatreOn;
  strip_update();

    memset(msg, ' ', 120);
  sprintf(msg, 
  "%s, <B>%s</B>, %s, %s.",
  state.allOn ? "All On" : "All Off",
  state.theatreOn ? "Theatre On" : "Theatre Off",
  state.gameroomOn ? "Gameroom On" : "Gameroom Off",
  state.tworooms ? "Separate Rooms" : "One Room");
  msg[strlen(msg)] = ' ';
  
  redirectToRoot();
}

void handleGameroomOnOff() {
  state.gameroomOn = !state.gameroomOn;
  strip_update();

    memset(msg, ' ', 120);
  sprintf(msg, 
  "%s, %s, <B>%s</B>, %s.",
  state.allOn ? "All On" : "All Off",
  state.theatreOn ? "Theatre On" : "Theatre Off",
  state.gameroomOn ? "Gameroom On" : "Gameroom Off",
  state.tworooms ? "Separate Rooms" : "One Room");
  msg[strlen(msg)] = ' ';
  
  redirectToRoot();
}

void handleTworoomsOnOff() {
  state.tworooms = !state.tworooms;
  strip_update();

    memset(msg, ' ', 120);
  sprintf(msg, 
  "%s, %s, %s, <B>%s</B>.",
  state.allOn ? "All On" : "All Off",
  state.theatreOn ? "Theatre On" : "Theatre Off",
  state.gameroomOn ? "Gameroom On" : "Gameroom Off",
  state.tworooms ? "Separate Rooms" : "One Room");
  msg[strlen(msg)] = ' ';
  
  redirectToRoot();
}

void redirectToRoot() {
  server.sendHeader(F("Location"), F("/"));

  server.send(303, F("text/plain"), F("redirect")); // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleFavicon() {
  server.send_P(200, (PGM_P) F("image/x-icon"), (PGM_P) favicon, favicon_len);  
}
