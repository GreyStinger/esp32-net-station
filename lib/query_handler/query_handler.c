#include <query_handler.h>

static char *QUERY_TAG = "query_handler: ";

static esp_query_pair_t *esp_create_query_pair(char *key, char *value)
{
  esp_query_pair_t *esp_new_query_pair = malloc(sizeof(esp_query_pair_t));
  if (esp_new_query_pair != NULL)
  {
    esp_new_query_pair->key = key;
    esp_new_query_pair->value = value;
    esp_new_query_pair->next = NULL;
  }
  return esp_new_query_pair;
}

void esp_delete_query_pair(esp_query_pair_t *esp_old_query_pair)
{
  if (esp_old_query_pair->next != NULL)
  {
    esp_delete_query_pair(esp_old_query_pair->next);
  }
  free(esp_old_query_pair);
}

static esp_query_pair_t *add_pair(esp_query_pair_t *query_list, char *key, char *value)
{
  esp_query_pair_t *esp_new_query_pair = esp_create_query_pair(key, value);
  if (esp_new_query_pair != NULL)
  {
    esp_new_query_pair->next = query_list;
  }
  return esp_new_query_pair;
}

// esp_query_pair_t handle_query(const char *uri)
void handle_query(const char *uri)
{  
  ESP_LOGI(QUERY_TAG, "The full uri is:%s", uri);

  char *start = strrchr(uri, '?') + 1;
  char *end = strchr(uri, '=');
  if (start == NULL) return;
  // char current_query[255];
  // char query_double[255];
  int buffer_len = 0;

  esp_query_pair_t *my_esp_query_pair = esp_create_query_pair("Hello", "World");
  char *key_buff = (char *) calloc(0, 32 * sizeof(char));
  char *val_buff = (char *) calloc(0, 64 * sizeof(char));

  for (int i = 0; start != NULL && end != NULL; ++i)
  {
    // Handle Key:
    buffer_len = end - start;

    ESP_LOGW(QUERY_TAG, "Start pointer = %s and end pointer = %s", start, end);

    if (buffer_len + 1 > 32) key_buff = (char *) realloc(key_buff, (buffer_len + 1) * sizeof(char));
    memcpy(&key_buff, &start, buffer_len);
    key_buff[buffer_len] = '\0';

    ESP_LOGW(QUERY_TAG, "Start pointer = %s and end pointer = %s", start, end + 1);

    start = (char *) end + 1;
    end = strchr(start, '&');

    buffer_len = end - start;

    ESP_LOGW(QUERY_TAG, "Start pointer = %s and end pointer = %s", start, end);
    
    // TODO: Find last element if there is no &
    if (end == NULL) end = (char *) &start[strlen(start) - 1];

    // Handle Value:
    if (end - start + 1 > 64) val_buff = (char *) realloc(val_buff, (end - start + 1) * sizeof(char));
    memcpy(&val_buff, &start, buffer_len);
    val_buff[buffer_len] = '\0';

    if (*end != '\0') start = (char *) end + 1;
    end = strchr(start, '=');

    ESP_LOGI(QUERY_TAG, "The val is: %s", val_buff);

    add_pair(my_esp_query_pair, key_buff, val_buff);
  }

  free(key_buff);
  free(val_buff);

  esp_query_pair_t *current_map;
  for (current_map = my_esp_query_pair; NULL != current_map; current_map = current_map->next)
  {
    ESP_LOGI(QUERY_TAG, "Key: %s, Value: %s", current_map->key, current_map->value);
  }
  esp_delete_query_pair(my_esp_query_pair);
}

// static esp_query_map_t *esp_create_query_map(size_t block_size)
// {
//   esp_query_map_t *pMapList = malloc(sizeof(esp_query_map_t));
//   if (pMapList != NULL)
//   {
//     pMapList->nMaps = 0;
//     pMapList->size = block_size;
//     pMapList->block_size = block_size;
//     pMapList->maps = malloc(sizeof(char *) * block_size);
//     if (NULL == pMapList->maps)
//     {
//       free(pMapList);
//       return NULL;
//     }
//   }
//   return pMapList;
// }

// void esp_delete_query_map(esp_query_map_t *pMapList)
// {
//   free(pMapList->maps);
//   free(pMapList);
// }

// static int esp_add_query_map(esp_query_map_t *pMapList, char *map)
// {
//   size_t nMaps = pMapList->nMaps;
//   if (nMaps >= pMapList->size)
//   {
//     size_t newSize = pMapList->size + pMapList->block_size;
//     void *newMaps = realloc(pMapList->maps, sizeof(char *) * newSize);
//     if (NULL == newMaps)
//     {
//       return 0;
//     }
//     else
//     {
//       pMapList->size = newSize;
//       pMapList->maps = (char **)newMaps;
//     }
//   }

//   // pMapList->esp_query_pair_t[nMaps] = word;
//   pMapList->maps[nMaps] = map;
//   // memcpy(pMapList->maps[nMaps], map, strlen(map->key));
//   // memcpy(pMapList->maps[nMaps], map, strlen(map->value));

//   ++pMapList->nMaps;

//   return 1;
// }

// static char **esp_query_map_start(esp_query_map_t *pMapList)
// {
//   return pMapList->maps;
// }

// static char **esp_query_map_end(esp_query_map_t *pMapList)
// {
//   return &pMapList->maps[pMapList->nMaps];
// }

// // Possible Bug
// char fetch_query(esp_query_map_t *pMapList, char *key)
// {
//   char **pair;
//   for (pair = esp_query_map_start(pMapList); pair != esp_query_map_end; ++pair)
//   {
//     if (strcmp(key, *pair))
//     {
//       return *pair;
//     }
//   }
// }

// esp_query_map_t handle_query(const char *uri)
// {
//   esp_query_map_t *myMaps = esp_create_query_map(2);
//   char *temp_map = malloc(sizeof(char));
//   char current_query[255];
//   char *query_index = strrchr(uri, '?');
//   int buff_size;

//   memcpy(current_query, query_index + 1, strlen(query_index) - 1);
//   char *value_index = strrchr(current_query, '=');

//   ESP_LOGI(QUERY_TAG, "Query Now: %s", current_query);

//   query_index = strrchr(uri, '&');
//   // int query_len;

//   for (int i = 0; query_index; ++i)
//   {
//     // memcpy(value_index, strrchr(current_query, '='), sizeof(char *) * 1);
//     // memcpy(current_query[*value_index], (char* )'\0', 1);
//     // current_query[*value_index] = '\0';
//     // for (buff_size = 0; current_query[*value_index + 1 + buff_size] != '\0'; ++buff_size);
//     // memcpy(&current_query[*value_index], &current_query[buff_size + 1], 1);
//     // memcpy(temp_map[0], current_query[*value_index + 1], buff_size);

//     // memcpy(&current_query[*query_index], &current_query[buff_size + 1], 1);
//     // // current_query[*query_index] = '\0';
//     // for (buff_size = 0; current_query[*query_index + 1 + buff_size] != '\0'; ++buff_size);
//     // memmove(temp_map[1], current_query[*query_index + 1], buff_size);

//     esp_add_query_map(myMaps, temp_map);

//     query_index = strrchr(uri, '&');
//     value_index = strrchr(current_query, '=');
//     // memcpy(value_index, strrchr(current_query, '='), sizeof(char *));
//   }

//   char *pair;
//   for (pair = esp_query_map_start(myMaps); pair != esp_query_map_end; ++pair)
//   {
//     ESP_LOGI(QUERY_TAG, "Key: %s, Value: %s", pair[0], pair[1]);
//   }

//   free(temp_map);
// }