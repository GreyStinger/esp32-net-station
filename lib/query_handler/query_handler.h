#ifndef _ESP_QUERY_HANDLER
#define _ESP_QUERY_HANDLER

#include <esp_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct esp_query_pair {
    const char * key;                       /*!< item key name */
    const char * value;                     /*!< item value string */
    struct esp_query_pair* next;
} esp_query_pair_t;

// typedef struct esp_query_map {
//   struct 
//   struct esp_query_pair* next;
// } esp_query_map_t;

void esp_delete_query_pair(esp_query_pair_t *esp_old_query_pair);
// esp_query_pair_t handle_query(const char *uri);
void handle_query(const char *uri);

#endif