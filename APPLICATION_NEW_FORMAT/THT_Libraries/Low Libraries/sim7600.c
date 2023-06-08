/*
 * sim7600.c
 *
 *  Created on: Mar 24, 2023
 *      Author: Hoang Van Binhan Binh
 */

#include "sim7600.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief  Send command and check expected information in response. If
 * no data received, return timeout.
 * @param me [in] Pointer to SIM7600_HandleTypeDef.
 * @param command [in] Pointer to AT command string.
 * @param expect [in] The expected information string in response buffer.
 * @param unexpect [in] The unexpected information string in response buffer.
 * @param timeout [in] The timeout to cancel function if no response or no information is matched.
 * @retval THT_StatusTypeDef status
 */
THT_StatusTypeDef SIM7600_sendATCommand(SIM7600_HandleTypeDef *const me, const char *command, const char *expect, const char *unexpect, uint32_t timeout)
{
	if (me == NULL || command == NULL || expect == NULL || unexpect == NULL)
	{
		return THT_ERROR;
	}

	THT_StatusTypeDef answer = THT_BUSY;

	/* Enable UART receive to IDLE DMA */
	HAL_UARTEx_ReceiveToIdle_DMA((UART_HandleTypeDef *)me->huart, (uint8_t *)me->parent.buffer,
								 SIM_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT((DMA_HandleTypeDef *)me->hdma, DMA_IT_HT);

	/* Clear buffer before receive new data */
	memset(me->parent.buffer, '\0', SIM_BUFFER_SIZE);

	/* Sends command to SIM module: command + "\r\n" */
	HAL_UART_Transmit((UART_HandleTypeDef *)me->huart, (uint8_t *)command, strlen(command),
					  HAL_MAX_DELAY);
	HAL_UART_Transmit((UART_HandleTypeDef *)me->huart, (uint8_t *)"\r\n", 2, 1000);

	uint32_t tickStart = HAL_GetTick();

	/* Wait to receive the response - if the response is empty, the timeout is occured */
	while (answer == THT_BUSY)
	{
		const uint32_t timeElapsed = HAL_GetTick() - tickStart;

		if (timeElapsed >= timeout)
		{
			answer = THT_TIMEOUT;
			me->parent.timeoutCnt++;

			/* Print the command and timeout counter */
			printf(command);
			printf("\n");
			printf("Timeout: %u\n", me->parent.timeoutCnt);
			break;
		}
		else
		{
			if (SIM_GET_FLAG(&me->parent, SIM_FLAG_RX))
			{
				if (strstr(me->parent.buffer, expect) != NULL)
				{
					answer = THT_OK;
					break;
				}
				else if (strstr(me->parent.buffer, unexpect) != NULL)
				{
					answer = THT_ERROR;
					break;
				}
			}
		}
	}

	SIM_CLR_FLAG(&me->parent, SIM_FLAG_RX);
	/*	printf(me->parent.buffer); // Fatal Error: Không uncommment dòng này!
		 Khi thực hiện tính năng FOTA, dòng này nhận được dữ liệu từ module và in ra,
		 khi in ra thì nó k dừng lại mà sẽ in toàn bộ dữ liệu trong bộ nhớ của vi điều
		 khiển và treo
	*/
	return answer;
}

/**
 * @brief  Wake up module SIM
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @retval None
 */
void SIM7600_wakeup(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return;
	}
	HAL_GPIO_WritePin(me->sleepPort, me->sleepPin, GPIO_PIN_RESET); // DTR Low: Wakeup, DTR high: Sleep
	HAL_Delay(100UL);
}

/**
 * @brief  Put the module to sleep
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @retval None
 */
void SIM7600_sleep(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return;
	}
	SIM7600_sendATCommand(me, "AT+CSCLK=1", SIM_URC_OK, SIM_URC_ERROR, 3000);
	HAL_GPIO_WritePin(me->sleepPort, me->sleepPin, GPIO_PIN_SET);
	HAL_Delay(100UL);
}

/**
 * @brief  Initializes module SIM
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @retval THT_StatusTypeDef status
 */
THT_StatusTypeDef SIM7600_init(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	me->huart->Instance = USART2;
	me->huart->Init.BaudRate = 115200;
	me->huart->Init.WordLength = UART_WORDLENGTH_8B;
	me->huart->Init.StopBits = UART_STOPBITS_1;
	me->huart->Init.Parity = UART_PARITY_NONE;
	me->huart->Init.Mode = UART_MODE_TX_RX;
	me->huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	me->huart->Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(me->huart) != HAL_OK)
	{
		//	    Error_Handler();
	}

	/* Power on and reset module */
	SIM7600_wakeup(me);
//	SIM7600_POWER_ON(me);
	SIM7600_RESET_ON(me);

	/* Enable receive data through UART port */
	HAL_UARTEx_ReceiveToIdle_DMA((UART_HandleTypeDef *)me->huart, (uint8_t *)me->parent.buffer, SIM_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT((DMA_HandleTypeDef *)me->hdma, DMA_IT_HT);

	/* Powerkey on */
	SIM7600_PWRKEY_ON(me);

	for (size_t i = 0u; i < 60u; i++)
	{
		if (THT_OK == SIM7600_sendATCommand(me, "AT", SIM_URC_OK, SIM_URC_ERROR, 250))
		{
			SIM7600_sendATCommand(me, "ATE0", SIM_URC_OK, SIM_URC_ERROR, 500); /* Must be used this command */

			if (THT_OK == SIM7600_sendATCommand(me, "AT+CPIN?", SIM_URC_OK, SIM_URC_ERROR, 4000))
			{
				SIM7600_sendATCommand(me, "ATI", SIM_URC_OK, SIM_URC_ERROR, 500);
				SIM7600_sendATCommand(me, "AT+CREG?", "+CREG: 0,1", SIM_URC_ERROR, 4000);	//"\r\n+CREG: 0,1\r\n\r\nOK\r\n"
				SIM7600_sendATCommand(me, "AT+CGREG?", "+CGREG: 0,1", SIM_URC_ERROR, 4000); // "\r\n+CGREG: 0,1\r\n\r\nOK\r\n"
				SIM7600_sendATCommand(me, "AT+CNMP=39", SIM_URC_OK, SIM_URC_ERROR, 4000);
				SIM7600_sendATCommand(me, "AT+CTZU=0", SIM_URC_OK, SIM_URC_ERROR, 4000);					//"\r\nOK\r\n"
				SIM7600_sendATCommand(me, "AT+CPBS=\"ME\"", SIM_URC_OK, SIM_URC_ERROR, 4000);				//"\r\nOK\r\n"
				SIM7600_sendATCommand(me, "AT+CPMS=\"SM\",\"SM\",\"SM\"", SIM_URC_OK, SIM_URC_ERROR, 4000); //"\r\n+CPMS: 0,40,0,40,0,40\r\n\r\nOK\r\n"
				SIM7600_sendATCommand(me, "AT+CMGF=1", SIM_URC_OK, SIM_URC_ERROR, 4000);
				SIM7600_sendATCommand(me, "AT+CNMI=2,1,0,0,0", SIM_URC_OK, SIM_URC_ERROR, 4000);
				SIM7600_sendATCommand(me, "AT+CFGRI=0", SIM_URC_OK, SIM_URC_ERROR, 4000);

				return THT_OK;
			}
		}
	}

	return THT_ERROR;
}

/**
 * @brief  Setting the operating mode of the module
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param mode [in] Function mode
 * 				@arg: SIM_MODE_MINIMUM
 * 				@arg: SIM_MODE_FULL
 * 				@arg: SIM_MODE_FLIGHT
 * 				@arg: SIM_MODE_TEST
 * 				@arg: SIM_MODE_RESET
 * 				@arg: SIM_MODE_OFFLINE
 * @retval THT_StatusTypeDef status
 */
THT_StatusTypeDef SIM7600_setFunction(SIM7600_HandleTypeDef *const me, uint8_t mode)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	char command[10] = {0};
	snprintf(command, sizeof(command), "AT+CFUN=%d", mode);
	return SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 4000);
}

/**
 * @brief  Handling problems that occur during operation
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @retval None
 */
void SIM7600_process(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return;
	}

	/* If sim card is removed or not fit */
	if (SIM_GET_FLAG(&me->parent, SIM_FLAG_SRMV))
	{
		SIM7600_wakeup(me);

		/* Minimize function of SIM */
		SIM7600_setFunction(me, SIM_MODE_MINIMUM);
		HAL_Delay(1000UL);

		/* Enable all functions of module to reconnect sim card if the sim card is plugged */
		SIM7600_setFunction(me, SIM_MODE_FULL);

		uint32_t tickStart = HAL_GetTick();

		char nwType[SIM_NWTYPE_SIZE] = {0};

		while (HAL_GetTick() - tickStart < 15000u)
		{
			if (THT_OK == SIM7600_sendATCommand(me, "AT+CPIN?", SIM_URC_OK, SIM_URC_ERROR, 1000) || THT_OK == SIM7600_getNetworkType(me, nwType, SIM_NWTYPE_SIZE))
			{
				char *sPtr1 = strstr(nwType, "NO SERVICE");
				char *sPtr2 = strstr(nwType, "Unknown");

				if (sPtr1 == NULL && sPtr2 == NULL)
				{
					SIM_CLR_FLAG(&me->parent, SIM_FLAG_SRMV);
					break; /* Break out of while loop */
				}
			}
		}

		SIM7600_sleep(me);
	}

	/* If error in communition */
	if (me->parent.timeoutCnt >= 10u)
	{
		SIM7600_wakeup(me);
		SIM7600_RESET_ON(me);
		SIM7600_init(me);
		SIM7600_sleep(me);
		me->parent.timeoutCnt = 0u;
	}
}

/**
 * @brief  Handling problems that occur during operation
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @retval None
 */
void SIM7600_checkEvent(SIM7600_HandleTypeDef *const me, uint16_t size)
{
	if (me == NULL)
	{
		return;
	}

	/* Enable UART receive to IDLE DMA */
	HAL_UARTEx_ReceiveToIdle_DMA((UART_HandleTypeDef *)me->huart, (uint8_t *)me->parent.buffer, SIM_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT((DMA_HandleTypeDef *)me->hdma, DMA_IT_HT);

	SIM_SET_FLAG(&me->parent, SIM_FLAG_RX);

	if (strstr(me->parent.buffer, SIM_URC_SMS) != NULL)
	{
		SIM_SET_FLAG(&me->parent, SIM_FLAG_SMS);

		char *sPtr = strstr(me->parent.buffer, "\"SM\",");

		if (sPtr != NULL && size >= 5u) // xxx: size?
		{
			me->smsPos = atoi(sPtr + 5u);
		}
	}
	//	else if (strstr(me->parent.buffer, SIM_URC_AUDIO_STOP) != NULL)
	//	{
	//		SIM_SET_FLAG(&me->parent, SIM_FLAG_AUDIO_STOP);
	//	}
	else if (strstr(me->parent.buffer, SIM_URC_SIMCARD_UNPLUGIN) != NULL)
	{
		SIM_SET_FLAG(&me->parent, SIM_FLAG_SRMV);
	}
}

/**
 * @brief Read SMS
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param order [in] the order of SMS store in storage
 * @param pSMS [out] Pointer to SMS data structure
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_readSMS(SIM7600_HandleTypeDef *const me, const uint8_t order,
								  SIM_SMSTypeDef *pSMS)
{
	if (me == NULL || pSMS == NULL)
	{
		return THT_ERROR;
	}

	enum Enum
	{
		COMMAND_SIZE = 15
	};

	char command[COMMAND_SIZE] = {0};

	/* Clear the buffer */
	memset(pSMS->phone, '\0', SIM_PHONE_SIZE);
	memset(pSMS->time, '\0', SIM_TIME_SIZE);
	memset(pSMS->content, '\0', SIM_SMS_SIZE);

	snprintf(command, sizeof(command), "AT+CMGRD=%u", order);

	THT_StatusTypeDef status = SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 10000);

	if (THT_OK == status)
	{
		/* Copy the phonenumber, time and and content of SMS */
		char *sPtr = strstr(me->parent.buffer, "+84");
		THT_copyStringUntilToken(sPtr + 1, pSMS->phone, SIM_PHONE_SIZE, "\"");
		sPtr = strstr(me->parent.buffer, ",\"2");
		THT_copyStringUntilToken(sPtr + 2, pSMS->time, SIM_TIME_SIZE, "\"");
		sPtr = strstr(me->parent.buffer, "\"\r\n");
		THT_copyStringUntilToken(sPtr + 3, pSMS->content, SIM_SMS_SIZE, "\r");
	}

	return status;
}

/**
 * @brief Send SMS
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pPhone [in] a pointer to const string phonenumber
 * @param pMsg [in] a pointer to const string sms content
 * @param phoneSize [in] the length of the phonenumber
 * @param msgSize [in] the length of the SMS content
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_sendSMS(SIM7600_HandleTypeDef *const me, const char *pPhone, const char *pMsg,
								  uint8_t phoneSize, uint16_t msgSize)
{
	if (me == NULL || pPhone == NULL || pMsg == NULL ||
		phoneSize > SIM_PHONE_SIZE || msgSize == 0)
	{
		return THT_ERROR;
	}

	enum Enum
	{
		COMMAND_SIZE = 250u,
		MAX_SMS_LENGTH = 160u,
		MAX_SMS_SEGMENT_LENGTH = 146u,
		CTRL_Z_CHARACTER = 0x1A
	};

	char command[COMMAND_SIZE] = {0};

	THT_StatusTypeDef status = SIM7600_sendATCommand(me, "AT+CMGF=1", SIM_URC_OK, SIM_URC_ERROR, 4000);

	/* Send SMS */
	if (msgSize < MAX_SMS_LENGTH)
	{
		snprintf(command, sizeof(command), "AT+CMGS=\"%s\"", pPhone);

		status = SIM7600_sendATCommand(me, command, ">", SIM_URC_ERROR, 15000);

		if (THT_OK == status)
		{
			snprintf(command, sizeof(command), "%s%c", pMsg, CTRL_Z_CHARACTER);
			status = SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 10000);
		}
	}
	/* Send long SMS */
	else
	{
		uint8_t smsSegmentSize = 0;
		uint8_t totalSms = (msgSize / MAX_SMS_SEGMENT_LENGTH) + 1;
		/* Split and send SMS messages one by one */
		for (size_t seg = 1; seg <= totalSms; seg++)
		{
			if (msgSize < MAX_SMS_SEGMENT_LENGTH)
			{
				smsSegmentSize = msgSize;
			}
			else
			{
				smsSegmentSize = MAX_SMS_SEGMENT_LENGTH;
			}

			snprintf(command, sizeof(command), "AT+CMGSEX=\"%s\",190,%u,%u",
					 pPhone, seg, totalSms);

			status = SIM7600_sendATCommand(me, command, ">", SIM_URC_ERROR, 15000);

			if (THT_OK == status)
			{
				memset(command, '\0', sizeof(command));
				memcpy(command, pMsg + (seg - 1) * MAX_SMS_SEGMENT_LENGTH,
					   smsSegmentSize);
				command[smsSegmentSize] = CTRL_Z_CHARACTER;
				status = SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 10000);
				msgSize -= smsSegmentSize;
			}
		}
	}

	return status;
}

/**
 * @brief Get number of SMS in storage
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @return int8_t
 */
int8_t SIM7600_getSMSCounter(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	THT_StatusTypeDef status = SIM7600_sendATCommand(me, "AT+CPMS=\"SM\",\"SM\",\"SM\"", SIM_URC_OK, SIM_URC_ERROR, 10000);

	if (THT_OK == status)
	{
		char *sPtr = strstr(me->parent.buffer, ": ");

		if (sPtr != NULL)
		{
			me->smsCnt = atoi(sPtr + 2);
		}

		if (me->smsCnt > me->smsPos)
		{
			return me->smsCnt;
		}
		else
		{
			return me->smsPos;
		}
	}

	return 0u;
}

/**
 * @brief Make a call to USSD , ie. *098#
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param ussd [in] USSD command
 * @param response [out] a buffer to store the response of the USSD request
 * @param size [in]
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_callUSSD(SIM7600_HandleTypeDef *const me, const char *ussd, char response[], const uint16_t size)
{
	if (me == NULL || ussd == NULL)
	{
		return THT_ERROR;
	}

	enum Enum
	{
		COMMAND_SIZE = 40
	};

	char command[COMMAND_SIZE] = {0};

	snprintf(command, sizeof(command), "AT+CUSD=1,\"%s\",15", ussd);

	THT_StatusTypeDef status = SIM7600_sendATCommand(me, command, ",15", SIM_URC_ERROR, 15000);

	if (THT_OK == status)
	{
		char *sPtr = strstr(me->parent.buffer, ",\"");

		THT_copyStringUntilToken(sPtr + 1, response, size, NULL);
		SIM7600_sendATCommand(me, "AT+CUSD=2,\"\",15", SIM_URC_OK, SIM_URC_ERROR, 15000);
	}
	return status;
}

/**
 * @brief Get RSSI
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @return int8_t
 */
int8_t SIM7600_getRSSI(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return 0;
	}

	int8_t rssi = 0;

	THT_StatusTypeDef status = SIM7600_sendATCommand(me, "AT+CSQ", SIM_URC_OK, SIM_URC_ERROR, 4000);

	if (THT_OK == status)
	{
		char *sPtr = strstr(me->parent.buffer, " ");

		if (sPtr != NULL)
		{
			rssi = (2u * atoi(sPtr)) - 113u;
		}
	}

	return rssi;
}

/**
 * @brief Get Network Type
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pNwtype [out] A buffer to store the string network type
 * @param size [in] The size of buffer
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_getNetworkType(SIM7600_HandleTypeDef *const me, char pNwtype[], const uint8_t size)
{
	if (me == NULL || pNwtype == NULL)
	{
		return THT_ERROR;
	}

	memset(pNwtype, '\0', size);

	THT_StatusTypeDef status = SIM7600_sendATCommand(me, "AT+CPSI?", SIM_URC_OK, SIM_URC_ERROR, 4000);

	if (THT_OK == status)
	{
		if (strstr(me->parent.buffer, "LTE") != NULL)
		{
			snprintf(pNwtype, size, "LTE");
		}
		else if (strstr(me->parent.buffer, "WCDMA") != NULL)
		{
			snprintf(pNwtype, size, "WCDMA");
		}
		else if (strstr(me->parent.buffer, "GSM") != NULL)
		{
			snprintf(pNwtype, size, "GSM");
		}
		else if (strstr(me->parent.buffer, "NO SERVICE") != NULL)
		{
			snprintf(pNwtype, size, "NO SERVICE");
		}
		else
		{
			snprintf(pNwtype, size, "Unknown");
		}
	}
	else
	{
		snprintf(pNwtype, size, "Unknown");
	}

	return status;
}

/**
 * @brief Get temperature of module
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @return int8_t
 */
int8_t SIM7600_getTemperature(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	int8_t temperature = 0.0f;
	THT_StatusTypeDef status = SIM7600_sendATCommand(me, "AT+CPMUTEMP", SIM_URC_OK, SIM_URC_ERROR, 4000);

	if (THT_OK == status)
	{
		char *sPtr = strstr(me->parent.buffer, " ");

		if (sPtr != NULL)
		{
			temperature = atoi(sPtr + 1);
		}
	}

	return temperature;
}

/**
 * @brief Get phone number of SIM card
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pPhone [out] A buffer to store the phone number
 * @param size [in] The size of the buffer
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_getSimPhone(SIM7600_HandleTypeDef *const me, char pPhone[], const uint8_t size)
{
	if (me == NULL || pPhone == NULL || size < SIM_PHONE_SIZE)
	{
		return THT_ERROR;
	}

	enum Enum
	{
		USSD_BUFFER_SIZE = 150
	};

	memset(pPhone, '\0', size);

	uint8_t counter = 0;

	char ussdResponse[USSD_BUFFER_SIZE] = {0};

	THT_StatusTypeDef status = SIM7600_sendATCommand(me, "AT+COPS?", SIM_URC_OK, SIM_URC_ERROR, 4000);

	if (THT_OK == status)
	{
		/* "\r\n+COPS: 0,0,\"Viettel Viettel\",7\r\n\r\nOK\r\n" */
		do
		{
			/* Check if sim is Viettel Operator */
			if (strstr(me->parent.buffer, "Viettel") != NULL ||
				strstr(me->parent.buffer, "VIETTEL") != NULL)
			{
				status = SIM7600_callUSSD(me, "*098#", ussdResponse, sizeof(ussdResponse));

				if (THT_OK == status)
				{
					/* "Moi 01699741245 chon: */
					memcpy(pPhone, "84", 2);

					char *sPtr = strstr(ussdResponse, " ");
					THT_copyStringUntilToken(sPtr + 2, pPhone + 2, size, " ");
				}
			}
			/* Check if sim is Vina Operator */
			else if (strstr(me->parent.buffer, "Vina") != NULL ||
					 strstr(me->parent.buffer, "VINA") != NULL)
			{
				status = SIM7600_callUSSD(me, "*110#", ussdResponse, sizeof(ussdResponse));

				if (THT_OK == status)
				{
					/* "TB (VINACARD):914486786 */
					memcpy(pPhone, "84", 2);
					char *sPtr = strstr(ussdResponse, ":");
					THT_copyStringUntilToken(sPtr, pPhone + 2, size, "\r");
				}
			}
			/* Check if sim is Mobifone Operator */
			else if (strstr(me->parent.buffer, "MOBI") != NULL ||
					 strstr(me->parent.buffer, "Mobiphone") != NULL)
			{
				status = SIM7600_callUSSD(me, "*0#", ussdResponse, sizeof(ussdResponse));

				if (THT_OK == status)
				{
					/* "84914486786" */
					char *sPtr = strstr(ussdResponse, "84");
					THT_copyStringUntilToken(sPtr, pPhone, size, "\r");
				}
			}
			counter++;
		} while (counter < 3 && pPhone[0] == 0);
	}

	return status;
}

/**
 * @brief Get SIM IMEI
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pIMEI [out] A buffer store the SIM IMEI string
 * @param size [in] The size of buffer
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_getIMEI(SIM7600_HandleTypeDef *const me, char pIMEI[], const uint8_t size)
{
	if (me == NULL || pIMEI == NULL || size < SIM_IMEI_SIZE)
	{
		return THT_ERROR;
	}

	memset(pIMEI, '\0', size);

	uint8_t counter = 0;
	THT_StatusTypeDef status;

	do
	{
		status = SIM7600_sendATCommand(me, "AT+SIMEI?", SIM_URC_OK, SIM_URC_ERROR, 4000);

		if (THT_OK == status)
		{
			char *sPtr = strstr(me->parent.buffer, " ");
			THT_copyStringUntilToken(sPtr + 1, pIMEI, size, "\r");
		}
		counter++;
	} while (pIMEI[0] == 0 && counter < 3);

	return status;
}

/**
 * @brief Get SIM local time
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pTime [out] A buffer store the time string
 * @param size [in] The size of the buffer
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_getLocalTime(SIM7600_HandleTypeDef *const me, char pTime[], const uint8_t size)
{
	if (me == NULL || pTime == NULL || size < SIM_TIME_SIZE)
	{
		return THT_ERROR;
	}

	uint8_t counter = 0;

	memset(pTime, '\0', size);

	THT_StatusTypeDef status;
	do
	{
		status = SIM7600_sendATCommand(me, "AT+CCLK?", "+28", SIM_URC_ERROR, 3000);

		if (status == THT_OK)
		{
			if (strstr(me->parent.buffer, "80/0") == NULL)
			{
				char *sPtr = strstr(me->parent.buffer, "\"");
				THT_copyStringUntilToken(sPtr + 1, pTime, size, "\"");
			}
		}
		counter++;
	} while (counter < 3 && pTime[0] == 0);

	return status;
}

/**
 * @brief Get size of file store im module GSM memory
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pFilename [in] a pointer to filename string
 * @return uint32_t
 */
uint32_t SIM7600_getFileSize(SIM7600_HandleTypeDef *const me, const char *pFilename)
{
	if (me == NULL || pFilename == NULL)
	{
		return 0;
	}

	enum Enum
	{
		COMMAND_SIZE = 100
	};

	char command[COMMAND_SIZE] = {0};

	snprintf(command, sizeof(command), "AT+FSATTRI=%s", pFilename);

	SIM7600_sendATCommand(me, "AT+FSCD=F:", SIM_URC_OK, SIM_URC_ERROR, 3000);
	SIM7600_sendATCommand(me, "AT+FSLS=2", SIM_URC_OK, SIM_URC_ERROR, 3000);

	if (SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 3000) == THT_OK)
	{
		char *sPtr = strstr(me->parent.buffer, "+FSATTRI: ");

		if (sPtr != NULL)
		{
			uint32_t filesize = atol(sPtr + 10);
			return filesize;
		}
	}

	return 0u;
}

/**
 * @brief Delete file on module GSM memory
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pFilename [in] a pointer to filename string
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_deleteFile(SIM7600_HandleTypeDef *const me, const char *pFilename)
{
	if (me == NULL || pFilename == NULL)
	{
		return 0u;
	}

	enum Enum
	{
		COMMAND_SIZE = 100
	};

	char command[COMMAND_SIZE] = {0};

	snprintf(command, sizeof(command), "AT+FSDEL=\"/%s\"", pFilename);
	return SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 3000);
}

/**
 * @brief Set the SIM local time automatically
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_setLocalTimeAuto(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	uint8_t counter = 0U;

	THT_StatusTypeDef status;

	SIM7600_sendATCommand(me, "AT+CREG?", "+CREG: 0,1", SIM_URC_ERROR, 5000); //"\r\n+CREG: 0,1\r\n\r\nOK\r\n"
	SIM7600_sendATCommand(me, "AT+CPSI?", SIM_URC_OK, SIM_URC_ERROR, 5000);

	do
	{
		switch (counter)
		{
		case 0:
			status = SIM7600_sendATCommand(me, "AT+CNTP=\"time1.google.com\",28", SIM_URC_OK, SIM_URC_ERROR, 5000); //"\r\nOK\r\n"
			break;
		case 1:
			status = SIM7600_sendATCommand(me, "AT+CNTP=\"0.vn.pool.ntp.org\",28", SIM_URC_OK, SIM_URC_ERROR, 5000);
			break;
		case 2:
			status = SIM7600_sendATCommand(me, "AT+CNTP=\"0.asia.pool.ntp.org\",28", SIM_URC_OK, SIM_URC_ERROR, 5000);
			break;
		default:
			status = THT_ERROR;
			break;
		}
		counter++;
	} while (THT_OK != status && counter < 3U);

	if (THT_OK == status)
	{
		status = SIM7600_sendATCommand(me, "AT+CNTP", "+CNTP: 0", SIM_URC_ERROR, 4000); //"\r\n+CNTP: 0\r\n"
	}

	return status;
}

/**
 * @brief Set the SIM local time manually
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pTime [in] A pointer to the timestring
 * @param size [in] The length of the string
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_setLocalTimeManual(SIM7600_HandleTypeDef *const me, const char *pTime, const uint8_t size)
{
	if (me == NULL || pTime == NULL || size > SIM_TIME_SIZE)
	{
		return THT_ERROR;
	}
	enum Enum
	{
		COMMAND_SIZE = 40
	};

	char command[COMMAND_SIZE] = {0};
	snprintf(command, sizeof(command), "AT+CCLK=\"%s\"", pTime);
	return SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 4000);
}

/**
 * @brief Select memory to store the phonenumber
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_selectPhonebookMem(SIM7600_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return SIM7600_sendATCommand(me, "AT+CPBS=\"ME\"", SIM_URC_OK, SIM_URC_ERROR, 4000);
}

/* Should be pass strlen(phoneNumber) to avoid error */

/**
 * @brief Save the new phone number to phonebook
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param order [in] The position in the memory want to save phonenumber
 * @param pPhone [in] A pointer to the phonenumber string
 * @param phoneSize [in] The length of the phonenumber
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_savePhonebook(SIM7600_HandleTypeDef *const me, const uint8_t order, const char *pPhone, const uint8_t phoneSize)
{
	if (me == NULL || pPhone == NULL || phoneSize > SIM_PHONE_SIZE)
	{
		return THT_ERROR;
	}
	enum Enum
	{
		COMMAND_SIZE = 100
	};
	char command[COMMAND_SIZE] = {0};

	snprintf(command, sizeof(command), "AT+CPBW=%d,\"%s\",129,\"%s\"", order,
			 pPhone, pPhone);
	return SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 4000);
}

/**
 * @brief Read a phonenumber (contact) from phonebook
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param order [in] The position in the memory want to read phonenumber
 * @param pPhone [out] A buffer to store the phonenumber store at order
 * @param phoneSize [in] The size of buffer
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_readPhonebook(SIM7600_HandleTypeDef *const me, const uint8_t order, char pPhone[], const uint8_t phoneSize)
{
	if (me == NULL || pPhone == NULL)
	{
		return THT_ERROR;
	}
	enum Enum
	{
		COMMAND_SIZE = 100
	};
	char command[COMMAND_SIZE] = {0};

	snprintf(command, sizeof(command), "AT+CPBR=%d", order);

	THT_StatusTypeDef status = SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 4000);

	if (THT_OK == status)
	{
		char *sPtr = strstr(me->parent.buffer, "\"");

		THT_copyStringUntilToken(sPtr + 1, pPhone, phoneSize, "\"");
	}

	return status;
}

/**
 * @brief Find a phonenumber by hint
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pPhone [in] Hint, it is the phonenumber want to find
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_findPhonebook(SIM7600_HandleTypeDef *const me, const char *pPhone)
{
	if (me == NULL || pPhone == NULL)
	{
		return THT_ERROR;
	}

	enum Enum
	{
		COMMAND_SIZE = 35
	};
	char command[COMMAND_SIZE] = {0};

	snprintf(command, sizeof(command), "AT+CPBF=\"%s\"", pPhone);

	return SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 4000);
}

/**
 * @brief Send HTTP request
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pHTTP [in] A pointer to SIM_HTTPTypeDef
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_sendHTTP(SIM7600_HandleTypeDef *const me, SIM_HTTPTypeDef *pHTTP)
{
	if (me == NULL || pHTTP == NULL)
	{
		return THT_ERROR;
	}

	memset(pHTTP->response, '\0', SIM_HTTP_RESPONSE_SIZE);
	pHTTP->errorcode = 0;

	char command[SIM_HTTP_RESPONSE_SIZE] = {0};

	THT_StatusTypeDef status;

	/* Disconnect the last session */
	SIM7600_sendATCommand(me, "AT+HTTPTERM", SIM_URC_OK, SIM_URC_ERROR, 10000);

	/* Begin a new session */
	status = SIM7600_sendATCommand(me, "AT+HTTPINIT", SIM_URC_OK, SIM_URC_ERROR, 10000);

	if (THT_OK != status)
	{
		return status;
	}

	/* Insert URL */
	snprintf(command, sizeof(command), "AT+HTTPPARA=\"URL\",\"%s\"", pHTTP->url);
	status = SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 10000);
	if (THT_OK != status)
	{
		return status;
	}

	/* Check the method is POST ? */
	if (pHTTP->method == SIM_HTTP_POST)
	{
		/* Insert the content type */
		memset(command, '\0', sizeof(command));
		snprintf(command, sizeof(command), "AT+HTTPPARA=\"CONTENT\",\"%s\"", pHTTP->type);
		status = SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 10000);
		if (THT_OK != status)
		{
			return status;
		}

		/* Insert the data */
		memset(command, '\0', sizeof(command));
		snprintf(command, sizeof(command), "AT+HTTPDATA=%d,%d", strlen(pHTTP->data), 10000);
		status = SIM7600_sendATCommand(me, command, "DOWNLOAD", SIM_URC_ERROR, 10000);
		if (THT_OK != status)
		{
			return status;
		}

		status = SIM7600_sendATCommand(me, pHTTP->data, SIM_URC_OK, SIM_URC_ERROR, 10000);
		if (THT_OK != status)
		{
			return status;
		}
	}

	/* Send the request */
	memset(command, '\0', sizeof(command));
	snprintf(command, sizeof(command), "AT+HTTPACTION=%d", pHTTP->method);
	status = SIM7600_sendATCommand(me, command, "+HTTPACTION:", SIM_URC_ERROR, 20000);
	if (THT_OK != status)
	{
		return status;
	}
	HAL_Delay(100);

	/* Get the status of request */
	char *sPtr = strstr(me->parent.buffer, ",");

	if (sPtr != NULL)
	{
		pHTTP->errorcode = atoi(sPtr + 1);
	}

	sPtr = strstr(sPtr + 1, ",");
	if (sPtr != NULL)
	{
		pHTTP->datalen = atol(sPtr + 1);
	}

	if (pHTTP->datalen != 0)
	{
		memset(command, '\0', sizeof(command));

		/* Read the response */
		snprintf(command, sizeof(command), "AT+HTTPREAD=0,%ld", pHTTP->datalen);
		status = SIM7600_sendATCommand(me, command, "\r\n+HTTPREAD:", SIM_URC_ERROR, 20000);
		if (THT_OK == status)
		{
			HAL_Delay(100);
			sPtr = strstr(me->parent.buffer, "\r\n{");

			if (sPtr != NULL)
			{
				if (pHTTP->datalen > sizeof(pHTTP->response))
				{
					memcpy(pHTTP->response, sPtr + 2, sizeof(pHTTP->response));
					pHTTP->response[SIM_HTTP_RESPONSE_SIZE - 1] = '\0';
				}
				else
				{
					memcpy(pHTTP->response, sPtr + 2, pHTTP->datalen);
				}
			}
		}
	}

	/* Terminal the session */
	SIM7600_sendATCommand(me, "AT+HTTPTERM", SIM_URC_OK, SIM_URC_ERROR, 20000);

	if (pHTTP->errorcode == 200 || status == THT_OK)
	{
		return THT_OK;
	}
	else
	{
		return THT_ERROR;
	}
}

/**
 * @brief Download file using HTTP
 *
 * @param me [in] A pointer to SIM7600_HandleTypeDef structure
 * @param pUrl [in] A pointer to URL string
 * @param pFilename [in] A pointer to the filename want to be save
 * @return THT_StatusTypeDef
 */
THT_StatusTypeDef SIM7600_getFileHTTP(SIM7600_HandleTypeDef *const me, const char *pUrl, const char *pFilename)
{
	if (me == NULL || pUrl == NULL || pFilename == NULL)
	{
		return THT_ERROR;
	}

	char command[SIM_HTTP_RESPONSE_SIZE] = {0};

	THT_StatusTypeDef status;

	/* Terminate the last session */
	SIM7600_sendATCommand(me, "AT+HTTPTERM", SIM_URC_OK, SIM_URC_ERROR, 10000);

	/* Begin a new session */
	status = SIM7600_sendATCommand(me, "AT+HTTPINIT", SIM_URC_OK, SIM_URC_ERROR, 10000);
	if (THT_OK != status)
	{
		return status;
	}

	/* Insert the URL */
	snprintf(command, sizeof(command), "AT+HTTPPARA=\"URL\",\"%s\"", pUrl);
	status = SIM7600_sendATCommand(me, command, SIM_URC_OK, SIM_URC_ERROR, 10000);
	if (THT_OK != status)
	{
		return status;
	}

	/* Use GET method to get file */
	status = SIM7600_sendATCommand(me, "AT+HTTPACTION=0", "+HTTPACTION:", SIM_URC_ERROR, 10000);
	if (THT_OK != status)
	{
		return status;
	}

	/* Save file */
	memset(command, '\0', sizeof(command));
	snprintf(command, sizeof(command), "AT+HTTPREADFILE=\"%s\"", pFilename);
	status = SIM7600_sendATCommand(me, command, "+HTTP_PEER_CLOSED", SIM_URC_ERROR, 20000);

	/* Terminate the session */
	SIM7600_sendATCommand(me, "AT+HTTPTERM", SIM_URC_OK, SIM_URC_ERROR, 10000);

	return THT_OK;
}
