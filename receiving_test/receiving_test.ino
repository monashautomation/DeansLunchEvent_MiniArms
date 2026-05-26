#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

const int ledPin = 2;

// ===============================
// Pair Configuration
// ===============================
const uint8_t PAIR_NUMBER = 1; // Change 1-12

// This arm's MAC
uint8_t armMAC[] = {
  0xDE, 0xAD, 0xAA, 0xAA, 0xAA, PAIR_NUMBER
};

// Corresponding controller MAC
uint8_t senderMacAddress[] = {
  0xDE, 0xAD, 0xCC, 0xCC, 0xCC, PAIR_NUMBER
};

// Master controller MAC
uint8_t masterMacAddress[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE
};

void OnDataRecv(const esp_now_recv_info *recvInfo, const uint8_t *incomingData, int len) {
  // Check if message is from the expected sender
  if (memcmp(recvInfo->src_addr, senderMacAddress, 6) == 0) {
    char message[32];
    memcpy(message, incomingData, len);
    message[len] = '\0';
    
    Serial.print("Received from ESP-NOW: ");
    Serial.println(message);
    
    if (strcmp(message, "action1") == 0) {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED ON");
    } else if (strcmp(message, "action2") == 0) {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF");
    }
  }
  
  // Check if message is from the master controller
  if (memcmp(recvInfo->src_addr, masterMacAddress, 6) == 0) {
    char message[32];
    memcpy(message, incomingData, len);
    message[len] = '\0';
    
    Serial.print("Received from Master: ");
    Serial.println(message);
    
    if (strcmp(message, "master1") == 0) {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED ON (Master)");
    } else if (strcmp(message, "master2") == 0) {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF (Master)");
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  WiFi.mode(WIFI_STA);
  delay(100);

  // Apply this arm's MAC
  esp_err_t macResult = esp_wifi_set_mac(WIFI_IF_STA, armMAC);

  if (macResult == ESP_OK) {
    Serial.println("Custom MAC set successfully");
  } else {
    Serial.println("Failed to set MAC");
  }

  // Print pair info
  Serial.print("Pair Number: ");
  Serial.println(PAIR_NUMBER);

  // Print this arm MAC
  Serial.print("Arm MAC: ");
  Serial.println(WiFi.macAddress());

  // Print expected controller MAC
  Serial.print("Expected Controller MAC: ");
  for (int i = 0; i < 6; i++) {
    if (senderMacAddress[i] < 16) Serial.print("0");
    Serial.print(senderMacAddress[i], HEX);

    if (i < 5) Serial.print(":");
  }
  Serial.println();
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Receiver ready");
}

void loop() {
  // ESP-NOW runs via callbacks, no loop logic needed
  delay(10);
}