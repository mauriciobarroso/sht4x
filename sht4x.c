/**
  ******************************************************************************
  * @file           : sht4x.c
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

/* Includes ------------------------------------------------------------------*/
#include "sht4x.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

/* Private macros ------------------------------------------------------------*/
#define NOP() asm volatile ("nop")

/* External variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
static const char *TAG = "sht4x";

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief Function that implements the default I2C read transaction
 *
 * @param reg_addr : Register address to be read
 * @param reg_data : Pointer to the data to be read from reg_addr
 * @param data_len : Length of the data transfer
 * @param intf     : Pointer to the interface descriptor
 *
 * @return 0 if successful, non-zero otherwise
 */
static int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t data_len,
		                   void *intf);
/**
 * @brief Function that implements the default I2C write transaction
 *
 * @param reg_addr : Register address to be written
 * @param reg_data : Pointer to the data to be written to reg_addr
 * @param data_len : Length of the data transfer
 * @param intf     : Pointer to the interface descriptor
 *
 * @return 0 if successful, non-zero otherwise
 */
static int8_t i2c_write(uint8_t reg_addr, const uint8_t *reg_data,
		                    uint32_t data_len, void *intf);
/**
 * @brief Function that implements a micro seconds delay
 *
 * @param period_us: Time in us to delay
 */
static void delay_us(uint32_t period_us);

/**
 * @brief Function to convert temperature ticks to physical temperature
 *
 * @param ticks temperature in ticks
 *
 * @return Temperature in milli centigrade.
 */
static int32_t convert_ticks_to_celsius(uint16_t ticks);

/**
 * @brief Convert humidity ticks to physical humidity
 *
 * @param ticks relative humidity in ticks
 *
 * @return Humidity in milli percent relative humidity.
 */
static int32_t convert_ticks_to_percent_rh(uint16_t ticks);

/* Exported functions definitions --------------------------------------------*/
/**
 * @brief Function to initialize a SHT4x instance
 */
esp_err_t sht4x_init(sht4x_t *const me, i2c_bus_t *i2c_bus, uint8_t dev_addr,
		i2c_bus_read_t read, i2c_bus_write_t write) {
	/* Print initializing message */
	ESP_LOGI(TAG, "Initializing instance...");

	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Add device to bus */
	ret = i2c_bus_add_dev(i2c_bus, dev_addr, "sht4x", NULL, NULL);

	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to add device");
		return ret;
	}

	/**/
	me->i2c_dev = &i2c_bus->devs.dev[i2c_bus->devs.num - 1]; /* todo: write function to get the dev from name */

	/* Print successful initialization message */
	ESP_LOGI(TAG, "Instance initialized successfully");

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function for a single shot measurement with high repeatability.
 */
esp_err_t sht4x_measure_high_precision(sht4x_t *const me, int32_t *temperature,
		                                   int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_measure_high_precision_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function for a single shot measurement with medium repeatability.
 */
esp_err_t sht4x_measure_medium_precision(sht4x_t *const me, int32_t *temperature,
		                                     int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_measure_medium_precision_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);


	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function for a single shot measurement with lowest repeatability.
 */
esp_err_t sht4x_measure_lowest_precision(sht4x_t *const me, int32_t *temperature,
		                                     int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_measure_lowest_precision_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate highest heater power and perform a single
 * shot high precision measurement for 1s.
 */
esp_err_t sht4x_activate_highest_heater_power_long(sht4x_t *const me,
		                                               int32_t *temperature,
																									 int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_activate_highest_heater_power_long_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate highest heater power and perform a single
 * shot high precision measurement for 0.1s.
 */
esp_err_t sht4x_activate_highest_heater_power_short(sht4x_t *const me,
		                                                int32_t *temperature,
																										int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_activate_highest_heater_power_short_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate medium heater power and perform a single
 * shot high precision measurement for 1s.
 */
esp_err_t sht4x_activate_medium_heater_power_long(sht4x_t *const me,
		                                              int32_t *temperature,
																									int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_activate_medium_heater_power_long_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate medium heater power and perform a single
 * shot high precision measurement for 0.1s.
 */
esp_err_t sht4x_activate_medium_heater_power_short(sht4x_t *const me,
		                                               int32_t *temperature,
																									 int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_activate_medium_heater_power_short_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate lowest heater power and perform a single
 * shot high precision measurement for 1s.
 */
esp_err_t sht4x_activate_lowest_heater_power_long(sht4x_t *const me,
		                                              int32_t *temperature,
																									int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_activate_lowest_heater_power_long_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate lowest heater power and perform a single
 * shot high precision measurement for 0.1s.
 */
esp_err_t sht4x_activate_lowest_heater_power_short(sht4x_t *const me,
		                                               int32_t *temperature,
																									 int32_t *humidity) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	uint16_t temperature_ticks = 0;
	uint16_t humidity_ticks = 0;

	ret = sht4x_activate_lowest_heater_power_short_ticks(me, &temperature_ticks, &humidity_ticks);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Calculate physical temperature and humidity values */
	*temperature = convert_ticks_to_celsius(temperature_ticks);
	*humidity = convert_ticks_to_percent_rh(humidity_ticks);

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function for a single shot measurement with high repeatability.
 */
esp_err_t sht4x_measure_high_precision_ticks(sht4x_t *const me,
		                                         uint16_t *temperature_ticks,
																						 uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_MEASURE_HIGH_PRECISION_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(10 * 1000); /* Wait for 10 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function for a single shot measurement with medium repeatability.
 */
esp_err_t sht4x_measure_medium_precision_ticks(sht4x_t *const me,
		                                           uint16_t *temperature_ticks,
																							 uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_MEASURE_MEDIUM_PRECISION_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(5 * 1000); /* Wait for 5 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function for a single shot measurement with lowest repeatability.
 */
esp_err_t sht4x_measure_lowest_precision_ticks(sht4x_t *const me,
		                                           uint16_t *temperature_ticks,
																							 uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_MEASURE_LOWEST_PRECISION_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(2 * 1000); /* Wait for 2 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate highest heater power and perform a single
 * shot high precision measurement for 1s.
 */
esp_err_t sht4x_activate_highest_heater_power_long_ticks(sht4x_t *const me,
		                                                     uint16_t *temperature_ticks,
																												 uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_ACTIVATE_HIGHEST_HEATER_POWER_LONG_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(1100 * 1000); /* Wait for 1100 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate highest heater power and perform a single
 * shot high precision measurement for 0.1s.
 */
esp_err_t sht4x_activate_highest_heater_power_short_ticks(sht4x_t *const me,
		                                                      uint16_t *temperature_ticks,
																													uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_ACTIVATE_HIGHEST_HEATER_POWER_SHORT_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(110 * 1000); /* Wait for 110 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate medium heater power and perform a single
 * shot high precision measurement for 1s.
 */

esp_err_t sht4x_activate_medium_heater_power_long_ticks(sht4x_t *const me,
		                                                    uint16_t *temperature_ticks,
																												uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_ACTIVATE_MEDIUM_HEATER_POWER_LONG_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(1100 * 1000); /* Wait for 1100 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate medium heater power and perform a single
 * shot high precision measurement for 0.1s.
 */
esp_err_t sht4x_activate_medium_heater_power_short_ticks(sht4x_t *const me,
		                                                     uint16_t *temperature_ticks,
																												 uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_ACTIVATE_MEDIUM_HEATER_POWER_SHORT_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(110 * 1000); /* Wait for 110 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate lowest heater power and perform a single
 * shot high precision measurement for 1s.
 */
esp_err_t sht4x_activate_lowest_heater_power_long_ticks(sht4x_t *const me,
		                                                    uint16_t *temperature_ticks,
																												uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_ACTIVATE_LOWEST_HEATER_POWER_LONG_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(1100 * 1000); /* Wait for 1100 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to activate lowest heater power and perform a single
 * shot high precision measurement for 0.1s.
 */
esp_err_t sht4x_activate_lowest_heater_power_short_ticks(sht4x_t *const me,
		                                                     uint16_t *temperature_ticks,
																												 uint16_t *humidity_ticks) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature and humidity ticks */
	if (i2c_write(SHT4X_ACTIVATE_LOWEST_HEATER_POWER_SHORT_TICKS_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(110 * 1000); /* Wait for 110 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	*temperature_ticks = (uint16_t)((data[0] << 8) | (data[1]));
	*humidity_ticks = (uint16_t)((data[3] << 8) | (data[4]));

	/* todo: check crc */

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Read out the serial number
 */
esp_err_t sht4x_get_serial_number(sht4x_t *const me, uint32_t *serial_number) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Get temperature the serial number */
	if (i2c_write(SHT4X_SERIAL_NUMBER_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(10 * 1000); /* Wait for 10 ms */

	uint8_t data[6] = {0};
	if (i2c_read(0, data, 6, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	/* todo: check crc */

	*serial_number = (uint32_t)((data[0] << 8) | (data[1]) | (data[3] << 8) | (data[4]));

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Perform a soft reset.
 */
esp_err_t sht4x_soft_reset(sht4x_t *const me) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Perform a software reset */
	if (i2c_write(SHT4X_SOFT_RESET_CMD, NULL, 0, me->i2c_dev) < 0) {
		return ESP_FAIL;
	}

	delay_us(10 * 1000); /* Wait for 10 ms */

	/* Return ESP_OK */
	return ret;
}

/* Private function definitions ----------------------------------------------*/
/**
 * @brief Function that implements the default I2C read transaction
 */
static int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t data_len,
		                   void *intf) {
	i2c_bus_dev_t *dev = (i2c_bus_dev_t *)intf;

	return dev->read(reg_addr ? &reg_addr : NULL, reg_addr ? 1 : 0, reg_data, data_len, intf);
}

/**
 * @brief Function that implements the default I2C write transaction
 */
static int8_t i2c_write(uint8_t reg_addr, const uint8_t *reg_data,
		                    uint32_t data_len, void *intf) {
	i2c_bus_dev_t *dev = (i2c_bus_dev_t *)intf;

	return dev->write(&reg_addr, 1, reg_data, data_len, intf);
}

/**
 * @brief Function that implements a micro seconds delay
 */
static void delay_us(uint32_t period_us) {
	uint64_t m = (uint64_t)esp_timer_get_time();

  if (period_us) {
  	uint64_t e = (m + period_us);

  	if (m > e) { /* overflow */
  		while ((uint64_t)esp_timer_get_time() > e) {
  			NOP();
  		}
  	}

  	while ((uint64_t)esp_timer_get_time() < e) {
  		NOP();
  	}
  }
}

/**
 * @brief Convert temperature ticks to physical temperature
 */
static int32_t convert_ticks_to_celsius(uint16_t ticks) {
	return ((21875 * (int32_t)ticks) >> 13) - 45000;
}

/**
 * @brief Convert humidity ticks to physical humidity
 */
static int32_t convert_ticks_to_percent_rh(uint16_t ticks) {
	return ((15625 * (int32_t)ticks) >> 13) - 6000;
}

/***************************** END OF FILE ************************************/
