#include "net_nvs.h"

int32_t nvs_fetch_int32_t(char *key)
{
    int32_t value = 0;
    nvs_handle_t nvs_handler;
    esp_err_t err = nvs_open("config", NVS_READONLY, &nvs_handler);

    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_fetch_int32_t", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return 0;
    }

    err = nvs_get_i32(nvs_handler, key, &value);

    if (err != ESP_OK)
    {
        if (err == ESP_ERR_NVS_NOT_FOUND)
        {
            ESP_LOGW("nvs_fetch_int32_t", "Value (%s) not yet initialized", key);
        }
        else
        {
            ESP_LOGE("nvs_fetch_int32_t", "Error (%s) reading the value size", esp_err_to_name(err));
        }
    }
    else
    {
        ESP_LOGI("nvs_fetch_int32_t", "Successfully pulled sizeof key (%s)", key);
    }

    nvs_close(nvs_handler);
    return value;
}

char *nvs_fetch_string(char *key)
{
    char *value_data = NULL;
    size_t string_size = 0;
    nvs_handle_t nvs_handler;
    esp_err_t err = nvs_open("config", NVS_READONLY, &nvs_handler);

    if (err != ESP_OK)
    {
        ESP_LOGE("fetch_val", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return NULL;
    }

    err = nvs_get_str(nvs_handler, key, NULL, &string_size);

    if (err == ESP_OK)
    {
        value_data = malloc(string_size);
        if (value_data)
        {
            err = nvs_get_str(nvs_handler, key, value_data, &string_size); // Get the string data
            if (err != ESP_OK)
            {
                ESP_LOGE("fetch_val", "Error (%s) reading the value for key (%s)", esp_err_to_name(err), key);
                free(value_data);
                value_data = NULL;
            }
        }
        else
        {
            ESP_LOGE("fetch_val", "Error allocating memory for value_data");
        }
    }
    else
    {
        ESP_LOGE("fetch_val", "Error (%s) getting the size for key (%s)", esp_err_to_name(err), key);
    }

    nvs_close(nvs_handler);
    return value_data;
};

uint8_t nvs_set_int32_t(char *key, size_t value)
{
    nvs_handle_t nvs_handler;
    esp_err_t err = nvs_open("config", NVS_READWRITE, &nvs_handler);

    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_int32_t", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return 1;
    }

    err = nvs_set_i32(nvs_handler, key, (int32_t)value); // Use nvs_set_i32 to set a size_t

    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_int32_t", "Error (%s) setting the key (%s)", esp_err_to_name(err), key);
        nvs_close(nvs_handler);
        return 1;
    }

    err = nvs_commit(nvs_handler);
    nvs_close(nvs_handler);

    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_int32_t", "Error (%s) committing changes to NVS", esp_err_to_name(err));
        return 1;
    }

    return 0;
}

uint8_t nvs_set_string(char *key, char *val)
{
    nvs_handle_t nvs_handler;
    esp_err_t err = nvs_open("config", NVS_READWRITE, &nvs_handler);

    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_string", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return 1;
    }

    err = nvs_set_str(nvs_handler, key, val);

    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_string", "Error (%s) setting the value for key (%s)", esp_err_to_name(err), key);
        nvs_close(nvs_handler);
        return 1;
    }

    err = nvs_commit(nvs_handler);
    nvs_close(nvs_handler);

    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_string", "Error (%s) committing changes to NVS", esp_err_to_name(err));
        return 1;
    }

    return 0;
}
