#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Esp.h>

#include "decklighting.h"
#include "webserver.h"
#include "passwords.h"
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

void setupStaticEffect();
void handleStaticEffect();

void setupAlternateEffect();
void handleAlternateEffect();

char *strcat(char *b, const __FlashStringHelper *P) {
  char *p = (char *)(void *)P;
  while (*b) b++;
  while (*b++ = pgm_read_byte_near(p++));
  return b - 1;
}

const char _SECTION1[] PROGMEM =
  R"zzzz(<html><head>
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
  <script src="https://code.jquery.com/jquery-3.5.1.min.js" integrity="sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=" crossorigin="anonymous"></script>

  <script>
    $( function() {
      $("#theatre-color1").change(function() {
        $("#theatrecolorform").submit();
      });
      $("#theatre-color2").change(function() {
        $("#theatrecolorform").submit();
      });
      $("#gameroom-color1").change(function() {
        $("#gameroomcolorform").submit();
      });
      $("#gameroom-color2").change(function() {
        $("#gameroomcolorform").submit();
      });
    } );
  </script>
  
  <style>
  @viewport { width: device-width; zoom: 1.0;} 
  .main { border: thin solid gray; margin: 2px; padding: .5em;} 
  </style>
  </head><body>
<p>)zzzz";

const char _SECTION2[] PROGMEM =
  R"zzzz(</p>
<div class="main">
 <form action="http://192.168.0.14/allonoff"><input type="submit" value="On/Off"></form>
 <form action="http://192.168.0.14/theatreonoff"><input type="submit" value="Theatre On/Off"></form>
 <form action="http://192.168.0.14/gameroomonoff"><input type="submit" value="Game Room On/Off"></form>
   <form action="http://192.168.0.14/tworoomsonoff"><input type="submit" value="One Room/Two Rooms"></form>
</div>)zzzz";

const char _SECTION3[] PROGMEM =
  R"zzzz(<div class="main">
 <form action="http://192.168.0.14/global">
  <div><label for="brightness">Brightness <input type="number" id="brightness" name="brightness" value=")zzzz";

const char _SECTION4[] PROGMEM =
  R"zzzz("></label><input type="submit" value="Set"></div>
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

const __FlashStringHelper *SECTION1 = (const __FlashStringHelper *)_SECTION1;
const __FlashStringHelper *SECTION2 = (const __FlashStringHelper *)_SECTION2;
const __FlashStringHelper *SECTION3 = (const __FlashStringHelper *)_SECTION3;
const __FlashStringHelper *SECTION4 = (const __FlashStringHelper *)_SECTION4;


char hex[] = "0123456789ABCDEF";

char encodehex(unsigned n) { return hex[n&15]; }
int decodehex(char c) { if(c>='a') c-='a'-'A';return  strchr(hex,c)-hex; }

const int jump[] = {1,2,3,4,5,7,11,13};

class RoomStateHandler {
  public:
  RoomState& rs;

  RoomStateHandler(RoomState& rs) : rs(rs) {}


  void writeState(const char *title, const char *key) {
    pagep = strcat(pagep, F("<div class=\"main\">"));
    pagep = strcat(pagep, title);
    pagep = strcat(pagep, F(" <form action=\"http://192.168.0.14/")); pagep = strcat(pagep, key); strcat(pagep, F("\">"));
    pagep = strcat(pagep, F("  <input type=\"hidden\" name=\"color1\" value=\"#FFB090\">"));
    pagep = strcat(pagep, F("  <input type=\"hidden\" name=\"color2\" value=\"#FFB090\">"));
    pagep = strcat(pagep, F("  <input type=\"submit\" value=\"ALL WHITE\">"));
    pagep = strcat(pagep, F(" </form>"));
  
    for(int i = 0; i<sizeof(jump)/sizeof(*jump);i++) {
      pagep = strcat(pagep, F(" <form action=\"http://192.168.0.14/")); 
      pagep = strcat(pagep, key); 
      pagep = strcat(pagep, F("\" style=\"display: inline-block;\"><input type=\"hidden\" name=\"density\" value=\""));
      boppage();
      itoa(jump[i], pagep, 10);
      boppage();
      pagep = strcat(pagep, F("\"><input type=\"submit\" value=\""));
      if(jump[i]==1) {
        pagep=strcat(pagep, "all on");
      }
      else {
        pagep = strcat(pagep, "1/");
        boppage();
        itoa(jump[i], pagep, 10);
        boppage();
      }
    pagep = strcat(pagep, F("\"></form>"));
    }
    
    pagep = strcat(pagep, F(" <form action=\"http://192.168.0.14/")); 
    pagep = strcat(pagep, key); 
    pagep = strcat(pagep, F("\" id=\"")); 
    pagep = strcat(pagep, key); 
    pagep = strcat(pagep, F("colorform\">"));
    pagep = strcat(pagep, F("  <div><label for=\""));
    pagep = strcat(pagep, key); 
    pagep = strcat(pagep, F("-color1\">Colour 1 <input type=\"color\" id=\"")); 
    pagep = strcat(pagep, key); 
    pagep = strcat(pagep, F("-color1\" name=\"color1\" value=\"#"));
    boppage();
    writeColor(rs.color1, pagep);
    pagep += 6;
    *pagep = '\0';
    pagep = strcat(pagep, F("\"></label></div>"));
    pagep = strcat(pagep, F("  <div><label for=\"")); 
    pagep = strcat(pagep, key); 
    pagep = strcat(pagep, F("-color2\">Colour 2 <input type=\"color\" id=\"")); 
    pagep = strcat(pagep, key); 
    pagep = strcat(pagep, F("-color2\" name=\"color2\" value=\"#"));
    boppage();
    writeColor(rs.color2, pagep);
    pagep += 6;
    *pagep = '\0';
    pagep = strcat(pagep, F("\"></label></div>"));
    pagep = strcat(pagep, F("  <div><input type=\"submit\" value=\"Set\"></div>"));
    pagep = strcat(pagep, F(" </form>"));
    pagep = strcat(pagep, F(" <form  style=\"display: inline-block;\" action=\"http://192.168.0.14/effect/static\"><input type=\"hidden\" name=\"room\" value=\"")); 
    pagep = strcat(pagep, key); 
    pagep = strcat(pagep, F("\"><input type=\"submit\" value=\"Static\"></form>"));
    pagep = strcat(pagep, F(" <form  style=\"display: inline-block;\" action=\"http://192.168.0.14/effect/alternate\"><input type=\"hidden\" name=\"room\" value=\"")); 
    pagep = strcat(pagep, key); 
    pagep = strcat(pagep, F("\"><input type=\"submit\" value=\"Alternating\"></form>"));
    pagep = strcat(pagep, F("</div>"));
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
      if(strcmp(argName, "density")==0) {
        rs.density = atoi(arg);
        if(rs.density<1) rs.density = 1;
        if(rs.density>10) rs.density = 10;
        update = true;
      }
    }

    if(update) strip_update();  
  }

} gameroom(state.gameroom), theatre(state.theatre);


void webserver_setup() {
  setupStaticEffect();
  setupAlternateEffect();
  
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

  server.on("/effect/static", handleStaticEffect);
  server.on("/effect/alternate", handleAlternateEffect);

  
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
  pagep = page;
  *pagep = '\0';
  pagep = strcat(pagep, SECTION1);
  pagep = strcat(pagep, msg);
  pagep = strcat(pagep, SECTION2);
  theatre.writeState("Theatre", "theatre");
  gameroom.writeState("Game Room", "gameroom");
  pagep = strcat(pagep, SECTION3);
  boppage();
  itoa(state.brightness, pagep, 10);
  boppage();
  pagep = strcat(pagep, SECTION4);
    
  server.send(200, "text/html", page); // Send HTTP status 200 (Ok) and send some text to the browser/client

  memset(msg, ' ', 120);
  msg[0] = '\0';
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
    sprintf(msg, "gameroom updated to %d/%d/%d - %d/%d/%d, sparse %d",
     state.gameroom.color1.r, state.gameroom.color1.g, state.gameroom.color1.b,
     state.gameroom.color2.r, state.gameroom.color2.g, state.gameroom.color2.b,
     state.gameroom.density
    );
    msg[strlen(msg)] = ' ';
    
    redirectToRoot();
}

void handleTheatre() {
    theatre.readHttp();
    memset(msg, ' ', 120);
    sprintf(msg, "theatre updated to %d/%d/%d - %d/%d/%d, sparse %d",
     state.theatre.color1.r, state.theatre.color1.g, state.theatre.color1.b,
     state.theatre.color2.r, state.theatre.color2.g, state.theatre.color2.b,
     state.theatre.density
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
  server.send(303, F("text/plain"), F("Ok")); // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleFavicon() {
  server.send_P(200, (PGM_P) F("image/x-icon"), (PGM_P) favicon, favicon_len);  
}

char *effectroomstringwithquotes = "\"theatre\"";
char *effectroomstringwithoutquotes = "theatre";
RoomState *effectRoom = &state.theatre;

void readEffectRoom() {
  for(int i = 0; i< server.args(); i++) {
    const char *arg = server.arg(i).c_str();
    const char *argName = server.argName(i).c_str();

    if((strcmp(argName, "room")==0) && (strcmp(arg, "theatre")==0)) {
      effectRoom = &state.theatre;

//      Serial.println("effectroom is now state.theatre");
//      Serial.println((long)effectRoom);
      
      effectroomstringwithoutquotes = "theatre";
      effectroomstringwithquotes = "\"theatre\"";
    }
    if((strcmp(argName, "room")==0) && (strcmp(arg, "gameroom")==0)) {
      effectRoom = &state.gameroom;
//      Serial.println("effectroom is now state.gameroom");
//      Serial.println((long)effectRoom);
      effectroomstringwithoutquotes = "gameroom";
      effectroomstringwithquotes = "\"gameroom\"";
    }
  }

}


void  setupAlternateEffect() {
}

void handleAlternateEffect() {
  readEffectRoom();
  
  effectRoom->effect = ALTERNATE;
  strip_update();
  memset(msg, ' ', 120);
  sprintf(msg,  "Alternate effect selected");
  msg[strlen(msg)] = ' ';
  
  redirectToRoot();

}
