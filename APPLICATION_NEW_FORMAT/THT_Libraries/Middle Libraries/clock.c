/*
 * clock.c
 *
 *  Created on: Mar 23, 2023
 *      Author: Hoàng Văn Bình
 *      Gmail: binhhv.23.1.99@gmail.com
 */

#include "clock.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 *
 */

/**
 * @brief  Initializes the CLOCK according to the specified parameters in
 *         CLOCK_HandleTypeDef
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure that contains the
 *                 configuration information and data for the CLOCK module.
 * @retval None
 */
THT_StatusTypeDef CLOCK_init(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	/** Initialize RTC Only
	 */
	me->hrtc->Instance = RTC;
	me->hrtc->Init.HourFormat = RTC_HOURFORMAT_24;
	me->hrtc->Init.AsynchPrediv = 127;
	me->hrtc->Init.SynchPrediv = 255;
	me->hrtc->Init.OutPut = RTC_OUTPUT_DISABLE;
	me->hrtc->Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	me->hrtc->Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(me->hrtc) != HAL_OK)
	{
//	    Error_Handler(); /* Do not uncomment */
	}

	/** Initialize RTC and set the Time and Date
	 */
	me->sTime.Hours = 0x0;
	me->sTime.Minutes = 0x0;
	me->sTime.Seconds = 0x0;
	me->sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	me->sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(me->hrtc, &me->sTime, RTC_FORMAT_BCD) != HAL_OK)
	{
//	    Error_Handler(); /* Do not uncomment */
	}
	me->sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	me->sDate.Month = RTC_MONTH_JANUARY;
	me->sDate.Date = 0x1;
	me->sDate.Year = 0x0;

	if (HAL_RTC_SetDate(me->hrtc, &me->sDate, RTC_FORMAT_BCD) != HAL_OK)
	{
//	    Error_Handler(); /* Do not uncomment */
	}
	/** Enable the Alarm A
	 */
	me->sAlarm.AlarmTime.Hours = 0x0;
	me->sAlarm.AlarmTime.Minutes = 0x0;
	me->sAlarm.AlarmTime.Seconds = 0x0;
	me->sAlarm.AlarmTime.SubSeconds = 0x0;
	me->sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	me->sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	me->sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS;
	me->sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	me->sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	me->sAlarm.AlarmDateWeekDay = 0x1;
	me->sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(me->hrtc, &me->sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
//	    Error_Handler(); /* Do not uncomment */
	}
	return THT_OK;
}

/**
 * @brief  Check and set CLOCK flag
 * @Note   Call this function in the HAL_RTC_AlarmAEventCallback()
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval None
 */
THT_StatusTypeDef CLOCK_callback(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	HAL_RTC_GetDate(me->hrtc, &me->sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(me->hrtc, &me->sTime, RTC_FORMAT_BIN);

	/* Update time and date*/
	me->clockTime[1].hour = me->sTime.Hours;
	me->clockTime[1].minute = me->sTime.Minutes;
	me->clockTime[1].second = me->sTime.Seconds;

	me->clockDate[1].year = me->sDate.Year;
	me->clockDate[1].month = me->sDate.Month;
	me->clockDate[1].date = me->sDate.Date;

	/* Check time now is start of hour ? */
	if (CLOCK_IS_START_HOUR(me))
	{
		/* Set CLOCK_FLAG_STH flag */
		CLOCK_SET_FLAG(me, CLOCK_FLAG_STH);
	}

	/* Check time now is time to read data ? */
	if (CLOCK_IS_READ_TIME(me))
	{
		/* Set CLOCK_FLAG_RT flag */
		CLOCK_SET_FLAG(me, CLOCK_FLAG_RT);
	}

	/* Check time now is time to send data ? */
	if (CLOCK_IS_SEND_TIME(me))
	{
		/* Set CLOCK_FLAG_ST flag */
		CLOCK_SET_FLAG(me, CLOCK_FLAG_ST);
	}
	return THT_OK;
}

/**
 * @brief  Set Current Time
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[in]  timestring Pointer to a string literal that is time string
 * @param[in]  size The length of time string
 * @retval None
 */
THT_StatusTypeDef CLOCK_setTime(CLOCK_HandleTypeDef *const me, const char *timestring, uint8_t size)
{
	if (me == NULL || timestring == NULL || size < CLOCK_TIME_STRING_LEN)
	{
		return THT_ERROR;
	}

	me->clockDate[1].year = atoi(timestring);
	me->clockDate[1].month = atoi(timestring + 3u);
	me->clockDate[1].date = atoi(timestring + 6u);
	me->clockTime[1].hour = atoi(timestring + 9u);
	me->clockTime[1].minute = atoi(timestring + 12u);
	me->clockTime[1].second = atoi(timestring + 15u);

	/**
	 * Initialize RTC and set the Time and Date
	 */
	me->sTime.Hours = RTC_ByteToBcd2(me->clockTime[1].hour);
	me->sTime.Minutes = RTC_ByteToBcd2(me->clockTime[1].minute);
	me->sTime.Seconds = RTC_ByteToBcd2(me->clockTime[1].second);
	me->sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	me->sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(me->hrtc, &me->sTime, RTC_FORMAT_BCD) != HAL_OK)
	{
//	      Error_Handler(); /* Do not uncomment - Lỗi có thể xảy ra khi sử dụng hàm HAL_PWR_Enable_AccessBkup và Disable*/
		printf("Set time error!\n");
	}
	me->sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	me->sDate.Month = RTC_ByteToBcd2(me->clockDate[1].month);
	me->sDate.Date = RTC_ByteToBcd2(me->clockDate[1].date);
	me->sDate.Year = RTC_ByteToBcd2(me->clockDate[1].year);

	if (HAL_RTC_SetDate(me->hrtc, &me->sDate, RTC_FORMAT_BCD) != HAL_OK)
	{
//	      Error_Handler(); /* Do not uncomment */
		printf("Set date error!\n");
	}
	return THT_OK;
}

/**
 * @brief  Set Next Alarm
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval None
 */
THT_StatusTypeDef CLOCK_setAlarm(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	HAL_RTC_GetTime(me->hrtc, &me->sTime, RTC_FORMAT_BIN);

	/* Set next minute */
	if (((me->sTime.Minutes / me->readPeriod) + 1) * me->readPeriod > (uint8_t) LAST_MINUTE_OF_HOUR)
	{
		me->sAlarm.AlarmTime.Minutes = FIRST_MINUTE_OF_HOUR;
	}
	else
	{
		me->sAlarm.AlarmTime.Minutes = ((me->sTime.Minutes / me->readPeriod) + 1) * me->readPeriod;
	}

	me->sAlarm.AlarmTime.Hours = 0x0;
	me->sAlarm.AlarmTime.Minutes = RTC_ByteToBcd2(me->sAlarm.AlarmTime.Minutes);
	me->sAlarm.AlarmTime.Seconds = 0x0;
	me->sAlarm.AlarmTime.SubSeconds = 0x0;
	me->sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	me->sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	me->sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS;
	me->sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	me->sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	me->sAlarm.AlarmDateWeekDay = 0x1;
	me->sAlarm.Alarm = RTC_ALARM_A;

	if (HAL_OK != HAL_RTC_SetAlarm_IT(me->hrtc, &me->sAlarm, RTC_FORMAT_BCD))
	{
		//	      Error_Handler(); /* Do not uncomment */
		printf("Set alarm error!\n");
	}
	return THT_OK;
}

/**
 * @brief  Set the reset time
 *         The template is: 20:30:19 23-03-2023
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval None
 */
THT_StatusTypeDef CLOCK_setResetTime(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	HAL_RTC_GetDate(me->hrtc, &me->sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(me->hrtc, &me->sTime, RTC_FORMAT_BIN);

	/* Update reset time and date*/
	me->clockTime[0].hour = me->sTime.Hours;
	me->clockTime[0].minute = me->sTime.Minutes;
	me->clockTime[0].second = me->sTime.Seconds;

	me->clockDate[0].year = me->sDate.Year;
	me->clockDate[0].month = me->sDate.Month;
	me->clockDate[0].date = me->sDate.Date;

	return THT_OK;
}

/**
 * @brief  Get the reset time in string format
 *         The template is: 20:30:19 23-03-2023
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[out] pData The pointer to an array contains the time string
 * @param[in]  size The size of array must be more than 20 elements
 * @retval None
 */
THT_StatusTypeDef CLOCK_getResetTimeString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size)
{
	if (size < 20 || me == NULL)
	{
		return THT_ERROR;
	}

	snprintf(pData, size, "%02d:%02d:%02d %02d-%02d-20%02d", me->clockTime[0].hour, me->clockTime[0].minute,
			me->clockTime[0].second, me->clockDate[0].date, me->clockDate[0].month, me->clockDate[0].year);

	return THT_OK;
}

/**
 * @brief  Get the current time in string format
 *         The template is: 20:30:19 23-03-2023
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[out] pData The pointer to an array contains the time string
 * @param[in]  size The size of array must be more than 20 elements
 * @retval None
 */
THT_StatusTypeDef CLOCK_getCurrentTimeString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size)
{
	if (size < 20 || me == NULL)
	{
		return THT_ERROR;
	}

	HAL_RTC_GetDate(me->hrtc, &me->sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(me->hrtc, &me->sTime, RTC_FORMAT_BIN);

	snprintf(pData, size, "%02d:%02d:%02d %02d-%02d-20%02d", me->sTime.Hours, me->sTime.Minutes, me->sTime.Seconds,
			me->sDate.Date, me->sDate.Month, me->sDate.Year);

	return THT_OK;
}

/**
 * @brief  Set the last alarm
 *         The template is: 20:30:19 23-03-2023
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval None
 */
THT_StatusTypeDef CLOCK_setLastAlarm(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	HAL_RTC_GetTime(me->hrtc, &me->sTime, RTC_FORMAT_BIN);

	/* Update reset time and date*/
	me->clockTime[2].hour = me->sTime.Hours;
	me->clockTime[2].minute = me->sTime.Minutes;
	me->clockTime[2].second = me->sTime.Seconds;

	return THT_OK;
}

/**
 * @brief  Get the last alarm time in string format
 *         The template is: 20:30:19
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[out] pData The pointer to an array contains the time string
 * @param[in]  size The size of array must be more than 8 elements
 * @retval None
 */
THT_StatusTypeDef CLOCK_getLastAlarmString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size)
{
	if (size < 8 || me == NULL)
	{
		return THT_ERROR;
	}

	snprintf(pData, size, "%02d:%02d:%02d", me->clockTime[2].hour, me->clockTime[2].minute, me->clockTime[2].second);

	return THT_OK;
}

/**
 * @brief  Set the next alarm (next time is time to send data, distinc with time to read)
 *         The template is: 20:30:19 23-03-2023
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval None
 */
THT_StatusTypeDef CLOCK_setNextAlarm(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	HAL_RTC_GetTime(me->hrtc, &me->sTime, RTC_FORMAT_BIN);

	/* Set next minute */
	if (((me->sTime.Minutes / me->sendPeriod) + 1) * me->sendPeriod > (uint8_t) LAST_MINUTE_OF_HOUR)
	{
		me->clockTime[3].minute = FIRST_MINUTE_OF_HOUR;
	}
	else
	{
		me->clockTime[3].minute = ((me->sTime.Minutes / me->sendPeriod) + 1) * me->sendPeriod;
	}

	/* Set next hour */
	if (me->clockTime[3].minute != 0u)
	{
		me->clockTime[3].hour = me->sTime.Hours;
	}
	else
	{
		if ((me->sTime.Hours + 1) > (uint8_t) LAST_HOUR_OF_DAY)
		{
			me->clockTime[3].hour = FIRST_HOUR_OF_DAY;
		}
		else
		{
			me->clockTime[3].hour = me->sTime.Hours + 1;
		}
	}

	/* Set next second */
	me->clockTime[3].second = 0u;

	return THT_OK;
}

/**
 * @brief  Get the next alarm time in string format
 *         The template is: 20:30:19
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[out] pData The pointer to an array contains the time string
 * @param[in]  size The size of array must be more than 8 elements
 * @retval None
 */
THT_StatusTypeDef CLOCK_getNextAlarmString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size)
{
	if (size < 8 || me == NULL)
	{
		return THT_ERROR;
	}

	snprintf(pData, size, "%02d:%02d:%02d", me->clockTime[3].hour, me->clockTime[3].minute, me->clockTime[3].second);

	return THT_OK;
}

/**
 * @brief  Get the period time in string format
 *         The template is: 20:30-20:40 23-03-2023
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[out] pData The pointer to an array contains the time string
 * @param[in]  size The size of array must be more than 22 elements
 * @retval None
 */
THT_StatusTypeDef CLOCK_getPeriodTimeString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size)
{
	if (me == NULL || size < CLOCK_TIME_PERIOD_LEN)
	{
		return THT_ERROR;
	}

	HAL_RTC_GetTime(me->hrtc, &me->sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(me->hrtc, &me->sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(me->hrtc, &me->sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(me->hrtc, &me->sDate, RTC_FORMAT_BIN);

	me->clockTime[1].hour = me->sTime.Hours;
	me->clockTime[1].minute = me->sTime.Minutes;
	me->clockTime[1].second = me->sTime.Seconds;

	me->clockDate[1].year = me->sDate.Year;
	me->clockDate[1].month = me->sDate.Month;
	me->clockDate[1].date = me->sDate.Date;

	snprintf(pData, size, "%02d:%02d-%02d:%02d %02d-%02d-20%02d",
			me->clockTime[1].minute == FIRST_MINUTE_OF_HOUR ? me->clockTime[2].hour : me->sTime.Hours,
			me->clockTime[2].minute, me->clockTime[1].hour, me->clockTime[1].minute, me->clockDate[1].date,
			me->clockDate[1].month, me->clockDate[1].year);

	return THT_OK;
}

/**
 * @brief  Set send period time
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[in] value value of send period need to be set. It must be between 0 and 60
 * @retval None
 */
THT_StatusTypeDef CLOCK_setSendPeriod(CLOCK_HandleTypeDef *const me, uint8_t value)
{
	if (value <= 60u && value > 0u)
	{
		me->sendPeriod = value;
	}
	else
	{
		me->sendPeriod = 10u;
	}
	return THT_OK;
}

/**
 * @brief  Get read period time
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval value of send period
 */
uint8_t CLOCK_getSendPeriod(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return 10u;    //Be careful,  if return 0, an division by 0 may be occured
	}

	return me->sendPeriod;
}

/**
 * @brief  Set read period time
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[in] value value of read period need to be set. It must be between 0 and 60
 * @retval None
 */
THT_StatusTypeDef CLOCK_setReadPeriod(CLOCK_HandleTypeDef *const me, uint8_t value)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	if (value <= 60u && value > 0u)
	{
		me->readPeriod = value;
	}
	else
	{
		me->readPeriod = 10u;
	}

	return THT_OK;
}

/**
 * @brief  Get read period time
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval value of read period
 */
uint8_t CLOCK_getReadPeriod(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return 10u;    //Be careful, if return 0, an division by 0 may be occured
	}
	return me->readPeriod;
}

/**
 * @brief  Get the period time in string format
 *         The template is: /2023/03/23.txt
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @param[out] pData The pointer to an array contains the time string
 * @param[in]  size The size of array must be more than 15 elements
 * @retval None
 */
THT_StatusTypeDef CLOCK_createPathForSD(CLOCK_HandleTypeDef *const me, char pData[], uint8_t size)
{
	if (me == NULL || size < 15)
	{
		pData = '\0';
		return THT_ERROR;
	}

	if ((me->clockTime[1].hour == FIRST_HOUR_OF_DAY) || (pData[0] == 0))
	{
		snprintf(pData, size, "/20%02d/%02d/%02d.txt", me->clockDate[1].year, me->clockDate[1].month,
				me->clockDate[1].date);
	}
	else
	{
		pData = '\0';
	}

	return THT_OK;
}

/**
 * @brief  Read data config in EEPROM
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval None
 */
THT_StatusTypeDef CLOCK_readEPPROMs(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	/* Read section from EEPROM */
	uint8_t readPeriodStoredInDataEEPROM = *(volatile uint8_t*) CLOCK_READ_PERIOD;
	uint8_t sendPeriodStoredInDataEEPROM = *(volatile uint8_t*) CLOCK_SEND_PERIOD;

	/* Save section in the first run into EEPROM */
	if (readPeriodStoredInDataEEPROM == 0 || sendPeriodStoredInDataEEPROM == 0)
	{
		me->readPeriod = 10;
		me->sendPeriod = 10;
		CLOCK_writeEPPROMs(me);
	}
	/* Read section from EEPROM again */
	readPeriodStoredInDataEEPROM = *(volatile uint8_t*) CLOCK_READ_PERIOD;
	sendPeriodStoredInDataEEPROM = *(volatile uint8_t*) CLOCK_SEND_PERIOD;

	CLOCK_setReadPeriod(me, readPeriodStoredInDataEEPROM);
	CLOCK_setSendPeriod(me, sendPeriodStoredInDataEEPROM);

	return THT_OK;
}

/**
 * @brief  Read data config in EEPROM
 * @param[in]  me  Pointer to a CLOCK_HandleTypeDef structure.
 * @retval None
 */
THT_StatusTypeDef CLOCK_writeEPPROMs(CLOCK_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	/* Read section from EEPROM */
	uint8_t readPeriodStoredInDataEEPROM = *(volatile uint8_t*) CLOCK_READ_PERIOD;
	uint8_t sendPeriodStoredInDataEEPROM = *(volatile uint8_t*) CLOCK_SEND_PERIOD;

	/* Check if current values is different new values, then
	 * write new values to EEPROM data
	 */
	if ((readPeriodStoredInDataEEPROM != me->readPeriod) || (sendPeriodStoredInDataEEPROM != me->sendPeriod))
	{
		HAL_FLASHEx_DATAEEPROM_Unlock();
		HAL_FLASHEx_DATAEEPROM_EnableFixedTimeProgram();
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE, CLOCK_READ_PERIOD);
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE, CLOCK_SEND_PERIOD);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, CLOCK_READ_PERIOD, me->readPeriod);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, CLOCK_SEND_PERIOD, me->sendPeriod);
		HAL_FLASHEx_DATAEEPROM_DisableFixedTimeProgram();
		HAL_FLASHEx_DATAEEPROM_Lock();
	}

	return THT_OK;
}
