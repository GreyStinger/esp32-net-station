#pragma once
#ifndef GREYS_ESP_BOOT_WIFI
#define GREYS_ESP_BOOT_WIFI

/// --- Modules ---

#include <esp_types.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_event_base.h>
#include <esp_wifi.h>
#include <esp_wps.h>
#include <esp_err.h>
#include <esp_log.h>

#include "net_station_gpio_handler.h"
#include "net_globals.h"
#include "net_config.h"

/// --- Constants ---

#define ESP_MAXIMUM_RETRY 10

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/// -- Function Definitions ---

void setup_wifi();

#endif
