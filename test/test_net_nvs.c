#include <unity.h>
#include "esp_types.h"
#include "net_nvs.h"
#include "test_net.h"

void setUp(void) {
    // Set up for your test cases, if needed
}

void tearDown(void) {
    // Tear down after your test cases, if needed
}

void test_nvs_set_int32_t(void) {
    // Test for nvs_set_int32_t function
    uint8_t result = nvs_set_int32_t("example_num", 42); // You should modify this test
    TEST_ASSERT_EQUAL_UINT8(0, result); // You should modify this test
}
void test_nvs_fetch_int32_t(void) {
    // Test for nvs_fetch_int32_t function
    int32_t result = nvs_fetch_int32_t("example_num");
    TEST_ASSERT_EQUAL_INT(42, result); // You should modify this test
}

void test_nvs_set_string(void) {
    // Test for nvs_set_string function
    uint8_t result = nvs_set_string("example_str", "example_value"); // You should modify this test
    TEST_ASSERT_EQUAL_UINT8(0, result); // You should modify this test
}
void test_nvs_fetch_string(void) {
    // Test for nvs_fetch_string function
    char *result = nvs_fetch_string("example_str");
    TEST_ASSERT_EQUAL_STRING("example_value", result);
    if (result != NULL) {
        free(result); // Free allocated memory if the function returned a string
    }
}

void test_nvs_read_write_string(void) {
    uint8_t result_num = nvs_set_string("example_str", "example_value");
    TEST_ASSERT_EQUAL_UINT8(0, result_num);
    char *result = nvs_fetch_string("example_str");
    TEST_ASSERT_EQUAL_STRING("example_value", result);
}

