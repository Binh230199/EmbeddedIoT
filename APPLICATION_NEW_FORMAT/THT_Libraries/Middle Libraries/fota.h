/*
 * fota.h
 *
 *  Created on: Mar 25, 2023
 *      Author: Hoàng Văn Bình
 *      Gmail: binhhv.23.1.99@gmail.com
 */

#ifndef INC_FOTA_H_
#define INC_FOTA_H_

#include "http.h"
#include "sim.h"
#include "wlv_def.h"

typedef struct
{
	volatile uint32_t SR;
	uint16_t curVersion[3]; /* major - minor - patch */
	uint16_t newVersion[3]; /* major - minor - patch */
	uint32_t curAddress;
	uint32_t newAddress;
	uint32_t firmwareSize;
	uint32_t CRC32;
	uint8_t isNeedUpdate;
	char firmwareURL[180];
	char firmwareName[60];
} FOTA_HandleTypeDef;

/*
 * FOTA Status Register
 */

#define FOTA_SR_STT_Pos			(0U)
#define FOTA_SR_STT_Msk			(0xFUL << FOTA_SR_STT_Pos)
#define FOTA_SR_STT				FOTA_SR_STT_Msk

/**
 *		FOTA flag definitions
 */
#define FOTA_FLAG_UPEN				(0x1UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_UPOK				(0x2UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_GUER				(0x3UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_VERER				(0x4UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_NWSIG				(0x5UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_DOWER				(0x6UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_FFER				(0x7UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_WRIER				(0x8UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_CRCER				(0x9UL << FOTA_SR_STT_Pos)
#define FOTA_FLAG_RST				(0xAUL << FOTA_SR_STT_Pos)

/**
 * @brief Get the FOTA's flag status.
 * @param __ME__: FOTA handle
 * @param __FLAG__: FOTA flag
 * 			This parameter can be any combination of the following values:
 * 			@arg FOTA_FLAG_UPEN: FOTA Update Enable
 * 			@arg FOTA_FLAG_UPOK: FOTA Update Status
 * 			@arg FOTA_FLAG_VERER: FOTA Version Error
 * 			@arg FOTA_FLAG_NWSIG: FOTA Bad signal
 * 			@arg FOTA_FLAG_DOWER: FOTA Dowload file Error
 * 			@arg FOTA_FLAG_FFER: FOTA File Format or Oversize Error
 * 			@arg FOTA_FLAG_WRIER: FOTA Write Firmware Error
 * 			@arg FOTA_FLAG_CRCER: FOTA Check CRC Error
 * 			@arg FOTA_FLAG_RST: System Signal Reset
 * @retval None
 */
#define FOTA_GET_FLAG(__ME__, __FLAG__)		\
			((((__ME__)->SR) & (__FLAG__)) == (__FLAG__))

/**
 * @brief Set the FOTA's flag status.
 * @param __ME__: FOTA handle
 * @param __FLAG__: FOTA flag
 * 			This parameter can be any combination of the following values:
 * 			@arg FOTA_FLAG_UPEN: FOTA Update Enable
 * 			@arg FOTA_FLAG_UPOK: FOTA Update Status
 * 			@arg FOTA_FLAG_VERER: FOTA Version Error
 * 			@arg FOTA_FLAG_NWSIG: FOTA Bad signal
 * 			@arg FOTA_FLAG_DOWER: FOTA Dowload file Error
 * 			@arg FOTA_FLAG_FFER: FOTA File Format Error
 * 			@arg FOTA_FLAG_WRIER: FOTA Write Firmware Error
 * 			@arg FOTA_FLAG_CRCER: FOTA Check CRC Error
 * 			@arg FOTA_FLAG_RST: System Signal Reset
 * @retval None
 */
#define FOTA_SET_FLAG(__ME__, __FLAG__)		\
			(((__ME__)->SR) |= (__FLAG__))

/**
 * @brief Clear the FOTA's flag status.
 * @param __ME__: FOTA handle
 * @param __FLAG__: FOTA flag
 * 			This parameter can be any combination of the following values:
 * 			@arg FOTA_FLAG_UPEN: FOTA Update Enable
 * 			@arg FOTA_FLAG_UPOK: FOTA Update Status
 * 			@arg FOTA_FLAG_VERER: FOTA Version Error
 * 			@arg FOTA_FLAG_NWSIG: FOTA Bad signal
 * 			@arg FOTA_FLAG_DOWER: FOTA Dowload file Error
 * 			@arg FOTA_FLAG_FFER: FOTA File Format Error
 * 			@arg FOTA_FLAG_WRIER: FOTA Write Firmware Error
 * 			@arg FOTA_FLAG_CRCER: FOTA Check CRC Error
 * 			@arg FOTA_FLAG_RST: System Signal Reset
 * @retval None
 */
#define FOTA_CLR_FLAG(__ME__, __FLAG__)		\
			(((__ME__)->SR) &= ~(__FLAG__))

#define FOTA_BOOT_START_ADDR 	0x08000000U
#define FOTA_BOOT_MAX_SIZE		(32U)
#define FOTA_BOOT_END_ADDR		((FOTA_BOOT_START_ADDR) + ((FOTA_BOOT_MAX_SIZE) * 1024))

#define FOTA_APP_1_START_ADDR 	FOTA_BOOT_END_ADDR
#define FOTA_APP_1_MAX_SIZE		(240U)
#define FOTA_APP_1_END_ADDR		((FOTA_APP_1_START_ADDR) + ((FOTA_APP_1_MAX_SIZE) * 1024))

#define FOTA_APP_2_START_ADDR 	FOTA_APP_1_END_ADDR
#define FOTA_APP_2_MAX_SIZE		(FOTA_APP_1_MAX_SIZE)
#define FOTA_APP_2_END_ADDR		((FOTA_APP_2_START_ADDR) + ((FOTA_APP_2_MAX_SIZE) * 1024))

#define FOTA_NUMBER_FLASH_PAGES	((FOTA_APP_1_MAX_SIZE * 1024) / (256U))

#define FOTA_FIRMWARE_HEADER_NAME	"vwatercontroller_"

/* Next variable's EEPROM address = Prev variable's EEPROM address + sizeof(Prev variable's EEPROM address) */
#define FOTA_EEPROM_CURRENT_ADDR	(THT_FOTA_BASE)
#define FOTA_EEPROM_CURRENT_MAJOR 	(FOTA_EEPROM_CURRENT_ADDR + 4)
#define FOTA_EEPROM_CURRENT_MINOR 	(FOTA_EEPROM_CURRENT_MAJOR + 2)
#define FOTA_EEPROM_CURRENT_PATCH 	(FOTA_EEPROM_CURRENT_MINOR + 2)
#define FOTA_EEPROM_SR				(FOTA_EEPROM_CURRENT_PATCH + 2)
#define FOTA_EEPROM_MFD_DATE		(FOTA_EEPROM_SR + 4)
#define FOTA_EEPROM_MFD_MONTH		(FOTA_EEPROM_MFD_DATE + 1)
#define FOTA_EEPROM_MFD_YEAR		(FOTA_EEPROM_MFD_MONTH + 1)

void FOTA_process(FOTA_HandleTypeDef *const me);
THT_StatusTypeDef FOTA_setDownloadURL(FOTA_HandleTypeDef *const me, const char *pURL, uint16_t URLSize);
THT_StatusTypeDef FOTA_parseFirmawreInfo(FOTA_HandleTypeDef *const me);
THT_StatusTypeDef FOTA_compareVersion(FOTA_HandleTypeDef *const me);
THT_StatusTypeDef FOTA_downloadFirmware(FOTA_HandleTypeDef *const me, SIM_HTTPTypeDef *pHTTP);
THT_StatusTypeDef FOTA_writeFirmware(FOTA_HandleTypeDef *const me);
uint32_t FOTA_getCurrentAddress(void);
void FOTA_getCurrentVersion(uint16_t pVersion[], size_t size);
THT_StatusTypeDef FOTA_readEPPROMs(FOTA_HandleTypeDef *const me);
THT_StatusTypeDef FOTA_writeEPPROMs(FOTA_HandleTypeDef *const me);
void FOTA_gotoFirmware(uint32_t address);

#endif /* INC_FOTA_H_ */
