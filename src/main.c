#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <driver/gpio.h>
#include <esp_spiffs.h>
#include <nvs_flash.h>
#include <mdns.h>
#include <esp_netif.h>
#include <esp_err.h>
#include <esp_log.h>
#include <lwip/apps/netbiosns.h>

#include "net_rest_server.h"
#include "net_wifi.h"
#include "net_config.h"
#include "net_globals.h"
#include "net_station_gpio_handler.h"
#include "net_nvs.h"

static char *TAG = "[MAIN]";

/**
 * @brief Initialize mDNS (Multicast DNS) service.
 *
 * This function initializes mDNS and configures the hostname and instance name.
 * It also adds a service for the ESP32 Web Server with the specified service text data.
 * The service runs on port 80 using the "_http" and "_tcp" protocols.
 */
static void init_mdns()
{
    // Initialize mDNS
    ESP_ERROR_CHECK(mdns_init());

    // Set the hostname and instance name
    mdns_hostname_set(HOSTNAME);
    mdns_instance_name_set(MDNS_INSTANCE);

    // Define the service text data
    mdns_txt_item_t serviceTextData[] = {
        {"board", "esp32"},
        {"path", "/"}};

    // Add the ESP32 Web Server service
    mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTextData, sizeof(serviceTextData) / sizeof(serviceTextData[0]));
}

esp_err_t init_fs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = SITE_MOUNT_POINT,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialise SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ESP_OK;
}

/**
 * @brief Main application function.
 *
 * This function serves as the entry point of the application.
 * It initializes necessary components, such as NVS flash, GPIO, WiFi, mDNS, NetBIOS, file system, and the REST server.
 * Error checks are performed to ensure proper initialization.
 */
#ifndef CONFIG_IDF_TARGET_TEST
void app_main()
{
    ESP_LOGI(TAG, "Startup..");
    ESP_LOGI(TAG, "Free memory: %d bytes", esp_get_free_heap_size());

    // Initialize NVS flash
    esp_err_t ret = nvs_flash_init();

    // Handle NVS initialization errors
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // Erase NVS flash and retry initialization
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    // Setup GPIO
    gpio_setup();

    // Setup WiFi
    setup_wifi();

    // Initialize mDNS
    init_mdns();

    // Initialize NetBIOS
    netbiosns_init();
    netbiosns_set_name(HOSTNAME);

    // Initialize the file system
    ESP_ERROR_CHECK(init_fs());

    // Start the REST server
    ESP_ERROR_CHECK(start_rest_server(SITE_MOUNT_POINT));

    size_t restart_counter = nvs_fetch_int32_t("restart_counter");
    if (!restart_counter) restart_counter = 0;
    ESP_LOGI(TAG, "Restart counter = %" PRIu32 "\n", restart_counter);
    ++restart_counter;
    if (nvs_set_int32_t("restart_counter", restart_counter) != 0) {
        ESP_LOGE(TAG, "Failed to save restart_counter to nvs");
    }

    for (int i = 10; i >= 0; i--) {
        ESP_LOGI(TAG, "Restarting in %d seconds...", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Restarting now.");
    fflush(stdout);
    esp_restart();
}
#endif
