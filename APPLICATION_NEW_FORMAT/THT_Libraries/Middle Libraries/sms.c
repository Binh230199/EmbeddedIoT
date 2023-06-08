/*
 * sms.c
 *
 *  Created on: Mar 24, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx_hal.h"

#include "midlib.h"

extern SIM_HandleTypeDef *simPtr;
extern SENSOR_HandleTypeDef *sensorPtr;
extern POWER_HandleTypeDef *powerPtr;
extern SDCARD_HandleTypeDef *sdcardPtr;
extern CLOCK_HandleTypeDef *clockPtr;
extern CFG_HandleTypeDef *cfgPtr;
extern Telegram_HandleTypeDef *telePtr;

static THT_StatusTypeDef SMS_replySyntaxError(SIM_SMSTypeDef *const me)
{
	char *pMsg = "Kiem tra lai cu phap";

	return SIM_sendSMS(simPtr, me->phone, pMsg, strlen(me->phone), strlen(pMsg));
}

void SMS_process(void)
{
	if (SIM_GET_FLAG(simPtr, SIM_FLAG_SMS))
	{
		SIM_CLR_FLAG(simPtr, SIM_FLAG_SMS);

		SIM_wakeup(simPtr);

		int8_t smsCnt = SIM_getSMSCounter(simPtr);

		SIM_SMSTypeDef sms = { 0 };

		for (int8_t i = 0; i < smsCnt; i++)
		{
			SIM_readSMS(simPtr, i, &sms);

			if (THT_OK == SMS_checkNumber(&sms))
			{
				SMS_checkSyntax(&sms);
			}
			else
			{
				/*-------------Send HTTP--------------*/
				char reply[300] = { 0 };
				telePtr->disableNotification = TELEGRAM_TRUE_STRING;
				snprintf(reply, sizeof(reply), "%s: Invalid sender:\n%s\n%s\n%s", CFG_getDeviceID(cfgPtr), sms.phone,
						sms.time, sms.content);
				TELEGRAM_sendMessage(telePtr, reply);
			}
		}

		SIM_sleep(simPtr);
	}
}

THT_StatusTypeDef SMS_checkNumber(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	char debugNumber[SIM_PHONE_SIZE] = { 0 };
	SIM_readPhonebook(simPtr, SMS_DBPHONE_ORDER_8, debugNumber, strlen(debugNumber));

	if (strstr(me->phone, SMS_ADPHONE_1) != NULL || strstr(me->phone, SMS_ADPHONE_2) != NULL
			|| strstr(me->phone, SMS_ADPHONE_3) != NULL || strstr(me->phone, SMS_ADPHONE_4) != NULL
			|| strstr(me->phone, SMS_ADPHONE_5) != NULL || strstr(me->phone, SMS_ADPHONE_6) != NULL
			|| strstr(me->phone, SMS_ADPHONE_7) != NULL
			|| (strstr(me->phone, debugNumber) != NULL && debugNumber[0] != 0))
	{
		return THT_OK;
	}
	else
	{
		return THT_ERROR;
	}
}

THT_StatusTypeDef SMS_checkSyntax(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	char *smsHeader[SMS_NUMBER_SYNTAX] = { SMS_SYNTAX_DEBUG, SMS_SYNTAX_SEND,
	SMS_SYNTAX_SETCONFIG, SMS_SYNTAX_SETDEBUG, SMS_SYNTAX_SETGATE,
	SMS_SYNTAX_SETID, SMS_SYNTAX_SETURL, SMS_SYNTAX_SETTIME,
	SMS_SYNTAX_RESTORE, SMS_SYNTAX_RESET, SMS_SYNTAX_UPFW,
	SMS_SYNTAX_RESEND, SMS_SYNTAX_SETADMIN, SMS_SYNTAX_SETTOKEN, SMS_SYNTAX_SETCHATID };

	THT_StatusTypeDef (*sms[SMS_NUMBER_SYNTAX])(SIM_SMSTypeDef *const me) =
	{
		SMS_debug, SMS_send, SMS_setConfig, SMS_setDebug,
		SMS_setGate, SMS_setID, SMS_setURL, SMS_setTime,
		SMS_restore, SMS_reset, SMS_update, SMS_resend,
		SMS_setAdmin, SMS_setToken, SMS_setChatID
	};

	for (size_t i = 0; i < SMS_NUMBER_SYNTAX; i++)
	{
		if (strstr(me->content, smsHeader[i]) != NULL)
		{
			return sms[i](me);
		}
	}

	return THT_ERROR;
}

THT_StatusTypeDef SMS_debug(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	CLOCK_DataTypeDef clockData = { 0 };
	SIM_DataTypeDef simData = { 0 };
	char debugNumber[SIM_PHONE_SIZE] = { 0 };
	char reply[SIM_SMS_RESPONSE_SIZE];

	uint16_t curVersion[3] = { 0 };

	CLOCK_getResetTimeString(clockPtr, clockData.resetTime, CLOCK_TIME_STRING_LEN);
	CLOCK_getCurrentTimeString(clockPtr, clockData.timeNow, CLOCK_TIME_STRING_LEN);
	CLOCK_getLastAlarmString(clockPtr, clockData.lastAlarm, CLOCK_ALARM_STRING_LEN);
	CLOCK_getNextAlarmString(clockPtr, clockData.nextAlarm, CLOCK_ALARM_STRING_LEN);

	SIM_getData(simPtr, SIM_DR_RSSI | SIM_DR_NWTYPE | SIM_DR_TIME | SIM_DR_IMEI | SIM_DR_TEMP, &simData);

	SIM_readPhonebook(simPtr, SMS_DBPHONE_ORDER_8, debugNumber, SIM_PHONE_SIZE);

	FOTA_getCurrentVersion(curVersion, 3);
	uint8_t mfdDate = *(volatile uint8_t*) FOTA_EEPROM_MFD_DATE;
	uint8_t mfdMonth = *(volatile uint8_t*) FOTA_EEPROM_MFD_MONTH;
	uint16_t mfdYear = *(volatile uint8_t*) FOTA_EEPROM_MFD_YEAR + 2000;

	snprintf(reply, SIM_SMS_RESPONSE_SIZE, "%s\n"
			"-RESET: %s\n"
			"-LAST ALARM: %s\n"
			"-NEXT ALARM: %s\n"
			"-TIMENOW: %s\n"
			"-OWNNUM: %s\n"
			"-DBNUM: %s\n"
			"-IMEI: %s\n"
			"-GS: %s, GR: %s\n"
			"-PWR: %s\n"
			"-BAT: %.2fV\n"
			"-TEMP: %d\n"
			"-URL: %s\n"
			"-PSEND: %d\n"
			"-STARTHOUR: %u\n"
			"-WARNING: 50\n"
			"-RESOLU: 0.2\n"
			"-FWADD: %lx\n"
			"-FWVER: %u.%u.%u\n"
			"-SD: Write %s\n"
			"-SIGS: %ddBm\n"
			"-NWTYPE: %s\n"
			"-DATEMFE: %u/%u/%u", CFG_GET_HEADER_STRING(cfgPtr), clockData.resetTime, clockData.lastAlarm,
			clockData.nextAlarm, clockData.timeNow, CFG_getDeviceID(cfgPtr), debugNumber, simData.imei,
			CFG_getGateSend(cfgPtr), CFG_getGateRead(cfgPtr), "Good", POWER_getBatVoltage(powerPtr), simData.temp,
			CFG_getURL(cfgPtr), CLOCK_getSendPeriod(clockPtr), CFG_GET_START_HOUR(cfgPtr), FOTA_getCurrentAddress(),
			curVersion[0], curVersion[1], curVersion[2], SDCARD_IS_WRITE_OK(sdcardPtr), simData.rssi, simData.network,
			mfdDate, mfdMonth, mfdYear);

	return SIM_sendSMS(simPtr, me->phone, reply, strlen(me->phone), strlen(reply));
}

THT_StatusTypeDef SMS_send(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Read level sensor data in value array string */
	char valuestring[60] = { 0 };

	SENSOR_setValue(sensorPtr);
	SENSOR_getValuesString(sensorPtr, valuestring, sizeof(valuestring));
	SENSOR_clearRainfallValue(sensorPtr);
	SENSOR_clearValueIndex(sensorPtr);
	/* Get sim data and enable send HTTP immediately */
	SIM_DataTypeDef simData = { 0 };
	SIM_HTTPTypeDef http = { 0 };

	SIM_getData(simPtr, SIM_DR_RSSI | SIM_DR_NWTYPE | SIM_DR_IMEI, &simData);

	/* Read battery */
	uint8_t batteryPercent = POWER_getBatPercent(powerPtr);
	uint8_t powerState = 0;

	powerState = (THT_OK == POWER_getPowerState(powerPtr)) ? 1 : 0;

	/* Get time period string */
	char timestring[CLOCK_TIME_PERIOD_LEN] = { 0 };

	CLOCK_getPeriodTimeString(clockPtr, timestring, CLOCK_TIME_PERIOD_LEN);

	CLOCK_setLastAlarm(clockPtr);

	/* Set HTTP URI */
	const char *pURL = CFG_getURL(cfgPtr);
	HTTP_init(&http, pURL, strlen(pURL), HTTP_URL_URI_RESULT);

	/*-------------Send HTTP--------------*/
	snprintf(http.data, SIM_HTTP_DATA_SIZE, "{\"%s\":{"
			"\"%s\":\"%s\","
			"\"%s\":\"%s\","
			"\"%s\":\"%s\","
			"\"%s\":\"%s\","
			"\"%s\":[%s],"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":\"%s\""
			"}}\n", HTTP_JSON_KEY_HEADER, HTTP_JSON_KEY_CMD,
	HTTP_JSON_VAL_RESULT, HTTP_JSON_KEY_ID, CFG_getDeviceID(cfgPtr),
	HTTP_JSON_KEY_IMEI, simData.imei, HTTP_JSON_KEY_TIME, timestring,
	HTTP_JSON_KEY_VALS, valuestring, HTTP_JSON_KEY_BATT, batteryPercent, HTTP_JSON_KEY_POWR, powerState,
	HTTP_JSON_KEY_SIGS, simData.rssi, HTTP_JSON_KEY_NWTYPE, simData.network);

	char reply[240] = { 0 };

	if (THT_OK != SIM_sendHTTP(simPtr, &http))
	{
		snprintf(reply, sizeof(reply), "DOMUA %s %s %s", CFG_GET_RESULT_KQD_STRING(cfgPtr), timestring, valuestring);
		char *gatePtr = CFG_getGateSend(cfgPtr);
		SIM_sendSMS(simPtr, gatePtr, reply, strlen(gatePtr), strlen(reply));
	}

	/*-------------Send SMS--------------*/
	memset(reply, '\0', sizeof(reply));

	snprintf(reply, sizeof(reply), "%s KQD %s %s &PhoneNumber=%s&Bat=%d&Powr=%d&SIGS=%d", CFG_GET_HEADER_STRING(cfgPtr),
			timestring, valuestring, CFG_getDeviceID(cfgPtr), batteryPercent, powerState, simData.rssi);

	return SIM_sendSMS(simPtr, me->phone, reply, strlen(me->phone), strlen(reply));
}

THT_StatusTypeDef SMS_setConfig(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	uint16_t msgLength = strlen(me->content);
	/* Check minimize sizeof msg is not less than 22u */
	if (msgLength < 22u)
	{
		return SMS_replySyntaxError(me);
	}

	uint8_t sendPeriod = 0;
	uint8_t readPeriod = 0;
	uint8_t warning[SENSOR_NUMBER_SENSOR_WARNING];

	char *pStr = strstr(me->content, "PERS");
	if (pStr != NULL)
	{
		sendPeriod = atoi(pStr + 5);
	}

	pStr = strstr(me->content, "PERM");
	if (pStr != NULL)
	{
		readPeriod = atoi(pStr + 5);
	}

	pStr = strstr(me->content, "WART");
	if (pStr != NULL)
	{
		warning[0] = atoi(pStr + 5);
	}

	char *pMsg = NULL;
	if (sendPeriod < readPeriod)
	{
		pMsg = "Kiem tra lai tham so (Pers >= Perm)";
	}
	else
	{
		pMsg = "Cau hinh thanh cong";
		CLOCK_setSendPeriod(clockPtr, sendPeriod);
		CLOCK_setReadPeriod(clockPtr, readPeriod);
		CLOCK_writeEPPROMs(clockPtr);
		SENSOR_setWarning(sensorPtr, warning, SENSOR_NUMBER_SENSOR_WARNING);
		SENSOR_writeEEPROMs(sensorPtr);
	}

	return SIM_sendSMS(simPtr, me->phone, pMsg, strlen(me->phone), strlen(pMsg));
}

THT_StatusTypeDef SMS_setDebug(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	uint16_t msgLength = strlen(me->content);
	/* Check minimize sizeof msg is not less than 18u */
	if (msgLength < 18u)
	{
		return SMS_replySyntaxError(me);
	}

	char *pMsg = NULL;
	if (THT_OK == SIM_findPhonebook(simPtr, me->content + 7))
	{
		pMsg = "So debug da ton tai";
	}
	else if (THT_OK == SIM_savePhonebook(simPtr, SMS_DBPHONE_ORDER_8, me->content + 7, strlen(me->content + 7)))
	{
		pMsg = "So debug da duoc them";
	}
	else
	{
		pMsg = "Cau hinh khong thanh cong";
	}

	return SIM_sendSMS(simPtr, me->phone, pMsg, strlen(me->phone), strlen(pMsg));
}

THT_StatusTypeDef SMS_setGate(SIM_SMSTypeDef *const me)
{    //*SETGATE#8088,7769
	if (me == NULL)
	{
		return THT_ERROR;
	}

	uint16_t msgLength = strlen(me->content);
	/* Check minimize sizeof msg is not less than 18u */
	if (msgLength != 18u)
	{
		return SMS_replySyntaxError(me);
	}

	char gateSend[CFG_GATE_SIZE] = { 0 };
	char gateRead[CFG_GATE_SIZE] = { 0 };

	THT_copyStringUntilToken(me->content + 9, gateSend, CFG_GATE_SIZE, ",");
	THT_copyStringUntilToken(me->content + 14, gateRead, CFG_GATE_SIZE, NULL);

	CFG_setGate(cfgPtr, gateSend, gateRead, strlen(gateSend), strlen(gateRead));
	CFG_writeEEPROM(cfgPtr);

	char reply[50] = { 0 };
	snprintf(reply, sizeof(reply), "Gate send: %s, Gate read: %s", CFG_getGateSend(cfgPtr), CFG_getGateRead(cfgPtr));

	return SIM_sendSMS(simPtr, me->phone, reply, strlen(me->phone), strlen(reply));
}

THT_StatusTypeDef SMS_setID(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	uint16_t msgLength = strlen(me->content);
	/* Check minimize sizeof msg is not less than 18u */
	if (msgLength < 18u && msgLength >= 20u)
	{
		return SMS_replySyntaxError(me);
	}

	char ID[CFG_DEVICE_ID_SIZE] = { 0 };

	THT_copyStringUntilToken(me->content + 7, ID, CFG_DEVICE_ID_SIZE, NULL);

	CFG_setDeviceID(cfgPtr, ID, strlen(ID));

	CFG_writeEEPROM(cfgPtr);

	char reply[50] = { 0 };
	snprintf(reply, sizeof(reply), "So ID da cap nhat: %s", CFG_getDeviceID(cfgPtr));

	return SIM_sendSMS(simPtr, me->phone, reply, strlen(me->phone), strlen(reply));
}

THT_StatusTypeDef SMS_setURL(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	uint16_t msgLength = strlen(me->content);
	/* Check minimize sizeof msg is more than 80u */
	if (msgLength > 80u)
	{
		return SMS_replySyntaxError(me);
	}

	char URL[CFG_HTTP_URL_SIZE] = { 0 };

	THT_copyStringUntilToken(me->content + 8, URL, CFG_HTTP_URL_SIZE, NULL);

	CFG_setURL(cfgPtr, URL, strlen(URL));
	CFG_writeEEPROM(cfgPtr);

	char *pMsg = "URL da cap nhat";

	return SIM_sendSMS(simPtr, me->phone, pMsg, strlen(me->phone), strlen(pMsg));
}

THT_StatusTypeDef SMS_setTime(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	SIM_DataTypeDef simData = { 0 };

	SIM_setLocalTimeAuto(simPtr);

	SIM_getData(simPtr, SIM_DR_TIME, &simData);

	uint8_t timeLength = strlen(simData.timezone);

	if (timeLength == 0)
	{
		CLOCK_setTime(clockPtr, me->time, strlen(me->time));
		SIM_setLocalTimeManual(simPtr, me->time, strlen(me->time));

		SIM_getData(simPtr, SIM_DR_TIME, &simData);

		timeLength = strlen(simData.timezone);

		if (timeLength == 0)
		{
			char *pMsg = "Cap nhat thoi gian that bai";

			return SIM_sendSMS(simPtr, me->phone, pMsg, strlen(me->phone), strlen(pMsg));
		}
	}

	CLOCK_setAlarm(clockPtr);

	char reply[50] = { 0 };
	snprintf(reply, sizeof(reply), "Thoi gian hien tai: %s", simData.timezone);

	return SIM_sendSMS(simPtr, me->phone, reply, strlen(me->phone), strlen(reply));
}

THT_StatusTypeDef SMS_restore(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	SIM_DataTypeDef simData = { 0 };
	SIM_getData(simPtr, SIM_DR_SIMPHONE, &simData);

	CFG_setDeviceID(cfgPtr, simData.simPhone, strlen(simData.simPhone));
	CFG_setGate(cfgPtr, CFG_GATE_SEND_DEFAULT, CFG_GATE_READ_DEFAULT, strlen(CFG_GATE_SEND_DEFAULT),
			strlen(CFG_GATE_READ_DEFAULT));
	char *urlPtr = CFG_HTTP_URL_DEFAULT(cfgPtr);
	CFG_setURL(cfgPtr, urlPtr, strlen(urlPtr));
	CLOCK_setSendPeriod(clockPtr, CFG_PERIOD_SEND_DEFAULT(cfgPtr));
	CLOCK_setReadPeriod(clockPtr, CFG_PERIOD_READ_DEFAULT(cfgPtr));

	CFG_writeEEPROM(cfgPtr);
	CLOCK_writeEPPROMs(clockPtr);
	SENSOR_writeEEPROMs(sensorPtr);

	char *pMsg = "Cau hinh da duoc dua ve mac dinh";

	return SIM_sendSMS(simPtr, me->phone, pMsg, strlen(me->phone), strlen(pMsg));
}

THT_StatusTypeDef SMS_reset(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	FOTA_HandleTypeDef hfota = { 0 };

	FOTA_SET_FLAG(&hfota, FOTA_FLAG_RST);

	FOTA_writeEPPROMs(&hfota);

	char *pMsg = "Thiet bi dang khoi dong lai! Vui long cho";
	SIM_sendSMS(simPtr, me->phone, pMsg, strlen(me->phone), strlen(pMsg));

	NVIC_SystemReset();

	return THT_OK;
}

THT_StatusTypeDef SMS_update(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	FOTA_HandleTypeDef fota = { 0 };

	FOTA_process(&fota);

	char reply[200] = { 0 };

	switch ((fota.SR & FOTA_SR_STT_Msk))
	{
		case FOTA_FLAG_UPOK:
			snprintf(reply, sizeof(reply), "Cap nhat firmware thanh cong tai dia chi: %lX", FOTA_getCurrentAddress());
			SIM_sendSMS(simPtr, me->phone, reply, strlen(me->phone), strlen(reply));
			NVIC_SystemReset();
			break;
		case FOTA_FLAG_VERER:
		case FOTA_FLAG_NWSIG:
		case FOTA_FLAG_DOWER:
		case FOTA_FLAG_FFER:
		case FOTA_FLAG_WRIER:
		case FOTA_FLAG_CRCER:
		case FOTA_FLAG_RST:
		case FOTA_FLAG_GUER:
			snprintf(reply, sizeof(reply), "Cap nhat firmware khong thanh cong tai. Ma loi: %lX\n"
					"3.GET URL ERROR\n"
					"4.OLD VERSION or ILLEGAL\n"
					"5.BAD SIGNAL\n"
					"6.DOWNLOAD ERROR\n"
					"7.FORMAT ERROR OR OVERSIZE\n"
					"8.WRITE ERROR\n"
					"9.CRC ERROR", fota.SR);
			SIM_sendSMS(simPtr, me->phone, reply, strlen(me->phone), strlen(reply));
			break;
		default:
			break;
	}

	return THT_OK;
}

THT_StatusTypeDef SMS_resend(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	SIM_HTTPTypeDef http = { 0 };
	/* Set HTTP URI */
	const char *pURL = CFG_getURL(cfgPtr);
	HTTP_init(&http, pURL, strlen(pURL), HTTP_URL_URI_RESULT);

	/* SMS content: *RESEND#00:00-05:00 28-04-2022 */
	uint16_t msgLength = strlen(me->content);
	/* Check minimize sizeof msg is not less than 38u */
	if (msgLength != 30u)
	{
		return SMS_replySyntaxError(me);
	}

	char filename[16] = { 0 };

	uint8_t beginHour = atoi(me->content + 8);
	uint8_t endHour = atoi(me->content + 14);
	uint8_t day = atoi(me->content + 20);
	uint8_t month = atoi(me->content + 23);
	uint16_t year = atoi(me->content + 26);

	sprintf(filename, "/%d/%02d/%02d.txt", year, month, day);

	SDCARD_mount(sdcardPtr);
	FILINFO fno;
	char *pMsg = NULL;
	if (FR_OK == f_stat(filename, &fno))
	{
		if (FR_OK == f_open(&USERFile, filename, FA_READ))
		{
			char findInfo[5] = { 0 };
			bool acceptPost = false;

			while (f_gets(http.data, SIM_HTTP_DATA_SIZE, &USERFile) != NULL)
			{
				sprintf(findInfo, "%d:", beginHour);
				if (strstr(http.data, findInfo) != NULL)
				{
					acceptPost = true;
				}

				if (acceptPost == true)
				{
					SIM_sendHTTP(simPtr, &http);
				}

				sprintf(findInfo, "%d:", endHour);

				if (strstr(http.data, findInfo) != NULL)
				{
					break;
				}

				memset(http.data, '\0', strlen(http.data));
			}

			f_close(&USERFile);

			pMsg = "Da gui lai du lieu thanh cong";
		}
	}
	else
	{
		pMsg = "Khong co du lieu trong ngay duoc chon";
	}

	SDCARD_unmount(sdcardPtr);
	return SIM_sendSMS(simPtr, me->phone, pMsg, strlen(me->phone), strlen(pMsg));
}

THT_StatusTypeDef SMS_setAdmin(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* *SETAD#84969009900,840000000000,8888888 */
	uint16_t msgLength = strlen(me->content);

	/* Check minimize sizeof msg is not less than 22u */
	if (msgLength < 18u)
	{
		return SMS_replySyntaxError(me);
	}

	char *token = strtok(me->content, "#");
	token = strtok(NULL, ",");

	/* Start save at SMS_ADPHONE_ORDER_9 */
	uint8_t adminPhonebookPos = SMS_ADPHONE_ORDER_9;
	uint8_t saveSuccesful[5] = { 0 };
	uint8_t posSuccess = 0;
	while (token != NULL)
	{
		if (THT_OK == SIM_savePhonebook(simPtr, adminPhonebookPos++, token, strlen(token)))
		{
			saveSuccesful[posSuccess] = 1;
		}

		posSuccess++;

		token = strtok(NULL, ",");    //Moi chi luu duoc 4 vi tri
		if (token == NULL)
		{
			token = strtok(NULL, "");    //Tach so cuoi cung
		}
	}

	char phoneSaveSuccess[5][SIM_PHONE_SIZE] = { 0 };
	adminPhonebookPos = SMS_ADPHONE_ORDER_9;
	/* Phan hoi lai nhung so dien thoai da duoc set thanh cong */
	for (size_t i = 0; i < 5; i++)
	{
		if (1 == saveSuccesful[i])
		{
			SIM_readPhonebook(simPtr, adminPhonebookPos, phoneSaveSuccess[i],
			SIM_PHONE_SIZE);
		}
		adminPhonebookPos++;
	}

	char reply[SIM_SMS_SIZE] = { 0 };
	snprintf(reply, SIM_SMS_SIZE, "Save successful:\n%s\n%s\n%s\n%s\n%s", phoneSaveSuccess[0], phoneSaveSuccess[1],
			phoneSaveSuccess[2], phoneSaveSuccess[3], phoneSaveSuccess[4]);
	return SIM_sendSMS(simPtr, me->phone, reply, strlen(me->phone), strlen(reply));
}

THT_StatusTypeDef SMS_setToken(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	/* SMS content: *SETTOKEN#<46chars> */
	uint16_t msgLength = strlen(me->content);

	if (msgLength < 10u && msgLength > 57u)
	{
		return SMS_replySyntaxError(me);
	}

	char teleToken[TELEGRAM_TOKEN_LENGTH] = { 0 };
	THT_copyStringUntilToken(me->content + 10, teleToken,
	TELEGRAM_TOKEN_LENGTH - 1, NULL);

	TELEGRAM_setToken(telePtr, teleToken, strlen(teleToken));
	TELEGRAM_writeEEPROM(telePtr);

	return SIM_sendSMS(simPtr, me->phone, telePtr->token, strlen(me->phone), strlen(telePtr->token));
}

THT_StatusTypeDef SMS_setChatID(SIM_SMSTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	/* SMS content: *SETCID#<max 20 chars> */
	uint16_t msgLength = strlen(me->content);

	if (msgLength < 10u && msgLength > 57u)
	{
		return SMS_replySyntaxError(me);
	}

	char teleChatID[TELEGRAM_CHAT_ID_LENGTH] = { 0 };
	THT_copyStringUntilToken(me->content + 8, teleChatID,
	TELEGRAM_CHAT_ID_LENGTH, NULL);

	TELEGRAM_setChatID(telePtr, teleChatID, strlen(teleChatID));
	TELEGRAM_writeEEPROM(telePtr);
	return SIM_sendSMS(simPtr, me->phone, telePtr->chatID, strlen(me->phone), strlen(telePtr->chatID));
}
