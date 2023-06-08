/*
 * level_sensor.c
 *
 *  Created on: Mar 26, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "level_sensor.h"

#include <stdio.h>

extern RTC_HandleTypeDef hrtc;

/**
 * @brief  Get water level
 * @param[in]  me  Pointer to a SENSOR_HandleTypeDef structure.
 * @retval Water level in milimeter
 */
float SENSOR_getRainfallInHour(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return 0;
	}

	return me->pulseCounterInHour * me->resolution;
}

/**
 * @brief Call this function in HAL_GPIO_EXTI_Callback
 *
 * @param me
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SENSOR_callback(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	me->pulseCounterInPeriod[me->valueIndex]++;
	me->pulseCounterInHour++;

	if ((me->warning[0] / me->resolution) <= me->pulseCounterInHour)
	{
		SENSOR_SET_FLAG(me, SENSOR_FLAG_WARN);
		HAL_PWR_DisableSleepOnExit();
	}

	uint32_t i = 0;
	for (i = 0; i < 20000; i++);
	while (!HAL_GPIO_ReadPin(me->ragPort, me->ragPin) & (i < 20000))
	{
		i++;
	}
	for (i = 0; i < 20000; i++);

	EXTI->PR |= me->ragPin;
	return THT_OK;
}

/**
 * @brief  Initializes the level sensor module
 * @param me [in] Pointer to a SENSOR_HandleTypeDef structure.
 * @retval THT_StatusTypeDef status
 */
THT_StatusTypeDef SENSOR_process(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return THT_ERROR;
}

THT_StatusTypeDef SENSOR_setWarning(SENSOR_HandleTypeDef *const me, uint8_t *pValue, size_t size)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	me->warning[0] = *pValue;
	return THT_OK;
}

/**
 * @brief Set the value for rainfall array
 * 
 * @param me 
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SENSOR_setValue(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	me->rainfallValueInPeriod[me->valueIndex] = me->pulseCounterInPeriod[me->valueIndex] * me->resolution;

	return THT_OK;
}

THT_StatusTypeDef SENSOR_getValuesString(SENSOR_HandleTypeDef *const me, char pBuffer[], uint16_t size)
{
	if (me == NULL || pBuffer == NULL || size == 0u)
	{
		return THT_ERROR;
	}

	switch (me->valueIndex)
	{
		case 0u:
			snprintf(pBuffer, size, "%.1f", me->rainfallValueInPeriod[0]);
			break;
		case 1u:
			snprintf(pBuffer, size, "%.1f,%.1f", me->rainfallValueInPeriod[0], me->rainfallValueInPeriod[1]);
			break;
		case 2u:
			snprintf(pBuffer, size, "%.1f,%.1f,%.1f", me->rainfallValueInPeriod[0], me->rainfallValueInPeriod[1],
					me->rainfallValueInPeriod[2]);
			break;
		case 3u:
			snprintf(pBuffer, size, "%.1f,%.1f,%.1f,%.1f", me->rainfallValueInPeriod[0], me->rainfallValueInPeriod[1],
					me->rainfallValueInPeriod[2], me->rainfallValueInPeriod[3]);
			break;
		case 4u:
			snprintf(pBuffer, size, "%.1f,%.1f,%.1f,%.1f,%.1f", me->rainfallValueInPeriod[0],
					me->rainfallValueInPeriod[1], me->rainfallValueInPeriod[2], me->rainfallValueInPeriod[3],
					me->rainfallValueInPeriod[4]);
			break;
		case 5u:
			snprintf(pBuffer, size, "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", me->rainfallValueInPeriod[0],
					me->rainfallValueInPeriod[1], me->rainfallValueInPeriod[2], me->rainfallValueInPeriod[3],
					me->rainfallValueInPeriod[4], me->rainfallValueInPeriod[5]);
			break;
		default:
			snprintf(pBuffer, size, "%d", -1);
			break;
	}

	return THT_OK;
}

uint8_t SENSOR_getValueIndex(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return 0;
	}

	return me->valueIndex;
}

THT_StatusTypeDef SENSOR_clearValueIndex(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	me->valueIndex = 0;

	return THT_OK;
}

/**
 * @brief
 * @param [in]
 * @param incOrDec [in] 0 dec, else inc
 * @reval
 */
THT_StatusTypeDef SENSOR_indecValueIndex(SENSOR_HandleTypeDef *const me, uint8_t incOrDec)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	if (incOrDec == 0)
	{
		me->valueIndex--;
	}
	else
	{
		me->valueIndex++;
	}

	return THT_OK;
}

THT_StatusTypeDef SENSOR_clearRainfallValue(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	for (size_t i = 0u; i < (sizeof(me->rainfallValueInPeriod) / sizeof(me->rainfallValueInPeriod[0])); i++)
	{
		me->rainfallValueInPeriod[i] = 0;
		me->pulseCounterInPeriod[i] = 0;
	}

	return THT_OK;
}

THT_StatusTypeDef SENSOR_readBKUPRegister(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	me->valueIndex = RTC->BKP0R;

	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef sDate = { 0 };
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	if (sTime.Hours == RTC->BKP8R && sDate.Date == RTC->BKP9R)
	{
		if (me->valueIndex > 5)
		{
			me->valueIndex = 0;
		}
		size_t position = 0x40002854UL;
		for (size_t i = 0; i <= me->valueIndex; i++)
		{
			me->rainfallValueInPeriod[i] = (uint32_t) (*(__IO uint32_t*) (position + i * 4u));
		}
	}
	else
	{
		me->valueIndex = 0;
	}
	return THT_OK;
}

THT_StatusTypeDef SENSOR_writeBKUPRegister(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR8, sTime.Hours);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR9, sDate.Date);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, me->valueIndex);

	size_t position = RTC_BKP_DR1;
	for (size_t i = 0; i <= me->valueIndex; i++)
	{
		HAL_RTCEx_BKUPWrite(&hrtc, position++, me->rainfallValueInPeriod[i]);
	}

	return THT_OK;
}

/**
 * @brief
 * @param[in]
 * @reval
 */
THT_StatusTypeDef SENSOR_readEEPROMs(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

//	uint32_t warningStoredInDataEEPROM[SENSOR_NUMBER_SENSOR_WARNING] = { 0 };
//	/* Read section from EEPROM */
//	warningStoredInDataEEPROM[0] = *(volatile uint32_t*) SENSOR_EEPROM_WARNING_SENSOR_1;
//	warningStoredInDataEEPROM[1] = *(volatile uint32_t*) SENSOR_EEPROM_WARNING_SENSOR_2;
//	warningStoredInDataEEPROM[2] = *(volatile uint32_t*) SENSOR_EEPROM_WARNING_SENSOR_3;

	return THT_OK;
}

THT_StatusTypeDef SENSOR_writeEEPROMs(SENSOR_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	uint32_t warningStoredInDataEEPROM[SENSOR_NUMBER_SENSOR_WARNING] = { 0 };

	/* Read section from EEPROM */
	warningStoredInDataEEPROM[0] = *(volatile uint32_t*) SENSOR_EEPROM_WARNING_SENSOR_1;
	warningStoredInDataEEPROM[1] = *(volatile uint32_t*) SENSOR_EEPROM_WARNING_SENSOR_2;
	warningStoredInDataEEPROM[2] = *(volatile uint32_t*) SENSOR_EEPROM_WARNING_SENSOR_3;

	/* Check if current values is different new values, then
	 * write new values to EEPROM data
	 */
	if ((warningStoredInDataEEPROM[0] != me->warning[0]) || (warningStoredInDataEEPROM[1] != me->warning[1])
			|| (warningStoredInDataEEPROM[2] != me->warning[2]))
	{
		HAL_FLASHEx_DATAEEPROM_Unlock();
		HAL_FLASHEx_DATAEEPROM_EnableFixedTimeProgram();

		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD,
		SENSOR_EEPROM_WARNING_SENSOR_1);
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD,
		SENSOR_EEPROM_WARNING_SENSOR_2);
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD,
		SENSOR_EEPROM_WARNING_SENSOR_3);

		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
		SENSOR_EEPROM_WARNING_SENSOR_1, me->warning[0]);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
		SENSOR_EEPROM_WARNING_SENSOR_2, me->warning[1]);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
		SENSOR_EEPROM_WARNING_SENSOR_3, me->warning[2]);

		HAL_FLASHEx_DATAEEPROM_DisableFixedTimeProgram();
		HAL_FLASHEx_DATAEEPROM_Lock();
	}

	return THT_OK;
}

#if TEST
/**
 * @brief
 * @param[in]
 * @reval
 */
THT_StatusTypeDef SENSOR_test(SENSOR_HandleTypeDef * const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	printf("==============\n"
			"SENSOR TEST: %.1f\n"
			"0. Unkonwn\n"
			"1. MD10\n"
			"2. EWG\n"
			"3. GX909\n"
			"4. Reserved\n"
			"==============\n", SENSOR_GET_TYPEOF_SENSOR(me) >> SENSOR_SR_TYPESEN_Pos);

	for (size_t i = 0; i < 6; i++)
	{
		SENSOR_setValue(me);
		char valueString[50] = {0};
		SENSOR_getValuesString(me, valueString, sizeof(valueString));
		printf("%s\n", valueString);
		HAL_Delay(1000);
		SENSOR_indecValueIndex(me, 1);
	}
	char valueString[50] = {0};
	SENSOR_getConfig(me,
			SENSOR_SR_GCFG_GX909_MESHCFG | SENSOR_SR_GCFG_MD10_MANUID | SENSOR_SR_GCFG_EWG_SECTION,
			valueString, sizeof(valueString));
	printf("%s\n", valueString);
	SENSOR_clearRainfallValue(me);
	SENSOR_clearValueIndex(me);
	return THT_OK;
}
/* Test result
	420
	420,420
	420,420,420
	420,420,420,420
	420,420,420,420,420
	420,420,420,420,420,420
 */

#endif
