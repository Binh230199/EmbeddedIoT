/*
 * ewg.h
 *
 *  Created on: Apr 1, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_EWG_H_
#define INC_EWG_H_

#include "wlv_def.h"
#include "level_sensor.h"

/* Array offset for water level sensor */
#define EWG_SECTION_ARR_OFFSET	(1u)
/* Number of sections of water level sensor */
#define EWG_MAX_SECTION			(12u + EWG_SECTION_ARR_OFFSET)
/* Length per section */
#define LENGTH_OF_ONE_SECTION		40
/* Default address code */
#define EWG_ADDRESS_CODE_DEFAULT	0x10u
/* Read function code - Modbus standard */
#define EWG_FUNCTION_CODE			0x03u
/* Address to save section in EEPROM */
#define EWG_EEPROM_SECTION			(SENSOR_EEPROM_ANGLE + 1)

/**
 * @brief  EWG handle Structure definition
 */
typedef struct
{
	float resolution;
} RGS_HandleTypeDef;

THT_StatusTypeDef EWG_init(RGS_HandleTypeDef *const me);
THT_StatusTypeDef EWG_deInit(RGS_HandleTypeDef *const me);
THT_StatusTypeDef EWG_process(RGS_HandleTypeDef *const me, uint16_t size);
uint32_t EWG_getLevel(RGS_HandleTypeDef *const me);
THT_StatusTypeDef EWG_setSection(RGS_HandleTypeDef *const me, uint8_t section);
THT_StatusTypeDef EWG_getConfig(RGS_HandleTypeDef *const me, uint32_t configType, char pData[], uint16_t size);
THT_StatusTypeDef EWG_writeEEPROMs(RGS_HandleTypeDef *const me);

#endif /* INC_EWG_H_ */
