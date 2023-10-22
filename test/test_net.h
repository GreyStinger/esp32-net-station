#pragma once
#ifndef NET_TESTS
#define NET_TESTS

/// --- Function Definitions ---

void setUp(void);
void tearDown(void);
void test_nvs_fetch_int32_t(void);
void test_nvs_fetch_string(void);
void test_nvs_set_int32_t(void);
void test_nvs_set_string(void);
void test_nvs_read_write_string(void);

#endif