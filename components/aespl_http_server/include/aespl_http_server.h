#ifndef _AESPL_HTTP_SERVER_H_
#define _AESPL_HTTP_SERVER_H_

#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_server.h"

/**
 * @brief Initializes and starts the HTTP server 
 */
esp_err_t http_server_start(const char *app_name, httpd_config_t *config);

/**
 * @brief Adds an URI handler
 */
esp_err_t http_server_handle(uint8_t method, const char *uri, esp_err_t (*handler)(httpd_req_t *r));

/**
 * @brief Prepares and sends a JSON response
 */
esp_err_t http_server_send_response_json(httpd_req_t *req, uint16_t status, cJSON *json);

/**
 * @brief Stops the HTTP server
 */
esp_err_t http_server_stop();

#endif