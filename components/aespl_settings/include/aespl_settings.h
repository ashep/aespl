#ifndef _AESPL_SETTINGS_H_
#define _AESPL_SETTINGS_H_

#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_server.h"

/**
 * @brief Initializes the settings storage
 */
esp_err_t aespl_settings_init(const char *app_name);

/**
 * @brief Stores a string value into the settings storage
 */
esp_err_t aespl_settings_set_str(const char *key, const char *value);

/**
 * @brief Retrieves a string value from the settings storage
 * 
 * WARNING: don't forget to free() returned value
 */
char *aespl_settings_get_str(const char *key);

/**
 * @brief Retrieves a string value and stores it into a JSON structure
 */
void aespl_settings_get_str_to_json(cJSON *json, const char *settings_k, const char *json_k, const char *dflt);

#endif
