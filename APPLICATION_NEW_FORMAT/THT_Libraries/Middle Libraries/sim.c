/*
 * sim.c
 *
 *  Created on: Mar 23, 2023
 *      Author: Hoàng Văn Bình
 */

#include "sim.h"

/**
 * @brief  Get data of module SIM
 * @param me [in] SIM Handle
 * @param data [in] combine of data want to get:
 * 				@arg SIM_DR_RSSI,
 * 				@arg SIM_DR_NWTYPE,
 * 				@arg SIM_DR_SIMPHONE,
 * 				@arg SIM_DR_IMEI,
 * 				@arg SIM_DR_TIME,
 * @param pData [out] a pointer to SIM data structure
 * @retval THT Status
 */
THT_StatusTypeDef SIM_getData(SIM_HandleTypeDef *const me, uint32_t data, SIM_DataTypeDef *pData)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	if ((data & SIM_DR_RSSI) == SIM_DR_RSSI)
	{
		pData->rssi = (*me->simGetRSSI)(me);
	}
	if ((data & SIM_DR_NWTYPE) == SIM_DR_NWTYPE)
	{
		(*me->simGetNwtype)(me, pData->network, sizeof(pData->network));
	}
	if ((data & SIM_DR_SIMPHONE) == SIM_DR_SIMPHONE)
	{
		(*me->simGetSimPhone)(me, pData->simPhone, sizeof(pData->simPhone));
	}
	if ((data & SIM_DR_IMEI) == SIM_DR_IMEI)
	{
		(*me->simGetIMEI)(me, pData->imei, sizeof(pData->imei));
	}
	if ((data & SIM_DR_TIME) == SIM_DR_TIME)
	{
		(*me->simGetLocalTime)(me, pData->timezone, sizeof(pData->timezone));
	}
	if ((data & SIM_DR_TEMP) == SIM_DR_TEMP)
	{
		pData->temp = (*me->simGetTemperature)(me);
	}
	return THT_OK;
}

/**
 * @brief  Send AT command
 * @param me [in] SIM Handle
 * @param command [in] AT command string
 * @param expected [in] String expect in response
 * @param unpected [in] String unexpect in response
 * @param timeout [in] Timeout to exit function
 * @retval THT Status
 */
THT_StatusTypeDef SIM_sendATCommand(SIM_HandleTypeDef *const me, const char *command, const char *expect,
		const char *unexpect, uint32_t timeout)
{
	if (me == NULL || command == NULL || expect == NULL || unexpect == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simSendATCommand)(me, command, expect, unexpect, timeout);
}

/**
 * @brief  Init module GSM
 * @param me [in] SIM Handle
 * @retval THT Status
 */
THT_StatusTypeDef SIM_init(SIM_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simInit)(me);
}

/**
 * @brief  Wake up module GSM
 * @param me [in] SIM Handle
 * @retval THT Status
 */
void SIM_wakeup(SIM_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return;
	}

	(*me->simWakeup)(me);
}

/**
 * @brief  Module GSM enter sleep mode
 * @param me [in] SIM Handle
 * @retval THT Status
 */
void SIM_sleep(SIM_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return;
	}

	(*me->simSleep)(me);
}

/**
 * @brief  Set function operation for module
 * @param me [in] SIM Handle
 * @param mode [in] Mode operation
 * 				@arg SIM_MODE_MINIMUM,
 * 				@arg SIM_MODE_FULL,
 * 				@arg SIM_MODE_FLIGHT,
 * 				@arg SIM_MODE_TEST,
 * 				@arg SIM_MODE_RESET,
 * 				@arg SIM_MODE_OFFLINE,
 * @retval THT Status
 */
THT_StatusTypeDef SIM_setFunction(SIM_HandleTypeDef *const me, uint8_t mode)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simSetFunction)(me, mode);
}

/**
 * @brief  Sim process
 * @param me [in] SIM Handle
 * @retval None
 */
void SIM_process(SIM_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return;
	}

	(*me->simProcess)(me);
}

/**
 * @brief  SIM check event
 * @param me [in] SIM Handle
 * @param size [in] The size of AT response
 * @retval None
 */
void SIM_checkEvent(SIM_HandleTypeDef *const me, uint16_t size)
{
	if (me == NULL)
	{
		return;
	}

	(*me->simCheckEvent)(me, size);
}

/**
 * @brief  Read SMS 
 * @param me [in] SIM Handle
 * @param order [in] The order of SMS in memory 
 * @param pSMS [out] Pointer to SIM_SMSTypeDef
 * @retval THT Status
 */
THT_StatusTypeDef SIM_readSMS(SIM_HandleTypeDef *const me, uint8_t order, SIM_SMSTypeDef *pSMS)
{
	if (me == NULL || pSMS == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simReadSMS)(me, order, pSMS);
}

/**
 * @brief  Send SMS 
 * @param me [in] SIM Handle
 * @param pPhone [in] The receiver phonenumber
 * @param pMsg [int] The messsage content
 * @param phoneSize [in] The length of the receiver phonenumber
 * @param msgSize [in] The length of message content
 * @retval THT Status
 */
THT_StatusTypeDef SIM_sendSMS(SIM_HandleTypeDef *const me, const char *pPhone, const char *pMsg, uint8_t phoneSize,
		uint16_t msgSize)
{
	if (me == NULL || pPhone == NULL || pMsg == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simSendSMS)(me, pPhone, pMsg, phoneSize, msgSize);
}

/**
 * @brief  Sim get number SMS in memory
 * @param me [in] SIM Handle
 * @retval Number of SMS
 */
int8_t SIM_getSMSCounter(SIM_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simGetSMSCounter)(me);
}

/**
 * @brief  Implement a call USSD
 * @param me [in] SIM Handle
 * @param ussd [in] USSD syntax
 * @param response [out] Response content
 * @param size [in] Size of response buffer want to be stored
 * @retval THT status
 */
THT_StatusTypeDef SIM_callUSSD(SIM_HandleTypeDef *const me, const char *ussd, char response[], uint16_t size)
{
	if (me == NULL || ussd == NULL || response == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simCallUSSD)(me, ussd, response, size);
}

/**
 * @brief Get RSSI in dBm
 * @param me [in] SIM Handle
 * @retval RSSI
 */
int8_t SIM_getRSSI(SIM_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simGetRSSI)(me);
}

/**
 * @brief Get network type
 * @param me [in] SIM Handle
 * @param pNwtype [out] a buffer to store network content
 * @param size [in] size of buffer
 * @retval THT Status
 */
uint8_t SIM_getNwtype(SIM_HandleTypeDef *const me, char pNwtype[], uint8_t size)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simGetNwtype)(me, pNwtype, size);
}

/**
 * @brief Get SIM phonenumber
 * 
 * @param me [in] SIM Handle
 * @param pPhone [out] a buffer to store phonenumber
 * @param size [in] size of buffer
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_getSimPhone(SIM_HandleTypeDef *const me, char pPhone[], uint8_t size)
{
	if (me == NULL || pPhone == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simGetSimPhone)(me, pPhone, size);
}

/**
 * @brief 
 * 
 * @param me [in] SIM Handle
 * @param pIMEI a buffer to store IMEI
 * @param size [in] size of buffer
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_getIMEI(SIM_HandleTypeDef *const me, char pIMEI[], uint8_t size)
{
	if (me == NULL || pIMEI == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simGetIMEI)(me, pIMEI, size);
}

/**
 * @brief 
 * 
 * @param me [in] SIM Handle
 * @param pTime a buffer to store timezone
 * @param size [in] size of buffer
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_getLocalTime(SIM_HandleTypeDef *const me, char pTime[], uint8_t size)
{
	if (me == NULL || pTime == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simGetLocalTime)(me, pTime, size);
}

/**
 * @brief Get size of file
 * 
 * @param me [in] SIM Handle
 * @param pFilename [in] pointer to a const char that is a filename
 * @return uint32_t 
 */
uint32_t SIM_getFileSize(SIM_HandleTypeDef *const me, const char *pFilename)
{
	if (me == NULL || pFilename == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simGetFileSize)(me, pFilename);
}

/**
 * @brief 
 * 
 * @param me [in] SIM Handle
 * @param pFilename [in] pointer to a const char that is a filename
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_deleteFile(SIM_HandleTypeDef *const me, const char *pFilename)
{
	if (me == NULL || pFilename == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simDeleteFile)(me, pFilename);
}

/**
 * @brief [in] Set local timezone automatically
 * 
 * @param me [in] SIM Handle
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_setLocalTimeAuto(SIM_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simSetLocalTimeAuto)(me);
}

/**
 * @brief Set local timezone manual by passed a timezone string 
 * 
 * @param me [in] SIM Handle
 * @param pTime [in] pointer to a const char that is a timezone string
 * @param size [in] sizeof timezone string
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_setLocalTimeManual(SIM_HandleTypeDef *const me, const char *pTime, uint8_t size)
{
	if (me == NULL || pTime)
	{
		return THT_ERROR;
	}

	return (*me->simSetLocalTimeManual)(me, pTime, size);
}

/**
 * @brief Select memory to manipulate with phonebook
 * 
 * @param me [in] SIM Handle
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_selectPhonebookMem(SIM_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simSelectPhonebookMem)(me);
}

/**
 * @brief Save a contact to phonebook memory
 * 
 * @param me [in] SIM Handle
 * @param order [in] the order in phonebook
 * @param pPhone [in] pointer to const char that is a phonenumber
 * @param phoneSize [in] the length of phonenumber
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_savePhonebook(SIM_HandleTypeDef *const me, uint8_t order, const char *pPhone, uint8_t phoneSize)
{
	if (me == NULL || pPhone == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simSavePhonebook)(me, order, pPhone, phoneSize);
}

/**
 * @brief Read a contact
 * 
 * @param me [in] SIM Handle
 * @param order [in] the order in phonebook
 * @param pPhone [out] a buffer used to store phonenumber
 * @param phoneSize [in] size of buffer
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_readPhonebook(SIM_HandleTypeDef *const me, uint8_t order, char pPhone[], uint8_t phoneSize)
{
	if (me == NULL || pPhone == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simReadPhonebook)(me, order, pPhone, phoneSize);
}

/**
 * @brief Find a contact
 * 
 * @param me [in] SIM Handle
 * @param pPhone [in] pointer to const char that is a phonenumber
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_findPhonebook(SIM_HandleTypeDef *const me, const char *pPhone)
{
	if (me == NULL || pPhone == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simFindPhonebook)(me, pPhone);
}

/**
 * @brief Send a request HTTP
 * @note Must be config pHTTP before use this fucntion
 * 
 * @param me [in] SIM Handle
 * @param pHTTP a pointer to SIM_HTTPTypeDef that contains HTTP information and data
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_sendHTTP(SIM_HandleTypeDef *const me, SIM_HTTPTypeDef *pHTTP)
{
	if (me == NULL || pHTTP == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simSendHTTP)(me, pHTTP);
}

/**
 * @brief Download a file using HTTP
 * 
 * @param me [in] SIM Handle
 * @param pHTTP a pointer to const char that contains file URL
 * @param pFilename a pointer to const char that contains name of file
 * @return THT_StatusTypeDef 
 */
THT_StatusTypeDef SIM_getFileHTTP(SIM_HandleTypeDef *const me, const char *pUrl, const char *pFilename)
{
	if (me == NULL || pUrl == NULL || pFilename == NULL)
	{
		return THT_ERROR;
	}

	return (*me->simGetFileHTTP)(me, pUrl, pFilename);
}

#if TEST
#include <stdio.h>
THT_StatusTypeDef SIM_test(SIM_HandleTypeDef * const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	SIM_wakeup(me);
	SIM_DataTypeDef simData = { 0 };
//	SIM_setLocalTimeAuto(me);
	SIM_getData(me, SIM_DR_IMEI | SIM_DR_RSSI | SIM_DR_NWTYPE | SIM_DR_SIMPHONE | SIM_DR_TIME , &simData);

	printf("==============\n"
			"IMEI: %s\n"
			"RSSI: %d\n"
			"NWTYPE: %s\n"
			"SIMPHONE: %s\n"
			"TIME: %s\n\n", simData.imei, simData.rssi, simData.network, simData.simPhone, simData.timezone);
	SIM_sleep(me);
	return THT_OK;
}
#endif
