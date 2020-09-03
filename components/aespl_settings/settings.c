#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *LOG_TAG = "aespl_settings";
static nvs_handle nvs_h = NULL;

esp_err_t aespl_settings_init(const char *app_name) {
    esp_err_t err;

    err = nvs_flash_init();
    if (err == ESP_OK) {
        ESP_LOGI(LOG_TAG, "NVS flash initialized");
    } else {
        ESP_LOGE(LOG_TAG, "NVS flash initialization error: %d", err);
        return err;
    }

    err = nvs_open(app_name, NVS_READWRITE, &nvs_h);
    if (err == ESP_OK) {
        ESP_LOGI(LOG_TAG, "NVS flash opened");
    } else {
        ESP_LOGE(LOG_TAG, "NVS flash open error: %d", err);
    }

    return err;
}

esp_err_t aespl_settings_set_str(const char *key, const char *value) {
    esp_err_t err;

    err = nvs_set_str(nvs_h, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Error %d while setting value '%s' for key '%s'", err, value, key);
    } else {
        err = nvs_commit(nvs_h);
        if (err != ESP_OK) {
            ESP_LOGE(LOG_TAG, "Error %d while committing value '%s' for key '%s'", err, value, key);
        }
    }

    return err;
}

char *aespl_settings_get_str(const char *key) {
    size_t sz;
    esp_err_t err;
    char *val;

    err = nvs_get_str(nvs_h, key, NULL, &sz);
    if (err != ESP_OK) {
        switch (err) {
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGW(LOG_TAG, "Value for key '%s' doesn't exist", key);
                break;

            default:
                ESP_LOGE(LOG_TAG, "Error %d while calculating value size for key '%s'", err, key);
                break;
        }

        return NULL;
    }

    val = malloc(sz);
    err = nvs_get_str(nvs_h, key, val, &sz);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Error %d while getting value for key '%s'", err, key);
        free(val);
        return NULL;
    }

    return val;
}

void aespl_settings_get_str_to_json(cJSON *json, const char *settings_k, const char *json_k, const char *dflt) {
    char *val = aespl_settings_get_str(settings_k);

    if (val) {
        cJSON_AddStringToObject(json, json_k, val);
        free(val);
    } else {
        cJSON_AddStringToObject(json, json_k, dflt);
    }
}
