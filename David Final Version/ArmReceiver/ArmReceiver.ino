#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define LED_PIN 2

// ===============================
// Pair Configuration & MAC Filters
// ===============================
const uint8_t PAIR_NUMBER = 10; // Change 1-12 to match your pairing setup

// This arm's Custom Spoofed MAC
uint8_t armMAC[] = {
  0xDE, 0xAD, 0xAA, 0xAA, 0xAA, PAIR_NUMBER
};

// Corresponding paired controller MAC
uint8_t senderMacAddress[] = {
  0xDE, 0xAD, 0xCC, 0xCC, 0xCC, PAIR_NUMBER
};

// Master controller MAC
uint8_t masterMacAddress[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE
};

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVO_FREQ 50

// PCA9685 channels
const byte SERVO_CH[6] = {0, 1, 2, 3, 4, 5};

// Servo indexes
#define J1 0
#define J2 1
#define J3 2
#define J4 3
#define J5 4
#define J6 5

// Safe pulse-width ranges in microseconds
const int SERVO_MIN_US[6] = { 800, 900, 900, 500, 500, 500 };
const int SERVO_MAX_US[6] = { 2300, 2200, 2100, 2500, 2500, 2500 };

// Current stored servo positions
int currentUs[6] = { 1500, 1500, 1500, 1500, 1500, 1500 };

// Gripper positions
const int GRIPPER_OPEN_US  = 2300;
const int GRIPPER_CLOSE_US = 1500;

// Smooth movement settings
const int DEFAULT_STEP_US = 10;
const int DEFAULT_DELAY_MS = 15;

// ESP-NOW message buffers
volatile bool actionBusy = false;
volatile bool gripperBusy = false;
volatile bool newActionMsg = false;
volatile bool newGripperMsg = false;

char actionMsg[251] = {0};
char gripperMsg[251] = {0};

// -------------------------------------------------
// Basic servo functions
// -------------------------------------------------

void writeServoUs(int servoIndex, int microseconds) {
  microseconds = constrain(microseconds, SERVO_MIN_US[servoIndex], SERVO_MAX_US[servoIndex]);
  pwm.writeMicroseconds(SERVO_CH[servoIndex], microseconds);
  currentUs[servoIndex] = microseconds;
}

void moveServoSmooth(int servoIndex, int targetUs, int stepUs = DEFAULT_STEP_US, int delayMs = DEFAULT_DELAY_MS) {
  targetUs = constrain(targetUs, SERVO_MIN_US[servoIndex], SERVO_MAX_US[servoIndex]);
  int startUs = currentUs[servoIndex];

  if (startUs < targetUs) {
    for (int pos = startUs; pos <= targetUs; pos += stepUs) {
      writeServoUs(servoIndex, pos);
      delay(delayMs);
    }
  } else {
    for (int pos = startUs; pos >= targetUs; pos -= stepUs) {
      writeServoUs(servoIndex, pos);
      delay(delayMs);
    }
  }
  writeServoUs(servoIndex, targetUs);
}

void homeAllServos() {
  Serial.println("Moving all servos to home position");
  moveServoSmooth(J1, 1500);
  moveServoSmooth(J2, 1500);
  moveServoSmooth(J3, 1500);
  moveServoSmooth(J4, 1500);
  moveServoSmooth(J5, 1500);
  moveServoSmooth(J6, 1500);
}

// -------------------------------------------------
// Gripper functions
// -------------------------------------------------

void opengripper() {
  Serial.println("Executing: opengripper");
  moveServoSmooth(J6, GRIPPER_OPEN_US, 20, 5);
}

void closegripper() {
  Serial.println("Executing: closegripper");
  moveServoSmooth(J6, GRIPPER_CLOSE_US, 20, 5);
}

// -------------------------------------------------
// Action Sequences
// -------------------------------------------------

void action1() {
  Serial.println("Executing action1: wave");
  homeAllServos();
  delay(200);
  opengripper();
  delay(200);

  moveServoSmooth(J3, 2000, 5, 5);
  moveServoSmooth(J2, 1800, 5, 5);
  moveServoSmooth(J1, 1700, 5, 5);
  delay(300);

  for (int i = 0; i < 2; i++) {
    moveServoSmooth(J5, 2200, 10, 5);
    moveServoSmooth(J5, 800, 10, 5);
  }

  moveServoSmooth(J5, 1500, 10, 5);
  moveServoSmooth(J3, 1500, 10, 5);
  moveServoSmooth(J2, 1500, 10, 5);
  closegripper();

  Serial.println("Action1 complete");
}

void action2() {
  Serial.println("Executing action2: smooth dance");
  homeAllServos();
  delay(300);

  moveServoSmooth(J1, 2000, 5, 5);
  moveServoSmooth(J2, 2000, 5, 5);
  moveServoSmooth(J3, 2000, 5, 5);
  moveServoSmooth(J4, 2000, 5, 5);

  for (int pos = 2100; pos >= 800; pos -= 20) {
    writeServoUs(J2, pos);
    writeServoUs(J3, pos);
    writeServoUs(J1, pos);
    opengripper();
    delay(20);
  }
    
  moveServoSmooth(J1, 1000, 5, 5);
  moveServoSmooth(J2, 1000, 5, 5);
  moveServoSmooth(J3, 1000, 5, 5);

  for (int pos = 800; pos >= 2100; pos -= 20) {
    writeServoUs(J2, pos);
    writeServoUs(J3, pos);
    writeServoUs(J1, pos);
    closegripper();
    delay(20);
  }

  homeAllServos();
  Serial.println("Action2 complete");
}

void action3() {
  Serial.println("Executing action3: twirl");
  homeAllServos();
  delay(300);

  moveServoSmooth(J1, 700, 5, 10);
  delay(200);
  moveServoSmooth(J1, 2300, 5, 10);
  delay(200);
  moveServoSmooth(J1, 1500, 5, 10);

  Serial.println("Action3 complete");
}

void action4() {
  Serial.println("Executing action4: crab pinching dance");
  homeAllServos();
  delay(300);

  moveServoSmooth(J2, 1500, 10, 5);
  moveServoSmooth(J3, 1500, 10, 5);
  moveServoSmooth(J4, 1500, 10, 5);
  moveServoSmooth(J5, 1500, 10, 5);
  opengripper();
  delay(300);

  for (int i = 0; i < 2; i++) {
    for (int pos = 1500; pos <= 2100; pos += 20) {
      writeServoUs(J2, pos);
      writeServoUs(J3, pos);
      writeServoUs(J1, pos);
      if (pos == 1700) closegripper();
      delay(20);
    }
    delay(200);

    for (int pos = 2100; pos >= 1000; pos -= 20) {
      writeServoUs(J2, pos);
      writeServoUs(J3, pos);
      writeServoUs(J1, pos);
      if (pos == 1700) opengripper();
      delay(20);
    }
    delay(200);

    for (int pos = 1000; pos <= 1500; pos += 20) {
      writeServoUs(J2, pos);
      delay(20);
    }
    delay(150);
  }

  opengripper();
  moveServoSmooth(J2, 1500, 10, 10);
  moveServoSmooth(J1, 1500, 10, 10);
  moveServoSmooth(J3, 1500, 10, 10);
  homeAllServos();

  Serial.println("Action4 complete");
}

// -------------------------------------------------
// ESP-NOW RTOS FreeRTOS Tasks
// -------------------------------------------------

void runActionTask(void* param) {
  char msg[251];
  strncpy(msg, (char*)param, sizeof(msg));
  msg[250] = '\0';

  if (strcmp(msg, "action1") == 0 || strcmp(msg, "master1") == 0) {
    action1();
  } 
  else if (strcmp(msg, "action2") == 0 || strcmp(msg, "master2") == 0) {
    action2();
  } 
  else if (strcmp(msg, "action3") == 0) {
    action3();
  } 
  else if (strcmp(msg, "action4") == 0) {
    action4();
  } 
  else {
    Serial.println("Unknown action command execution blocked");
  }

  actionBusy = false;
  vTaskDelete(NULL);
}

void runGripperTask(void* param) {
  char msg[251];
  strncpy(msg, (char*)param, sizeof(msg));
  msg[250] = '\0';

  if (strcmp(msg, "opengripper") == 0) {
    opengripper();
  } 
  else if (strcmp(msg, "closegripper") == 0) {
    closegripper();
  } 
  else {
    Serial.println("Unknown gripper command execution blocked");
  }

  gripperBusy = false;
  vTaskDelete(NULL);
}

// -------------------------------------------------
// ESP-NOW receive callback (with MAC Filtering)
// -------------------------------------------------

void onDataRecv(const esp_now_recv_info *recvInfo, const uint8_t *incomingData, int len) {
  bool validSender = false;

  // 1. Verify if the sender is the paired controller OR the master controller
  if (memcmp(recvInfo->src_addr, senderMacAddress, 6) == 0) {
    validSender = true;
    Serial.print("[ESP-NOW] Received from Paired Controller: ");
  } 
  else if (memcmp(recvInfo->src_addr, masterMacAddress, 6) == 0) {
    validSender = true;
    Serial.print("[ESP-NOW] Received from Master Controller: ");
  }

  // Reject packets from unapproved MAC addresses
  if (!validSender) {
    Serial.println("[ESP-NOW] Ignored command from unauthorized device.");
    return;
  }

  // Process data if verified
  int copyLen = len;
  if (copyLen > 250) copyLen = 250;

  char temp[251];
  memcpy(temp, incomingData, copyLen);
  temp[copyLen] = '\0';

  Serial.println(temp);
  digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink LED to acknowledge packet

  // 2. Classify data into Gripper or Base actions
  if (strcmp(temp, "opengripper") == 0 || strcmp(temp, "closegripper") == 0) {
    if (gripperBusy) {
      Serial.println("Gripper running an operation, command skipped.");
      return;
    }
    strncpy(gripperMsg, temp, sizeof(gripperMsg));
    gripperMsg[250] = '\0';
    newGripperMsg = true;
  } 
  else {
    if (actionBusy) {
      Serial.println("Arm busy executing a sequence, command skipped.");
      return;
    }
    strncpy(actionMsg, temp, sizeof(actionMsg));
    actionMsg[250] = '\0';
    newActionMsg = true;
  }
}

// -------------------------------------------------
// Setup
// -------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize PCA9685
  Wire.begin(21, 22);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(500);

  homeAllServos();

  // Initialize WiFi station
  WiFi.mode(WIFI_STA);
  delay(100);

  // Apply Custom Spoofed MAC Address
  esp_err_t macResult = esp_wifi_set_mac(WIFI_IF_STA, armMAC);
  if (macResult == ESP_OK) {
    Serial.println("Custom Hardware MAC applied successfully.");
  } else {
    Serial.println("Failed setting custom MAC address.");
  }

  // Diagnostic prints
  Serial.print("Pair Setup Identifier: ");
  Serial.println(PAIR_NUMBER);
  Serial.print("Assigned Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Expected Controller MAC: ");
  for (int i = 0; i < 6; i++) {
    if (senderMacAddress[i] < 16) Serial.print("0");
    Serial.print(senderMacAddress[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Initialize ESP-NOW protocol
  if (esp_now_init() != ESP_OK) {
    Serial.println("Critical Error: ESP-NOW failed to initialize.");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  Serial.println("Receiver setup complete. Awaiting signals...");
}

// -------------------------------------------------
// Main loop
// -------------------------------------------------

void loop() {
  if (newActionMsg) {
    newActionMsg = false;
    Serial.print("Spawning core task for action: ");
    Serial.println(actionMsg);

    actionBusy = true;
    xTaskCreate(runActionTask, "action_execution", 4096, (void*)actionMsg, 1, NULL);
  }

  if (newGripperMsg) {
    newGripperMsg = false;
    Serial.print("Spawning core task for gripper: ");
    Serial.println(gripperMsg);

    gripperBusy = true;
    xTaskCreate(runGripperTask, "gripper_execution", 2048, (void*)gripperMsg, 1, NULL);
  }

  delay(10);
}