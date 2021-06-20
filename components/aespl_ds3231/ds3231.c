/**
 * @brief     AESPL DS3231 Driver
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "aespl_i2c.h"
#include "aespl_ds3231.h"
#include "aespl_common.h"

esp_err_t aespl_ds3231_init(aespl_ds3231_t *ds3231, i2c_port_t num) {
    ds3231->mux = xSemaphoreCreateBinary();
    if (!ds3231->mux) {
        return ESP_FAIL;
    }

    if (xSemaphoreGive(ds3231->mux) != pdTRUE) {
        return ESP_FAIL;
    }

    ds3231->i2c_port = num;

    return ESP_OK;
}

esp_err_t aespl_ds3231_get_data(aespl_ds3231_t *ds3231, TickType_t timeout) {
    esp_err_t err;
    uint8_t buf[AESPL_DS3231_REG_LEN];

    // Lock
    if (xSemaphoreTake(ds3231->mux, 10) != pdTRUE) {
        return ESP_FAIL;
    }

    err = aespl_i2c_read(AESPL_DS3231_I2C_ADDR, AESPL_DS3231_REG_SECONDS, buf, AESPL_DS3231_REG_LEN, timeout);
    if (err) {
        xSemaphoreGive(ds3231->mux);
        return err;
    }

    // Second
    ds3231->sec = (buf[0] >> 4) * 10 + (0x0f & buf[0]);

    // Minute
    ds3231->min = (buf[1] >> 4) * 10 + (0x0f & buf[1]);

    // Hour
    ds3231->h12 = buf[2] >> 6;
    if (ds3231->h12) {
        ds3231->pm = (0x20 & buf[2]) >> 5;            // 5th bit is AM/PM flag
        ds3231->hour += ((0x10 & buf[2]) >> 4) * 10;  // 4th bit is 10-hours counter
        ds3231->hour += 0x0f & buf[2];                // bits 0-3 are 1-hour counter
    } else {
        ds3231->hour = ((0x20 & buf[2]) >> 5) * 20;   // 5th bit is 20-hours counter
        ds3231->hour += ((0x10 & buf[2]) >> 4) * 10;  // 4th bit is 10-hours counter
        ds3231->hour += 0x0f & buf[2];                // bits 0-3 are 1-hour counter
    }

    // Day of the week
    ds3231->dow = buf[3];

    // Day of the month
    ds3231->day = (buf[4] >> 4) * 10;  // bits 4-5 are 10-days counter
    ds3231->day += 0x0f & buf[4];      // bits 0-3 are 1-day counter

    // Month
    ds3231->mon = (0x1 & (buf[5] >> 4)) * 10;  // 4th bit is 10-month counter
    ds3231->mon += 0x0f & buf[5];              // bits 0-3 are 1-month counter

    // Year
    ds3231->year = (buf[6] >> 4) * 10;  // bits 7-4 are 10-year counter
    ds3231->year += (0x0f & buf[6]);    // bits 0-3 are 1-year counter

    // Temperature
    ds3231->temp = 0x7f & buf[17];          // bits 6-0 of the byte 17 are integer part without sign
    ds3231->temp += (buf[18] >> 6) * 0.25;  // bits 7-6 of the byte 18 are fractional 0.25-part
    if (buf[17] >> 7) {                     // 7th bit of 17th byte is the sign
        ds3231->temp *= -1;
    }

    // Unlock
    if (xSemaphoreGive(ds3231->mux) != pdTRUE) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t aespl_ds3231_set_data(const aespl_ds3231_t *ds3231, TickType_t timeout) {
    esp_err_t err;
    uint8_t buf[AESPL_DS3231_REG_LEN];

    // Lock
    if (xSemaphoreTake(ds3231->mux, 10) != pdTRUE) {
        return ESP_FAIL;
    }

    // Second
    buf[0] = ((ds3231->sec / 10) << 4) | (ds3231->sec % 10);

    // Minute
    buf[1] = ((ds3231->min / 10) << 4) | (ds3231->min % 10);

    // Hour
    buf[2] = ds3231->h12 << 6;
    if (ds3231->h12) {
        buf[2] |= (ds3231->pm << 5) | ((ds3231->hour / 10) << 4) | (ds3231->hour % 10);
    } else {
        buf[2] |= ((ds3231->hour / 20) << 5) | ((ds3231->hour / 10) << 4) | (ds3231->hour % 10);
    }

    // Day of the week
    buf[3] = ds3231->dow;

    // Day of the month
    buf[4] = ((ds3231->day / 10) << 4) | (ds3231->day % 10);

    // Month
    buf[5] = ((ds3231->mon / 10) << 4) | (ds3231->mon % 10);

    // Year
    buf[6] = ((ds3231->year / 10) << 4) | (ds3231->year % 10);

    // Send data to the device
    err = aespl_i2c_write(AESPL_DS3231_I2C_ADDR, AESPL_DS3231_REG_SECONDS, buf, 7, timeout);
    if (err) {
        xSemaphoreGive(ds3231->mux);
        return err;
    }

    // Unlock
    if (xSemaphoreGive(ds3231->mux) != pdTRUE) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
