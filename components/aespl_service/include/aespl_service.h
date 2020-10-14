/**
 * @brief AESPL Service
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_CFS_H_
#define _AESPL_CFS_H_

#include "esp_err.h"
#include "aespl_httpd.h"

/**
 * @brief Initialize the CFS
 *
 * @param httpd_prefix HTTP URIs prefix
 */
esp_err_t aespl_service_init(aespl_httpd_t *httpd, const char *httpd_prefix);

#endif
