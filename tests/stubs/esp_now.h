#pragma once
#include <stdint.h>
#include <string.h>

typedef enum { ESP_NOW_SEND_SUCCESS = 0, ESP_NOW_SEND_FAIL } esp_now_send_status_t;
typedef int esp_err_t;
#define ESP_OK 0

typedef struct { uint8_t src_addr[6]; } esp_now_recv_info;
typedef struct { uint8_t peer_addr[6]; int channel; int encrypt; } esp_now_peer_info_t;

static uint8_t  _last_send_data[256];
static size_t   _last_send_len  = 0;
static uint8_t  _last_send_mac[6];

static inline esp_err_t esp_now_init(void) { return ESP_OK; }
static inline esp_err_t esp_now_add_peer(esp_now_peer_info_t *p) { (void)p; return ESP_OK; }
static inline esp_err_t esp_now_register_send_cb(void *cb) { (void)cb; return ESP_OK; }
static inline esp_err_t esp_now_register_recv_cb(void *cb) { (void)cb; return ESP_OK; }

static inline esp_err_t esp_now_send(const uint8_t *mac, const uint8_t *data, size_t len) {
    memcpy(_last_send_mac, mac, 6);
    memcpy(_last_send_data, data, len);
    _last_send_len = len;
    return ESP_OK;
}
