/*
 * wlv_def.h
 *
 *  Created on: Mar 23, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_THT_DEF_H_
#define INC_THT_DEF_H_

#include "stm32l1xx_hal.h"
#include <stdint.h>
/**
 * @brief Water Level System Status
 */
typedef enum
{
	THT_OK,
	THT_ERROR,
	THT_BUSY,
	THT_TIMEOUT
} THT_StatusTypeDef;

#define THT_EEPROM_BASE		FLASH_EEPROM_BASE
#define THT_FOTA_BASE		(THT_EEPROM_BASE)
#define THT_CLOCK_BASE		(THT_FOTA_BASE + 0x80UL)
#define THT_CONFIG_BASE		(THT_CLOCK_BASE + 0x80UL)
#define THT_SENSOR_BASE		(THT_CONFIG_BASE + 0x80UL)
#define THT_TELE_BASE		(THT_SENSOR_BASE + 0x80UL)

#define TEST 0
void 				THT_copyStringUntilToken(const char *src, char des[], uint16_t size, const char *token);
THT_StatusTypeDef 	THT_convertDoubleToHex(double d, uint8_t pBuffer[], size_t sizeMustSetIs8);
uint8_t 			THT_Checksum8XOR(const uint8_t *array, size_t size);
THT_StatusTypeDef 	THT_CRC16(void);

#endif /* INC_THT_DEF_H_ */
