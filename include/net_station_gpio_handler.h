#pragma once
#ifndef _STATION_GPIO_HANDLER
#define _STATION_GPIO_HANDLER

#include <esp_err.h>

/// --- Function Definitions ---

void blink();
esp_err_t gpio_setup();

#endif
