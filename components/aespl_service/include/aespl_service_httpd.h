/**
 * @brief AESPL Service
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_CFS_HTTPD_H_
#define _AESPL_CFS_HTTPD_H_

#include "esp_err.h"
#include "esp_http_server.h"

esp_err_t httpd_get_wifi(httpd_req_t *req);
esp_err_t httpd_get_wifi_scan(httpd_req_t *req);
esp_err_t httpd_post_wifi(httpd_req_t *req);
esp_err_t httpd_post_wifi_connect(httpd_req_t *req);
esp_err_t httpd_post_wifi_disconnect(httpd_req_t *req);

#endif
