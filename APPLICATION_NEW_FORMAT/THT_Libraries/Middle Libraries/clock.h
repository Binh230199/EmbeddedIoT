/*
 * clock.h
 *
 *  Created on: Mar 23, 2023
 *      Author: Hoàng Văn Bình
 *      Gmail: binhhv.23.1.99@gmail.com
 */

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#include "stm32l1xx_hal.h"

#include "wlv_def.h"

typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} ClockTime;

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t date;
} ClockDate;

typedef struct
{
	/* Clock status (software) register */
	volatile uint32_t SR;

	RTC_HandleTypeDef *hrtc;

	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	RTC_AlarmTypeDef sAlarm;

	/* clockTime: 4 elements:
	 * clockTime[0]: resetTime
	 * clockTime[1]: nowTime
	 * clockTime[2]: lastTime
	 * clockTime[3]: nextTime
	 */
	ClockTime clockTime[4];
	/* clockDate: 4 elements:
	 * clockDate[0]: resetDate
	 * clockDate[1]: nowDate
	 * clockDate[2]: lastDate
	 * clockDate[3]: nextDate
	 */
	ClockDate clockDate[4];

	/* Application period time */
	uint8_t readPeriod;
	uint8_t sendPeriod;
} CLOCK_HandleTypeDef;

#define CLOCK_SR_RT_Pos			(0U)		/* IsReadTime bit */
#define CLOCK_SR_RT_Msk			(0x1UL << CLOCK_SR_RT_Pos)
#define CLOCK_SR_RT				CLOCK_SR_RT_Msk

#define CLOCK_SR_ST_Pos			(1U)		/* IsSendTime bit */
#define CLOCK_SR_ST_Msk			(1 << CLOCK_SR_ST_Pos)
#define CLOCK_SR_ST				(CLOCK_SR_ST_Msk)

#define CLOCK_SR_STH_Pos		(2U)		/* Start Hour bit */
#define CLOCK_SR_STH_Msk		(1 << CLOCK_SR_STH_Pos)
#define CLOCK_SR_STH			(CLOCK_SR_STH_Msk)

#define CLOCK_SR_STD_Pos		(3U)		/* Is New Day bit */
#define CLOCK_SR_STD_Msk		(1 << CLOCK_SR_STD_Pos)
#define CLOCK_SR_STD			(CLOCK_SR_STD_Msk)

#define CLOCK_SR_TGCF_Pos		(4U)		/* Time Get Config bit */
#define CLOCK_SR_TGCF_Msk		(0x1UL << CLOCK_SR_TGCF_Pos)
#define CLOCK_SR_TGCF			(CLOCK_SR_TGCF_Msk)

#define PERIOD_GET_CONFIG			(4U)
#define FIRST_MINUTE_OF_HOUR		(00U)
#define FIRST_HOUR_OF_DAY			(00U)
#define FIRST_DAY_OF_MONTH			(01U)
#define FIRST_MONTH_OF_YEAR			(01U)
#define LAST_MINUTE_OF_HOUR			(59U)
#define LAST_HOUR_OF_DAY			(23U)
#define LAST_DAY_OF_MONTH			((28U) || (29U) || (30U) || (31U))
#define LAST_MONTH_OF_YEAR			(12U)

/*
 * The length of time string
 */
#define CLOCK_TIME_STRING_LEN		(20U)
#define CLOCK_TIME_PERIOD_LEN		(23U)
#define CLOCK_ALARM_STRING_LEN		(9U)
typedef struct
{
	char resetTime[CLOCK_TIME_STRING_LEN];
	char lastAlarm[CLOCK_ALARM_STRING_LEN];
	char nextAlarm[CLOCK_ALARM_STRING_LEN];
	char timeNow[CLOCK_TIME_STRING_LEN];
} CLOCK_DataTypeDef;

/*
 * Period time EEPROM address
 */
#define CLOCK_READ_PERIOD 				(THT_CLOCK_BASE)
#define CLOCK_SEND_PERIOD 				(CLOCK_READ_PERIOD + 1)

/**
 *		CLOCK flag definitions
 */
#define CLOCK_FLAG_RT			CLOCK_SR_RT
#define CLOCK_FLAG_ST			CLOCK_SR_ST
#define CLOCK_FLAG_STH			CLOCK_SR_STH
#define CLOCK_FLAG_STD			CLOCK_SR_STD
#define CLOCK_FLAG_TGCF			CLOCK_SR_TGCF

/**
 * @brief Get the CLOCK's flag status.
 * @param __ME__: CLOCK handle
 * @param __FLAG__: CLOCK flag
 * 			This parameter can be any combination of the following values:
 * 			@arg CLOCK_FLAG_RT: CLOCK Read time flag
 * 			@arg CLOCK_FLAG_ST: CLOCK Send time flag
 * 			@arg CLOCK_FLAG_STH: CLOCK Start of Hour flag
 * 			@arg CLOCK_FLAG_STD: CLOCK Start of Date flag
 * 			@arg CLOCK_FLAG_TGCF: CLOCK Time to get configuration
 * @retval None
 */
#define CLOCK_GET_FLAG(__ME__, __FLAG__)		\
			((((__ME__)->SR) & (__FLAG__)) == (__FLAG__))

/**
 * @brief Set the CLOCK's flag status.
 * @param __ME__: CLOCK handle
 * @param __FLAG__: CLOCK flag
 * 			This parameter can be any combination of the following values:
 * 			@arg CLOCK_FLAG_RT: CLOCK Read time flag
 * 			@arg CLOCK_FLAG_ST: CLOCK Send time flag
 * 			@arg CLOCK_FLAG_STH: CLOCK Start of Hour flag
 * 			@arg CLOCK_FLAG_STD: CLOCK Start of Date flag
 * 			@arg CLOCK_FLAG_TGCF: CLOCK Time to get configuration
 * @retval None
 */
#define CLOCK_SET_FLAG(__ME__, __FLAG__)		\
			(((__ME__)->SR) |= (__FLAG__))

/**
 * @brief Clear the CLOCK's flag status.
 * @param __ME__: CLOCK handle
 * @param __FLAG__: CLOCK flag
 * 			This parameter can be any combination of the following values:
 * 			@arg CLOCK_FLAG_RT: CLOCK Read time flag
 * 			@arg CLOCK_FLAG_ST: CLOCK Send time flag
 * 			@arg CLOCK_FLAG_STH: CLOCK Start of Hour flag
 * 			@arg CLOCK_FLAG_STD: CLOCK Start of Date flag
 * 			@arg CLOCK_FLAG_TGCF: CLOCK Time to get configuration
 * @retval None
 */
#define CLOCK_CLR_FLAG(__ME__, __FLAG__)		\
			(((__ME__)->SR) &= ~(__FLAG__))

/**
 * @brief Checks whether now is time to read (data)
 * @param __ME__: CLOCK handle
 * @retval None
 */
#define CLOCK_IS_READ_TIME(__ME__)					\
			(((((__ME__)->clockTime[1].minute) % ((__ME__)->readPeriod)) == 0))

/**
 * @brief Checks whether now is time to send (data)
 * @param __ME__: CLOCK handle
 * @retval None
 */
#define CLOCK_IS_SEND_TIME(__ME__)					\
			(((((__ME__)->clockTime[1].minute) % ((__ME__)->sendPeriod)) == 0))

/**
 * @brief Checks whether now is start of hour
 * @param __ME__: CLOCK handle
 * @retval None
 */
#define CLOCK_IS_START_HOUR(__ME__)					\
			(((__ME__)->clockTime[1].minute) == FIRST_MINUTE_OF_HOUR)

/**
 * @brief Checks whether now is start of date
 * @param __ME__: CLOCK handle
 * @retval None
 */
#define CLOCK_IS_A_NEW_DAY(__ME__)					\
			((((__ME__)->clockTime[1].hour) == 0U) && (CLOCK_GET_FLAG((__ME__), CLOCK_FLAG_STH)))

/**
 * @brief Checks whether now is time to get configuration
 * @param __ME__: CLOCK handle
 * @retval None
 */
#define CLOCK_IS_TIME_GETCFG(__ME__)				\
			(((__ME__)->clockTime[1].hour % PERIOD_GET_CONFIG == 0U) &&	\
			((__ME__)->clockTime[1].minute < (__ME__)->readPeriod))

/* CLOCK Functions ------------------------------------------------------*/
THT_StatusTypeDef CLOCK_init(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_callback(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_setTime(CLOCK_HandleTypeDef *const me, const char *timestring, uint8_t stringLen);
THT_StatusTypeDef CLOCK_setAlarm(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_setResetTime(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_getResetTimeString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size);
THT_StatusTypeDef CLOCK_getCurrentTimeString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size);
THT_StatusTypeDef CLOCK_setLastAlarm(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_getLastAlarmString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size);
THT_StatusTypeDef CLOCK_setNextAlarm(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_getNextAlarmString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size);
THT_StatusTypeDef CLOCK_getPeriodTimeString(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size);
THT_StatusTypeDef CLOCK_setSendPeriod(CLOCK_HandleTypeDef *const me, uint8_t value);
uint8_t CLOCK_getSendPeriod(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_setReadPeriod(CLOCK_HandleTypeDef *const me, uint8_t value);
uint8_t CLOCK_getReadPeriod(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_createPathForSD(CLOCK_HandleTypeDef *const me, char *pData, uint8_t size);
THT_StatusTypeDef CLOCK_readEPPROMs(CLOCK_HandleTypeDef *const me);
THT_StatusTypeDef CLOCK_writeEPPROMs(CLOCK_HandleTypeDef *const me);

#endif /* INC_CLOCK_H_ */
