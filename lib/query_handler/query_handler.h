#pragma once
#ifndef _ESP_QUERY_HANDLER
#define _ESP_QUERY_HANDLER

// esp_query_pair_t is a linked list head
// 
// This will be used to create a linked list
// for query key value pairs
typedef struct esp_query_pair {
	char *key;
	char *val;
	struct esp_query_pair * next;
} esp_query_pair_t;

// Declarations for query_handler.c
esp_query_pair_t *handle_esp_query(const char *url);
char *fetch_query_val(esp_query_pair_t *query_list, char *key);
void free_esp_query_list(esp_query_pair_t *query_list);

#endif
