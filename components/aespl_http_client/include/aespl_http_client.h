/**
 * @brief     AESPL HTTP Client
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_HTTP_CLIENT_
#define _AESPL_HTTP_CLIENT_

#include "cJSON.h"
#include "http_parser.h"
#include "http_header.h"

/**
 * HTTP response
 */
typedef struct {
    uint8_t error;
    uint16_t status_code;
    uint64_t content_length;
    http_header_handle_t headers;
    char *tmp_header_field;
    char *tmp_header_value;
    char *body;
    cJSON *json;
} aespl_http_response;

/**
 * @brief Make an HTTP request.
 *        The client is responsible to free `resp` using `aespl_http_client_free()`.
 *
 * @param[out] response Response
 * @param[in]  method   Request method
 * @param[in]  url      Request URL
 * @param[in]  headers  Request headers
 * @param[in]  body     Request body
 * @return ESP_OK in case of success, ESP_FAIL otherwise
 */
esp_err_t aespl_http_client_request(aespl_http_response *response, enum http_method method, const char *url,
                                    http_header_handle_t headers, const char *body);

/**
 * @brief Shortcut for `aespl_http_client_request(response, "GET", url, headers, NULL)`.
 *
 * @param[out] response Response
 * @param[in]  url      Request URL
 * @param[in]  headers  Request headers
 */
esp_err_t aespl_http_client_get(aespl_http_response *response, const char *url, http_header_handle_t headers);

/**
 * @brief Shortcut for `aespl_http_client_get()` with "Accept: application/json" header set.
 *        After receiving an 2xx-response the body will be parsed into `response->json`.
 *
 * @param[out] response Response
 * @param[in]  url      Request URL
 * @param[in]  headers  Request headers
 */
esp_err_t aespl_http_client_get_json(aespl_http_response *response, const char *url, http_header_handle_t headers);

/**
 * @brief Free resources allocated by request functions
 *
 * @param[out] response Response object
 */
void aespl_http_client_free(aespl_http_response *response);

#endif
