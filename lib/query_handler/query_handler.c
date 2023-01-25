#include "query_handler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Private method that adds a query pair to the linked list
// 
// If no linked list exists create a new one
// Memory is allocated with malloc and thus you have
// to call `free_esp_query_list` with the head of the list
// when you are done with it
static esp_query_pair_t * add_query_pair(esp_query_pair_t *last_pair, char *key, char *val)
{
	esp_query_pair_t *query_pair;
	
	query_pair = (esp_query_pair_t *) malloc(sizeof(esp_query_pair_t));
	if (query_pair != NULL)
	{
		query_pair->key = key;
		query_pair->val = val;
		query_pair->next = last_pair;
	}

	return query_pair;
}

// Deallocates entire linked esp_query_pair_t list
//
// Takes head of linked list generated by `add_query_pair`
// and deallocates the whole linked list from memory
void free_esp_query_list(esp_query_pair_t * query_list)
{
	if (query_list->next != NULL) free_esp_query_list(query_list->next);
	free(query_list->next);
	free(query_list->key);
	free(query_list->val);
}

// Gets and returns the a ponter to the value for the corresponding key
//
// Searches throught the linked list looking for the given key
// and returns the corresponding value, it isn't efficient, but
// it's currently not a huge performance loss
char *fetch_query_val(esp_query_pair_t *query_list, char *key)
{
	esp_query_pair_t *current_pair;
	signed int is_current = -1;
	for (current_pair = query_list; current_pair != NULL; current_pair = current_pair->next)
	{
		is_current = strcmp(key, current_pair->key);
		if (!is_current) return current_pair->val;
	}
	return NULL;
}

// Extracts and handles a query from a url
//
// Url as parameter
// Returns null if there is no query
// Else returns a pointer to the first key_val pair
esp_query_pair_t *handle_esp_query(const char *url)
{
	char *unhandled_query_section;
	unhandled_query_section = strchr(url, '?');
	esp_query_pair_t *query_pair_list = NULL;
	if (unhandled_query_section == NULL) goto end;
	char *key = (char *) malloc(0);
	char *val = (char *) malloc(0);
	char *divider;
	unsigned int key_len = 0;
	unsigned int val_len = 0;
	do {
		divider = strchr(unhandled_query_section, '=');
		key_len = divider - (unhandled_query_section + 1);
		// TODO: Test if malloc has to be key_len + 1 or can be just key_len
		key = (char *) malloc(sizeof(char) * (key_len + 1));
		if (key == NULL) goto end;
		memcpy(key, unhandled_query_section + 1, key_len);
		key[key_len] = '\0';
		
		unhandled_query_section = divider;

		divider = strchr(unhandled_query_section, '&');
		if (divider == NULL) divider = strchr(unhandled_query_section, '\0');
		val_len = divider - (unhandled_query_section + 1);
		// TODO: Test if malloc has to be val_len + 1 or can be just val_len
		val = (char *) malloc(sizeof(char) * (val_len + 1));
		if (val == NULL) goto end;
		memcpy(val, unhandled_query_section + 1, val_len);
		val[val_len] = '\0';

		query_pair_list = add_query_pair(query_pair_list, key, val); 

		unhandled_query_section = strchr(unhandled_query_section, '&');
	} while(unhandled_query_section != NULL);

end:
	return query_pair_list;
}
