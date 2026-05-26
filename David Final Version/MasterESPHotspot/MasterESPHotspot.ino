// bridge_esp32.ino
#include <WiFi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <esp_wifi.h>

// ─── Config ───────────────────────────────────────────────
const char* AP_SSID = "Automation Robot Master";
const char* AP_PASS = "robot2026";

uint8_t masterMAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };

// All 12 possible arm MACs (indices 0-11 = pairs 1-12)
uint8_t armMACs[12][6] = {
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x01},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x02},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x03},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x04},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x05},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x06},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x07},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x08},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x09},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x0A},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x0B},
  {0xDE,0xAD,0xAA,0xAA,0xAA,0x0C},
};

const int TOTAL_ARMS = 10; // How many arms are active
// ──────────────────────────────────────────────────────────

WebServer server(80);

void sendToArm(int index, const char* cmd) {
  esp_now_send(armMACs[index], (uint8_t*)cmd, strlen(cmd));
  Serial.printf("[Bridge] Arm %d → %s\n", index + 1, cmd);
}

void sendToAll(const char* cmd) {
  for (int i = 0; i < TOTAL_ARMS; i++) {
    sendToArm(i, cmd);
    delay(10); // Small gap to avoid ESP-NOW collisions
  }
  Serial.printf("[Bridge] All arms → %s\n", cmd);
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
    padding: 20px 16px 80px;
    gap: 18px;
  }

  /* ── Header ── */
  .header {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 4px;
  }
  h1 { font-size: 18px; font-weight: 500; color: #9b97f0; letter-spacing: 0.04em; }
  .mode-label {
    font-size: 11px;
    font-weight: 500;
    letter-spacing: 0.08em;
    text-transform: uppercase;
    padding: 3px 10px;
    border-radius: 20px;
    transition: background 0.2s, color 0.2s;
  }
  .mode-label.master { background: #2a1f5a; color: #9b97f0; }
  .mode-label.single { background: #0d2e24; color: #1d9e75; }

  /* ── Mode toggle ── */
  .mode-toggle {
    display: flex;
    background: #1a1920;
    border: 1px solid #2e2d38;
    border-radius: 12px;
    padding: 3px;
    gap: 3px;
    width: 100%;
    max-width: 360px;
  }
  .mode-toggle button {
    flex: 1;
    padding: 9px;
    border: none;
    border-radius: 9px;
    font-size: 13px;
    font-weight: 500;
    cursor: pointer;
    background: transparent;
    color: #6b6964;
    transition: background 0.15s, color 0.15s;
    -webkit-tap-highlight-color: transparent;
  }
  .mode-toggle button.active {
    background: #252340;
    color: #9b97f0;
  }
  .mode-toggle button.active.green {
    background: #0f2820;
    color: #1d9e75;
  }

  /* ── Arm selector ── */
  .arm-selector {
    display: grid;
    grid-template-columns: repeat(5, 1fr);
    gap: 8px;
    width: 100%;
    max-width: 360px;
  }
  .arm-btn {
    aspect-ratio: 1;
    border: 1px solid #2e2d38;
    border-radius: 10px;
    background: #1e1d24;
    color: #6b6964;
    font-size: 14px;
    font-weight: 500;
    cursor: pointer;
    transition: background 0.12s, color 0.12s, border-color 0.12s, transform 0.08s;
    -webkit-tap-highlight-color: transparent;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .arm-btn:active { transform: scale(0.92); }
  .arm-btn.selected {
    background: #0f2820;
    color: #1d9e75;
    border-color: #1d9e75;
  }

  /* ── Command grid ── */
  .cmd-grid {
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
  .btn:active { transform: scale(0.95); }
  .btn .icon { font-size: 26px; }
  .btn.action { border-color: #3d3789; }
  .btn.action:active, .btn.action:hover { background: #252340; }
  .btn.open  { border-color: #0d6e56; }
  .btn.open:active, .btn.open:hover  { background: #0f2820; }
  .btn.close { border-color: #7c2e1a; }
  .btn.close:active, .btn.close:hover { background: #2a1208; }
  .btn:disabled {
    opacity: 0.3;
    cursor: not-allowed;
    transform: none;
  }

  .gripper-row {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 12px;
    width: 100%;
    max-width: 360px;
  }
  .divider {
    width: 100%; max-width: 360px;
    border: none;
    border-top: 1px solid #1e1d24;
  }

  /* ── Target banner ── */
  .target-banner {
    width: 100%;
    max-width: 360px;
    background: #1a1920;
    border: 1px solid #2e2d38;
    border-radius: 10px;
    padding: 10px 14px;
    font-size: 13px;
    color: #6b6964;
    text-align: center;
    transition: background 0.2s, border-color 0.2s, color 0.2s;
  }
  .target-banner.master { background: #1c1a30; border-color: #3d3789; color: #9b97f0; }
  .target-banner.single { background: #0d2218; border-color: #0d6e56; color: #1d9e75; }

  /* ── Status ── */
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

  /* ── Toast ── */
  .toast {
    position: fixed;
    bottom: 28px; left: 50%;
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
  .toast.show { opacity: 1; transform: translateX(-50%) translateY(0); }

  /* ── Hidden ── */
  .hidden { display: none !important; }
</style>
</head>
<body>

<div class="header">
  <h1>Robot Arms</h1>
  <span class="mode-label master" id="modeLabel">Master</span>
</div>

<!-- Mode toggle -->
<div class="mode-toggle">
  <button id="btnMaster" class="active" onclick="setMode('master')">
    🎛 Master — all TOTAL_ARMS# arms
  </button>
  <button id="btnSingle" onclick="setMode('single')">
    🦾 Single arm
  </button>
</div>

<!-- Arm selector (single mode only) -->
<div class="arm-selector hidden" id="armSelector">
  <!-- Populated by JS -->
</div>

<!-- Target banner -->
<div class="target-banner master" id="targetBanner">
  Sending to all TOTAL_ARMS# arms simultaneously
</div>

<!-- Action buttons -->
<div class="cmd-grid">
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

<div class="status"><span class="dot"></span> Bridge connected</div>
<div class="toast" id="toast"></div>

<script>
const TOTAL = TOTAL_ARMS#;
let mode = 'master';       // 'master' | 'single'
let selectedArm = null;    // 1-based arm number

// Build arm selector buttons
const sel = document.getElementById('armSelector');
for (let i = 1; i <= TOTAL; i++) {
  const b = document.createElement('button');
  b.className = 'arm-btn';
  b.textContent = i;
  b.id = 'arm' + i;
  b.onclick = () => selectArm(i);
  sel.appendChild(b);
}

function setMode(m) {
  mode = m;
  const isMaster = m === 'master';

  document.getElementById('btnMaster').className = isMaster ? 'active' : '';
  document.getElementById('btnSingle').className = isMaster ? '' : 'active green';
  document.getElementById('armSelector').classList.toggle('hidden', isMaster);
  document.getElementById('modeLabel').className = 'mode-label ' + m;
  document.getElementById('modeLabel').textContent = isMaster ? 'Master' : 'Single';

  updateBanner();

  if (isMaster) {
    selectedArm = null;
    document.querySelectorAll('.arm-btn').forEach(b => b.classList.remove('selected'));
  }
  setButtonsDisabled(m === 'single' && selectedArm === null);
}

function selectArm(n) {
  selectedArm = n;
  document.querySelectorAll('.arm-btn').forEach(b => b.classList.remove('selected'));
  document.getElementById('arm' + n).classList.add('selected');
  updateBanner();
  setButtonsDisabled(false);
}

function updateBanner() {
  const banner = document.getElementById('targetBanner');
  if (mode === 'master') {
    banner.className = 'target-banner master';
    banner.textContent = 'Sending to all ' + TOTAL + ' arms simultaneously';
  } else if (selectedArm !== null) {
    banner.className = 'target-banner single';
    banner.textContent = 'Controlling arm ' + selectedArm;
  } else {
    banner.className = 'target-banner';
    banner.textContent = 'Select an arm above';
  }
}

function setButtonsDisabled(disabled) {
  document.querySelectorAll('.btn').forEach(b => b.disabled = disabled);
}

async function send(cmd) {
  const target = mode === 'master' ? 'all' : String(selectedArm);
  try {
    const r = await fetch('/cmd?c=' + cmd + '&t=' + target);
    const t = await r.text();
    const label = mode === 'master' ? 'All arms' : 'Arm ' + selectedArm;
    showToast(t === 'ok' ? label + ': ' + cmd : 'Error');
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

// Start with buttons enabled in master mode
setButtonsDisabled(false);
</script>
</body>
</html>
)rawhtml";

// ─── HTTP handlers ─────────────────────────────────────────
void handleRoot() {
  String html = INDEX_HTML;
  html.replace("TOTAL_ARMS#", String(TOTAL_ARMS));
  server.send(200, "text/html", html);
}

void handleCmd() {
  if (!server.hasArg("c") || !server.hasArg("t")) {
    server.send(400, "text/plain", "missing"); return;
  }

  String cmd    = server.arg("c");
  String target = server.arg("t"); // "all" or "1"-"12"

  // Allowlist commands
  const char* allowed[] = {
    "action1","action2","action3","action4",
    "opengripper","closegripper"
  };
  bool valid = false;
  for (auto a : allowed) if (cmd == a) { valid = true; break; }
  if (!valid) { server.send(403, "text/plain", "denied"); return; }

  if (target == "all") {
    sendToAll(cmd.c_str());
  } else {
    int armNum = target.toInt();
    if (armNum < 1 || armNum > TOTAL_ARMS) {
      server.send(400, "text/plain", "invalid arm"); return;
    }
    sendToArm(armNum - 1, cmd.c_str()); // Convert to 0-based index
  }

  server.send(200, "text/plain", "ok");
}

// ─── Setup ─────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(100);

  // 1. Set mode first and let it fully settle
  WiFi.mode(WIFI_AP_STA);
  delay(200);

  // 2. Spoof MAC on the STA interface this time, not AP
  esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, masterMAC);
  Serial.printf("MAC set result: %s\n", err == ESP_OK ? "OK" : esp_err_to_name(err));
  delay(100);

  // 3. Start AP (uses AP interface, STA interface carries the spoofed MAC)
  WiFi.softAP(AP_SSID, AP_PASS, 1);
  Serial.print("AP MAC:  "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  Serial.print("IP:      "); Serial.println(WiFi.softAPIP());
  delay(100);

  // 4. Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed"); return;
  }

  // 5. Register peers on STA interface
  for (int i = 0; i < TOTAL_ARMS; i++) {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, armMACs[i], 6);
    peer.channel = 1;
    peer.ifidx = WIFI_IF_STA; // STA carries the spoofed MAC
    peer.encrypt = false;
    if (esp_now_add_peer(&peer) == ESP_OK) {
      Serial.printf("Peer %d registered\n", i + 1);
    } else {
      Serial.printf("Peer %d failed\n", i + 1);
    }
  }

  server.on("/",    handleRoot);
  server.on("/cmd", handleCmd);
  server.begin();
  Serial.println("Ready — open http://192.168.4.1");
}

void loop() {
  server.handleClient();
}