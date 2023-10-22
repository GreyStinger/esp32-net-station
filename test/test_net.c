#include "test_net.h"
#include <unity.h>

#ifdef CONFIG_IDF_TARGET_TEST
void app_main() {
    // Initialize Unity test framework
    nvs_flash_init();

    UNITY_BEGIN();

    // Run your test cases
    RUN_TEST(test_nvs_set_int32_t);
    RUN_TEST(test_nvs_fetch_int32_t);

    RUN_TEST(test_nvs_set_string);
    RUN_TEST(test_nvs_fetch_string);

    RUN_TEST(test_nvs_read_write_string);

    // Finish the test run
    UNITY_END();
}
#endif
