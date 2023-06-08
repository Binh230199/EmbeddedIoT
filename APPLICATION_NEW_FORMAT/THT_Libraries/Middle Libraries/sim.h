/*
 * sim.h
 *
 *  Created on: Mar 23, 2023
 *      Author: Hoang Van Binh
 */

#ifndef INC_SIM_H_
#define INC_SIM_H_

/* Includes ------------------------------------------------------------------*/
#include "wlv_def.h"

#define SIM_BUFFER_SIZE (560U)			 /* The max size of the buffer used to stored the data response from the GSM module */
#define SIM_PHONE_SIZE (13U)			 /* The sim phone number length */
#define SIM_IMEI_SIZE (16U)				 /* The sim imei length */
#define SIM_TIME_SIZE (22U)				 /* The local time string */
#define SIM_NWTYPE_SIZE (16U)			 /* The max network type string length */
#define SIM_SMS_SIZE (160U)				 /* The max SMS content length */
#define SIM_SMS_RESPONSE_SIZE (500U)	 /* The max size of the buffer stores the message to response the SMS */
#define SIM_HTTP_URL_SIZE (200U)		 /* The max size of the HTTP URL */
#define SIM_HTTP_CONTENT_TYPE_SIZE (20U) /* The max size of the HTTP content length */
#define SIM_HTTP_DATA_SIZE (500U)		 /* The max size of the buffer store the data to request the HTTP Server */
#define SIM_HTTP_RESPONSE_SIZE (500U)	 /* The max size of the buffer store the response of HTTP Server */

/**
 * @brief SIM data structure
 *
 */
typedef struct
{
	char simPhone[SIM_PHONE_SIZE];
	char timezone[SIM_TIME_SIZE];
	char imei[SIM_IMEI_SIZE];
	char network[SIM_NWTYPE_SIZE];
	int16_t rssi;
	int8_t temp;
} SIM_DataTypeDef;

/**
 * @brief SMS data structure
 *
 */
typedef struct
{
	char phone[SIM_PHONE_SIZE];
	char time[SIM_TIME_SIZE];
	char content[SIM_SMS_SIZE];
} SIM_SMSTypeDef;

/**
 * @brief HTTP data structure
 *
 */
typedef struct
{
	uint8_t method;
	char url[SIM_HTTP_URL_SIZE];
	char type[SIM_HTTP_CONTENT_TYPE_SIZE];
	char data[SIM_HTTP_DATA_SIZE];
	char response[SIM_HTTP_RESPONSE_SIZE];
	uint16_t errorcode;
	uint32_t datalen;
} SIM_HTTPTypeDef;


/**
 * @brief SIM handle structure definition
 *
 */
typedef struct sim
{
	volatile uint32_t CSR;	 /* Sim status (software) register */
	volatile uint32_t simDR; /* Sim data (software) register */
	char buffer[SIM_BUFFER_SIZE];
	uint16_t timeoutCnt;
	THT_StatusTypeDef (*simSendATCommand)(struct sim *const me, const char *command, const char *expect, const char *unexpect, uint32_t timeout);
	void (*simWakeup)(struct sim *const me);
	void (*simSleep)(struct sim *const me);
	THT_StatusTypeDef (*simInit)(struct sim *const me);
	THT_StatusTypeDef (*simSetFunction)(struct sim *const me, uint8_t mode);
	void (*simProcess)(struct sim *const me);
	void (*simCheckEvent)(struct sim *const me, uint16_t size);
	THT_StatusTypeDef (*simReadSMS)(struct sim *const me, uint8_t order, SIM_SMSTypeDef *pSMS);
	THT_StatusTypeDef (*simSendSMS)(struct sim *const me, const char *pPhone, const char *pMsg, uint8_t phoneSize, uint16_t msgSize);
	int8_t (*simGetSMSCounter)(struct sim *const me);
	THT_StatusTypeDef (*simCallUSSD)(struct sim *const me, const char *ussd, char response[], uint16_t size);
	int8_t (*simGetRSSI)(struct sim *const me);
	int8_t (*simGetTemperature)(struct sim *const me);
	uint8_t (*simGetNwtype)(struct sim *const me, char pNwtype[], uint8_t size);
	THT_StatusTypeDef (*simGetSimPhone)(struct sim *const me, char pPhone[], uint8_t size);
	THT_StatusTypeDef (*simGetIMEI)(struct sim *const me, char pIMEI[], uint8_t size);
	THT_StatusTypeDef (*simGetLocalTime)(struct sim *const me, char pTime[], uint8_t size);
	uint32_t (*simGetFileSize)(struct sim *const me, const char *pFilename);
	THT_StatusTypeDef (*simDeleteFile)(struct sim *const me, const char *pFilename);
	THT_StatusTypeDef (*simSetLocalTimeAuto)(struct sim *const me);
	THT_StatusTypeDef (*simSetLocalTimeManual)(struct sim *const me, const char *pTime, uint8_t size);
	THT_StatusTypeDef (*simSelectPhonebookMem)(struct sim *const me);
	THT_StatusTypeDef (*simSavePhonebook)(struct sim *const me, uint8_t order, const char *pPhone, uint8_t phoneSize);
	THT_StatusTypeDef (*simReadPhonebook)(struct sim *const me, uint8_t order, char pPhone[], uint8_t phoneSize);
	THT_StatusTypeDef (*simFindPhonebook)(struct sim *const me, const char *pPhone);
	THT_StatusTypeDef (*simSendHTTP)(struct sim *const me, SIM_HTTPTypeDef *pHTTP);
	THT_StatusTypeDef (*simGetFileHTTP)(struct sim *const me, const char *pUrl, const char *pFilename);
} SIM_HandleTypeDef;

typedef THT_StatusTypeDef (*simSendATCommand)(struct sim *const me, const char *command, const char *expect, const char *unexpect, uint32_t timeout);
typedef void (*simWakeup)(struct sim *const me);
typedef void (*simSleep)(struct sim *const me);
typedef THT_StatusTypeDef (*simInit)(struct sim *const me);
typedef THT_StatusTypeDef (*simSetFunction)(struct sim *const me, uint8_t mode);
typedef void (*simProcess)(struct sim *const me);
typedef void (*simCheckEvent)(struct sim *const me, uint16_t size);
typedef THT_StatusTypeDef (*simReadSMS)(struct sim *const me, uint8_t order, SIM_SMSTypeDef *pSMS);
typedef THT_StatusTypeDef (*simSendSMS)(struct sim *const me, const char *pPhone, const char *pMsg, uint8_t phoneSize, uint16_t msgSize);
typedef int8_t (*simGetSMSCounter)(struct sim *const me);
typedef THT_StatusTypeDef (*simCallUSSD)(struct sim *const me, const char *ussd, char response[], uint16_t size);
typedef int8_t (*simGetRSSI)(struct sim *const me);
typedef int8_t (*simGetTemperature)(struct sim *const me);
typedef uint8_t (*simGetNwtype)(struct sim *const me, char pNwtype[], uint8_t size);
typedef THT_StatusTypeDef (*simGetSimPhone)(struct sim *const me, char pPhone[], uint8_t size);
typedef THT_StatusTypeDef (*simGetIMEI)(struct sim *const me, char pIMEI[], uint8_t size);
typedef THT_StatusTypeDef (*simGetLocalTime)(struct sim *const me, char pTime[], uint8_t size);
typedef uint32_t (*simGetFileSize)(struct sim *const me, const char *pFilename);
typedef THT_StatusTypeDef (*simDeleteFile)(struct sim *const me, const char *pFilename);
typedef THT_StatusTypeDef (*simSetLocalTimeAuto)(struct sim *const me);
typedef THT_StatusTypeDef (*simSetLocalTimeManual)(struct sim *const me, const char *pTime, uint8_t size);
typedef THT_StatusTypeDef (*simSelectPhonebookMem)(struct sim *const me);
typedef THT_StatusTypeDef (*simSavePhonebook)(struct sim *const me, uint8_t order, const char *pPhone, uint8_t phoneSize);
typedef THT_StatusTypeDef (*simReadPhonebook)(struct sim *const me, uint8_t order, char pPhone[], uint8_t phoneSize);
typedef THT_StatusTypeDef (*simFindPhonebook)(struct sim *const me, const char *pPhone);
typedef THT_StatusTypeDef (*simSendHTTP)(struct sim *const me, SIM_HTTPTypeDef *pHTTP);
typedef THT_StatusTypeDef (*simGetFileHTTP)(struct sim *const me, const char *pUrl, const char *pFilename);

/**
 * @brief SIM Control & Status Register
 *
 */
#define SIM_CSR_RX_Pos (0U)
#define SIM_CSR_RX_Msk (0x1U << SIM_CSR_RX_Pos)
#define SIM_CSR_RX SIM_CSR_RX_Msk
#define SIM_CSR_SMS_Pos (1U)
#define SIM_CSR_SMS_Msk (0x1U << SIM_CSR_SMS_Pos)
#define SIM_CSR_SMS SIM_CSR_SMS_Msk
#define SIM_CSR_UDV_Pos (2U)
#define SIM_CSR_UDV_Msk (0x1U << SIM_CSR_UDV_Pos)
#define SIM_CSR_UDV SIM_CSR_UDV_Msk
#define SIM_CSR_UPV_Pos (3U)
#define SIM_CSR_UPV_Msk (0x1U << SIM_CSR_UPV_Pos)
#define SIM_CSR_UPV SIM_CSR_UPV_Msk
#define SIM_CSR_SRMV_Pos (4U)
#define SIM_CSR_SRMV_Msk (0x1U << SIM_CSR_SRMV_Pos)
#define SIM_CSR_SRMV SIM_CSR_SRMV_Msk
#define SIM_CSR_SEHTTPFRC_Pos (5U)
#define SIM_CSR_SEHTTPFRC_Msk (0x1UL << SIM_CSR_SEHTTPFRC_Pos)
#define SIM_CSR_SEHTTPFRC SIM_CSR_SEHTTPFRC_Msk

/**
 * @brief SIM Data Register
 *
 */
#define SIM_DR_RSSI_Pos (0U)
#define SIM_DR_RSSI_Msk (1UL << SIM_DR_RSSI_Pos)
#define SIM_DR_RSSI SIM_DR_RSSI_Msk
#define SIM_DR_NWTYPE_Pos (1U)
#define SIM_DR_NWTYPE_Msk (1UL << SIM_DR_NWTYPE_Pos)
#define SIM_DR_NWTYPE SIM_DR_NWTYPE_Msk
#define SIM_DR_SIMPHONE_Pos (2U)
#define SIM_DR_SIMPHONE_Msk (1UL << SIM_DR_SIMPHONE_Pos)
#define SIM_DR_SIMPHONE SIM_DR_SIMPHONE_Msk
#define SIM_DR_IMEI_Pos (3U)
#define SIM_DR_IMEI_Msk (1UL << SIM_DR_IMEI_Pos)
#define SIM_DR_IMEI SIM_DR_IMEI_Msk
#define SIM_DR_TIME_Pos (4U)
#define SIM_DR_TIME_Msk (1UL << SIM_DR_TIME_Pos)
#define SIM_DR_TIME SIM_DR_TIME_Msk
#define SIM_DR_TEMP_Pos (5U)
#define SIM_DR_TEMP_Msk (1UL << SIM_DR_TEMP_Pos)
#define SIM_DR_TEMP SIM_DR_TEMP_Msk

/**
 * @brief SIM flag definitions
 *
 */
#define SIM_FLAG_RX SIM_CSR_RX					   /* Flag receive response done from GSM module  */
#define SIM_FLAG_SMS SIM_CSR_SMS				   /* Flag has SMS */
#define SIM_FLAG_UDV SIM_CSR_UDV				   /* Flag undervoltage */
#define SIM_FLAG_UPV SIM_CSR_UPV				   /* Flag upper voltage */
#define SIM_FLAG_SRMV SIM_CSR_SRMV				   /* Flag simcard is removed */
#define SIM_FLAG_SEND_HTTP_FORCE SIM_CSR_SEHTTPFRC /* Flag ??? - do not use */

/**
 * @brief Get the SIM's flag status.
 * @param __ME__: SIM handle
 * @param __FLAG__: SIM flag
 * 			This parameter can be any combination of the following values:
 * 			@arg SIM_FLAG_RX: SIM Rx Done
 * 			@arg SIM_FLAG_SMS: SIM SMS come
 * 			@arg SIM_FLAG_UDV: SIM under voltage
 * 			@arg SIM_FLAG_UPV: SIM over voltage
 * 			@arg SIM_FLAG_SRMV: SIM removed (unplugin)
 * 			@arg SIM_FLAG_SEND_HTTP_FORCE: In addition to the binding conditions,
 * 										a packet can be sent when the SIM_FLAG_SEND_HTTP_FORCE
 * 										flag is set without having to satisfy those conditions.
 * @retval None
 */
#define SIM_GET_FLAG(__ME__, __FLAG__) \
	((((__ME__)->CSR) & (__FLAG__)) == (__FLAG__))

/**
 * @brief Set the SIM's flag status.
 * @param __ME__: SIM handle
 * @param __FLAG__: SIM flag
 * 			This parameter can be any combination of the following values:
 * 			@arg SIM_FLAG_RX: SIM Read time flag
 * 			@arg SIM_FLAG_SMS: SIM SMS come
 * 			@arg SIM_FLAG_UDV: SIM under voltage
 * 			@arg SIM_FLAG_UPV: SIM over voltage
 * 			@arg SIM_FLAG_SRMV: SIM removed (unplugin)
 * 			@arg SIM_FLAG_SEND_HTTP_FORCE: In addition to the binding conditions,
 * 										a packet can be sent when the SIM_FLAG_SEND_HTTP_FORCE
 * 										flag is set without having to satisfy those conditions.
 * @retval None
 */
#define SIM_SET_FLAG(__ME__, __FLAG__) \
	(((__ME__)->CSR) |= (__FLAG__))

/**
 * @brief Clear the SIM's flag status.
 * @param __ME__: SIM handle
 * @param __FLAG__: SIM flag
 * 			This parameter can be any combination of the following values:
 * 			@arg SIM_FLAG_RX: SIM Read time flag
 * 			@arg SIM_FLAG_SMS: SIM SMS come
 * 			@arg SIM_FLAG_UDV: SIM under voltage
 * 			@arg SIM_FLAG_UPV: SIM over voltage
 * 			@arg SIM_FLAG_SRMV: SIM removed (unplugin)
 * 			@arg SIM_FLAG_SEND_HTTP_FORCE: In addition to the binding conditions,
 * 										a packet can be sent when the SIM_FLAG_SEND_HTTP_FORCE
 * 										flag is set without having to satisfy those conditions.
 * @retval None
 */
#define SIM_CLR_FLAG(__ME__, __FLAG__) \
	(((__ME__)->CSR) &= ~(__FLAG__))

/*
 * @brief SIM HTTP Methods
 */
#define SIM_HTTP_GET (0U)
#define SIM_HTTP_POST (1U)
#define SIM_HTTP_PUT (2U)
#define SIM_HTTP_DELETE (3U)

/*
 * @brief SIM function mode
 */
#define SIM_MODE_MINIMUM (0U)
#define SIM_MODE_FULL (1U)
#define SIM_MODE_FLIGHT (4U)
#define SIM_MODE_TEST (5U)
#define SIM_MODE_RESET (6U)
#define SIM_MODE_OFFLINE (7U)

/*
 * @brief SIM URC
 */
#define SIM_URC_SMS "+CMTI: \"SM\","
#define SIM_URC_UNDER_VOLTAGE "UNDER-VOLTAGE WARNNING"
#define SIM_URC_SIMCARD_UNPLUGIN "+SIMCARD: NOT AVAILABLE"
#define SIM_URC_NETWORK_REPORT "+CNSMOD:"
#define SIM_URC_URC_SIGNAL_REPORT "+CSQ:"
#define SIM_URC_OK "OK"
#define SIM_URC_ERROR "ERROR"

/* Function macros ---------------------------------------------------------------------*/
#if 0
#define SIM_sendATCommand(__ME__, command, expect, unexpect, timeout) ((__ME__)->sendATCommand)((__ME__), (command), (expect), (unexpect), (timeout))
#define SIM_init(__ME__) ((__ME__)->init)((__ME__))
#define SIM_wakeup(__ME__) ((__ME__)->wakeup)((__ME__))
#define SIM_sleep(__ME__) ((__ME__)->sleep)((__ME__))
#define SIM_setFunction(__ME__, mode) ((__ME__)->setFunction)((__ME__), (mode))
#define SIM_process(__ME__) ((__ME__)->process)((__ME__))
#define SIM_checkEvent(__ME__, size) ((__ME__)->checkEvent)((__ME__), (size))
#define SIM_readSMS(__ME__, order, pSMS) ((__ME__)->readSMS)((__ME__), (order), (pSMS))
#define SIM_sendSMS(__ME__, pPhone, pMsg, phoneSize, msgSize) ((__ME__)->sendSMS)((__ME__), (pPhone), (pMsg), (phoneSize), (msgSize))
#define SIM_getSMSCounter(__ME__) ((__ME__)->getSMSCounter)((__ME__))
#define SIM_callUSSD(__ME__, ussd, response, size) ((__ME__)->callUSSD)((__ME__), (ussd), (response), (size))
#define SIM_getRSSI(__ME__) ((__ME__)->getRSSI)((__ME__))
#define SIM_getNwtype(__ME__) ((__ME__)->getNwtype)((__ME__))
#define SIM_getSimPhone(__ME__, pPhone, size) ((__ME__)->getSimPhone)((__ME__), (pPhone), (size))
#define SIM_getIMEI(__ME__, pIMEI, size) ((__ME__)->getIMEI)((__ME__), (pIMEI), (size))
#define SIM_getLocalTime(__ME__, pTime, size) ((__ME__)->getLocalTime)((__ME__), (pTime), (size))
#define SIM_getFileSize(__ME__, pFilename) ((__ME__)->getFileSize)((__ME__), (pFilename))
#define SIM_deleteFile(__ME__, pFilename) ((__ME__)->deleteFile)((__ME__), (pFilename))
#define SIM_setLocalTimeAuto(__ME__) ((__ME__)->setLocalTimeAuto)((__ME__))
#define SIM_setLocalTimeManual(__ME__, pTime, size) ((__ME__)->setLocalTimeManual)((__ME__), (pTime), (size))
#define SIM_selectPhonebookMem(__ME__) ((__ME__)->selectPhonebookMem)((__ME__))
#define SIM_savePhonebook(__ME__, order, pPhone, phoneSize) ((__ME__)->savePhonebook)((__ME__), (order), (pPhone), (phoneSize))
#define SIM_readPhonebook(__ME__, order, pPhone, phoneSize) ((__ME__)->readPhonebook)((__ME__), (order), (pPhone), (phoneSize))
#define SIM_findPhonebook(__ME__, pPhone) ((__ME__)->findPhonebook)((__ME__), (pPhone))
#define SIM_sendHTTP(__ME__, pHTTP) ((__ME__)->sendHTTP)((__ME__), (pHTTP))
#define SIM_getFileHTTP(__ME__, pUrl, pFilename) ((__ME__)->getFileHTTP)((__ME__), (pUrl), (pFilename))
#endif

/* Public functions --------------------------------------------------------------------*/
THT_StatusTypeDef SIM_getData(SIM_HandleTypeDef *const me, uint32_t data, SIM_DataTypeDef *pData);
THT_StatusTypeDef SIM_sendATCommand(SIM_HandleTypeDef *const me, const char *command, const char *expect, const char *unexpect, uint32_t timeout);
THT_StatusTypeDef SIM_init(SIM_HandleTypeDef *const me);
void SIM_wakeup(SIM_HandleTypeDef *const me);
void SIM_sleep(SIM_HandleTypeDef *const me);
THT_StatusTypeDef SIM_setFunction(SIM_HandleTypeDef *const me, uint8_t mode);
void SIM_process(SIM_HandleTypeDef *const me);
void SIM_checkEvent(SIM_HandleTypeDef *const me, uint16_t size);
THT_StatusTypeDef SIM_readSMS(SIM_HandleTypeDef *const me, uint8_t order, SIM_SMSTypeDef *pSMS);
THT_StatusTypeDef SIM_sendSMS(SIM_HandleTypeDef *const me, const char *pPhone, const char *pMsg, uint8_t phoneSize, uint16_t msgSize);
int8_t SIM_getSMSCounter(SIM_HandleTypeDef *const me);
THT_StatusTypeDef SIM_callUSSD(SIM_HandleTypeDef *const me, const char *ussd, char response[], uint16_t size);
int8_t SIM_getRSSI(SIM_HandleTypeDef *const me);
uint8_t SIM_getNwtype(SIM_HandleTypeDef *const me, char pNwtype[], uint8_t size);
THT_StatusTypeDef SIM_getSimPhone(SIM_HandleTypeDef *const me, char pPhone[], uint8_t size);
THT_StatusTypeDef SIM_getIMEI(SIM_HandleTypeDef *const me, char pIMEI[], uint8_t size);
THT_StatusTypeDef SIM_getLocalTime(SIM_HandleTypeDef *const me, char pTime[], uint8_t size);
uint32_t SIM_getFileSize(SIM_HandleTypeDef *const me, const char *pFilename);
THT_StatusTypeDef SIM_deleteFile(SIM_HandleTypeDef *const me, const char *pFilename);
THT_StatusTypeDef SIM_setLocalTimeAuto(SIM_HandleTypeDef *const me);
THT_StatusTypeDef SIM_setLocalTimeManual(SIM_HandleTypeDef *const me, const char *pTime, uint8_t size);
THT_StatusTypeDef SIM_selectPhonebookMem(SIM_HandleTypeDef *const me);
THT_StatusTypeDef SIM_savePhonebook(SIM_HandleTypeDef *const me, uint8_t order, const char *pPhone, uint8_t phoneSize);
THT_StatusTypeDef SIM_readPhonebook(SIM_HandleTypeDef *const me, uint8_t order, char pPhone[], uint8_t phoneSize);
THT_StatusTypeDef SIM_findPhonebook(SIM_HandleTypeDef *const me, const char *pPhone);
THT_StatusTypeDef SIM_sendHTTP(SIM_HandleTypeDef *const me, SIM_HTTPTypeDef *pHTTP);
THT_StatusTypeDef SIM_getFileHTTP(SIM_HandleTypeDef *const me, const char *pUrl, const char *pFilename);

#if TEST
THT_StatusTypeDef SIM_test(SIM_HandleTypeDef *const me);
#endif

#endif /* INC_SIM_H_ */
