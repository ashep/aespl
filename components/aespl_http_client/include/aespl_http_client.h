#ifndef _AESPL_HTTP_CLIENT_
#define _AESPL_HTTP_CLIENT_

#include "cJSON.h"
#include "http_parser.h"
#include "http_header.h"

typedef struct aespl_http_response {
    uint8_t error;
    uint16_t status_code;
    uint64_t content_length;
    http_header_handle_t headers;
    char *body;
} aespl_http_response;

/**
 * Make an HTTP request
 */
esp_err_t aespl_http_client_request(aespl_http_response *resp, enum http_method method, const char *url,
                                    http_header_handle_t headers, const char *body);

/**
 * Make a GET request
 */
esp_err_t aespl_http_client_get(aespl_http_response *resp, const char *url, http_header_handle_t headers);

/**
 * Free resources allocated for aespl_http_response
 */
void aespl_http_client_free(aespl_http_response *resp);

#endif
