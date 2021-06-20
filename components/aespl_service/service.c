/**
 * @brief AESPL Service
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include <string.h>
#include "aespl_httpd.h"
#include "aespl_service_httpd.h"

esp_err_t aespl_service_init(aespl_httpd_t *httpd, const char *httpd_prefix) {
    char buf[100];
    esp_err_t err;

    if (!httpd_prefix) {
        httpd_prefix = "";
    }

    strcpy(buf, httpd_prefix);
    strcat(buf, "/wifi");
    err = aespl_httpd_handle(httpd, HTTP_GET, buf, httpd_get_wifi);
    if (err != ESP_OK) {
        return err;
    }

    strcpy(buf, httpd_prefix);
    strcat(buf, "/wifi");
    err = aespl_httpd_handle(httpd, HTTP_POST, buf, httpd_post_wifi);
    if (err != ESP_OK) {
        return err;
    }

    strcpy(buf, httpd_prefix);
    strcat(buf, "/wifi/scan");
    err = aespl_httpd_handle(httpd, HTTP_POST, buf, httpd_post_wifi_scan);
    if (err != ESP_OK) {
        return err;
    }

    strcpy(buf, httpd_prefix);
    strcat(buf, "/wifi/connect");
    err = aespl_httpd_handle(httpd, HTTP_POST, buf, httpd_post_wifi_connect);
    if (err != ESP_OK) {
        return err;
    }

    strcpy(buf, httpd_prefix);
    strcat(buf, "/wifi/disconnect");
    err = aespl_httpd_handle(httpd, HTTP_POST, buf, httpd_post_wifi_disconnect);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}
