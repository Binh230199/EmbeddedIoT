/*
 * sim7600.h
 *
 *  Created on: Mar 24, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_SIM7600_H_
#define INC_SIM7600_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "midlib.h"

/**
 * @brief SIM7600_HandleTypeDef structure definition
 *
 */
typedef struct
{
	SIM_HandleTypeDef parent;
	uint16_t resetPin;
	uint16_t sleepPin;
	uint16_t pwrkeyPin;
	GPIO_TypeDef *resetPort;
	GPIO_TypeDef *sleepPort;
	GPIO_TypeDef *pwrkeyPort;
#if 0
	uint16_t 			powerEnPin;
	GPIO_TypeDef 		*powerEnPort;
#endif
	UART_HandleTypeDef *huart;
	DMA_HandleTypeDef *hdma;
	int8_t smsPos;
	int8_t smsCnt;
} SIM7600_HandleTypeDef;

//#define SIM7600_POWER_ON(__ME__)		HAL_GPIO_WritePin((__ME__)->powerEnPort, (__ME__)->powerEnPin, GPIO_PIN_SET)
//#define SIM7600_POWER_OFF(__ME__)		HAL_GPIO_WritePin((__ME__)->powerEnPort, (__ME__)->powerEnPin, GPIO_PIN_RESET)
#define SIM7600_PWRKEY_ON(__ME__)		do { 																		\
											HAL_GPIO_WritePin((__ME__)->pwrkeyPort, (__ME__)->pwrkeyPin, GPIO_PIN_SET);		\
											HAL_Delay(1000UL);														\
											HAL_GPIO_WritePin((__ME__)->pwrkeyPort, (__ME__)->pwrkeyPin, GPIO_PIN_RESET);			\
											HAL_Delay(10000UL);														\
										} while(0)
#define SIM7600_RESET_ON(__ME__)		do { 																		\
											HAL_GPIO_WritePin((__ME__)->resetPort, (__ME__)->resetPin, GPIO_PIN_SET);			\
											HAL_Delay(500UL);														\
											HAL_GPIO_WritePin((__ME__)->resetPort, (__ME__)->resetPin, GPIO_PIN_RESET);			\
											HAL_Delay(1000UL);														\
										} while(0)

THT_StatusTypeDef SIM7600_sendATCommand(SIM7600_HandleTypeDef *const me, const char *command, const char *expect,
		const char *unexpect, uint32_t timeout);
void SIM7600_wakeup(SIM7600_HandleTypeDef *const me);
void SIM7600_sleep(SIM7600_HandleTypeDef *const me);
THT_StatusTypeDef SIM7600_init(SIM7600_HandleTypeDef *const me);
THT_StatusTypeDef SIM7600_setFunction(SIM7600_HandleTypeDef *const me, uint8_t mode);
void SIM7600_process(SIM7600_HandleTypeDef *const me);
void SIM7600_checkEvent(SIM7600_HandleTypeDef *const me, uint16_t size);
THT_StatusTypeDef SIM7600_readSMS(SIM7600_HandleTypeDef *const me, uint8_t order, SIM_SMSTypeDef *pSMS);
THT_StatusTypeDef SIM7600_sendSMS(SIM7600_HandleTypeDef *const me, const char *pPhone, const char *pMsg,
		uint8_t phoneSize, uint16_t msgSize);
int8_t SIM7600_getSMSCounter(SIM7600_HandleTypeDef *const me);
THT_StatusTypeDef SIM7600_callUSSD(SIM7600_HandleTypeDef *const me, const char *ussd, char response[], uint16_t size);
int8_t SIM7600_getRSSI(SIM7600_HandleTypeDef *const me);
int8_t SIM7600_getTemperature(SIM7600_HandleTypeDef *const me);
THT_StatusTypeDef SIM7600_getNetworkType(SIM7600_HandleTypeDef *const me, char pNwtype[], uint8_t size);
THT_StatusTypeDef SIM7600_getSimPhone(SIM7600_HandleTypeDef *const me, char pPhone[], uint8_t size);
THT_StatusTypeDef SIM7600_getIMEI(SIM7600_HandleTypeDef *const me, char pIMEI[], uint8_t size);
THT_StatusTypeDef SIM7600_getLocalTime(SIM7600_HandleTypeDef *const me, char pTime[], uint8_t size);
uint32_t SIM7600_getFileSize(SIM7600_HandleTypeDef *const me, const char *pFilename);
THT_StatusTypeDef SIM7600_deleteFile(SIM7600_HandleTypeDef *const me, const char *pFilename);
THT_StatusTypeDef SIM7600_setLocalTimeAuto(SIM7600_HandleTypeDef *const me);
THT_StatusTypeDef SIM7600_setLocalTimeManual(SIM7600_HandleTypeDef *const me, const char *pTime, uint8_t size);
THT_StatusTypeDef SIM7600_selectPhonebookMem(SIM7600_HandleTypeDef *const me);
THT_StatusTypeDef SIM7600_savePhonebook(SIM7600_HandleTypeDef *const me, uint8_t order, const char *pPhone,
		uint8_t phoneSize);
THT_StatusTypeDef SIM7600_readPhonebook(SIM7600_HandleTypeDef *const me, uint8_t order, char pPhone[],
		uint8_t phoneSize);
THT_StatusTypeDef SIM7600_findPhonebook(SIM7600_HandleTypeDef *const me, const char *pPhone);
THT_StatusTypeDef SIM7600_sendHTTP(SIM7600_HandleTypeDef *const me, SIM_HTTPTypeDef *pHTTP);
THT_StatusTypeDef SIM7600_getFileHTTP(SIM7600_HandleTypeDef *const me, const char *pUrl, const char *pFilename);

#endif /* INC_SIM7600_H_ */
