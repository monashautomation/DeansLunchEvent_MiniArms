// bridge_esp32.ino
// Hosts a web UI on WiFi and bridges HTTP commands → ESP-NOW to the arm
// Set PAIR_NUMBER to match whichever arm you want to control

#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <esp_wifi.h>

// ─── Config ───────────────────────────────────────────────
const uint8_t PAIR_NUMBER = 10;          // Change to match the arm
const char*   SSID        = "MA Robot Master";  // Your WiFi network
const char*   PASSWORD    = "robot";

// Bridge spoofs the master MAC — already whitelisted in arm firmware
uint8_t masterMAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };

// Target arm MAC (pair-matched)
uint8_t armMAC[] = { 0xDE, 0xAD, 0xAA, 0xAA, 0xAA, PAIR_NUMBER };
// ──────────────────────────────────────────────────────────

WebServer server(80);

void sendCommand(const char* cmd) {
  esp_now_send(armMAC, (uint8_t*)cmd, strlen(cmd));
  Serial.printf("[Bridge] Sent: %s\n", cmd);
}

// ─── Web UI ───────────────────────────────────────────────
const char INDEX_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
<title>Arm Controller</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
    background: #0f0f11;
    color: #e8e6df;
    min-height: 100svh;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 24px 16px;
    gap: 20px;
  }
  h1 { font-size: 18px; font-weight: 500; color: #9b97f0; letter-spacing: 0.04em; }
  .subtitle { font-size: 12px; color: #6b6964; }
  .grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 12px;
    width: 100%;
    max-width: 360px;
  }
  .btn {
    background: #1e1d24;
    border: 1px solid #2e2d38;
    border-radius: 14px;
    color: #e8e6df;
    font-size: 15px;
    font-weight: 500;
    padding: 22px 12px;
    cursor: pointer;
    transition: background 0.12s, transform 0.08s, border-color 0.12s;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 6px;
    -webkit-tap-highlight-color: transparent;
    user-select: none;
  }
  .btn:active { transform: scale(0.95); background: #2a2836; }
  .btn .icon { font-size: 26px; }
  .btn.action { border-color: #3d3789; }
  .btn.action:hover { background: #252340; border-color: #534ab7; }
  .btn.open  { border-color: #0d6e56; }
  .btn.open:hover  { background: #0f2820; border-color: #1d9e75; }
  .btn.close { border-color: #7c2e1a; }
  .btn.close:hover { background: #2a1208; border-color: #d85a30; }
  .gripper-row {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 12px;
    width: 100%;
    max-width: 360px;
  }
  .divider {
    width: 100%;
    max-width: 360px;
    border: none;
    border-top: 1px solid #1e1d24;
  }
  .status {
    font-size: 12px;
    color: #4a4845;
    display: flex;
    align-items: center;
    gap: 6px;
  }
  .dot {
    width: 7px; height: 7px;
    border-radius: 50%;
    background: #1d9e75;
    box-shadow: 0 0 6px #1d9e75;
    animation: pulse 2s ease-in-out infinite;
  }
  @keyframes pulse { 0%,100%{opacity:1} 50%{opacity:0.4} }
  .toast {
    position: fixed;
    bottom: 28px;
    left: 50%;
    transform: translateX(-50%) translateY(20px);
    background: #1e1d24;
    border: 1px solid #2e2d38;
    border-radius: 20px;
    padding: 8px 18px;
    font-size: 13px;
    color: #9b97f0;
    opacity: 0;
    transition: opacity 0.2s, transform 0.2s;
    pointer-events: none;
    white-space: nowrap;
  }
  .toast.show {
    opacity: 1;
    transform: translateX(-50%) translateY(0);
  }
</style>
</head>
<body>
<h1>Robot Arm</h1>
<p class="subtitle">Pair #PAIR_NUM#</p>

<div class="grid">
  <button class="btn action" onclick="send('action1')">
    <span class="icon">👋</span>Wave
  </button>
  <button class="btn action" onclick="send('action2')">
    <span class="icon">💃</span>Dance
  </button>
  <button class="btn action" onclick="send('action3')">
    <span class="icon">🌀</span>Twirl
  </button>
  <button class="btn action" onclick="send('action4')">
    <span class="icon">🦀</span>Pinch
  </button>
</div>

<hr class="divider">

<div class="gripper-row">
  <button class="btn open" onclick="send('opengripper')">
    <span class="icon">✋</span>Open
  </button>
  <button class="btn close" onclick="send('closegripper')">
    <span class="icon">✊</span>Close
  </button>
</div>

<div class="status"><span class="dot"></span> Connected to bridge</div>
<div class="toast" id="toast"></div>

<script>
async function send(cmd) {
  try {
    const r = await fetch('/cmd?c=' + cmd);
    const t = await r.text();
    showToast(t === 'ok' ? 'Sent: ' + cmd : 'Error');
  } catch(e) {
    showToast('No response');
  }
}
function showToast(msg) {
  const el = document.getElementById('toast');
  el.textContent = msg;
  el.classList.add('show');
  clearTimeout(el._t);
  el._t = setTimeout(() => el.classList.remove('show'), 1800);
}
</script>
</body>
</html>
)rawhtml";

// ─── HTTP handlers ─────────────────────────────────────────
void handleRoot() {
  String html = INDEX_HTML;
  html.replace("PAIR_NUM#", String(PAIR_NUMBER));  // inject pair number
  server.send(200, "text/html", html);
}

void handleCmd() {
  if (!server.hasArg("c")) { server.send(400, "text/plain", "missing"); return; }
  String cmd = server.arg("c");

  // Allowlist — never relay arbitrary strings
  const char* allowed[] = {
    "action1","action2","action3","action4",
    "opengripper","closegripper"
  };
  bool valid = false;
  for (auto a : allowed) if (cmd == a) { valid = true; break; }

  if (!valid) { server.send(403, "text/plain", "denied"); return; }

  sendCommand(cmd.c_str());
  server.send(200, "text/plain", "ok");
}

// ─── Setup ─────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);   // STA for ESP-NOW channel, AP+STA serves the web UI on LAN
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print('.'); }
  Serial.println();
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  // Spoof master MAC so the arm accepts us
  esp_wifi_set_mac(WIFI_IF_STA, masterMAC);
  Serial.print("Bridge MAC set to: "); Serial.println(WiFi.macAddress());

  // ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed"); return;
  }

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, armMAC, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
  Serial.println("ESP-NOW ready");

  // Web server
  server.on("/",    handleRoot);
  server.on("/cmd", handleCmd);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}