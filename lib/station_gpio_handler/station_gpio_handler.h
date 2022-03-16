#ifndef _STATION_GPIO_HANDLER
#define _STATION_GPIO_HANDLER

#include <freertos/FreeRTOS.h>
#include <esp_err.h>

#include <driver/gpio.h>
#include <config.h>
#include <globals.h>

void blink();
esp_err_t gpio_setup();

#endif