/**
 * AESPL HTTP Server
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef _AESPL_HTTPD_H_
#define _AESPL_HTTPD_H_

#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "aespl_common.h"

#define AESPL_ERR_HTTPD_BASE (AESPL_ERR_BASE + 0x0200)
#define AESPL_ERR_HTTPD_NOT_STARTED (AESPL_ERR_HTTPD_BASE + 0x1)
#define AESPL_ERR_HTTPD_ALREADY_STARTED (AESPL_ERR_HTTPD_BASE + 0x2)

/**
 * @brief Initialize and start the HTTP server
 *
 * @param app_name     Application's name
 * @param httpd_config ESP HTTP server config
 */
esp_err_t aespl_httpd_start(const char *app_name, httpd_config_t *config);

/**
 * @brief Add an URI handler
 *
 * @param method  Method
 * @param uri     URI
 * @param handler Request handler
 */
esp_err_t aespl_httpd_handle(uint8_t method, const char *uri, esp_err_t (*handler)(httpd_req_t *r));

/**
 * @brief Send a response
 *
 * @param req    Request object
 * @param status Response status
 * @param body   Response body
 */
esp_err_t aespl_httpd_send(httpd_req_t *req, const char *status, const char *body);

/**
 * @brief Prepare and send a JSON response
 *
 * @param req    Request object
 * @param status Response status
 * @param json   JSON data
 */
esp_err_t aespl_httpd_send_json(httpd_req_t *req, const char *status, cJSON *json);

/**
 * @brief Stop the HTTP server
 */
esp_err_t aespl_httpd_stop();

#endif
