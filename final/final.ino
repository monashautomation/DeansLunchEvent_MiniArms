#include <Arduino.h>
#include <esp_display_panel.hpp>

#include <lvgl.h>
#include "lvgl_v8_port.h"
#include "ui.h"

#include <esp_now.h>
#include <WiFi.h>

using namespace esp_panel::drivers;
using namespace esp_panel::board;

uint8_t broadcastAddress[] = {0x70, 0x4b, 0xca, 0x26, 0xdb, 0x3c};

/**
 * To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 */
 // #include <demos/lv_demos.h>
 // #include <examples/lv_examples.h>

void OnDataSent(const wifi_tx_info_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

extern "C" {
    
    void action1_event(lv_event_t * e)
    {
        Serial.println("Button Pressed! Sending data to other ESP...");
        const char * message = "action1";
        
        // NEW: Send the message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) message, strlen(message));
        
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }
    void action2_event(lv_event_t * e)
    {
        Serial.println("Button Pressed! Sending data to other ESP...");
        const char * message = "action2";
        
        // NEW: Send the message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) message, strlen(message));
        
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }

    void action3_event(lv_event_t * e)
    {
        
        Serial.println("Button Pressed! Sending data to other ESP...");
        const char * message = "action3";
        
        // NEW: Send the message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) message, strlen(message));
        
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }

    void action4_event(lv_event_t * e)
    {
        
        Serial.println("Button Pressed! Sending data to other ESP...");
        const char * message = "action4";
        
        // NEW: Send the message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) message, strlen(message));
        
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }

    void open_gripper(lv_event_t * e)
    {
        
        Serial.println("Button Pressed! Sending data to other ESP...");
        const char * message = "opengripper";
        
        // NEW: Send the message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) message, strlen(message));
        
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }

    void close_gripper(lv_event_t * e)
    {
        
        Serial.println("Button Pressed! Sending data to other ESP...");
        const char * message = "closegripper";
        
        // NEW: Send the message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) message, strlen(message));
        
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }
}

void setup()
{

    Serial.begin(115200);

    Serial.println("Initializing WiFi...");
    WiFi.mode(WIFI_STA);
    delay(100);
    Serial.print("Sender MAC Address: ");
    Serial.println(WiFi.macAddress());

    Serial.println("Initializing ESP-NOW...");
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
    Serial.println("ESP-NOW initialized");

    Serial.println("Initializing board");
    Board *board = new Board();
    board->init();

    #if LVGL_PORT_AVOID_TEARING_MODE
    auto lcd = board->getLCD();
    // When avoid tearing function is enabled, the frame buffer number should be set in the board driver
    lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
    auto lcd_bus = lcd->getBus();
    /**
     * As the anti-tearing feature typically consumes more PSRAM bandwidth, for the ESP32-S3, we need to utilize the
     * "bounce buffer" functionality to enhance the RGB data bandwidth.
     * This feature will consume `bounce_buffer_size * bytes_per_pixel * 2` of SRAM memory.
     */
    if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    }
#endif
#endif
    assert(board->begin());

    Serial.println("Initializing LVGL");
    lvgl_port_init(board->getLCD(), board->getTouch());

    Serial.println("Creating UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    if (lvgl_port_lock(-1)) {
        
        // 1. Initialize the SquareLine UI
        ui_init();

        /* Release the mutex */
        lvgl_port_unlock();
    }

    Serial.println("LVGL SquareLine UI started");

    // Enable backlight - board->begin() should handle this, but let's ensure it's on
    auto backlight = board->getBacklight();
    if (backlight != nullptr) {
        Serial.println("Enabling backlight...");
        backlight->on();
    } else {
        Serial.println("Warning: Backlight control not found");
    }
}

void loop()
{
    Serial.println("IDLE loop");
    delay(1000);
}
