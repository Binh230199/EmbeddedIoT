/*
 * dev_config.c
 *
 *  Created on: Mar 26, 2023
 *      Author: Hoàng Văn Bình
 *      Gmail: binhhv.23.1.99@gmail.com
 */

#include "dev_config.h"

#include <string.h>

THT_StatusTypeDef CFG_setMode(CFG_HandleTypeDef *const me, CFG_ModeTypeDef mode)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	if (mode == CFG_MODE_10 || mode == CFG_MODE_60)
	{
		me->mode = mode;
		return THT_OK;
	}
	else
	{
		return THT_ERROR;
	}
}
/**
 * @brief
 * @param
 * @retval
 */
THT_StatusTypeDef CFG_setDeviceID(CFG_HandleTypeDef *const me, const char *pID, uint8_t size)
{
	if (me == NULL || pID == NULL || size < 11U || size >= CFG_DEVICE_ID_SIZE)
	{
		return THT_ERROR;
	}
	memset(me->deviceID, '\0', CFG_DEVICE_ID_SIZE);

	memcpy(me->deviceID, pID, size);
	me->deviceID[size] = '\0';

	return THT_OK;
}

/**
 * @brief
 * @param
 * @retval
 */
/* Should be passed strlen(gSend) and strlen(gRead)
 * Should not be passed sizeof(gSend) and sizeof(gRead)
 * because function check fixed size of gate number is 4 bytes
 */
THT_StatusTypeDef CFG_setGate(CFG_HandleTypeDef *const me, const char *pGSend, const char *pGRead, uint8_t gsSize,
		uint8_t grSize)
{
	if (me == NULL || pGSend == NULL || pGRead == NULL || gsSize < 4u || grSize < 4u)
	{
		return THT_ERROR;
	}
	memset(me->gateRead, '\0', CFG_GATE_SIZE);
	memset(me->gateSend, '\0', CFG_GATE_SIZE);

	memcpy(me->gateRead, pGRead, grSize);
	me->gateRead[grSize] = '\0';

	memcpy(me->gateSend, pGSend, gsSize);
	me->gateSend[gsSize] = '\0';

	return THT_OK;
}

/**
 * @brief
 * @param
 * @retval
 */
THT_StatusTypeDef CFG_setURL(CFG_HandleTypeDef *const me, const char *pURL, uint8_t size)
{
	if (me == NULL || pURL == NULL || size >= CFG_HTTP_URL_SIZE)
	{
		return THT_ERROR;
	}

	memset(me->httpURL, '\0', CFG_HTTP_URL_SIZE);

	memcpy(me->httpURL, pURL, size);
	me->httpURL[size] = '\0';

	return THT_OK;
}

/**
 * @brief
 * @param
 * @retval
 */
char* CFG_getDeviceID(CFG_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return '\0';
	}

	return me->deviceID;
}

/**
 * @brief
 * @param
 * @retval
 */
char* CFG_getGateSend(CFG_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return '\0';
	}

	return me->gateSend;
}

/**
 * @brief
 * @param
 * @retval
 */
char* CFG_getGateRead(CFG_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return '\0';
	}

	return me->gateRead;
}

/**
 * @brief
 * @param
 * @retval
 */
char* CFG_getURL(CFG_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return '\0';
	}

	return me->httpURL;
}

/**
 * @brief
 * @param
 * @retval
 */
THT_StatusTypeDef CFG_readEEPROM(CFG_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Read section from EEPROM */
	uint8_t modeStoredInDataEEPROM = 0;
	char gReadStoredInDataEEPROM[CFG_GATE_SIZE] = { 0 };
	char gSendStoredInDataEEPROM[CFG_GATE_SIZE] = { 0 };
	char URLStoredInDataEEPROM[CFG_HTTP_URL_SIZE] = { 0 };
	char deviceIDStoredInDataEEPROM[CFG_DEVICE_ID_SIZE] = { 0 };

	modeStoredInDataEEPROM = *(volatile uint8_t*) CFG_EEPROM_MODE;

	for (size_t i = 0; i < CFG_DEVICE_ID_SIZE - 1; i++)
	{
		deviceIDStoredInDataEEPROM[i] = *(volatile uint8_t*) (CFG_EEPROM_DEVICE_ID + i);
	}

	for (size_t i = 0; i < CFG_GATE_SIZE - 1; i++)
	{
		gReadStoredInDataEEPROM[i] = *(volatile uint8_t*) (CFG_EEPROM_GATE_READ + i);
		gSendStoredInDataEEPROM[i] = *(volatile uint8_t*) (CFG_EEPROM_GATE_SEND + i);
	}

	for (size_t i = 0; i < CFG_HTTP_URL_SIZE - 1; i++)
	{
		URLStoredInDataEEPROM[i] = *(volatile uint8_t*) (CFG_EEPROM_HTTP_URL + i);
	}

	CFG_setMode(me, modeStoredInDataEEPROM);

	if (deviceIDStoredInDataEEPROM[0] == 0)
	{
		CFG_setDeviceID(me, "841699774241", strlen("841699774241"));
	}
	else
	{
		CFG_setDeviceID(me, deviceIDStoredInDataEEPROM, strlen(deviceIDStoredInDataEEPROM));
	}

	if (gReadStoredInDataEEPROM[0] == 0 || gSendStoredInDataEEPROM[0] == 0)
	{
		CFG_setGate(me, CFG_GATE_SEND_DEFAULT, CFG_GATE_READ_DEFAULT, strlen(CFG_GATE_SEND_DEFAULT),
				strlen(CFG_GATE_READ_DEFAULT));
	}
	else
	{
		CFG_setGate(me, gSendStoredInDataEEPROM, gReadStoredInDataEEPROM, strlen(gSendStoredInDataEEPROM),
				strlen(gReadStoredInDataEEPROM));
	}

	if (URLStoredInDataEEPROM[0] == 0)
	{
		CFG_setURL(me, CFG_HTTP_URL_DEFAULT_VER10, strlen(CFG_HTTP_URL_DEFAULT_VER10));
	}
	else
	{
		CFG_setURL(me, URLStoredInDataEEPROM, strlen(URLStoredInDataEEPROM));
	}

	return THT_OK;
}

/**
 * @brief
 * @param
 * @retval
 */
THT_StatusTypeDef CFG_writeEEPROM(CFG_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Read section from EEPROM */
	uint8_t modeStoredInDataEEPROM = 0;
	char gReadStoredInDataEEPROM[CFG_GATE_SIZE] = { 0 };
	char gSendStoredInDataEEPROM[CFG_GATE_SIZE] = { 0 };
	char URLStoredInDataEEPROM[CFG_HTTP_URL_SIZE] = { 0 };
	char deviceIDStoredInDataEEPROM[CFG_DEVICE_ID_SIZE] = { 0 };

	for (size_t i = 0; i < CFG_DEVICE_ID_SIZE - 1; i++)
	{
		deviceIDStoredInDataEEPROM[i] = *(volatile uint8_t*) (CFG_EEPROM_DEVICE_ID + i);
	}

	for (size_t i = 0; i < CFG_GATE_SIZE - 1; i++)
	{
		gReadStoredInDataEEPROM[i] = *(volatile uint8_t*) (CFG_EEPROM_GATE_READ + i);
		gSendStoredInDataEEPROM[i] = *(volatile uint8_t*) (CFG_EEPROM_GATE_SEND + i);
	}

	for (size_t i = 0; i < CFG_HTTP_URL_SIZE - 1; i++)
	{
		URLStoredInDataEEPROM[i] = *(volatile uint8_t*) (CFG_EEPROM_HTTP_URL + i);
	}

	HAL_FLASHEx_DATAEEPROM_Unlock();
	HAL_FLASHEx_DATAEEPROM_EnableFixedTimeProgram();

	if (me->mode != modeStoredInDataEEPROM)
	{
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_MODE);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_MODE, (uint32_t) me->mode);
	}

	/* If current and new deviceID are not equal, must be write new value to DATAEEPROM */
	if (strncmp(deviceIDStoredInDataEEPROM, me->deviceID, 12) != 0)
	{
		for (size_t i = 0; i < CFG_DEVICE_ID_SIZE - 1; i++)
		{
			HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_DEVICE_ID + i);
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_DEVICE_ID + i,
					(uint32_t) *(me->deviceID + i));
		}
	}

	/* If current and new gate number are not equal, must be write new value to DATAEEPROM */
	if (strncmp(gReadStoredInDataEEPROM, me->gateRead, 4) != 0
			|| strncmp(gSendStoredInDataEEPROM, me->gateSend, 4) != 0)
	{
		for (size_t i = 0; i < CFG_GATE_SIZE - 1; i++)
		{
			HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_GATE_READ + i);
			HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_GATE_SEND + i);
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_GATE_READ + i,
					(uint32_t) *(me->gateRead + i));
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_GATE_SEND + i,
					(uint32_t) *(me->gateSend + i));
		}
	}

	/* If current and new HTTPURL are not equal, must be write new value to DATAEEPROM */
	if (strncmp(URLStoredInDataEEPROM, me->httpURL, 12) != 0)
	{
		for (size_t i = 0; i < CFG_HTTP_URL_SIZE - 1; i++)
		{
			HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_HTTP_URL + i);
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, CFG_EEPROM_HTTP_URL + i,
					(uint32_t) *(me->httpURL + i));
		}
	}

	HAL_FLASHEx_DATAEEPROM_DisableFixedTimeProgram();
	HAL_FLASHEx_DATAEEPROM_Lock();

	return THT_OK;
}

#if TEST
#include <stdio.h>

THT_StatusTypeDef CFG_test(CFG_HandleTypeDef * const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	printf("==============\n"
			"CFG TEST\n"
			"==============\n");
	printf("Before Read EEPROM\n"
			"ID: %s\n"
			"GSend: %s\n"
			"GRead: %s\n"
			"URL: %s\n", me->deviceID, me->gateSend, me->gateRead, me->httpURL);
	CFG_readEEPROM(me);
	printf("After Read EEPROM\n"
			"ID: %s\n"
			"GSend: %s\n"
			"GRead: %s\n"
			"URL: %s\n", me->deviceID, me->gateSend, me->gateRead, me->httpURL);
	printf("Set New Value and write EEPROM\n"
			"ID: 841699741000\n"
			"GSend: 1122\n"
			"GRead: 6789\n"
			"URL: hahahoho@gmail.com\n");
	CFG_setDeviceID(me, "841699741000", strlen("841699741000"));
	CFG_setGate(me, "1122", "6789", 4, 4);
	CFG_setURL(me, "hahahoho@gmail.com", strlen("hahahoho@gmail.com"));
	CFG_writeEEPROM(me);
	CFG_readEEPROM(me);
	printf("Read EEPROM again\n"
			"ID: %s\n"
			"GSend: %s\n"
			"GRead: %s\n"
			"URL: %s\n"
			"Done test!\n\n", me->deviceID, me->gateSend, me->gateRead, me->httpURL);

	return THT_OK;
}
#endif
