/**
 * MAX7219 driver for ESP8266
 * 
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef _AESPL_MAX7219_H_
#define _AESPL_MAX7219_H_

#include "driver/gpio.h"

#define AESPL_MAX7219_POWER_OFF 0x0
#define AESPL_MAX7219_POWER_ON 0x1

#define AESPL_MAX7219_DECODE_NONE 0x00
#define AESPL_MAX7219_DECODE_1 0x01
#define AESPL_MAX7219_DECODE_2 0x0f
#define AESPL_MAX7219_DECODE_3 0xff

#define AESPL_MAX7219_INTENSITY_MIN 0x0
#define AESPL_MAX7219_INTENSITY_1 0x1
#define AESPL_MAX7219_INTENSITY_2 0x2
#define AESPL_MAX7219_INTENSITY_3 0x3
#define AESPL_MAX7219_INTENSITY_4 0x4
#define AESPL_MAX7219_INTENSITY_5 0x5
#define AESPL_MAX7219_INTENSITY_6 0x6
#define AESPL_MAX7219_INTENSITY_7 0x7
#define AESPL_MAX7219_INTENSITY_8 0x8
#define AESPL_MAX7219_INTENSITY_9 0x9
#define AESPL_MAX7219_INTENSITY_10 0xa
#define AESPL_MAX7219_INTENSITY_11 0xb
#define AESPL_MAX7219_INTENSITY_12 0xc
#define AESPL_MAX7219_INTENSITY_13 0xd
#define AESPL_MAX7219_INTENSITY_14 0xe
#define AESPL_MAX7219_INTENSITY_MAX 0xf

#define AESPL_SCAN_LIMIT_1 0x0
#define AESPL_SCAN_LIMIT_2 0x1
#define AESPL_SCAN_LIMIT_3 0x2
#define AESPL_SCAN_LIMIT_4 0x3
#define AESPL_SCAN_LIMIT_5 0x4
#define AESPL_SCAN_LIMIT_6 0x5
#define AESPL_SCAN_LIMIT_7 0x6
#define AESPL_SCAN_LIMIT_8 0x7

#define AESPL_TEST_MODE_DISABLE 0
#define AESPL_TEST_MODE_ENABLE 1

/**
 * Configuration structure
 */
typedef struct {
    gpio_num_t pin_cs;
    gpio_num_t pin_clk;
    gpio_num_t pin_data;
    uint8_t n_displays;
} aespl_max7219_config_t;

typedef enum {
    AESPL_MAX7219_ADDR_NOOP,
    AESPL_MAX7219_ADDR_DIGIT_0,
    AESPL_MAX7219_ADDR_DIGIT_1,
    AESPL_MAX7219_ADDR_DIGIT_2,
    AESPL_MAX7219_ADDR_DIGIT_3,
    AESPL_MAX7219_ADDR_DIGIT_4,
    AESPL_MAX7219_ADDR_DIGIT_5,
    AESPL_MAX7219_ADDR_DIGIT_6,
    AESPL_MAX7219_ADDR_DIGIT_7,
    AESPL_MAX7219_ADDR_DECODE_MODE,
    AESPL_MAX7219_ADDR_INTENSITY,
    AESPL_MAX7219_ADDR_SCAN_LIMIT,
    AESPL_MAX7219_ADDR_POWER,
    AESPL_MAX7219_ADDR_TEST,
} aespl_max7219_addr_t;

/**
 * @brief Initialize an MCU to work with MAX7219
 * 
 * @param cfg Configuration
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t
aespl_max7219_init(const aespl_max7219_config_t *cfg);

/**
 * @brief Send a command to single device
 * 
 * @param cfg Configuration
 * @param addr Address
 * @param data Data
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t aespl_max7219_send(const aespl_max7219_config_t *cfg, aespl_max7219_addr_t addr, uint8_t data);

/**
 * @brief Send a command to all devices
 * 
 * @param cfg Configuration
 * @param addr Address
 * @param data Data
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t aespl_max7219_send_all(const aespl_max7219_config_t *cfg, aespl_max7219_addr_t addr, uint8_t data);

/**
 * @brief Latch sent data into device's registers
 * 
 * @param cfg Configuration
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t aespl_max7219_latch(const aespl_max7219_config_t *cfg);

#endif
