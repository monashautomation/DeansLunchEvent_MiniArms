/* Host-side tests: ESP-NOW command string protocol */
#include "unity/unity.h"
#include "stubs/esp_now.h"
#include <string.h>

/* Command strings mirrored from final/final.ino */
#define CMD_ACTION1      "action1"
#define CMD_ACTION2      "action2"
#define CMD_ACTION3      "action3"
#define CMD_ACTION4      "action4"
#define CMD_OPEN_GRIPPER  "opengripper"
#define CMD_CLOSE_GRIPPER "closegripper"

/* Receiver MAC from final.ino */
static const uint8_t kReceiverMac[6] = {0x70, 0x4b, 0xca, 0x26, 0xdb, 0x3c};

static void send_cmd(const char *cmd) {
    esp_now_send(kReceiverMac, (const uint8_t *)cmd, strlen(cmd));
}

void setUp(void) {
    memset(_last_send_data, 0, sizeof(_last_send_data));
    _last_send_len = 0;
}
void tearDown(void) {}

void test_action1_sends_correct_string(void) {
    send_cmd(CMD_ACTION1);
    TEST_ASSERT_EQUAL_STRING("action1", (char *)_last_send_data);
}

void test_action2_sends_correct_string(void) {
    send_cmd(CMD_ACTION2);
    TEST_ASSERT_EQUAL_STRING("action2", (char *)_last_send_data);
}

void test_action3_sends_correct_string(void) {
    send_cmd(CMD_ACTION3);
    TEST_ASSERT_EQUAL_STRING("action3", (char *)_last_send_data);
}

void test_action4_sends_correct_string(void) {
    send_cmd(CMD_ACTION4);
    TEST_ASSERT_EQUAL_STRING("action4", (char *)_last_send_data);
}

void test_open_gripper_sends_correct_string(void) {
    send_cmd(CMD_OPEN_GRIPPER);
    TEST_ASSERT_EQUAL_STRING("opengripper", (char *)_last_send_data);
}

void test_close_gripper_sends_correct_string(void) {
    send_cmd(CMD_CLOSE_GRIPPER);
    TEST_ASSERT_EQUAL_STRING("closegripper", (char *)_last_send_data);
}

void test_commands_fit_in_espnow_payload(void) {
    /* ESP-NOW max payload is 250 bytes */
    TEST_ASSERT_LESS_THAN(250, (int)strlen(CMD_ACTION1));
    TEST_ASSERT_LESS_THAN(250, (int)strlen(CMD_ACTION2));
    TEST_ASSERT_LESS_THAN(250, (int)strlen(CMD_ACTION3));
    TEST_ASSERT_LESS_THAN(250, (int)strlen(CMD_ACTION4));
    TEST_ASSERT_LESS_THAN(250, (int)strlen(CMD_OPEN_GRIPPER));
    TEST_ASSERT_LESS_THAN(250, (int)strlen(CMD_CLOSE_GRIPPER));
}

void test_send_targets_correct_mac(void) {
    send_cmd(CMD_ACTION1);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(kReceiverMac, _last_send_mac, 6);
}

void test_send_length_matches_string(void) {
    send_cmd(CMD_ACTION1);
    TEST_ASSERT_EQUAL_size_t(strlen(CMD_ACTION1), _last_send_len);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_action1_sends_correct_string);
    RUN_TEST(test_action2_sends_correct_string);
    RUN_TEST(test_action3_sends_correct_string);
    RUN_TEST(test_action4_sends_correct_string);
    RUN_TEST(test_open_gripper_sends_correct_string);
    RUN_TEST(test_close_gripper_sends_correct_string);
    RUN_TEST(test_commands_fit_in_espnow_payload);
    RUN_TEST(test_send_targets_correct_mac);
    RUN_TEST(test_send_length_matches_string);
    return UNITY_END();
}
