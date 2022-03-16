#include <station_gpio_handler.h>

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

  for (int i = 0; i < N_GPIOS_IN; ++i) gpio_reset_pin(GPIOS_IN[i]), gpio_set_direction(GPIOS_IN[i], GPIO_MODE_INPUT);

  for (int i = 0; i < N_GPIOS_OUT; ++i) gpio_reset_pin(GPIOS_OUT[i]), gpio_set_direction(GPIOS_OUT[i], GPIO_MODE_OUTPUT);

  return ESP_OK;
}