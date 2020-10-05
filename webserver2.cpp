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
void ws2Favicon();


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
  server2.on("/favicon.ico", ws2Favicon);
  server2.on("favicon.ico", ws2Favicon);

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
  pagep = strcat(pagep, F("<html><body>Hello, World! "));
  ltoa(millis(), pagep, 10);
  pagep = strcat(pagep, F("</body></html>"));
  
  server2.send(200, "text/html", page); // Send HTTP status 200 (Ok) and send some text to the browser/client

  memset(msg, ' ', 120);
  msg[0] = '\0';
}

void ws2Favicon() {
  server2.send_P(200, (PGM_P) F("image/x-icon"), (PGM_P) favicon, favicon_len);  
}
