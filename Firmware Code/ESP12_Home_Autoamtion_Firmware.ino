#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

char* ssid = "Home Automation";
char* password = "PASSWORD";

/* Put IP Address details */
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

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

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", handle_OnConnect);
  server.on("/action", handle_action);
  server.on("/load1on", handle_load1on);
  server.on("/load2on", handle_load2on);
  server.on("/load3on", handle_load3on);
  server.on("/load4on", handle_load4on);
  server.on("/load1off", handle_load1off);
  server.on("/load2off", handle_load2off);
  server.on("/load3of", handle_load3off);
  server.on("/load4off", handle_load4off);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  digitalWrite(Load1, Load1status);
  digitalWrite(Load2, Load2status);
  digitalWrite(Load3, Load3status);
  digitalWrite(Load4, Load4status);
}

void handle_OnConnect() {
  //  Load1status = LOW;
  //  Load2status = LOW;
  //  Load3status = LOW;
  //  Load4status = LOW;
  Serial.println("GPIO04 Status: OFF | GPIO14 Status: OFF | GPIO12 Status: OFF | GPIO13 Status: OFF");
  server.send(200, "application/json", SendJSON());
}

void handle_action() {
  String load1val = server.arg("load1"); //this lets you access a query param (http://x.x.x.x/action1?load1=1)
  String load2val = server.arg("load2");
  String load3val = server.arg("load3");
  String load4val = server.arg("load4");
  Load1status = BoolStatus(load1val);
  Load2status = BoolStatus(load2val);
  Load3status = BoolStatus(load3val);
  Load4status = BoolStatus(load4val);
  Serial.println("GPIO04 Status: " + load1val + " GPIO14 Status: " + load2val + " GPIO12 Status: " + load3val + " GPIO13 Status: " + load4val);
  server.send(200, "application/json", SendJSON());
}

void handle_load1on() {
  Load1status = HIGH;
  Serial.println("GPIO04 Status: ON");
  server.send(200, "application/json", SendJSON());
}
void handle_load1off() {
  Load1status = LOW;
  Serial.println("GPIO04 Status: OFF");
  server.send(200, "application/json", SendJSON());
}
void handle_load2on() {
  Load2status = HIGH;
  Serial.println("GPIO14 Status: ON");
  server.send(200, "application/json", SendJSON());
}
void handle_load2off() {
  Load2status = LOW;
  Serial.println("GPIO14 Status: OFF");
  server.send(200, "application/json", SendJSON());
}
void handle_load3on() {
  Load3status = HIGH;
  Serial.println("GPIO12 Status: ON");
  server.send(200, "application/json", SendJSON());
}
void handle_load3off() {
  Load3status = LOW;
  Serial.println("GPIO12 Status: ON");
  server.send(200, "application/json", SendJSON());
}
void handle_load4on() {
  Load4status = HIGH;
  Serial.println("GPIO13 Status: ON");
  server.send(200, "application/json", SendJSON());
}
void handle_load4off() {
  Load4status = LOW;
  Serial.println("GPIO13 Status: OFF");
  server.send(200, "application/json", SendJSON());
}

void handle_NotFound() {
  server.send(404, "application/json", "{\"status\":\"failure\",\"message\":\"404 Error. Route not found\"}");
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
  bool Status = LOW;
  if (argument== "") {
    Status = LOW;
  } else {
    Status = argument== "HIGH"?HIGH:LOW;
  }
  return Status;
}
