/**
 * AESPL Common Functions Service HTTPd handlers
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include <stdio.h>
#include <stdbool.h>
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "aespl_http_util.h"
#include "aespl_httpd.h"

static const char *log_tag = "aespl_cfs";

esp_err_t httpd_get_wifi(httpd_req_t *req) {
    esp_err_t err;
    cJSON *root = cJSON_CreateObject();

    // Current connection status
    wifi_ap_record_t ap_info = {};
    err = esp_wifi_sta_get_ap_info(&ap_info);
    if (err == ESP_OK) {
        cJSON_AddTrueToObject(root, "connected");
    } else {
        ESP_LOGW(log_tag, "esp_wifi_sta_get_ap_info(): %#x", err);
        cJSON_AddFalseToObject(root, "connected");
    }

    aespl_httpd_send_json(req, HTTPD_200, root);

    return ESP_OK;
}

esp_err_t httpd_get_wifi_scan(httpd_req_t *req) {
    esp_err_t err;
    cJSON *root = cJSON_CreateArray();

    // Available WiFi AP names
    wifi_scan_config_t scan_cfg = {};
    ESP_LOGI(log_tag, "Starting WiFi scan");
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
                ESP_LOGI(log_tag, "Found WiFi AP: %s, %ddBm", res.ssid, res.rssi);
            }
            free(scan_results);
        } else {
            ESP_LOGE(log_tag, "esp_wifi_scan_get_ap_records() failed: %#x", err);
        }
    } else {
        ESP_LOGE(log_tag, "esp_wifi_scan_start() failed: %#x", err);
    }

    aespl_httpd_send_json(req, HTTPD_200, root);

    return ESP_OK;
}

esp_err_t httpd_post_wifi(httpd_req_t *req) {
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

    cJSON *ssid = cJSON_GetObjectItem(req_json, "ap_ssid");
    cJSON *pwd = cJSON_GetObjectItem(req_json, "ap_pass");
    if (cJSON_IsString(ssid) && cJSON_IsString(pwd)) {
        ESP_LOGI(log_tag, "WiFi settings received: \"%s\", \"%s\"", ssid->valuestring, pwd->valuestring);

        wifi_config_t cfg = {.sta = {}};
        strncpy((char *)cfg.sta.ssid, ssid->valuestring, 32);
        strncpy((char *)cfg.sta.password, pwd->valuestring, 64);

        err = esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg);
        if (err != ESP_OK) {
            status = HTTPD_500;
            ESP_LOGW(log_tag, "esp_wifi_set_config(): %#x", err);
            cJSON_AddStringToObject(resp_json, "error", "esp_wifi_set_config() error");
        }
    }

    aespl_httpd_send_json(req, status, resp_json);
    cJSON_Delete(req_json);
    cJSON_Delete(resp_json);

    return ESP_OK;
}

esp_err_t httpd_post_wifi_connect(httpd_req_t *req) {
    char *status = HTTPD_200;
    cJSON *resp_json = cJSON_CreateObject();

    esp_err_t err = esp_wifi_connect();
    cJSON_AddNumberToObject(resp_json, "result", err);

    if (err != ESP_OK) {
        status = HTTPD_500;
    }

    aespl_httpd_send_json(req, status, resp_json);
    cJSON_Delete(resp_json);

    return ESP_OK;
}

esp_err_t httpd_post_wifi_disconnect(httpd_req_t *req) {
    char *status = HTTPD_200;
    cJSON *resp_json = cJSON_CreateObject();

    esp_err_t err = esp_wifi_disconnect();
    cJSON_AddNumberToObject(resp_json, "result", err);

    if (err != ESP_OK) {
        status = HTTPD_500;
    }

    aespl_httpd_send_json(req, status, resp_json);
    cJSON_Delete(resp_json);

    return ESP_OK;
}
