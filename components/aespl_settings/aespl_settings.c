#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static char *log_tag = NULL;
static nvs_handle nvs_h = NULL;

esp_err_t aespl_settings_init(const char *app_name) {
    esp_err_t err;

    log_tag = malloc(strlen(app_name) + strlen("_settings") + 1);
    if (!log_tag) {
        ESP_LOGE("aespl_settings_init", "malloc() error");
        return ESP_ERR_NO_MEM;
    }
    strcpy(log_tag, app_name);
    strcat(log_tag, "_settings");

    err = nvs_flash_init();
    if (err == ESP_OK) {
        ESP_LOGI(log_tag, "NVS flash initialized");
    } else {
        ESP_LOGE(log_tag, "NVS flash initialization error: %d", err);
        return err;
    }

    err = nvs_open(app_name, NVS_READWRITE, &nvs_h);
    if (err == ESP_OK) {
        ESP_LOGI(log_tag, "NVS flash opened");
    } else {
        ESP_LOGE(log_tag, "NVS flash open error: %d", err);
    }

    return err;
}

esp_err_t settings_set_str(const char *key, const char *value) {
    esp_err_t err;

    err = nvs_set_str(nvs_h, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(log_tag, "Error %d while setting value '%s' for key '%s'", err, value, key);
    } else {
        err = nvs_commit(nvs_h);
        if (err != ESP_OK) {
            ESP_LOGE(log_tag, "Error %d while committing value '%s' for key '%s'", err, value, key);
        }
    }

    return err;
}

esp_err_t settings_get_str(const char *key, char *out_value) {
    size_t sz;
    esp_err_t err;

    err = nvs_get_str(nvs_h, key, NULL, &sz);
    if (err != ESP_OK) {
        switch (err) {
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGW(log_tag, "Value for key '%s' doesn't exist", key);
                break;

            default:
                ESP_LOGE(log_tag, "Error %d while calculating value size for key '%s'", err, key);
                break;
        }

        return err;
    }

    char *ov = malloc(sz);
    err = nvs_get_str(nvs_h, key, ov, &sz);
    if (err != ESP_OK)
        ESP_LOGE(log_tag, "Error %d while getting value for key '%s'", err, key);



    return err;
}
