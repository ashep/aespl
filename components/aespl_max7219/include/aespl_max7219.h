/**
 * MAX7219 Driver for ESP8266
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef _AESPL_MAX7219_H_
#define _AESPL_MAX7219_H_

#include "stdbool.h"
#include "driver/gpio.h"

/**
 * MAX7219 register addresses
 */
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
 * MAX7219 decode modes
 */
typedef enum {
    AESPL_MAX7219_DECODE_NONE = 0x00,
    AESPL_MAX7219_DECODE_1 = 0x01,
    AESPL_MAX7219_DECODE_2 = 0x0f,
    AESPL_MAX7219_DECODE_3 = 0xff,
} aespl_max7219_decode_mode_t;

/**
 * MAX7219 intensity modes
 */
typedef enum {
    AESPL_MAX7219_INTENSITY_MIN,
    AESPL_MAX7219_INTENSITY_1,
    AESPL_MAX7219_INTENSITY_2,
    AESPL_MAX7219_INTENSITY_3,
    AESPL_MAX7219_INTENSITY_4,
    AESPL_MAX7219_INTENSITY_5,
    AESPL_MAX7219_INTENSITY_6,
    AESPL_MAX7219_INTENSITY_7,
    AESPL_MAX7219_INTENSITY_8,
    AESPL_MAX7219_INTENSITY_9,
    AESPL_MAX7219_INTENSITY_10,
    AESPL_MAX7219_INTENSITY_11,
    AESPL_MAX7219_INTENSITY_12,
    AESPL_MAX7219_INTENSITY_13,
    AESPL_MAX7219_INTENSITY_14,
    AESPL_MAX7219_INTENSITY_MAX,
} aespl_max7219_intensity_t;

/**
 * MAX7219 scan limit modes
 */
typedef enum {
    AESPL_MAX7219_SCAN_LIMIT_1,
    AESPL_MAX7219_SCAN_LIMIT_2,
    AESPL_MAX7219_SCAN_LIMIT_3,
    AESPL_MAX7219_SCAN_LIMIT_4,
    AESPL_MAX7219_SCAN_LIMIT_5,
    AESPL_MAX7219_SCAN_LIMIT_6,
    AESPL_MAX7219_SCAN_LIMIT_7,
    AESPL_MAX7219_SCAN_LIMIT_8,
} aespl_max7219_scan_limit_t;

/**
 * MAX7219 power modes
 */
typedef enum {
    AESPL_MAX7219_POWER_OFF,
    AESPL_MAX7219_POWER_ON,
} aespl_max7219_power_mode_t;

/**
 * MAX7219 test modes
 */
typedef enum {
    AESPL_MAX7219_TEST_MODE_DISABLE,
    AESPL_MAX7219_TEST_MODE_ENABLE,
} aespl_max_7219_test_mode_t;

/**
 * MAX7219 configuration structure
 */
typedef struct {
    gpio_num_t pin_cs;
    gpio_num_t pin_clk;
    gpio_num_t pin_data;
    uint8_t n_displays;
    aespl_max7219_decode_mode_t decode;
} aespl_max7219_config_t;

/**
 * @brief Initialize an MCU to work with MAX7219
 *
 * @param cfg Configuration
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t aespl_max7219_init(const aespl_max7219_config_t *cfg);

/**
 * @brief Latch sent data into device's registers
 *
 * @param cfg Configuration
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t aespl_max7219_latch(const aespl_max7219_config_t *cfg);

/**
 * @brief Send a command to single device
 *
 * @param cfg    Configuration
 * @param addr   Address
 * @param data   Data
 * @param latch  Latch
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t aespl_max7219_send(const aespl_max7219_config_t *cfg, aespl_max7219_addr_t addr, uint8_t data, bool latch);

/**
 * @brief Send a command to all devices
 *
 * @param cfg    Configuration
 * @param addr   Address
 * @param data   Data
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t aespl_max7219_send_all(const aespl_max7219_config_t *cfg, aespl_max7219_addr_t addr, uint8_t data);

/**
 * @brief Clear digits of all devices
 *
 * @param cfg    Configuration
 * @return
 *     - ESP_ERR_INVALID_ARG GPIO number error
 */
esp_err_t aespl_max7219_clear(const aespl_max7219_config_t *cfg);

#endif
