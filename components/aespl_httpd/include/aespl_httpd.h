/**
 * @brief     AESPL HTTP Server
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_HTTPD_H_
#define _AESPL_HTTPD_H_

#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "aespl_common.h"

#define AESPL_ERR_HTTPD_BASE (AESPL_ERR_BASE + 0x0200)
#define AESPL_ERR_HTTPD_NOT_STARTED (AESPL_ERR_HTTPD_BASE + 0x1)

typedef struct {
    const httpd_config_t *config;
    httpd_handle_t server;
} aespl_httpd_t;

/**
 * @brief Add an URI handler
 *
 * @param method  Method
 * @param uri     URI
 * @param handler Request handler
 */
esp_err_t aespl_httpd_handle(aespl_httpd_t *server, httpd_method_t method, const char *uri,
                             esp_err_t (*handler)(httpd_req_t *r));

/**
 *
 */
esp_err_t aespl_httpd_handle_spiffs(aespl_httpd_t *server);

/**
 * @brief Send a response
 *
 * @param req    Request object
 * @param status Response status
 * @param body   Response body
 */
esp_err_t aespl_httpd_send(httpd_req_t *req, const char *status, const char *body);

/**
 * @brief Send a JSON response
 *
 * @param req    Request object
 * @param status Response status
 * @param json   JSON data
 */
esp_err_t aespl_httpd_send_json(httpd_req_t *req, const char *status, cJSON *json);

/**
 * @brief Stop the HTTP server
 */
esp_err_t aespl_httpd_stop(aespl_httpd_t *server);

/**
 * @brief Initialize and start the HTTP server
 *
 * If the `config` is NULL, it will be initialized with default values.
 *
 * @param httpd_config ESP HTTP server config
 */
esp_err_t aespl_httpd_start(aespl_httpd_t *server, const httpd_config_t *config);

#endif
