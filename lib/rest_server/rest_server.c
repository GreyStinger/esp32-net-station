#include <rest_server.h>

static const char *REST_TAG = "station-rest";

// static uint8_t gpio_num = 2;

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

static esp_err_t rest_get_relay_state(httpd_req_t *req)
{
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();
  if (relay_state) {
    cJSON_AddStringToObject(root, "state", "on");
  }
  else {
    cJSON_AddStringToObject(root, "state", "off");
  }
  const char *relay_data = cJSON_Print(root);
  httpd_resp_sendstr(req, relay_data);

  free((void *) relay_data);
  cJSON_Delete(root);

  return ESP_OK;
}

static esp_err_t rest_set_relay_state(httpd_req_t *req)
{
  httpd_resp_set_type(req, "application/json");

  esp_query_pair_t *query_list = handle_esp_query(req->uri);

  cJSON *root = cJSON_CreateObject();

  char* key = fetch_query_val(query_list, "key");
  if (key == NULL) 
  {
    ESP_LOGI(REST_TAG, "No key");
    httpd_resp_send_500(req);
    goto finish_set_relay;
  }

  if (!!strcmp(PV_KEY, key))
  {
    ESP_LOGI(REST_TAG, "Bad Key");
    httpd_resp_send_500(req);
    goto finish_set_relay;
  }
  
  char* state = fetch_query_val(query_list, "state");
  if (!strcmp(state, "on")) 
  {
    relay_state = 1;
  } 
  else if (!strcmp(state, "off")) 
  {
    relay_state = 0;
  }
  else {
    ESP_LOGI(REST_TAG, "Bad state");
    httpd_resp_send_500(req);
    goto finish_set_relay;
  }

  gpio_set_level(25, relay_state);

  cJSON_AddStringToObject(root, "state", state);
  const char *gpio_data = cJSON_Print(root);
  httpd_resp_sendstr(req, gpio_data);

  free((void *) gpio_data);

finish_set_relay:
  cJSON_Delete(root);
  free_esp_query_list(query_list);

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
  
  httpd_uri_t set_relay_state = {
    .uri = "/relay*",
    .method = HTTP_POST,
    .handler = rest_set_relay_state,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &set_relay_state);
  httpd_uri_t fetch_relay_state = {
    .uri = "/relay*",
    .method = HTTP_GET,
    .handler = rest_get_relay_state,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &fetch_relay_state);

  httpd_uri_t common_get_uri = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = rest_common_get_handler,
    .user_ctx = rest_context
  };
  httpd_register_uri_handler(server, &common_get_uri);
  
  ESP_LOGI(REST_TAG, "Successfully started Rest Server");
  return ESP_OK;
err_start:
  free(rest_context);

err:
  return ESP_FAIL;
}