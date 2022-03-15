#ifndef _ESP_REST_SERVER
#define _ESP_REST_SERVER

#include <string.h>
#include <fcntl.h>
#include <driver/gpio.h>
#include <esp_http_server.h>
#include <esp_system.h>
#include <esp_random.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_vfs.h>
#include <cJSON.h>
#include <query_handler.h>

#define REST_CHECK(a, str, goto_tag, ...)                                      \
  do                                                                           \
  {                                                                            \
    if (!(a))                                                                  \
    {                                                                          \
      ESP_LOGE(REST_TAG, "%s(%d)" str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
      goto goto_tag;                                                           \
    }                                                                          \
  } while (0)

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFFSIZE (10240)

typedef struct rest_server_context {
  char base_path[ESP_VFS_PATH_MAX + 1];
  char scratch[SCRATCH_BUFFSIZE];
} rest_server_context_t;

esp_err_t start_rest_server(const char *base_path);

#endif