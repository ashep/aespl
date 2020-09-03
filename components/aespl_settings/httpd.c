#include <stdio.h>
#include <stdbool.h>
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "aespl_settings.h"
#include "aespl_http_util.h"
#include "aespl_http_server.h"

static const char *LOG_TAG = "aespl_settings";

static esp_err_t httpd_get_settings(httpd_req_t *req) {
    esp_err_t err;
    cJSON *root = cJSON_CreateObject();
    cJSON *wifi = cJSON_AddObjectToObject(root, "wifi");

    // Current connection status
    wifi_ap_record_t ap_info = {};
    err = esp_wifi_sta_get_ap_info(&ap_info);
    if (err == ESP_OK) {
        cJSON_AddTrueToObject(wifi, "connected");
    } else {
        ESP_LOGW(LOG_TAG, "esp_wifi_sta_get_ap_info(): %#x", err);
        cJSON_AddFalseToObject(wifi, "connected");
    }

    aespl_httpd_send_json(req, HTTPD_200, root);

    return ESP_OK;
}

static esp_err_t httpd_get_settings_ap_list(httpd_req_t *req) {
    esp_err_t err;
    cJSON *root = cJSON_CreateArray();
    
    // Available WiFi AP names
    wifi_scan_config_t scan_cfg = {};
    ESP_LOGI(LOG_TAG, "Starting WiFi scan");
    err = esp_wifi_scan_start(&scan_cfg, true);
    
    if (err == ESP_OK) {
        uint16_t n_rec = 10;
        wifi_ap_record_t *scan_results = calloc(n_rec, sizeof(wifi_ap_record_t));
        err = esp_wifi_scan_get_ap_records(&n_rec, scan_results);
        if (err == ESP_OK) {
            for (uint16_t i = 0; i < n_rec; i++) {
                wifi_ap_record_t res = scan_results[i];
                cJSON *ap_desc = cJSON_CreateArray();
                cJSON_AddItemToArray(ap_desc, cJSON_CreateNumber((double)res.rssi));
                cJSON_AddItemToArray(ap_desc, cJSON_CreateString((const char *)res.ssid));
                cJSON_AddItemToArray(root, ap_desc);
                ESP_LOGI(LOG_TAG, "Found WiFi AP: %s, %ddBm", res.ssid, res.rssi);
            }
            free(scan_results);
        } else {
            ESP_LOGE(LOG_TAG, "esp_wifi_scan_get_ap_records() failed: %#x", err);
        }
    } else {
        ESP_LOGE(LOG_TAG, "esp_wifi_scan_start() failed: %#x", err);
    }

    aespl_httpd_send_json(req, HTTPD_200, root);

    return ESP_OK;
}

static esp_err_t httpd_post_settings(httpd_req_t *req) {
    esp_err_t err;
    char *status = HTTPD_200;
    cJSON *req_json = http_util_parse_req_body_json(req);
    cJSON *resp_json = cJSON_CreateObject();

    if (req_json == NULL) {
        cJSON_AddStringToObject(resp_json, "error", "Malformed JSON");
        aespl_httpd_send_json(req, HTTPD_400, resp_json);
        cJSON_Delete(req_json);
        cJSON_Delete(resp_json);
        return ESP_OK;
    }

    // Process WiFi settings
    cJSON *wifi = cJSON_GetObjectItem(req_json, "wifi");
    if (wifi) {
        ESP_LOGI(LOG_TAG, "JSON received: %s", cJSON_PrintUnformatted(wifi));

        cJSON *ssid = cJSON_GetObjectItem(wifi, "ap_ssid");
        cJSON *pwd = cJSON_GetObjectItem(wifi, "ap_pass");
        if (cJSON_IsString(ssid) && cJSON_IsString(pwd)) {
            ESP_LOGI(LOG_TAG, "WiFi settings received: \"%s\", \"%s\"", ssid->valuestring, pwd->valuestring);

            wifi_config_t cfg = {.sta = {}};
            strncpy((char *)cfg.sta.ssid, ssid->valuestring, 32);
            strncpy((char *)cfg.sta.password, pwd->valuestring, 64);

            err = esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg);
            if (err == ESP_OK) {
                esp_wifi_disconnect();
                esp_wifi_connect();
            } else {
                status = HTTPD_500;
                ESP_LOGW(LOG_TAG, "esp_wifi_set_config(): %#x", err);
                cJSON_AddStringToObject(resp_json, "error", "esp_wifi_set_config() error");
            }
        }
    }

    aespl_httpd_send_json(req, status, resp_json);
    cJSON_Delete(req_json);
    cJSON_Delete(resp_json);

    return ESP_OK;
}

esp_err_t aespl_settings_httpd_register_handlers(const char *prefix) {
    char buf[100];
    esp_err_t err;

    err = aespl_httpd_handle(HTTP_GET, prefix, httpd_get_settings);
    if (err != ESP_OK) {
        return err;
    }

    err = aespl_httpd_handle(HTTP_POST, prefix, httpd_post_settings);
    if (err != ESP_OK) {
        return err;
    }

    strcpy(buf, prefix);
    strcat(buf, "/ap-list");
    err = aespl_httpd_handle(HTTP_GET, buf, httpd_get_settings_ap_list);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}
