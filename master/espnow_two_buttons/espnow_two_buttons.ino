/*
 * ESP32-S3 ESPNOW Two-Button Controller
 * Press Button 1 to send "CMD_1" to all nearby ESP32s
 * Press Button 2 to send "CMD_2" to all nearby ESP32s
 */

#include <esp_now.h>
#include <WiFi.h>

// ============================================
// CONFIGURATION
// ============================================

// Button GPIO pins (change these to match your wiring)
#define BUTTON_1_PIN    0    // Boot button by default on ESP32-S3-DevKit
#define BUTTON_2_PIN    47   // Change to your desired pin

// ESPNOW Channel (must match on all devices)
#define ESPNOW_CHANNEL  1

// Debounce delay in milliseconds
#define DEBOUNCE_DELAY  50

// ============================================
// ESPNOW STRUCTURES
// ============================================

// Command structure (9 bytes max for ESPNOW)
typedef struct {
  char command[8];       // Command string (7 chars + null)
  uint8_t senderMac[6];  // Sender's MAC address
} espnow_message_t;

// ============================================
// GLOBAL VARIABLES
// ============================================

// Button state tracking
volatile bool button1Pressed = false;
volatile bool button2Pressed = false;
unsigned long button1LastPress = 0;
unsigned long button2LastPress = 0;

// LED pin for status feedback
#define LED_PIN 2

// ============================================
// ESPNOW CALLBACKS
// ============================================

// Callback when ESPNOW data is sent
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Last Packet - Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  
  // LED feedback
  digitalWrite(LED_PIN, status == ESP_NOW_SEND_SUCCESS ? HIGH : LOW);
}

// ============================================
// ESPNOW FUNCTIONS
// ============================================

// Initialize ESPNOW
void initESPNOW() {
  WiFi.mode(WIFI_STA);
  
  // Get and print MAC address
  Serial.print("My MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  // Initialize ESPNOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESPNOW");
    return;
  }
  
  // Register send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Set broadcast peer (no encryption)
  esp_now_peer_info_t broadcastPeer;
  memset(&broadcastPeer, 0, sizeof(broadcastPeer));
  for (int i = 0; i < 6; i++) {
    broadcastPeer.peer_addr[i] = 0xFF;  // Broadcast address
  }
  broadcastPeer.channel = ESPNOW_CHANNEL;
  broadcastPeer.encrypt = false;
  
  // Add broadcast peer
  if (esp_now_add_peer(&broadcastPeer) != ESP_OK) {
    Serial.println("Failed to add broadcast peer");
    return;
  }
  
  Serial.println("ESPNOW Initialized - Broadcasting to all devices");
}

// Send command via ESPNOW
void sendCommand(const char* command) {
  espnow_message_t message;
  
  // Fill message
  strncpy(message.command, command, sizeof(message.command) - 1);
  message.command[sizeof(message.command) - 1] = '\0';
  
  // Copy sender MAC
  uint8_t mac[6];
  WiFi.macAddress(mac);
  memcpy(message.senderMac, mac, 6);
  
  // Send to broadcast address
  const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&message, sizeof(message));
  
  Serial.print("Sending '");
  Serial.print(command);
  Serial.print("' - Result: ");
  Serial.println(result == ESP_OK ? "SUCCESS" : "FAILED");
}

// ============================================
// BUTTON HANDLING
// ============================================

// Check buttons (call this in loop)
void checkButtons() {
  unsigned long now = millis();
  
  // Button 1 with debounce
  bool btn1State = digitalRead(BUTTON_1_PIN);
  if (btn1State == LOW && !button1Pressed && (now - button1LastPress > DEBOUNCE_DELAY)) {
    button1Pressed = true;
    button1LastPress = now;
    Serial.println("Button 1 PRESSED - Sending master1");
    sendCommand("master1");
    digitalWrite(LED_PIN, HIGH);  // LED on when button pressed
  } else if (btn1State == HIGH && button1Pressed) {
    button1Pressed = false;
    digitalWrite(LED_PIN, LOW);
  }
  
  // Button 2 with debounce
  bool btn2State = digitalRead(BUTTON_2_PIN);
  if (btn2State == LOW && !button2Pressed && (now - button2LastPress > DEBOUNCE_DELAY)) {
    button2Pressed = true;
    button2LastPress = now;
    Serial.println("Button 2 PRESSED - Sending master2");
    sendCommand("master2");
    digitalWrite(LED_PIN, HIGH);
  } else if (btn2State == HIGH && button2Pressed) {
    button2Pressed = false;
    digitalWrite(LED_PIN, LOW);
  }
}

// ============================================
// SETUP & LOOP
// ============================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n===================================");
  Serial.println("ESP32-S3 ESPNOW Two-Button Controller");
  Serial.println("===================================");
  
  // Configure button pins with internal pull-up
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  
  // Configure LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize ESPNOW
  initESPNOW();
  
  Serial.println("\nReady! Press buttons to send commands.");
  Serial.println("Button 1 -> master1");
  Serial.println("Button 2 -> master2");
}

void loop() {
  checkButtons();
  
  // Small delay to prevent watchdog issues
  delay(10);
}