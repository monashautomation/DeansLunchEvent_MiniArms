#include "power_manager.h"

#include <Arduino.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/i2c.h"

using namespace esp_panel::board;

// GT911 is on I2C0 (SDA=GPIO8, SCL=GPIO9), address 0x5D (INT driven LOW during reset).
// Register 0x814E is the "point status" / ready flag — writing 0 releases the INT line.
static void gt911_clear_int() {
    uint8_t cmd[] = {0x81, 0x4E, 0x00};
    i2c_master_write_to_device(I2C_NUM_0, 0x5D, cmd, sizeof(cmd), pdMS_TO_TICKS(10));
}

// GPIO4 = TP_IRQ: open-drain, pulled HIGH at rest, GT911 asserts LOW on touch.
// The touch driver polls via I2C so this pin is safe to read directly.
static constexpr gpio_num_t kTouchIntPin = GPIO_NUM_4;

// How long a continuous touch must be held to trigger deep sleep.
static constexpr uint32_t kSleepHoldMs = 2000;

static Board *s_board        = nullptr;
static uint32_t s_grace_end  = 0; // millis() deadline before sleep is allowed

static bool touch_is_down() {
    return digitalRead((uint8_t)kTouchIntPin) == LOW;
}

[[noreturn]] static void enter_sleep() {
    auto bl = s_board->getBacklight();
    if (bl) bl->off();
    delay(100);

    // Hold the pullup active during deep sleep so the line stays HIGH
    // until GT911 asserts it LOW (touch event = wake trigger).
    rtc_gpio_pullup_en(kTouchIntPin);
    rtc_gpio_pulldown_dis(kTouchIntPin);
    esp_sleep_enable_ext0_wakeup(kTouchIntPin, 0); // 0 = wake on LOW

    Serial.println("Entering deep sleep. Touch screen to wake.");
    Serial.flush();
    esp_deep_sleep_start();
    __builtin_unreachable();
}

void power_manager_init(Board *board) {
    s_board = board;

    // board->begin() → TOUCH_PRE_BEGIN_FUNCTION already called gpio_reset_pin(GPIO_NUM_4),
    // which releases RTC domain hold. Enable pullup so the line reads HIGH when idle.
    gpio_pullup_en(kTouchIntPin);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
        Serial.println("Woke from deep sleep via touch.");

        // Drain GT911 after wake — same LVGL-blocked scenario applies here.
        bool pin_was_high = false;
        uint32_t stable_start = 0;
        uint32_t deadline = millis() + 4000;
        while (millis() < deadline) {
            if (touch_is_down()) {
                gt911_clear_int();
                pin_was_high = false;
                stable_start = 0;
            } else {
                if (!pin_was_high) {
                    pin_was_high = true;
                    stable_start = millis();
                } else if (millis() - stable_start >= 600) {
                    break;
                }
            }
            delay(10);
        }
    }

    // Grace period: don't allow sleep for the first 5 seconds after any boot,
    // so a floating or briefly-asserted INT line can't trigger sleep during init.
    s_grace_end = millis() + 5000;
}

static uint32_t s_hold_start = 0;
static bool     s_holding    = false;

void power_manager_sleep() {
    if (!s_board) return;
    // LVGL task is blocked here, so GT911 scans keep asserting INT LOW with no one to
    // read via I2C. Actively clear the GT911 ready flag each time INT is LOW so the
    // pin can return HIGH. Then wait for 600ms of stable HIGH before sleeping.
    delay(200);
    bool pin_was_high = false;
    uint32_t stable_start = 0;
    uint32_t deadline = millis() + 6000;
    while (millis() < deadline) {
        if (touch_is_down()) {
            gt911_clear_int();
            pin_was_high = false;
            stable_start = 0;
        } else {
            if (!pin_was_high) {
                pin_was_high = true;
                stable_start = millis();
            } else if (millis() - stable_start >= 600) {
                break;
            }
        }
        delay(10);
    }
    enter_sleep();
}

void power_manager_tick() {
    if (!s_board) return;

    bool down = touch_is_down();

    if (down && !s_holding) {
        s_hold_start = millis();
        s_holding    = true;
    } else if (!down) {
        s_holding = false;
    }

    if (s_holding && (millis() - s_hold_start >= kSleepHoldMs) && (millis() >= s_grace_end)) {
        enter_sleep();
    }
}
