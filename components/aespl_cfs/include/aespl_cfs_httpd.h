#ifndef _AESPL_HTTPD_H_
#define _AESPL_HTTPD_H_

esp_err_t httpd_get_wifi(httpd_req_t *req);
esp_err_t httpd_get_wifi_scan(httpd_req_t *req);
esp_err_t httpd_post_wifi(httpd_req_t *req);
esp_err_t httpd_post_wifi_connect(httpd_req_t *req);
esp_err_t httpd_post_wifi_disconnect(httpd_req_t *req);

#endif
