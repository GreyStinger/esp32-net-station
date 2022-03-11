#include <rest_server.h>

static const char *REST_TAG = "station-rest";
static uint8_t gpio_state = 0;
// static uint8_t gpio_num = 2;

static void handle_query(const char *uri)
{
  esp_query_map_t q_map[16];
  char current_query[255];
  char * query_index = strrchr(uri, '?');

  memcpy(current_query, query_index + 1, strlen(query_index) - 1);
  ESP_LOGI(REST_TAG, "Query Now: %s", current_query);
  
  query_index = strrchr(uri, '&');
  // int query_len;

  for (int i = 0; query_index; ++i)
  {
    query_index = strrchr(uri, '&');
  }
}

static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
  const char *type = "text/plain";

  if (CHECK_FILE_EXTENSION(filepath, ".html"))
  {
    type = "text/html";
  }
  else if (CHECK_FILE_EXTENSION(filepath, ".js")) 
  {
    type = "application/javascript";
  }
  else if (CHECK_FILE_EXTENSION(filepath, ".css"))
  {
    type = "text/css";
  }
  else if (CHECK_FILE_EXTENSION(filepath, ".png"))
  {
    type = "image/png";
  }
  else if (CHECK_FILE_EXTENSION(filepath, ".ico")) 
  {
    type = "image/x-icon";
  }
  else if (CHECK_FILE_EXTENSION(filepath, ".svg"))
  {
    type = "text/xml";
  }

  return httpd_resp_set_type(req, type);
}


static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
  ESP_LOGI(REST_TAG, "The full uri is: %s", req->uri);

  char filepath[FILE_PATH_MAX];

  rest_server_context_t *rest_context = (rest_server_context_t *) req->user_ctx;
  strlcpy(filepath, rest_context->base_path, sizeof(filepath));
  if (req->uri[strlen(req->uri) - 1] == '/')
  {
    strlcat(filepath, "/index.html", sizeof(filepath));
  }
  else
  {
    strlcat(filepath, req->uri, sizeof(filepath));
  }

  int fd = open(filepath, O_RDONLY, 0);
  if (fd == -1)
  {
    ESP_LOGE(REST_TAG, "Failed to open file : %s", filepath);
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read the file you were looking for.");
    return ESP_FAIL;
  }

  set_content_type_from_file(req, filepath);

  char *chunk = rest_context->scratch;
  ssize_t read_bytes;
  do 
  {
    read_bytes = read(fd, chunk, SCRATCH_BUFFSIZE);
    if (read_bytes == -1)
    {
      ESP_LOGE(REST_TAG, "Failed to read file: %s", filepath);
    }
    else if (read_bytes > 0)
    {
      if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK)
      {
        close(fd);
        ESP_LOGE(REST_TAG, "File sending failed");
        httpd_resp_sendstr_chunk(req, NULL);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file.");
        return ESP_FAIL;
      }
    }
  } while(read_bytes > 0);

  close(fd);
  ESP_LOGI(REST_TAG, "File sending complete");
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static esp_err_t gpio_set_handler(httpd_req_t *req)
{
  ESP_LOGI(REST_TAG, "The full uri is:%s", req->uri);

  handle_query(req->uri);

  httpd_resp_set_type(req, "application/json");

  gpio_state = !gpio_state;
  gpio_set_level(2, gpio_state);

  cJSON *root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "gpio-num", 2);
  cJSON_AddNumberToObject(root, "Set on too: ", gpio_state);

  const char *gpio_data = cJSON_Print(root);
  httpd_resp_sendstr(req, gpio_data);

  free((void *) gpio_data);
  cJSON_Delete(root);

  return ESP_OK;
}


esp_err_t start_rest_server(const char *base_path)
{
  REST_CHECK(base_path, "wrong base path", err);

  rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));

  REST_CHECK(rest_context, "No memory for rest context", err);
  strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.uri_match_fn = httpd_uri_match_wildcard;

  ESP_LOGI(REST_TAG, "Starting HTTP Server");
  REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Server starting failed", err_start);
  
  httpd_uri_t gpio_set_uri = {
    .uri = "/api/v1/gpio/toggle/*",
    .method = HTTP_POST,
    .handler = gpio_set_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &gpio_set_uri);

  httpd_uri_t common_get_uri = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = rest_common_get_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &common_get_uri);

  return ESP_OK;
err_start:
  free(rest_context);

err:
  return ESP_FAIL;
}