#pragma once
#ifndef _STATION_CONFIG
#define _STATION_CONFIG

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_types.h"
#include <string.h>

// Hostname and MDNS instance name for local machines
#define MDNS_INSTANCE "Example MDNS Instance"
#define HOSTNAME "Example Local Hostname"

// Local wifi configurations
#define ESP_WIFI_SSID "Example SSID"
#define ESP_WIFI_PASSWORD "Example Password"
#define ESP_IP "192.168.1.60"
#define ESP_WEB_SOCKET_SERVER "192.168.1.37"

// Idk
#define N_GPIOS_IN 1
#define N_GPIOS_OUT 2

// heh
int GPIOS_IN[N_GPIOS_IN];
int GPIOS_OUT[N_GPIOS_OUT];

// Mount point in VFS
// Recommended to leave unless you know exactly what you are doing
#define SITE_MOUNT_POINT "/www"

#define PV_KEY "<key>"


/// --- Function Definitions ---

/**
 * @brief Fetches a value based on a key from ESP NVS.
 * 
 * Returns a pointer char allocated to memory. Remember to free
 * later if not being used.
 *
 * @param key The key to fetch a value for.
 * @return char* The value associated with the key, or NULL if it doesn't exist.
 */
char* fetch_val(char* key);

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
uint8_t nvs_set_string(char* key, char* val);

#endif