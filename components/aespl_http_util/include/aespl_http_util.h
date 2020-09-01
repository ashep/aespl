#ifndef _AESPL_HTTP_UTIL_
#define _AESPL_HTTP_UTIL_

#include "cJSON.h"

/**
 * @brief Parses request body JSON
 * 
 * WARNING: dont't forget to free returned value with cJSON_Delete()
 */
cJSON *http_util_parse_req_body_json(httpd_req_t *req);

#endif
