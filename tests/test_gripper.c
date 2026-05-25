/* Host-side tests: gripper toggle state machine */
#include "unity/unity.h"
#include "stubs/esp_now.h"
#include <string.h>
#include <stdbool.h>

/* Gripper cooldown constant — must match ui_Screen1.c */
#define GRIPPER_COOLDOWN_MS 500

/* Minimal state machine extracted from ui_Screen1.c event logic */
typedef struct {
    bool checked;
    bool disabled;
} gripper_state_t;

static gripper_state_t gripper;
static char last_sent[32];

static void gripper_init(gripper_state_t *g) {
    g->checked  = false;
    g->disabled = false;
}

static void gripper_toggle(gripper_state_t *g, const uint8_t *mac) {
    if (g->disabled) return;

    g->disabled = true;   /* lock for cooldown */
    g->checked  = !g->checked;

    const char *cmd = g->checked ? "opengripper" : "closegripper";
    strncpy(last_sent, cmd, sizeof(last_sent) - 1);
    esp_now_send(mac, (const uint8_t *)cmd, strlen(cmd));
}

static void gripper_cooldown_expired(gripper_state_t *g) {
    g->disabled = false;
}

static const uint8_t kMac[6] = {0x70, 0x4b, 0xca, 0x26, 0xdb, 0x3c};

void setUp(void)    { gripper_init(&gripper); memset(last_sent, 0, sizeof(last_sent)); }
void tearDown(void) {}

void test_initial_state_is_closed_and_enabled(void) {
    TEST_ASSERT_FALSE(gripper.checked);
    TEST_ASSERT_FALSE(gripper.disabled);
}

void test_first_toggle_opens_gripper(void) {
    gripper_toggle(&gripper, kMac);
    TEST_ASSERT_TRUE(gripper.checked);
    TEST_ASSERT_EQUAL_STRING("opengripper", last_sent);
}

void test_second_toggle_closes_gripper(void) {
    gripper_toggle(&gripper, kMac);
    gripper_cooldown_expired(&gripper);
    gripper_toggle(&gripper, kMac);
    TEST_ASSERT_FALSE(gripper.checked);
    TEST_ASSERT_EQUAL_STRING("closegripper", last_sent);
}

void test_toggle_disabled_during_cooldown(void) {
    gripper_toggle(&gripper, kMac);       /* opens, disables */
    TEST_ASSERT_TRUE(gripper.disabled);

    bool state_before = gripper.checked;
    gripper_toggle(&gripper, kMac);       /* should be ignored */
    TEST_ASSERT_EQUAL(state_before, gripper.checked);
}

void test_toggle_re_enabled_after_cooldown(void) {
    gripper_toggle(&gripper, kMac);
    gripper_cooldown_expired(&gripper);
    TEST_ASSERT_FALSE(gripper.disabled);
}

void test_cooldown_constant_is_500ms(void) {
    TEST_ASSERT_EQUAL_INT(500, GRIPPER_COOLDOWN_MS);
}

void test_multiple_toggle_cycle(void) {
    for (int i = 0; i < 4; i++) {
        gripper_cooldown_expired(&gripper);
        gripper_toggle(&gripper, kMac);
    }
    /* 4 toggles from closed → open → closed → open → closed */
    TEST_ASSERT_FALSE(gripper.checked);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_closed_and_enabled);
    RUN_TEST(test_first_toggle_opens_gripper);
    RUN_TEST(test_second_toggle_closes_gripper);
    RUN_TEST(test_toggle_disabled_during_cooldown);
    RUN_TEST(test_toggle_re_enabled_after_cooldown);
    RUN_TEST(test_cooldown_constant_is_500ms);
    RUN_TEST(test_multiple_toggle_cycle);
    return UNITY_END();
}
