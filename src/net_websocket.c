#include "esp_websocket_client.h"
#include "esp_log.h"

#include "net_globals.h"

static char *WEBSOCKET_TAG = "WEBSOCKET";

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
	switch (event_id)
	{
	case WEBSOCKET_EVENT_CONNECTED:
		ESP_LOGI(WEBSOCKET_TAG, "WEBSOCKET_EVENT_CONNECTED");
		break;
	case WEBSOCKET_EVENT_DISCONNECTED:
		ESP_LOGI(WEBSOCKET_TAG, "WEBSOCKET_EVENT_DISCONNECTED");
		break;
	case WEBSOCKET_EVENT_DATA:
		ESP_LOGI(WEBSOCKET_TAG, "WEBSOCKET_EVENT_DATA");
		ESP_LOGI(WEBSOCKET_TAG, "Received opcode=%d", data->op_code);
		if (data->op_code == 0x08 && data->data_len == 2)
		{
			ESP_LOGW(WEBSOCKET_TAG, "Received closed message with code=%d", 256*data->data_ptr[0] + data->data_ptr[1]);
		}
		else
		{
			ESP_LOGW(WEBSOCKET_TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);
		}
		ESP_LOGW(WEBSOCKET_TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);

		// xTimerReset perhaps
		break;
	case WEBSOCKET_EVENT_ERROR:
		ESP_LOGI(WEBSOCKET_TAG, "WEBSOCKET_EVENT_ERROR");
		break;
	}
}

void start_websocket(void)
{
	esp_websocket_client_config_t websocket_cfg = {};

	websocket_cfg.uri = WEBSOCKET_URI;

	ESP_LOGI(WEBSOCKET_TAG, "Connecting to %s...", websocket_cfg.uri);

	esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
	esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

	esp_websocket_client_start(client);

	char data[32];
	int i = 0;
	while (i < 10)
	{
		if (esp_websocket_client_is_connected(client))
		{
			int len = sprintf(data, "hello %04d", ++i);
			ESP_LOGI(WEBSOCKET_TAG, "Sending %s", data);
			esp_websocket_client_send_text(client, data, len, portMAX_DELAY);
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}

	
}
