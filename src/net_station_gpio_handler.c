#include <net_station_gpio_handler.h>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_err.h>

#include <driver/gpio.h>
#include <net_config.h>
#include <net_globals.h>

static char *GPIO_TAG = "gpio_handler";

void blink()
{
    gpio_set_level(2, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(2, 0);
}

esp_err_t gpio_setup()
{
    ESP_LOGI(GPIO_TAG, "Configuring pins for output.");

    gpio_reset_pin(2);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);

    for (int i = 0; i < N_GPIOS_IN; ++i)
        gpio_reset_pin(GPIOS_IN[i]), gpio_set_direction(GPIOS_IN[i], GPIO_MODE_INPUT);

    for (int i = 0; i < N_GPIOS_OUT; ++i)
        gpio_reset_pin(GPIOS_OUT[i]), gpio_set_direction(GPIOS_OUT[i], GPIO_MODE_OUTPUT);

    return ESP_OK;
}

/**
 * @brief Set the direction of the specified GPIO pin.
 *
 * This function allows you to configure the direction of a GPIO pin as either input or output.
 *
 * @param[in] io_num The GPIO pin number to change the direction.
 * @param[in] direction The desired direction for the GPIO pin ("in" for input, "out" for output).
 */
esp_err_t api_gpio_direction(int io_num, char *direction)
{
    if (strcmp(direction, "in") == 0)
    {
        // Set the GPIO pin as input
        return gpio_set_direction(io_num, GPIO_MODE_INPUT);
    }
    else if (strcmp(direction, "out") == 0)
    {
        // Set the GPIO pin as output
        return gpio_set_direction(io_num, GPIO_MODE_OUTPUT);
    }
    else return ESP_ERR_INVALID_ARG;
}
