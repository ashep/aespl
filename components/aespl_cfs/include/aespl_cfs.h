/**
 * AESPL Common Functions Service
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef _AESPL_CFS_H_
#define _AESPL_CFS_H_

#include "esp_err.h"

/**
 * @brief Initialize the CFS
 *
 * @param httpd_prefix HTTP URIs prefix
 */
esp_err_t aespl_cfs_init(const char *httpd_prefix);

#endif
