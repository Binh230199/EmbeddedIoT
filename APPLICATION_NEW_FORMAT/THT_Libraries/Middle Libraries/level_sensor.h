/*
 * level_sensor.h
 *
 *  Created on: Mar 26, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_SENSOR_SENSOR_H_
#define INC_SENSOR_SENSOR_H_

#include "wlv_def.h"

#define SENSOR_BUFFER_SIZE 99U
#define SENSOR_NUMBER_SENSOR_WARNING 3u
#define SENSOR_MAX_ELEMENT 12u //(60/5)
typedef struct level
{
	GPIO_TypeDef *ragPort;
	uint16_t ragPin;
	volatile uint32_t SR;
	uint8_t warning[SENSOR_NUMBER_SENSOR_WARNING]; /* 3 mức cảnh báo */
	float rainfallValueInPeriod[SENSOR_MAX_ELEMENT];
	float rainfallValueInHour;
	volatile float pulseCounterInPeriod[SENSOR_MAX_ELEMENT];
	volatile float pulseCounterInHour;
	volatile uint8_t valueIndex;
	float resolution;    //Do phan giai, mm
} SENSOR_HandleTypeDef;

typedef THT_StatusTypeDef (*sensorInit)(struct level *const me);
typedef THT_StatusTypeDef (*sensorDeInit)(struct level *const me);
typedef uint32_t (*sensorGetRainfall)(struct level *const me);
typedef THT_StatusTypeDef (*sensorProcess)(struct level *const me, uint16_t size);

#define SENSOR_SR_WARN_Pos				(4U)
#define SENSOR_SR_WARN_Msk				(0x7UL << SENSOR_SR_WARN_Pos)
#define SENSOR_SR_WARN					SENSOR_SR_WARN_Msk

/* Warn level */
#define SENSOR_SR_WARN_NONE				(0x0UL << SENSOR_SR_WARN_Pos)
#define SENSOR_SR_WARN_SENSOR_1			(0x1UL << SENSOR_SR_WARN_Pos)
#define SENSOR_SR_WARN_SENSOR_2			(0x2UL << SENSOR_SR_WARN_Pos)
#define SENSOR_SR_WARN_SENSOR_3			(0x3UL << SENSOR_SR_WARN_Pos)

/**
 *		SENSOR flag definitions
 */
#define SENSOR_FLAG_WARN		SENSOR_SR_WARN
/**
 * @brief Get the SENSOR's flag status.
 * @param __ME__: SENSOR handle
 * @param __FLAG__: SENSOR flag
 * 			This parameter can be any combination of the following values:
 * 			@arg SENSOR_FLAG_RX: SENSOR Read rx flag
 * 			@arg SENSOR_FLAG_TYPESEN: SENSOR type of sensor flag
 * 			@arg SENSOR_FLAG_WARN: SENSOR warning flag
 * @retval None
 */
#define SENSOR_GET_FLAG(__ME__, __FLAG__)		\
			((((__ME__)->SR) & (__FLAG__)) == (__FLAG__))

/**
 * @brief Set the SENSOR's flag status.
 * @param __ME__: SENSOR handle
 * @param __FLAG__: SENSOR flag
 * 			This parameter can be any combination of the following values:
 * 			@arg SENSOR_FLAG_RX: SENSOR Read rx flag
 * 			@arg SENSOR_FLAG_TYPESEN: SENSOR type of sensor flag
 * 			@arg SENSOR_FLAG_WARN: SENSOR warning flag
 * @retval None
 */
#define SENSOR_SET_FLAG(__ME__, __FLAG__)		\
			(((__ME__)->SR) |= (__FLAG__))

/**
 * @brief Clear the SENSOR's flag status.
 * @param __ME__: SENSOR handle
 * @param __FLAG__: SENSOR flag
 * 			This parameter can be any combination of the following values:
 * 			@arg SENSOR_FLAG_RX: SENSOR Read rx flag
 * 			@arg SENSOR_FLAG_TYPESEN: SENSOR type of sensor flag
 * 			@arg SENSOR_FLAG_WARN: SENSOR warning flag
 * @retval None
 */
#define SENSOR_CLR_FLAG(__ME__, __FLAG__)		\
			(((__ME__)->SR) &= ~(__FLAG__))

#define SENSOR_EEPROM_WARNING_SENSOR_1	(THT_SENSOR_BASE)
#define SENSOR_EEPROM_WARNING_SENSOR_2	(SENSOR_EEPROM_WARNING_SENSOR_1 + 4)
#define SENSOR_EEPROM_WARNING_SENSOR_3	(SENSOR_EEPROM_WARNING_SENSOR_2 + 4)
#define SENSOR_EEPROM_WARNING_SENSOR_4	(SENSOR_EEPROM_WARNING_SENSOR_3 + 4)
#define SENSOR_EEPROM_WARNING_SENSOR_5	(SENSOR_EEPROM_WARNING_SENSOR_4 + 4)

#define SENSOR_GET_WARN_LEVEL(__ME__)		((__ME__)->SR & SENSOR_FLAG_WARN)

THT_StatusTypeDef SENSOR_init(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_deInit(SENSOR_HandleTypeDef *const me);
uint32_t SENSOR_getLevel(SENSOR_HandleTypeDef *const me);
float SENSOR_getRainfallInHour(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_callback(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_process(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_setWarning(SENSOR_HandleTypeDef *const me, uint8_t *pValue, size_t size);
THT_StatusTypeDef SENSOR_setValue(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_getValuesString(SENSOR_HandleTypeDef *const me, char pBuffer[], uint16_t size);
THT_StatusTypeDef SENSOR_readEEPROMs(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_writeEEPROMs(SENSOR_HandleTypeDef *const me);
uint8_t SENSOR_getValueIndex(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_clearValueIndex(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_indecValueIndex(SENSOR_HandleTypeDef *const me, uint8_t incOrDec);
THT_StatusTypeDef SENSOR_clearRainfallValue(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_readBKUPRegister(SENSOR_HandleTypeDef *const me);
THT_StatusTypeDef SENSOR_writeBKUPRegister(SENSOR_HandleTypeDef *const me);

#if TEST
THT_StatusTypeDef 	SENSOR_test(SENSOR_HandleTypeDef * const me);
#endif

#endif /* INC_SENSOR_SENSOR_H_ */
