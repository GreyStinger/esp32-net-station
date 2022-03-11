#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <driver/gpio.h>
#include <esp_spiffs.h>
#include <nvs_flash.h>
#include <mdns.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <esp_err.h>
#include <esp_log.h>
#include <lwip/apps/netbiosns.h>
#include <rest_server.h>
#include <config.h>


#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1


#define ESP_MAXIMUM_RETRY 10

static EventGroupHandle_t s_wifi_event_group;

static char *TAG = "station board";

static int s_retry_num = 0;

static void init_mdns()
{
  ESP_ERROR_CHECK(mdns_init());
  
  mdns_hostname_set(HOSTNAME);
  mdns_instance_name_set(MDNS_INSTANCE);

  mdns_txt_item_t serviceTextData[] = {
    {"board", "esp32"},
    {"path", "/"}
  };

  mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTextData, sizeof(serviceTextData) / sizeof(serviceTextData[0]));
}

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    if (s_retry_num < ESP_MAXIMUM_RETRY)
    {
      esp_wifi_connect();
      ++s_retry_num;
      ESP_LOGW(TAG, "Retrying the accesspoint connection.");
    }
    else
    {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGW(TAG, "AP Connection Failed");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void blink() 
{
  gpio_set_level(2, 1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  gpio_set_level(2, 0);
}

void gpio_setup()
{
  ESP_LOGI(TAG, "Configuring pin 2 for output.");
  gpio_reset_pin(2);

  gpio_set_direction(2, GPIO_MODE_OUTPUT);
}

esp_err_t init_fs(void)
{
  esp_vfs_spiffs_conf_t conf = {
    .base_path = SITE_MOUNT_POINT,
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = false
  };

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

void setup_wifi()
{
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  
  // esp_netif_create_default_wifi_sta();
  // Block that turns off dhcp on the station and manually sets ip_info
  esp_netif_t *sta = esp_netif_create_default_wifi_sta();
  esp_netif_dhcpc_stop(sta);
  esp_netif_ip_info_t ip_info;
  ip_info.ip.addr = ipaddr_addr("192.168.1.60");
  ip_info.gw.addr = ipaddr_addr("192.168.1.1");
  ip_info.netmask.addr = ipaddr_addr("255.255.255.0");
  esp_netif_set_ip_info(sta, &ip_info);

  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&config);

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
    WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
    IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

  wifi_config_t wifi_config = {
    .sta = {
      .ssid = ESP_WIFI_SSID,
      .password = ESP_WIFI_PASSWORD,
      .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "wifi_init_sta finished");
  
  EventBits_t bits = xEventGroupWaitBits(
    s_wifi_event_group,
    WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
    pdFALSE,
    pdFALSE,
    portMAX_DELAY
  );

  if (bits & WIFI_CONNECTED_BIT)
  {
    ESP_LOGI(TAG, "Connected to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASSWORD);
    blink();
  }
  else if (bits & WIFI_FAIL_BIT)
  {
    ESP_LOGW(TAG, "Failed to connect to SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASSWORD);
  }
  else
  {
    ESP_LOGE(TAG, "UNEXPECTED EVENT");
  }

  // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
  // vEventGroupDelete(s_wifi_event_group);
}

void app_main() 
{
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  gpio_setup();
  setup_wifi();
  init_mdns();
  netbiosns_init();
  netbiosns_set_name(HOSTNAME);

  ESP_ERROR_CHECK(init_fs());
  ESP_ERROR_CHECK(start_rest_server(SITE_MOUNT_POINT));
}