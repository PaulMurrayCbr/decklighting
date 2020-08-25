#include <Arduino.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Esp.h>

extern ESP8266WebServer server;

#include "decklighting.h"
#include "strip.h"

void redirectToRoot();

char encodehex(unsigned n) ;
int decodehex(char c) ;
void writeColor(RGB& color, char* p);
void strip_update();

void readEffectRoom();
extern RoomState *effectRoom;
extern char *effectroomstringwithquotes;
extern char *effectroomstringwithoutquotes;

char staticEffectContent[] =
  R"zzzz(<html><head>
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
  <script src="https://code.jquery.com/jquery-3.5.1.min.js" integrity="sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=" crossorigin="anonymous"></script>

  <script>
    var currentinterp = "VALUEVALUEVALUE_INTERP";
  
    $( function() {
      $("#" + currentinterp).prop("checked", true);
      
      $("#color1").change(function() {
        $("#colorform").submit();
      });
      $("#color2").change(function() {
        $("#colorform").submit();
      });
    } );
  </script>
  
  <style>
  @viewport { width: device-width; zoom: 1.0;} 
  .main { border: thin solid gray; margin: 2px; padding: .5em;} 
  </style>
  </head><body>
<p>012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789  </p>
 <p><B><a href="http://192.168.0.14/">Back</a></B></p>
 <div class="main">
 <form action="http://192.168.0.14/effect/static/setcolor">
  <input type="hidden" name="room" value="VALUEVALUEVALUE_ROOM1" >
  <input type="hidden" name="color1" value="#FFF0E0">
  <input type="hidden" name="color2" value="#FFF0E0">
  <input type="submit" value="ALL WHITE">
 </form>
 <form action="http://192.168.0.14/effect/static/setcolor" id="colorform">
  <input type="hidden" name="room" value="VALUEVALUEVALUE_ROOM2" >
  <div><label for="color1">Colour 1 <input type="color" id="color1" name="color1" value="VALUEVALUEVALUE_COLOR1"></label></div>
  <div><label for="color2">Colour 2 <input type="color" id="color2" name="color2" value="VALUEVALUEVALUE_COLOR2"></label></div>
  <div><input type="submit" value="Set"></div>
 </form>

  <form style="display: inline-block" action="http://192.168.0.14/effect/static/setinterpolation"><input type="hidden" name="interpolation" value="linear"><input type="submit" value="Linear"></form>
  <form style="display: inline-block" action="http://192.168.0.14/effect/static/setinterpolation"><input type="hidden" name="interpolation" value="hueup"><input type="submit" value="Hue Up"></form>
  <form style="display: inline-block" action="http://192.168.0.14/effect/static/setinterpolation"><input type="hidden" name="interpolation" value="huedown"><input type="submit" value="Hue Down"></form>
  <form style="display: inline-block" action="http://192.168.0.14/effect/static/setinterpolation"><input type="hidden" name="interpolation" value="huenear"><input type="submit" value="Hue Near"></form>
  <form style="display: inline-block" action="http://192.168.0.14/effect/static/setinterpolation"><input type="hidden" name="interpolation" value="huefar"><input type="submit" value="Hue Far"></form>

 </div>


</body></html>
)zzzz";

char *staticMsg;

char *static_room_1;
char *static_room_2;
char *static_color_1;
char *static_color_2;

void handleStaticSetcolor();
void handleStaticSetinterpolation();
void redirectToStaticEffect();

void  setupStaticEffect() {
  staticMsg = strstr(staticEffectContent, "0123456789");
  memset(staticMsg, ' ', 120);
  sprintf(staticMsg, "ok, ready");
  staticMsg[strlen(staticMsg)]=' ';

  static_room_1 = strstr(staticEffectContent, "VALUEVALUEVALUE_ROOM1")-1;
  static_room_2 = strstr(staticEffectContent, "VALUEVALUEVALUE_ROOM2")-1;
  memset(static_room_1, ' ', 23);
  memset(static_room_2, ' ', 23);

  static_color_1 = strstr(staticEffectContent, "VALUEVALUEVALUE_COLOR1")-1;
  static_color_2 = strstr(staticEffectContent, "VALUEVALUEVALUE_COLOR2")-1;
  memset(static_color_1, ' ', 24);
  memset(static_color_2, ' ', 24);
  memcpy(static_color_1, "\"#ABCDEF\"", 9);
  memcpy(static_color_2, "\"#ABCDEF\"", 9);
  static_color_1 += 2;
  static_color_2 += 2;

  server.on("/effect/static/setcolor", handleStaticSetcolor);
  server.on("/effect/static/setinterpolation", handleStaticSetinterpolation);
  
}


void handleStaticEffect() {
  readEffectRoom();

  effectRoom->effect = STATIC;
  strip_update();
  
  writeColor(effectRoom->color1, static_color_1);
  writeColor(effectRoom->color2, static_color_2);

  memset(static_room_1, ' ', 23);
  memset(static_room_2, ' ', 23);
  memcpy(static_room_1, effectroomstringwithquotes, strlen(effectroomstringwithquotes));
  memcpy(static_room_2, effectroomstringwithquotes, strlen(effectroomstringwithquotes));
  
  server.send(200, "text/html", staticEffectContent); // Send HTTP status 200 (Ok) and send some text to the browser/client
  memset(staticMsg, ' ', 120);

}

void redirectToStaticEffect() {
  char location[80];
  strcpy(location, "/effect/static?room=");
  strcat(location, effectroomstringwithoutquotes);
  
  server.sendHeader("Location", location);
  server.send(303, F("text/plain"), F("Ok")); // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleStaticSetcolor() {
  boolean update = false;
  for(int i = 0; i< server.args(); i++) {
    const char *arg = server.arg(i).c_str();
   const char *argName = server.argName(i).c_str();
    if(strcmp(argName, "color1")==0) {
//      Serial.println("settig color1 for");
//      Serial.println((long)effectRoom);
      arg++;
      effectRoom->color1.r = (decodehex(arg[0]) << 4) | decodehex(arg[1]);
      effectRoom->color1.g = (decodehex(arg[2]) << 4) | decodehex(arg[3]);
      effectRoom->color1.b = (decodehex(arg[4]) << 4) | decodehex(arg[5]);
      update = true;
    }
    if(strcmp(argName, "color2")==0) {
//      Serial.println("settig color2 for");
//      Serial.println((long)effectRoom);
      arg++;
      effectRoom->color2.r = (decodehex(arg[0]) << 4) | decodehex(arg[1]);
      effectRoom->color2.g = (decodehex(arg[2]) << 4) | decodehex(arg[3]);
      effectRoom->color2.b = (decodehex(arg[4]) << 4) | decodehex(arg[5]);
      update = true;
    }
  }  

  if(update) strip_update();  

  memset(staticMsg, ' ', 120);
  sprintf(staticMsg, "%s updated", effectroomstringwithoutquotes);
  staticMsg[strlen(staticMsg)] = ' ';
   
  redirectToStaticEffect();
  
}


void handleStaticSetinterpolation() {
  boolean update = false;
  for(int i = 0; i< server.args(); i++) {
    const char *arg = server.arg(i).c_str();
   const char *argName = server.argName(i).c_str();

    if(strcmp(argName, "interpolation")==0) {
      if(strcmp(arg, "linear")==0) {
        effectRoom->interpolation = LINEAR;
      }
      else if(strcmp(arg, "hueup")==0) {
        effectRoom->interpolation = HUEUP;
      }
      else if(strcmp(arg, "huedown")==0) {
        effectRoom->interpolation = HUEDOWN;
      }
      else if(strcmp(arg, "huenear")==0) {
        effectRoom->interpolation = HUENEAR;
      }
      else if(strcmp(arg, "huefar")==0) {
        effectRoom->interpolation = HUEFAR;
      }
      
      memset(staticMsg, ' ', 120);
      sprintf(staticMsg, "interpolation set to %s", arg);
      staticMsg[strlen(staticMsg)] = ' ';

      update = true;
    }
  }  

  if(update) strip_update();  

  redirectToStaticEffect();
  
}
