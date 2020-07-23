#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>  //6.15.1
#include "index.h"

char* ssid = "Home Automation";
char* password = "PASSWORD";

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

int Load1 = 04;
int Load2 = 14;
int Load3 = 12;
int Load4 = 13;

bool Load1status = LOW;
bool Load2status = LOW;
bool Load3status = LOW;
bool Load4status = LOW;

void setup() {
  pinMode(Load1, OUTPUT);
  pinMode(Load2, OUTPUT);
  pinMode(Load3, OUTPUT);
  pinMode(Load4, OUTPUT);

  digitalWrite(Load1, LOW);
  digitalWrite(Load2, LOW);
  digitalWrite(Load3, LOW);
  digitalWrite(Load4, LOW);
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.print("SoftAP IP:   ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", webpage);
  });
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("GPIO04 Status:" + StringStatus(Load1status) + " | GPIO14 Status: " + StringStatus(Load1status) + " | GPIO12 Status: " + StringStatus(Load1status) + " | GPIO13 Status: " + StringStatus(Load1status) + "");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load1on", HTTP_GET, [](AsyncWebServerRequest * request) {
    Load1status = HIGH;
    updateRelays();
    Serial.println("GPIO04 Status: ON");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load2on", HTTP_GET, [](AsyncWebServerRequest * request) {
    Load2status = HIGH;
    updateRelays();
    Serial.println("GPIO14 Status: ON");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load3on", HTTP_GET, [](AsyncWebServerRequest * request) {
    Load3status = HIGH;
    updateRelays();
    Serial.println("GPIO12 Status: ON");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load4on", HTTP_GET, [](AsyncWebServerRequest * request) {
    Load4status = HIGH;
    updateRelays();
    Serial.println("GPIO13 Status: ON");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load1off", HTTP_GET, [](AsyncWebServerRequest * request) {
    Load1status = LOW;
    updateRelays();
    Serial.println("GPIO04 Status: OFF");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load2off", HTTP_GET, [](AsyncWebServerRequest * request) {
    Load2status = LOW;
    updateRelays();
    Serial.println("GPIO14 Status: OFF");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load3off", HTTP_GET, [](AsyncWebServerRequest * request) {
    Load3status = LOW;
    updateRelays();
    Serial.println("GPIO12 Status: OFF");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load4off", HTTP_GET, [](AsyncWebServerRequest * request) {
    Load4status = LOW;
    updateRelays();
    Serial.println("GPIO13 Status: OFF");
    request->send(200, "application/json", SendJSON());
  });
  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(404, "application/json", "{\"status\":\"failure\",\"message\":\"404 Error. Route not found\"}");
  });


  server.begin();  // Start server
  webSocket.begin(); //start websocket
  webSocket.onEvent(webSocketEvent);
  Serial.println("HTTP server started");
}

void loop() {
  webSocket.loop();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        String JSON_Data =SendJSON();
        webSocket.sendTXT(num, JSON_Data);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String message = String((char*)( payload));
      Serial.println(message);

      DynamicJsonDocument doc(200);  // deserialize the data
      DeserializationError error = deserializeJson(doc, message); // parse the parameters we expect to receive (TO-DO: error handling)
      // Test if parsing succeeds.
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      String load1val = doc["Load1"];
      String load2val = doc["Load2"];
      String load3val = doc["Load3"];
      String load4val = doc["Load4"];
      Load1status = BoolStatus(load1val);
      Load2status = BoolStatus(load2val);
      Load3status = BoolStatus(load3val);
      Load4status = BoolStatus(load4val);
      updateRelays();
      Serial.println("GPIO04 Status: " + load1val + " GPIO14 Status: " + load2val + " GPIO12 Status: " + load3val + " GPIO13 Status: " + load4val);
      String JSON_Data =SendJSON();
      webSocket.broadcastTXT(JSON_Data);
  }
}

String SendJSON() {
  String ptr = "{ \"status\": \"success\",\"Load1\": \"";
  ptr += StringStatus(Load1status);
  ptr +=  "\",\"Load2\": \"";
  ptr +=  StringStatus(Load2status);
  ptr +=  "\", \"Load3\": \"";
  ptr +=  StringStatus(Load3status);
  ptr +=  "\",\"Load4\": \"";
  ptr +=  StringStatus(Load4status);
  ptr +=  "\"}";
  return ptr;
}

String StringStatus(uint8_t Status) {
  return (Status ? "HIGH" : "LOW");
}

bool BoolStatus(String argument) {
  return (argument == "HIGH" ? HIGH : LOW);
}

void updateRelays() {
  digitalWrite(Load1, Load1status);
  digitalWrite(Load2, Load2status);
  digitalWrite(Load3, Load3status);
  digitalWrite(Load4, Load4status);
}
