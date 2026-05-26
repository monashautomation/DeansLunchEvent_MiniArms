#include "power_manager.h"

#include <Arduino.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

using namespace esp_panel::board;

// GPIO4 = TP_IRQ: open-drain, pulled HIGH at rest, GT911 asserts LOW on touch.
// The touch driver polls via I2C so this pin is safe to read directly.
static constexpr gpio_num_t kTouchIntPin = GPIO_NUM_4;

// How long a continuous touch must be held to trigger deep sleep.
static constexpr uint32_t kSleepHoldMs = 2000;

static Board *s_board = nullptr;

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

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
        Serial.println("Woke from deep sleep via touch.");

        // Wait for the wakeup finger to lift before starting the hold timer,
        // otherwise the same touch immediately re-triggers sleep.
        uint32_t deadline = millis() + 3000;
        while (touch_is_down() && millis() < deadline) {
            delay(20);
        }
        delay(200);
    }
}

static uint32_t s_hold_start = 0;
static bool     s_holding    = false;

void power_manager_tick() {
    if (!s_board) return;

    bool down = touch_is_down();

    if (down && !s_holding) {
        s_hold_start = millis();
        s_holding    = true;
    } else if (!down) {
        s_holding = false;
    }

    if (s_holding && (millis() - s_hold_start >= kSleepHoldMs)) {
        enter_sleep();
    }
}
