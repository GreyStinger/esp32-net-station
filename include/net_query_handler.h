/**
 * @file query_handler.h
 * @brief Header file for query handling in an ESP (Embedded Systems Programming) application.
 */

#pragma once
#ifndef _ESP_QUERY_HANDLER
#define _ESP_QUERY_HANDLER

/// --- Data Structure Definitions ---

/**
 * @struct esp_query_pair
 * @brief Structure for key-value pairs used in ESP queries.
 *
 * The `esp_query_pair` structure represents a key-value pair, typically used to map and manipulate
 * query parameters in an Embedded Systems Programming (ESP) application. This structure is used
 * in the context of the `query_handler` module to manage and organize queries.
 */
typedef struct esp_query_pair {
	char *key;
	char *val;
	struct esp_query_pair * next;
} esp_query_pair_t;

/// --- Function Definitions ---

// Declarations for query_handler.c
esp_query_pair_t *handle_esp_query(const char *url);
char *fetch_query_val(esp_query_pair_t *query_list, char *key);
void free_esp_query_list(esp_query_pair_t *query_list);

#endif
