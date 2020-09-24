/**
 * AESPL Common Functions
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */
#ifndef _AESPL_COMMON_H_
#define _AESPL_COMMON_H_

#include "stdint.h"

#define AESPL_ERR_BASE 0x1000

/**
 * @brief Print binary representation of an integer
 *
 * @param v      Value
 * @param width  Value width
 */
void print_bin(uint32_t v, uint8_t width);

#endif
