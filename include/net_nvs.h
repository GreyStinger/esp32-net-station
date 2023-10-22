#pragma once
#ifndef NET_NVS
#define NET_NVS

/// --- Modules ---

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_types.h"
#include <string.h>

/// --- Function Definitions ---

/**
 * @brief Fetches a size_t value based on a key from ESP NVS.
 *
 * Returns the size_t value associated with the key, or 0 if it doesn't exist.
 *
 * @param key The key to fetch a value for.
 * @return size_t The value associated with the key, or 0 if it doesn't exist.
 */
int32_t nvs_fetch_int32_t(char *key);

/**
 * @brief Fetches a value based on a key from ESP NVS.
 *
 * Returns a pointer char allocated to memory. Remember to free
 * later if not being used.
 *
 * @param key The key to fetch a value for.
 * @return char* The value associated with the key, or NULL if it doesn't exist.
 */
char *nvs_fetch_string(char *key);

/**
 * @brief Sets a size_t value associated with a key in ESP NVS.
 *
 * This function sets the size_t value for a given key and also records
 * the size of the value in a corresponding {key}-size key-value pair
 * in the ESP NVS.
 *
 * @param key The key to set the value to.
 * @param value The size_t value to set.
 * @return uint8_t 0 if successful, non-zero if an error occurs.
 */
uint8_t nvs_set_int32_t(char *key, size_t value);

/**
 * @brief Set the val object based on the key.
 *
 * This function sets the string value for a given key and also records
 * the size of the string in a corresponding {key}-size key-value pair
 * in the ESP NVS.
 *
 * @param key The key to set the value to.
 * @param val The value to set.
 * @return uint8_t 0 if successful, non-zero if an error occurs.
 */
uint8_t nvs_set_string(char *key, char *val);

#endif
