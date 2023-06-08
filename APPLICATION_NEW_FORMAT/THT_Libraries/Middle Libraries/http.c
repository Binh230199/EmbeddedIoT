/*
 * http.c
 *
 *  Created on: Mar 24, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "midlib.h"

extern SIM_HandleTypeDef *simPtr;
extern SENSOR_HandleTypeDef *sensorPtr;
extern POWER_HandleTypeDef *powerPtr;
extern SDCARD_HandleTypeDef *sdcardPtr;
extern CLOCK_HandleTypeDef *clockPtr;
extern CFG_HandleTypeDef *cfgPtr;
extern Telegram_HandleTypeDef *telePtr;

/**
 * @brief
 *
 */
void HTTP_process(void)
{
	SIM_HTTPTypeDef http = { 0 };
	/* ---------------1. Read data------------- */
	if (CLOCK_GET_FLAG(clockPtr, CLOCK_FLAG_RT))
	{
		SIM_wakeup(simPtr);
		/* Set new alarm to wake up MCU to read new data */
		CLOCK_setAlarm(clockPtr);

		/* Read the new sensor level value */
		SENSOR_setValue(sensorPtr);
		SENSOR_indecValueIndex(sensorPtr, 1);
		SENSOR_writeBKUPRegister(sensorPtr);

		/* Check whether time now is start of a day */
		if (CLOCK_IS_A_NEW_DAY(clockPtr))
		{
			char pathname[16] = { 0 };    // /2023/03/23.txt

			/* Get a new path file name to create new file stored data in a new day */
			CLOCK_createPathForSD(clockPtr, pathname, sizeof(pathname));

			SDCARD_setCurrentFile(sdcardPtr, pathname);
		}

		SIM_sleep(simPtr);

		/* Clear read time flag */
		CLOCK_CLR_FLAG(clockPtr, CLOCK_FLAG_RT);
	}

	/* ---------------2. Send data------------- */
	if (CLOCK_GET_FLAG(clockPtr, CLOCK_FLAG_ST))
	{
		SIM_wakeup(simPtr);

		HTTP_sendSensorWarn(&http);
		HTTP_sendCurrentData(&http);
		HTTP_sendLastData(&http);
		HTTP_sendBatteryWarn(&http);
		HTTP_sendGetConfig(&http);

		SIM_sleep(simPtr);

		CLOCK_setNextAlarm(clockPtr);
		CLOCK_setLastAlarm(clockPtr);

		CLOCK_CLR_FLAG(clockPtr, CLOCK_FLAG_ST);
	}
}

/**
 * @brief
 *
 * @param me
 * @param pURL
 * @param URLSize
 * @param URIType
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef HTTP_init(SIM_HTTPTypeDef *const me, const char *pURL, uint8_t URLSize, uint8_t URIType)
{
	if (me == NULL || pURL == NULL)
	{
		return THT_ERROR;
	}

	memset(me->url, '\0', sizeof(me->url));
	memset(me->type, '\0', sizeof(me->type));
	snprintf(me->url, sizeof(me->url), "%s", pURL);
	memcpy(me->type, HTTP_CONTENT_TYPE, strlen(HTTP_CONTENT_TYPE));
	me->method = SIM_HTTP_POST;

	return THT_OK;
}

THT_StatusTypeDef HTTP_checkResponse(SIM_HTTPTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	char *pStr = NULL;

	uint32_t warning = 0;

	/* Khi response phan hoi get config */
	pStr = strstr(me->response, "WART");
	if (pStr != NULL)
	{
		warning = atoi(pStr + 6);
	}
	/* Khi response phan hoi get firmware */

	pStr = strstr(me->response, "\"FW\":\"");
	if (pStr != NULL)
	{
		THT_copyStringUntilToken(pStr + 6, me->data, SIM_HTTP_DATA_SIZE, "\"}");
	}

	uint8_t levelWarning[SENSOR_NUMBER_SENSOR_WARNING] = { 0 };

	if (warning != levelWarning[0])
	{
		levelWarning[0] = warning; /* Warning trên server la muc warning 1 */
		SENSOR_setWarning(sensorPtr, levelWarning, SENSOR_NUMBER_SENSOR_WARNING);
		SENSOR_writeEEPROMs(sensorPtr);
	}

	return THT_OK;
}

THT_StatusTypeDef HTTP_sendCurrentData(SIM_HTTPTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Read level sensor data in value array string */
	char valuestring[60] = { 0 };

	SENSOR_indecValueIndex(sensorPtr, 0);
	SENSOR_getValuesString(sensorPtr, valuestring, sizeof(valuestring));

	/* Get sim data and enable send HTTP immediately */
	SIM_DataTypeDef simData = { 0 };

	SIM_getData(simPtr, SIM_DR_RSSI | SIM_DR_NWTYPE | SIM_DR_IMEI, &simData);

	/* Get time period string */
	char timestring[CLOCK_TIME_PERIOD_LEN] = { 0 };
	CLOCK_getPeriodTimeString(clockPtr, timestring, CLOCK_TIME_PERIOD_LEN);

	/* Set HTTP URI */
	const char *pURL = CFG_getURL(cfgPtr);
	HTTP_init(me, pURL, strlen(pURL), HTTP_URL_URI_RESULT);

	/*-------------Send HTTP--------------*/
	memset(me->data, '\0', SIM_HTTP_DATA_SIZE);
	snprintf(me->data, SIM_HTTP_DATA_SIZE, "{\"%s\":{"
			"\"%s\":\"%s\","
			"\"%s\":\"%s\","
			"\"%s\":\"%s\","
			"\"%s\":\"%s\","
			"\"%s\":[%s],"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":\"%s\""
			"}}\n", HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD, HTTP_JSON_VAL_RESULT, HTTP_JSON_KEY_ID,
			CFG_getDeviceID(cfgPtr), HTTP_JSON_KEY_IMEI, simData.imei, HTTP_JSON_KEY_TIME, timestring,
			HTTP_JSON_KEY_VALS, valuestring, HTTP_JSON_KEY_BATT, POWER_getBatPercent(powerPtr), HTTP_JSON_KEY_POWR, 1,
			HTTP_JSON_KEY_SIGS, simData.rssi, HTTP_JSON_KEY_NWTYPE, "LTE");

	SDCARD_logData(sdcardPtr, sdcardPtr->currentFile, me->data, strlen(me->data));
	if (THT_OK != SIM_sendHTTP(simPtr, me))
	{
		SDCARD_logData(sdcardPtr, sdcardPtr->currentFile, me->data, strlen(me->data));

		static uint16_t sendFailedPacketCounter = 0;

		sendFailedPacketCounter++;

		char reply[SIM_SMS_SIZE] = { 0 };

		snprintf(reply, sizeof(reply), "%s:%s:%s,%s:%s,%s:%s,%s:%s", HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD,
		HTTP_JSON_VAL_RESULT, HTTP_JSON_KEY_ID, CFG_getDeviceID(cfgPtr), HTTP_JSON_KEY_TIME, timestring,
		HTTP_JSON_KEY_VALS, valuestring);

		if (SIM_sendSMS(simPtr, CFG_getGateSend(cfgPtr), reply, 4, strlen(reply)) != THT_OK)
		{
			snprintf(reply, sizeof(reply), "%s: Send SMS to gate fail. %s:%s:%s,%s:%s,%s:%s,%s:%s",
					CFG_getDeviceID(cfgPtr), HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD, HTTP_JSON_VAL_RESULT,
					HTTP_JSON_KEY_ID, CFG_getDeviceID(cfgPtr), HTTP_JSON_KEY_TIME, timestring, HTTP_JSON_KEY_VALS,
					valuestring);
			TELEGRAM_sendMessage(telePtr, reply);
		}

		if (sendFailedPacketCounter >= 2)
		{
			SIM_init(simPtr);
			SIM_setFunction(simPtr, SIM_MODE_FULL);
			SIM_setLocalTimeAuto(simPtr);
			memset(reply, '\0', SIM_SMS_SIZE);
			snprintf(reply, sizeof(reply), "%s: Post fail %u times", CFG_getDeviceID(cfgPtr), sendFailedPacketCounter);
			telePtr->disableNotification = TELEGRAM_TRUE_STRING;
			if (THT_OK == TELEGRAM_sendMessage(telePtr, reply))
			{
				sendFailedPacketCounter = 0;
			}
		}
	}

	CLOCK_setLastAlarm(clockPtr);

	SENSOR_clearRainfallValue(sensorPtr);
	SENSOR_clearValueIndex(sensorPtr);

	return THT_OK;
}

THT_StatusTypeDef HTTP_sendLastData(SIM_HTTPTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	/* Set HTTP URI */
	const char *pURL = CFG_getURL(cfgPtr);
	HTTP_init(me, pURL, strlen(pURL), HTTP_URL_URI_RESULT);

	SDCARD_mount(sdcardPtr);
	if (SDCARD_getFileSize(sdcardPtr, SDCARD_LOST_FILE) == 0)
	{
		SDCARD_unmount(sdcardPtr);
		return THT_ERROR;
	}

	static uint32_t pRead = 0;
	uint32_t sizeOfFize = 0;

	/* Open file to read data */
	if (FR_OK == f_open(&USERFile, SDCARD_LOST_FILE, FA_READ))
	{
		/* Get size of file */
		sizeOfFize = USERFile.obj.objsize;

		memset(me->data, '\0', strlen(me->data));

		/* Move pRead to last processing pointer */
		f_lseek(&USERFile, pRead);

		/* Read and send */
		while (f_gets(me->data, SIM_HTTP_DATA_SIZE, &USERFile) != NULL)
		{
			if (THT_OK != SIM_sendHTTP(simPtr, me))
			{
				break;
			}
			pRead += strlen(me->data);
			memset(me, '\0', strlen(me->data));
		}

		/* Close file */
		f_close(&USERFile);

		/* Check pRead to delete and create new file */
		if (pRead == sizeOfFize)
		{
			f_unlink(SDCARD_LOST_FILE);
			f_open(&USERFile, SDCARD_LOST_FILE, FA_CREATE_ALWAYS | FA_WRITE);
			f_close(&USERFile);
			pRead = 0;
		}
	}

	SDCARD_unmount(sdcardPtr);

	return THT_OK;
}

THT_StatusTypeDef HTTP_sendBatteryWarn(SIM_HTTPTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	if (POWER_IS_ENABLE_WARNING(powerPtr))
	{
		POWER_SET_FLAG(powerPtr, POWER_FLAG_WARNEN);
	}
	if (POWER_IS_LOW_BATTERY(powerPtr))
	{
		POWER_SET_FLAG(powerPtr, POWER_FLAG_LOWBAT);
	}

	if (POWER_GET_FLAG(powerPtr, POWER_FLAG_LOWBAT | POWER_FLAG_WARNEN))
	{
		SIM_DataTypeDef simData = { 0 };
		SIM_getData(simPtr, SIM_DR_IMEI, &simData);
		/* Set HTTP URI */
		const char *pURL = CFG_getURL(cfgPtr);
		HTTP_init(me, pURL, strlen(pURL), HTTP_URL_URI_CMDS);

		uint8_t batteryPercent = POWER_getBatPercent(powerPtr);

		memset(me->data, '\0', SIM_HTTP_DATA_SIZE);
		snprintf(me->data, SIM_HTTP_DATA_SIZE, "{\"%s\":{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":%u}}",
		HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD, HTTP_JSON_VAL_BATCAP, HTTP_JSON_KEY_ID, CFG_getDeviceID(cfgPtr),
		HTTP_JSON_KEY_IMEI, simData.imei, HTTP_JSON_KEY_VAL, batteryPercent);

		if (THT_OK != SIM_sendHTTP(simPtr, me))
		{
			char *pGate = CFG_getGateSend(cfgPtr);
			char reply[SIM_SMS_SIZE] = { 0 };

			snprintf(reply, sizeof(reply), "%s:%s:%s,%s:%s,%s:%u", HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD,
			HTTP_JSON_VAL_WART, HTTP_JSON_KEY_ID, CFG_getDeviceID(cfgPtr), HTTP_JSON_KEY_VAL, batteryPercent);

			SIM_sendSMS(simPtr, pGate, reply, 4u, strlen(reply));

			memset(reply, '\0', SIM_SMS_SIZE);
			snprintf(reply, SIM_SMS_SIZE, "%s: Battery (%u%%) warning fail!", CFG_getDeviceID(cfgPtr), batteryPercent);
			telePtr->disableNotification = TELEGRAM_FALSE_STRING;
			TELEGRAM_sendMessage(telePtr, reply);
		}
		else
		{
			char reply[SIM_SMS_SIZE] = { 0 };
			snprintf(reply, SIM_SMS_SIZE, "%s: Battery (%u%%) warning done!", CFG_getDeviceID(cfgPtr), batteryPercent);
			telePtr->disableNotification = TELEGRAM_FALSE_STRING;
			TELEGRAM_sendMessage(telePtr, reply);
			POWER_CLR_FLAG(powerPtr, POWER_FLAG_WARNEN);
		}
	}

	return THT_OK;
}

THT_StatusTypeDef HTTP_sendSensorWarn(SIM_HTTPTypeDef *const me)    //TODO: kejp them dieu kien 1 tieng canh bao 1 lan
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	if (SENSOR_GET_WARN_LEVEL(sensorPtr) != SENSOR_SR_WARN_NONE)
	{
		enum
		{
			SMS_MSG_SIZE = 100
		};
		char msgSMS[SMS_MSG_SIZE] = { 0 };

		SIM_wakeup(simPtr);

		/* 3. Cảnh báo đến Server */
		SIM_DataTypeDef simData = { 0 };
		SIM_getData(simPtr, SIM_DR_IMEI, &simData);
		/* Set HTTP URI */
		const char *pURL = CFG_getURL(cfgPtr);
		HTTP_init(me, pURL, strlen(pURL), HTTP_URL_URI_CMDS);

		memset(me->data, '\0', SIM_HTTP_DATA_SIZE);
		snprintf(me->data, SIM_HTTP_DATA_SIZE, "{\"%s\":{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":%.1f}}",
		HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD, HTTP_JSON_VAL_WART, HTTP_JSON_KEY_ID, CFG_getDeviceID(cfgPtr),
		HTTP_JSON_KEY_IMEI, simData.imei, HTTP_JSON_KEY_VAL, SENSOR_getRainfallInHour(sensorPtr));

		if (THT_OK != SIM_sendHTTP(simPtr, me))
		{
			char *pGate = CFG_getGateSend(cfgPtr);
			char reply[160] = { 0 };

			snprintf(reply, sizeof(reply), "%s:%s:%s,%s:%s,%s:%.1f", HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD,
			HTTP_JSON_VAL_WART, HTTP_JSON_KEY_ID, CFG_getDeviceID(cfgPtr), HTTP_JSON_KEY_VAL,
					SENSOR_getRainfallInHour(sensorPtr));

			SIM_sendSMS(simPtr, pGate, reply, 4u, strlen(reply));
			memset(reply, '\0', SIM_SMS_SIZE);
			snprintf(reply, SIM_SMS_SIZE, "%s: %s. Sent fail!", CFG_getDeviceID(cfgPtr), msgSMS);
			telePtr->disableNotification = TELEGRAM_FALSE_STRING;
			TELEGRAM_sendMessage(telePtr, reply);
		}
		else
		{
			char reply[SIM_SMS_SIZE] = { 0 };
			snprintf(reply, SIM_SMS_SIZE, "%s: %s. Sent done!", CFG_getDeviceID(cfgPtr), msgSMS);
			telePtr->disableNotification = TELEGRAM_FALSE_STRING;
			TELEGRAM_sendMessage(telePtr, reply);
			SENSOR_CLR_FLAG(sensorPtr, SENSOR_FLAG_WARN);
		}

		SIM_sleep(simPtr);
	}

	return THT_OK;
}

THT_StatusTypeDef HTTP_sendGetConfig(SIM_HTTPTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	if (CLOCK_IS_TIME_GETCFG(clockPtr))
	{
		SIM_DataTypeDef simData = { 0 };
		SIM_getData(simPtr, SIM_DR_IMEI | SIM_DR_TIME, &simData);
		SIM_setLocalTimeAuto(simPtr);
		CLOCK_setTime(clockPtr, simData.timezone, strlen(simData.timezone));
		/* Set HTTP URI */
		const char *pURL = CFG_getURL(cfgPtr);
		HTTP_init(me, pURL, strlen(pURL), HTTP_URL_URI_CMDS);

		memset(me->data, '\0', SIM_HTTP_DATA_SIZE);
		snprintf(me->data, SIM_HTTP_DATA_SIZE, "{\"%s\":{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"}}",
		HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD, HTTP_JSON_VAL_GETCFG, HTTP_JSON_KEY_ID, CFG_getDeviceID(cfgPtr),
		HTTP_JSON_KEY_IMEI, simData.imei);

		if (THT_OK == SIM_sendHTTP(simPtr, me))
		{
			return HTTP_checkResponse(me);
		}
	}

	return THT_ERROR;
}

THT_StatusTypeDef HTTP_sendGetFirmwareURL(SIM_HTTPTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
//	const char *pURL = CFG_getURL(cfgPtr);
//	HTTP_init(&http, pURL, strlen(pURL), HTTP_URL_URI_CMDS);

	memcpy(me->type, HTTP_CONTENT_TYPE, strlen(HTTP_CONTENT_TYPE));
	me->method = SIM_HTTP_GET;
	char url[] = "https://http-post-61534-default-rtdb.firebaseio.com/DOMUCNUOC/GETFW/-NJOfAXmrWHHuA9ATGxL.json";
	memcpy(me->url, url, strlen(url));

	if (THT_OK == SIM_sendHTTP(simPtr, me))
	{
		HTTP_checkResponse(me);
		return THT_OK;
	}

	return THT_ERROR;
}

THT_StatusTypeDef HTTP_downloadFile(SIM_HTTPTypeDef *const me, const char *pURL, const char *pFilename)
{
	if (me == NULL || pFilename == NULL)
	{
		return THT_ERROR;
	}

	return SIM_getFileHTTP(simPtr, pURL, pFilename);
}

