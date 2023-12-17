/**
  ******************************************************************************
  * @file           : sht4x.h
  * @author         : Mauricio Barroso Benavides
  * @date           : Nov 29, 2023
  * @brief          : todo: write brief 
  ******************************************************************************
  * @attention
  *
  * MIT License
  *
  * Copyright (c) 2023 Mauricio Barroso Benavides
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to
  * deal in the Software without restriction, including without limitation the
  * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  * sell copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  * 
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  * IN THE SOFTWARE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SHT4X_H_
#define SHT4X_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "i2c_bus.h"

/* Exported Macros -----------------------------------------------------------*/
#define SHT40_I2C_ADDR_44	0x44
#define SHT40_I2C_ADDR_45	0x45
#define SHT41_I2C_ADDR_44	0x44
#define SHT41_I2C_ADDR_45	0x45
#define SHT45_I2C_ADDR_44	0x44
#define SHT45_I2C_ADDR_45	0x45

/* SHT4x commands */
#define SHT4X_MEASURE_HIGH_PRECISION_TICKS_CMD							0xfd
#define SHT4X_MEASURE_MEDIUM_PRECISION_TICKS_CMD						0xf6
#define SHT4X_MEASURE_LOWEST_PRECISION_TICKS_CMD						0xe0
#define SHT4X_ACTIVATE_HIGHEST_HEATER_POWER_LONG_TICKS_CMD	0x39
#define SHT4X_ACTIVATE_HIGHEST_HEATER_POWER_SHORT_TICKS_CMD	0x32
#define SHT4X_ACTIVATE_MEDIUM_HEATER_POWER_LONG_TICKS_CMD		0x2f
#define SHT4X_ACTIVATE_MEDIUM_HEATER_POWER_SHORT_TICKS_CMD	0x24
#define SHT4X_ACTIVATE_LOWEST_HEATER_POWER_LONG_TICKS_CMD		0x1e
#define SHT4X_ACTIVATE_LOWEST_HEATER_POWER_SHORT_TICKS_CMD	0x15
#define SHT4X_SERIAL_NUMBER_CMD															0x89
#define SHT4X_SOFT_RESET_CMD																0x94

/* Exported typedef ----------------------------------------------------------*/
typedef struct {
	i2c_bus_dev_t *i2c_dev;
	uint16_t id;
} sht4x_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Function to initialize a SHT4x instance
 *
 * @param me       : Pointer to a sht4x_t instance
 * @param i2c_bus  : Pointer to a structure with the data to initialize the
 * 								   I2C device
 * @param dev_addr : I2C device address
 * @param read     : Pointer to I2C read function
 * @param write    : Pointer to I2C write function
 *
 * @return ESP_OK on success
 */
esp_err_t sht4x_init(sht4x_t *const me, i2c_bus_t *i2c_bus, uint8_t dev_addr,
		                 i2c_bus_read_t read, i2c_bus_write_t write);

/**
 * @brief Function for a single shot measurement with high repeatability.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_measure_high_precision(sht4x_t *const me, float *temperature,
		                                   float *humidity);

/**
 * @brief Function for a single shot measurement with medium repeatability.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_measure_medium_precision(sht4x_t *const me, float *temperature,
		                                     float *humidity);

/**
 * @brief Function for a single shot measurement with lowest repeatability.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_measure_lowest_precision(sht4x_t *const me, float *temperature,
		                                     float *humidity);

/**
 * @brief Function to activate highest heater power and perform a single
 * shot high precision measurement for 1s.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_activate_highest_heater_power_long(sht4x_t *const me,
		                                               float *temperature,
																									 float *humidity);

/**
 * @brief Function to activate highest heater power and perform a single
 * shot high precision measurement for 0.1s.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_activate_highest_heater_power_short(sht4x_t *const me,
		                                                float *temperature,
																										float *humidity);

/**
 * @brief Function to activate medium heater power and perform a single
 * shot high precision measurement for 1s.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_activate_medium_heater_power_long(sht4x_t *const me,
		                                              float *temperature,
																									float *humidity);

/**
 * @brief Function to activate medium heater power and perform a single
 * shot high precision measurement for 0.1s.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_activate_medium_heater_power_short(sht4x_t *const me,
		                                               float *temperature,
																									 float *humidity);

/**
 * @brief Function to activate lowest heater power and perform a single
 * shot high precision measurement for 1s.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_activate_lowest_heater_power_long(sht4x_t *const me,
		                                              float *temperature,
																									float *humidity);

/**
 * @brief Function to activate lowest heater power and perform a single
 * shot high precision measurement for 0.1s.
 *
 * @param me          : Pointer to a sht4x_t instance
 * @param temperature : Temperature in degrees centigrade.
 * @param humidity    : Humidity in percent relative humidity.
 *
 * @return 0 on success, an error code otherwise
 */
esp_err_t sht4x_activate_lowest_heater_power_short(sht4x_t *const me,
		                                               float *temperature,
																									 float *humidity);

/**
 * @brief Function for a single shot measurement with high repeatability.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    :Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_measure_high_precision_ticks(sht4x_t *const me,
		                                         uint16_t *temperature_ticks,
																						 uint16_t *humidity_ticks);

/**
 * @brief Function for a single shot measurement with medium repeatability.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    : Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_measure_medium_precision_ticks(sht4x_t *const me,
		                                           uint16_t *temperature_ticks,
																							 uint16_t *humidity_ticks);

/**
 * @brief Function for a single shot measurement with lowest repeatability.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    : Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_measure_lowest_precision_ticks(sht4x_t *const me,
		                                           uint16_t *temperature_ticks,
																							 uint16_t *humidity_ticks);

/**
 * @brief Function to activate highest heater power and perform a single shot high
 * precision measurement for 1s.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    : Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_activate_highest_heater_power_long_ticks(sht4x_t *const me,
		                                                     uint16_t *temperature_ticks,
																												 uint16_t *humidity_ticks);

/**
 * @brief Function to activate highest heater power and perform a single
 * shot high precision measurement for 0.1s.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    : Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_activate_highest_heater_power_short_ticks(sht4x_t *const me,
		                                                      uint16_t *temperature_ticks,
																													uint16_t *humidity_ticks);

/**
 * @brief Function to activate medium heater power and perform a single
 * shot high precision measurement for 1s.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    : Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */

esp_err_t sht4x_activate_medium_heater_power_long_ticks(sht4x_t *const me,
		                                                    uint16_t *temperature_ticks,
																												uint16_t *humidity_ticks);

/**
 * @brief Function to activate medium heater power and perform a single
 * shot high precision measurement for 0.1s.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    : Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_activate_medium_heater_power_short_ticks(sht4x_t *const me,
		                                                     uint16_t *temperature_ticks,
																												 uint16_t *humidity_ticks);

/**
 * @brief Function to activate lowest heater power and perform a single
 * shot high precision measurement for 1s.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    : Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_activate_lowest_heater_power_long_ticks(sht4x_t *const me,
		                                                    uint16_t *temperature_ticks,
																												uint16_t *humidity_ticks);

/**
 * @brief Function to activate lowest heater power and perform a single
 * shot high precision measurement for 0.1s.
 *
 * @param me                     : Pointer to a sht4x_t instance
 * @param[out] temperature_ticks : Temperature ticks. Convert to degrees celsius
 * by (175 * value / 65535) - 45
 * @param[out] humidity_ticks    : Humidity ticks. Convert to degrees celsius by
 * (125 * value / 65535) - 6
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_activate_lowest_heater_power_short_ticks(sht4x_t *const me,
		                                                     uint16_t *temperature_ticks,
																												 uint16_t *humidity_ticks);

/**
 * @brief Read out the serial number
 *
 * @param me                 : Pointer to a sht4x_t instance
 * @param[out] serial_number : Unique serial number
 *
 * @note Each sensor has a unique serial number that is assigned by Sensirion
 * during production.It is stored in the one-time-programmable memory and cannot
 * be manipulated after production.
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_get_serial_number(sht4x_t *const me, uint32_t *serial_number);

/**
 * @brief Perform a soft reset.
 *
 * @param me : Pointer to a sht4x_t instance
 *
 * @note A reset of the sensor can be achieved in three ways: • Soft reset: use
 * this function • I2C general call: all devices on the I2C bus are reset by
 * sending the command 0x06 to the I2C address 0x00. • Power down (incl.pulling
 * SCL and SDA low)
 *
 * @return error_code 0 on success, an error code otherwise.
 */
esp_err_t sht4x_soft_reset(sht4x_t *const me);

#ifdef __cplusplus
}
#endif

#endif /* SHT4X_H_ */

/***************************** END OF FILE ************************************/
