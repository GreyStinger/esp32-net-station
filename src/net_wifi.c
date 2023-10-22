#include "net_wifi.h"

#ifndef PIN2STR
#define PIN2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define PINSTR "%c%c%c%c%c%c%c%c"
#endif

// Static local variables
static const char* WIFI_TAG = "WIFI";

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

static wifi_config_t wps_ap_creds[MAX_WPS_AP_CRED];
static int s_ap_creds_num = 0;

static esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);

static void wps_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**
 * @brief WiFi event handler function.
 *
 * This function is called when WiFi-related events occur. It handles different events such as
 * WiFi station start, WiFi disconnection, and obtaining the IP address.
 *
 * @param[in] arg         Pointer to the user-defined argument.
 * @param[in] event_base  Event base type.
 * @param[in] event_id    Event ID.
 * @param[in] event_data  Pointer to the event data.
 */
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        // WiFi station started, initiate connection
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < ESP_MAXIMUM_RETRY)
        {
            // Retry the connection if the maximum retry limit is not reached
            esp_wifi_connect();
            ++s_retry_num;
            ESP_LOGW(WIFI_TAG, "Retrying the access point connection.");
        }
        else
        {
            // Set the WiFi fail bit if maximum retry limit is reached
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGW(WIFI_TAG, "AP Connection Failed");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        // IP address obtained
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(WIFI_TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


/**
 * @brief Set up the wifi controller
 * 
 * Public method to initialize the wifi controller
 */
void setup_wifi()
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // esp_netif_create_default_wifi_sta();
    // Block that turns off dhcp on the station and manually sets ip_info

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

    ESP_LOGI(WIFI_TAG, "wifi_init_sta finished");

    EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(WIFI_TAG, "Connected to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASSWORD);
        blink();
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGW(WIFI_TAG, "Failed to connect to SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASSWORD);
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");
    }

    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    // vEventGroupDelete(s_wifi_event_group);
}

static void wps_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(WIFI_TAG, "WIFI_EVENT_STA_START");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(WIFI_TAG, "WIFI_EVENT_STA_DISCONNECTED");
        esp_wifi_connect();
    case WIFI_EVENT_STA_WPS_ER_SUCCESS:
        ESP_LOGI(WIFI_TAG, "WIFI_EVENT_STA_WPS_ER_SUCCESS");
        {
            wifi_event_sta_wps_er_success_t *evt =
                (wifi_event_sta_wps_er_success_t *)event_data;
            int i;

            if (evt)
            {
                s_ap_creds_num = evt->ap_cred_cnt;
                for (i = 0; i < s_ap_creds_num; ++i)
                {
                    memcpy(wps_ap_creds[i].sta.ssid, evt->ap_cred[i].ssid,
                            sizeof(evt->ap_cred[i].passphrase));
                    memcpy(wps_ap_creds[i].sta.password, evt->ap_cred[i].passphrase,
                            sizeof(evt->ap_cred[i].passphrase));
                }
                // TODO: Add NVS storage add to save ssid and passwd from event
                ESP_LOGI(WIFI_TAG, "Connecting to SSID: %s, Passphrase: %s",
                        wps_ap_creds[0].sta.ssid, wps_ap_creds[0].sta.password);
                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wps_ap_creds[0]));
            }
        }
        break;
    case WIFI_EVENT_STA_WPS_ER_FAILED:
        ESP_LOGI(WIFI_TAG, "WIFI_EVENT_STA_WPS_ER_FAILED");
        ESP_ERROR_CHECK(esp_wifi_wps_disable());
        ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
        ESP_ERROR_CHECK(esp_wifi_wps_start(0));
        break;
    case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
        ESP_LOGI(WIFI_TAG, "WIFI_EVENT_STA_WPS_ER_TIMEOUT");
        ESP_ERROR_CHECK(esp_wifi_wps_disable());
        ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
        ESP_ERROR_CHECK(esp_wifi_wps_start(0));
        break;
    case WIFI_EVENT_STA_WPS_ER_PIN:
        ESP_LOGI(WIFI_TAG, "WIFI_EVENT_STA_WPS_ER_PIN");
        wifi_event_sta_wps_er_pin_t* event = (wifi_event_sta_wps_er_pin_t*) event_data;
        ESP_LOGI(WIFI_TAG, "WPS_PIN = " PINSTR, PIN2STR(event->pin_code));
        break;
    default:
        break;
    }
}

static void got_ip_event_handler(void* arg, esp_event_base_t event_base,
        int32_t event_id, void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(WIFI_TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
}

void start_wps(void)
{
    ESP_ERROR_CHECK(esp_netif_init);
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                &wps_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, 
                &got_ip_event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFI_TAG, "starting wps");

    ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
    ESP_ERROR_CHECK(esp_wifi_wps_start(0));
}

