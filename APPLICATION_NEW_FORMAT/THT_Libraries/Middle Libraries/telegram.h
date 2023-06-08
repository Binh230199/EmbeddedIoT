/*
 * telegram.h
 *
 *  Created on: Apr 24, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_TELEGRAM_H_
#define INC_TELEGRAM_H_

#include "http.h"
#include "sim.h"
#include "wlv_def.h"

#define USE_TELEGRAM_REST_API

#define TELEGRAM_TOKEN_LENGTH			48U
#define TELEGRAM_CHAT_ID_LENGTH			24U
#define TELEGRAM_EEPROM_TOKEN			(THT_TELE_BASE)
#define TELEGRAM_EEPROM_CHAT_ID			(TELEGRAM_EEPROM_TOKEN + TELEGRAM_TOKEN_LENGTH)
#define TELEGRAM_TOKEN_DEFAULT			"6164292395:AAFQuCJ8Db5t0LF1Nd3ijxQiIpsdIeuTRZo"
#define TELEGRAM_CHAT_ID_DEFAULT		"-853747679"
#define TELEGRAM_TRUE_STRING			"true"
#define TELEGRAM_FALSE_STRING			"false"
#define TELEGRAM_NULL_STRING			"null"
#define TELEGRAM_METHOD_SEND_MSG		"sendMessage"

typedef struct
{
	char *method;
	char *text;
	char *parseMode;
	char *disableWebPagePreview;
	char *disableNotification;
	char *replyToMessageID;
	char chatID[TELEGRAM_CHAT_ID_LENGTH];
	char token[TELEGRAM_TOKEN_LENGTH];
} Telegram_HandleTypeDef;

THT_StatusTypeDef TELEGRAM_init(Telegram_HandleTypeDef *const me);
THT_StatusTypeDef TELEGRAM_sendMessage(Telegram_HandleTypeDef *const me, const char *msg);
THT_StatusTypeDef TELEGRAM_setToken(Telegram_HandleTypeDef *const me, const char *pToken, size_t size);
THT_StatusTypeDef TELEGRAM_setChatID(Telegram_HandleTypeDef *const me, const char *pChatID, size_t size);
THT_StatusTypeDef TELEGRAM_readEEPROM(Telegram_HandleTypeDef *const me);
THT_StatusTypeDef TELEGRAM_writeEEPROM(Telegram_HandleTypeDef *const me);

#endif /* INC_TELEGRAM_H_ */
