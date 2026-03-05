#include <DFRobot_HuskylensV2.h>
#include <ProtocolV2.h>
#include <Result.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <SinricPro.h>
#include <SinricProLight.h>
#include <WiFiManager.h> 

// --- CONFIGURATION ---
const char* kobraIP = "192.168.1.242"; 
const char* discordWebhook = "https://discord.com/api/webhooks/1479219312039428288/ahZOZb1Df_rS0cwJ7JH7R1vzYAL8wTj41eI_MnASYtwyz9G1vS9IdaEre68-oe-K67Mg";

#define APP_KEY    "821a4e8a-737c-44fc-abab-81f3652de4c1"
#define APP_SECRET "88f75e44-3a0c-4ea9-a7c9-8f72f0d52f7f-bc3116f6-59ee-43ad-8b2a-52edfbeaa4b1"
#define LIGHT_ID   "69a8ce7217b32c0941cc8a84"

HuskylensV2 huskylens; 
WebServer server(80);
WiFiManager wm; 

int aiConfidence = 100;
int sensitivity = 50; 
String printerStatus = "IDLE";
bool isPrinting = false;
bool hasFailed = false;

// --- NOTIFICATION FUNCTIONS ---
void sendDiscordAlert(String status, int conf) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(discordWebhook);
    http.addHeader("Content-Type", "application/json");
    String color = (status == "FAILURE") ? "15158332" : "3066993"; 
    String msg = "{\"embeds\": [{\"title\": \"Kobra-Guard AI Alert\",\"description\": \"**Status:** " + status + "\\n**Confidence:** " + String(conf) + "%\",\"color\": " + color + "}]}";
    http.POST(msg);
    http.end();
  }
}

void sendStatusToGoogle(int mode) {
  SinricProLight &myLight = SinricPro[LIGHT_ID];
  myLight.sendPowerStateEvent(true); 
  if (mode == 1) myLight.sendBrightnessEvent(10);  
  else if (mode == 2) myLight.sendBrightnessEvent(50);  
  else if (mode == 3) myLight.sendBrightnessEvent(100); 
}

void stopPrinter() {
  HTTPClient http;
  http.begin("http://" + String(kobraIP) + "/api/v1/stop");
  http.POST(""); 
  http.end();
  sendDiscordAlert("EMERGENCY STOP TRIGGERED", aiConfidence);
}

// --- WEB DASHBOARD ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head><meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body { font-family: sans-serif; background: #121212; color: white; text-align: center; margin:0; }
  .card { background: #1e1e1e; border-radius: 15px; padding: 20px; margin: 15px; border: 1px solid #333; }
  #conf-bar { background: #0072ff; height: 20px; border-radius: 10px; width: 100%; transition: 0.5s; }
  .btn { border: none; padding: 15px 30px; border-radius: 10px; font-weight: bold; cursor: pointer; margin: 10px; width: 80%; }
  .stop-btn { background: #ff4b2b; color: white; }
  .reset-btn { background: #4CAF50; color: white; }
  input[type=range] { width: 80%; margin: 20px 0; }
</style></head><body>
  <div class="card"><h1>Kobra-Guard AI</h1><p>Status: <b id="p-status">IDLE</b></p></div>
  <div class="card"><h3>AI Confidence</h3><div id="conf-bar"></div><p id="conf-text">100%</p></div>
  <div class="card"><h3>Sensitivity</h3>
    <input type="range" min="10" max="90" value="50" onchange="updateSens(this.value)">
    <p>Alert at: <span id="sens-val">50</span>%</p>
  </div>
  <div class="card">
    <button class="btn reset-btn" onclick="fetch('/reset')">RESET CONFIDENCE</button>
    <button class="btn stop-btn" onclick="if(confirm('Stop Print?')) fetch('/stop')">STOP PRINT</button>
  </div>
<script>
  function updateSens(val) { document.getElementById('sens-val').innerHTML = val; fetch('/setSens?val=' + val); }
  setInterval(function(){
    fetch('/data').then(r=>r.json()).then(d=>{
      document.getElementById('conf-bar').style.width=d.conf+'%';
      document.getElementById('conf-text').innerHTML=d.conf+'%';
      document.getElementById('p-status').innerHTML=d.status;
    });
  }, 2000);
</script></body></html>)rawliteral";

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  if(!wm.autoConnect("PrintMind_Setup")) { ESP.restart(); }
  WiFi.mode(WIFI_STA); 
  
  Wire.begin();
  while (!huskylens.begin(Wire)) { delay(1000); }
  huskylens.switchAlgorithm(ALGORITHM_OBJECT_CLASSIFICATION);

  SinricPro.begin(APP_KEY, APP_SECRET);

  server.on("/", []() { server.send(200, "text/html", index_html); });
  server.on("/data", []() {
    String json = "{\"conf\":" + String(aiConfidence) + ",\"status\":\"" + printerStatus + "\"}";
    server.send(200, "application/json", json);
  });
  server.on("/setSens", []() {
    sensitivity = server.arg("val").toInt();
    server.send(200);
  });
  server.on("/reset", []() {
    aiConfidence = 100;
    hasFailed = false;
    server.send(200, "text/plain", "Reset Complete");
  });
  server.on("/stop", []() {
    stopPrinter();
    server.send(200, "text/plain", "Stopping...");
  });
  server.begin();
}

// --- LOOP ---
void loop() {
  SinricPro.handle();
  server.handleClient();

  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000) {
    checkPrinterStatus();
    lastCheck = millis();
  }

  if (isPrinting && huskylens.getResult(ALGORITHM_OBJECT_CLASSIFICATION)) {
    if (huskylens.available(ALGORITHM_OBJECT_CLASSIFICATION)) {
      auto result = huskylens.getCachedCenterResult(ALGORITHM_OBJECT_CLASSIFICATION);
      if (result->ID == 2) { 
        aiConfidence -= 5;
        if (aiConfidence < sensitivity && !hasFailed) {
          hasFailed = true;
          sendStatusToGoogle(3);
          sendDiscordAlert("FAILURE", aiConfidence);
        }
      } else if (result->ID == 1 && aiConfidence < 100) {
        aiConfidence++; 
      }
    }
  }
}

void checkPrinterStatus() {
  HTTPClient http;
  http.begin("http://" + String(kobraIP) + "/api/v1/status"); 
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (!isPrinting) {
      isPrinting = true; hasFailed = false; aiConfidence = 100;
      sendStatusToGoogle(1);
      sendDiscordAlert("PRINT STARTED", 100);
    }
    printerStatus = "PRINTING";
  } else {
    if (isPrinting && !hasFailed) {
      sendStatusToGoogle(2);
      sendDiscordAlert("SUCCESS", 100);
    }
    isPrinting = false; printerStatus = "IDLE";
  }
  http.end();
}