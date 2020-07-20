#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

char* ssid = "Home Automation";
char* password = "PASSWORD";

/* Put IP Address details */
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

int Load1 = 04;
int Load2 = 14;
int Load3 = 12;
int Load4 = 13;

bool Load1status = LOW;
bool Load2status = LOW;
bool Load3status = LOW;
bool Load4status = LOW;

char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Home Automation</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Home Automation</h2>
  <h4>Device 1</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(1, load1Status)" id="device1"  \>
    <span class="slider"></span>
  </label>
  <h4>Device 2</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(2, load2Status)" id="device2"  \>
    <span class="slider"></span>
  </label>
  <h4>Device 3</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(3, load3Status)" id="device3"  \>
    <span class="slider"></span>
  </label>
  <h4>Device 4</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(4, load4Status)" id="device4"  \>
    <span class="slider"></span>
  </label>

<script>
  var device1 = document.getElementById("device1");
  var device2 = document.getElementById("device2");
  var device3 = document.getElementById("device3");
  var device4 = document.getElementById("device4");
  
  var load1Status, load2Status, load3Status, load4Status = null;
  
  
  function toggleCheckbox(device, status) {
    var xhr = new XMLHttpRequest();
    var url = `/load${device}${status?'off':'on'}`;
    console.log(url);
    xhr.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var res = JSON.parse(this.responseText);
        updateComp(res);
      }
    };
    xhr.open("GET", url, true);
    xhr.send();
  };
  
  function updateComp(res) {
    load1Status = res.Load1=='HIGH'?true:false;
    load2Status = res.Load2=='HIGH'?true:false;
    load3Status = res.Load3=='HIGH'?true:false;
    load4Status = res.Load4=='HIGH'?true:false;
    device1.checked = load1Status;
    device2.checked = load2Status;
    device3.checked = load3Status;
    device4.checked = load4Status;
  }
  
  document.addEventListener("DOMContentLoaded", () => {
    var xhr = new XMLHttpRequest();
    var url = "/status";
    xhr.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var res = JSON.parse(this.responseText);
        updateComp(res);
      }
    };
    xhr.open("GET", url, true);
    xhr.send();
  });
</script>
</body>
</html>
)=====";

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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", webpage);
  });
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("GPIO04 Status:" +StringStatus(Load1status) +" | GPIO14 Status: " +StringStatus(Load1status) +" | GPIO12 Status: " +StringStatus(Load1status) +" | GPIO13 Status: " +StringStatus(Load1status) +"");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/action", HTTP_GET, [](AsyncWebServerRequest *request){
    String load1val = StringStatus(Load1status);
    String load2val = StringStatus(Load2status);
    String load3val = StringStatus(Load3status);
    String load4val = StringStatus(Load4status);

    const char* PARAM_LOAD1 = "load1";
    const char* PARAM_LOAD2 = "load2";
    const char* PARAM_LOAD3 = "load3";
    const char* PARAM_LOAD4 = "load4";
    if(request->hasParam(PARAM_LOAD1))
      String load1val = request->getParam(PARAM_LOAD1)->value(); //this lets you access a query param (http://x.x.x.x/action1?load1=1)
    if(request->hasParam(PARAM_LOAD2))
      String load2val = request->getParam(PARAM_LOAD2)->value();
    if(request->hasParam(PARAM_LOAD3))
      String load3val = request->getParam(PARAM_LOAD3)->value();
    if(request->hasParam(PARAM_LOAD4))
      String load4val =request->getParam(PARAM_LOAD4)->value();
    
    Load1status = BoolStatus(load1val);
    Load2status = BoolStatus(load2val);
    Load3status = BoolStatus(load3val);
    Load4status = BoolStatus(load4val);
    Serial.println("GPIO04 Status: " + load1val + " GPIO14 Status: " + load2val + " GPIO12 Status: " + load3val + " GPIO13 Status: " + load4val);
   request->send(200, "application/json", SendJSON());
  });
  server.on("/load1on", HTTP_GET, [](AsyncWebServerRequest *request){
    Load1status = HIGH;
    Serial.println("GPIO04 Status: ON");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load2on", HTTP_GET, [](AsyncWebServerRequest *request){
    Load2status = HIGH;
    Serial.println("GPIO14 Status: ON");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load3on", HTTP_GET, [](AsyncWebServerRequest *request){
    Load3status = HIGH;
    Serial.println("GPIO12 Status: ON");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load4on", HTTP_GET, [](AsyncWebServerRequest *request){
    Load4status = HIGH;
    Serial.println("GPIO13 Status: ON");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load1off", HTTP_GET, [](AsyncWebServerRequest *request){
    Load1status = LOW;
    Serial.println("GPIO04 Status: OFF");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load2off", HTTP_GET, [](AsyncWebServerRequest *request){
    Load2status = LOW;
    Serial.println("GPIO14 Status: OFF");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load3off", HTTP_GET, [](AsyncWebServerRequest *request){
    Load3status = LOW;
    Serial.println("GPIO12 Status: OFF");
    request->send(200, "application/json", SendJSON());
  });
  server.on("/load4off", HTTP_GET, [](AsyncWebServerRequest *request){
    Load4status = LOW;
    Serial.println("GPIO13 Status: OFF");
    request->send(200, "application/json", SendJSON());
  });
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "application/json", "{\"status\":\"failure\",\"message\":\"404 Error. Route not found\"}");
  });

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  digitalWrite(Load1, Load1status);
  digitalWrite(Load2, Load2status);
  digitalWrite(Load3, Load3status);
  digitalWrite(Load4, Load4status);
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
  if (argument== "")
    Status = LOW;
  else
    Status = argument== "HIGH"?HIGH:LOW;
    
  return Status;
}
