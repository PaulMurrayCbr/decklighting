#ifndef DECKLIGHTING_WEBSERVER2_H
#define DECKLIGHTING_WEBSERVER2_H

#include <ESP8266WebServer.h>   // Include the WebServer library
extern ESP8266WebServer server2; // Create a webserver object that listens for HTTP request on port 80

void webserver2_setup();
void webserver2_loop();

#endif
