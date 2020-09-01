#ifndef _AESPL_SETTINGS_H_
#define _AESPL_SETTINGS_H_

#include "esp_err.h"

/**
 * @brief Initializes the settings storage
 */
esp_err_t aespl_settings_init(const char *app_name);

/**
 * @brief Stores a string value into the settings storage
 * 
 * WARNING: don't forget to free out_value in the caller
 */
esp_err_t settings_set_str(const char *key, const char *value);

/**
 * @brief Retrieves a string value from the settings storage
 */
esp_err_t settings_get_str(const char *key, char **out_value);

#endif