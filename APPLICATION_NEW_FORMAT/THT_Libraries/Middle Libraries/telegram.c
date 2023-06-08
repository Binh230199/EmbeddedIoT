/*
 * telegram.c
 *
 *  Created on: Apr 24, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "telegram.h"

#include <stdio.h>
#include <string.h>

extern SIM_HandleTypeDef *simPtr;

THT_StatusTypeDef TELEGRAM_init(Telegram_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	for (size_t i = 0; i < TELEGRAM_TOKEN_LENGTH - 1; i++)
	{
		me->token[i] = *(volatile uint8_t*) (TELEGRAM_EEPROM_TOKEN + i);
	}

	for (size_t i = 0; i < TELEGRAM_CHAT_ID_LENGTH - 1; i++)
	{
		me->chatID[i] = *(volatile uint8_t*) (TELEGRAM_EEPROM_CHAT_ID + i);
	}

	if (me->token[0] == 0x0)
	{
		TELEGRAM_setToken(me, TELEGRAM_TOKEN_DEFAULT, strlen(TELEGRAM_TOKEN_DEFAULT));
	}

	if (me->chatID[0] == 0)
	{
		TELEGRAM_setChatID(me, TELEGRAM_CHAT_ID_DEFAULT, strlen(TELEGRAM_CHAT_ID_DEFAULT));
	}

	return THT_OK;
}
THT_StatusTypeDef TELEGRAM_sendMessage(Telegram_HandleTypeDef *const me, const char *msg)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	SIM_HTTPTypeDef http = { .type = HTTP_CONTENT_TYPE, .method = SIM_HTTP_POST };

	snprintf(http.url, SIM_HTTP_URL_SIZE, "https://api.telegram.org/bot%s/sendMessage", me->token);
	snprintf(http.data, SIM_HTTP_DATA_SIZE, "{"
			"\"text\":\"%s\","
			"\"parse_mode\":\"%s\","
			"\"disable_web_page_preview\":%s,"
			"\"disable_notification\":%s,"
			"\"reply_to_message_id\":%s,"
			"\"chat_id\": \"%s\"}", msg, me->parseMode, me->disableWebPagePreview, me->disableNotification,
			me->replyToMessageID, me->chatID);

	if (THT_OK == SIM_sendHTTP(simPtr, &http))
	{
		printf("Tele: Sent OK\n");
	}

	return THT_OK;
}
THT_StatusTypeDef TELEGRAM_setToken(Telegram_HandleTypeDef *const me, const char *pToken, size_t size)
{
	if (me == NULL || pToken == NULL || size >= TELEGRAM_TOKEN_LENGTH)
	{
		return THT_ERROR;
	}

	memset(me->token, '\0', TELEGRAM_TOKEN_LENGTH);
	memcpy(me->token, pToken, size);
	me->token[size] = '\0';

	return THT_OK;
}
THT_StatusTypeDef TELEGRAM_setChatID(Telegram_HandleTypeDef *const me, const char *pChatID, size_t size)
{
	if (me == NULL || pChatID == NULL || size >= TELEGRAM_TOKEN_LENGTH)
	{
		return THT_ERROR;
	}

	memset(me->chatID, '\0', TELEGRAM_CHAT_ID_LENGTH);
	memcpy(me->chatID, pChatID, size);

	me->chatID[size] = '\0';

	return THT_OK;
}

/**
 * @brief
 * @param
 * @retval
 */
THT_StatusTypeDef TELEGRAM_readEEPROM(Telegram_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Read section from EEPROM */
	char tokenStoredInDataEEPROM[TELEGRAM_TOKEN_LENGTH] = { 0 };
	char chatIDStoredInDataEEPROM[TELEGRAM_CHAT_ID_LENGTH] = { 0 };

	for (size_t i = 0; i < TELEGRAM_TOKEN_LENGTH; i++)
	{
		tokenStoredInDataEEPROM[i] = *(volatile uint8_t*) (TELEGRAM_EEPROM_TOKEN + i);
	}
	for (size_t i = 0; i < TELEGRAM_CHAT_ID_LENGTH; i++)
	{
		chatIDStoredInDataEEPROM[i] = *(volatile uint8_t*) (TELEGRAM_EEPROM_CHAT_ID + i);
	}

	if (tokenStoredInDataEEPROM[0] == 0)
	{
		TELEGRAM_setToken(me, TELEGRAM_TOKEN_DEFAULT, strlen(TELEGRAM_TOKEN_DEFAULT));
	}
	else
	{
		TELEGRAM_setToken(me, tokenStoredInDataEEPROM, strlen(tokenStoredInDataEEPROM));
	}

	if (chatIDStoredInDataEEPROM[0] == 0)
	{
		TELEGRAM_setChatID(me, TELEGRAM_CHAT_ID_DEFAULT, strlen(TELEGRAM_CHAT_ID_DEFAULT));
	}
	else
	{
		TELEGRAM_setChatID(me, chatIDStoredInDataEEPROM, strlen(chatIDStoredInDataEEPROM));
	}

	return THT_OK;
}

/**
 * @brief
 * @param
 * @retval
 */
THT_StatusTypeDef TELEGRAM_writeEEPROM(Telegram_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Read section from EEPROM */
	char tokenStoredInDataEEPROM[TELEGRAM_TOKEN_LENGTH] = { 0 };
	char chatIDStoredInDataEEPROM[TELEGRAM_CHAT_ID_LENGTH] = { 0 };

	for (size_t i = 0; i < TELEGRAM_TOKEN_LENGTH; i++)
	{
		tokenStoredInDataEEPROM[i] = *(volatile uint8_t*) (TELEGRAM_EEPROM_TOKEN + i);
	}
	for (size_t i = 0; i < TELEGRAM_CHAT_ID_LENGTH; i++)
	{
		chatIDStoredInDataEEPROM[i] = *(volatile uint8_t*) (TELEGRAM_EEPROM_CHAT_ID + i);
	}

	HAL_FLASHEx_DATAEEPROM_Unlock();
	HAL_FLASHEx_DATAEEPROM_EnableFixedTimeProgram();

	/* If current and new deviceID are not equal, must be write new value to DATAEEPROM */
	if (strncmp(tokenStoredInDataEEPROM, me->token, strlen(me->token)) != 0)
	{
		for (size_t i = 0; i < TELEGRAM_TOKEN_LENGTH; i++)
		{
			HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE,
			TELEGRAM_EEPROM_TOKEN + i);
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,
			TELEGRAM_EEPROM_TOKEN + i, (uint32_t) *(me->token + i));
		}
	}

	if (strncmp(chatIDStoredInDataEEPROM, me->chatID, strlen(me->chatID)) != 0)
	{
		for (size_t i = 0; i < TELEGRAM_CHAT_ID_LENGTH; i++)
		{
			HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_BYTE,
			TELEGRAM_EEPROM_CHAT_ID + i);
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,
			TELEGRAM_EEPROM_CHAT_ID + i, (uint32_t) *(me->chatID + i));
		}
	}

	HAL_FLASHEx_DATAEEPROM_DisableFixedTimeProgram();
	HAL_FLASHEx_DATAEEPROM_Lock();

	return THT_OK;
}

