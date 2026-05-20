#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

const int ledPin = 2;

// Replace with the sender's MAC address
uint8_t senderMacAddress[] = {0x1c, 0xdb, 0xd4, 0x45, 0x01, 0x50};

// Master controller MAC address
uint8_t masterMacAddress[] = {0x70, 0x4b, 0xca, 0x25, 0xf3, 0x3c};

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
  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());
  
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