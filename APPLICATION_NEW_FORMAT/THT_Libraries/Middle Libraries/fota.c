/*
 * fota.c
 *
 *  Created on: Mar 25, 2023
 *      Author: Hoàng Văn Bình
 *      Gmail: binhhv.23.1.99@gmail.com
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "midlib.h"
#include "fota.h"

extern SIM_HandleTypeDef *simPtr;
extern CFG_HandleTypeDef *cfgPtr;
extern CRC_HandleTypeDef hcrc;

void FOTA_process(FOTA_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return;
	}

	SIM_HTTPTypeDef http = { 0 };

	if (THT_OK != HTTP_sendGetFirmwareURL(&http))
	{
		FOTA_SET_FLAG(me, FOTA_FLAG_GUER);
		return;
	}

	if (THT_OK != FOTA_setDownloadURL(me, http.data, strlen(http.data)))
	{
		FOTA_SET_FLAG(me, FOTA_FLAG_GUER);
		return;
	}

	if (THT_OK != FOTA_parseFirmawreInfo(me))
	{
		FOTA_SET_FLAG(me, FOTA_FLAG_VERER);
		return;
	}

	if (THT_OK != FOTA_compareVersion(me))
	{
		FOTA_SET_FLAG(me, FOTA_FLAG_VERER);
		return;
	}

	if (THT_OK != FOTA_downloadFirmware(me, &http))
	{
		FOTA_SET_FLAG(me, FOTA_FLAG_DOWER);
		return;
	}

	if (THT_OK != FOTA_writeFirmware(me))
	{
		FOTA_SET_FLAG(me, FOTA_FLAG_WRIER);
		return;
	}

	FOTA_writeEPPROMs(me);
}

THT_StatusTypeDef FOTA_setDownloadURL(FOTA_HandleTypeDef *const me, const char *pURL, uint16_t URLSize)
{
	if (me == NULL || pURL == NULL)
	{
		return THT_ERROR;
	}

	if (URLSize < sizeof(me->firmwareURL))
	{
		memset(me->firmwareURL, '\0', sizeof(me->firmwareURL));
		memcpy(me->firmwareURL, pURL, URLSize);
		me->firmwareURL[URLSize] = '\0';
		return THT_OK;
	}
	else
	{
		return THT_ERROR;
	}
}
THT_StatusTypeDef FOTA_parseFirmawreInfo(FOTA_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Parse firmware filename */
	char *hPtr = NULL;
	char *tPtr = NULL;

	hPtr = strstr(me->firmwareURL, FOTA_FIRMWARE_HEADER_NAME);
	tPtr = strstr(hPtr, ".bin");

	if ((hPtr == NULL) || (tPtr == NULL))
	{
		printf("Parse firmawre error!\n");
		return THT_ERROR;
	}
	else
	{
		memset(me->firmwareName, '\0', sizeof(me->firmwareName));
		uint8_t firmwareNameLength = tPtr + sizeof("bin") - hPtr;
		strncpy(me->firmwareName, hPtr, firmwareNameLength);
		printf("Firmware name: %s\n", me->firmwareName);

	}

	/*Parse firmware version */
	/* vfasscontroller_1.4.0.bin */
	char *sPtr = NULL;

	/* Parse major */
	sPtr = strstr(me->firmwareName, "_");

	if (sPtr != NULL)
	{
		me->newVersion[0] = atoi(sPtr + 1);

		/* Parse minor */
		sPtr = strstr(sPtr + 1, ".");

		if (sPtr != NULL)
		{
			me->newVersion[1] = atoi(sPtr + 1);

			/* Parse patch */
			sPtr = strstr(sPtr + 1, ".");

			if (sPtr != NULL)
			{
				me->newVersion[2] = atoi(sPtr + 1);
			}
		}
	}
	printf("New firmware version: %u.%u.%u\n", me->newVersion[0], me->newVersion[1], me->newVersion[2]);

	return THT_OK;
}

THT_StatusTypeDef FOTA_compareVersion(FOTA_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	FOTA_getCurrentVersion(me->curVersion, 3);

	if (me->newVersion[0] > me->curVersion[0])
	{
		if ((me->curVersion[2] == 0 && me->newVersion[2] == 1) || (me->curVersion[2] == 1 && me->newVersion[2] == 0))
		{
			return THT_OK;
		}
		else
		{
			return THT_ERROR;
		}
	}
	else if (me->newVersion[0] < me->curVersion[0])
	{
		return THT_ERROR;
	}
	else
	{
		if (me->newVersion[1] > me->curVersion[1])
		{
			if ((me->curVersion[2] == 0 && me->newVersion[2] == 1)
					|| (me->curVersion[2] == 1 && me->newVersion[2] == 0))
			{
				return THT_OK;
			}
			else
			{
				return THT_ERROR;
			}
		}
		else
		{
			return THT_ERROR;
		}
	}
}

THT_StatusTypeDef FOTA_downloadFirmware(FOTA_HandleTypeDef *const me, SIM_HTTPTypeDef *pHTTP)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return HTTP_downloadFile(pHTTP, me->firmwareURL, me->firmwareName);
}

THT_StatusTypeDef FOTA_writeFirmware(FOTA_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	me->curAddress = FOTA_getCurrentAddress();

	/* Set new firmware address */
	if (me->curAddress == FOTA_APP_1_START_ADDR)
	{
		me->newAddress = FOTA_APP_2_START_ADDR;
	}
	else
	{
		me->newAddress = FOTA_APP_1_START_ADDR;
	}

	char command[150] = { 0 };

	uint8_t counter = 0;
	do
	{
		HAL_FLASH_Unlock();

		FLASH_EraseInitTypeDef pEraseInit = { FLASH_TYPEERASE_PAGES, me->newAddress, FOTA_NUMBER_FLASH_PAGES };

		uint32_t PageError;

		HAL_FLASHEx_Erase(&pEraseInit, &PageError);
		me->firmwareSize = SIM_getFileSize(simPtr, me->firmwareName);
		if (me->firmwareSize >= (FOTA_APP_1_END_ADDR - FOTA_APP_1_START_ADDR))
		{
			FOTA_SET_FLAG(me, FOTA_FLAG_FFER);
			printf(" File : %s\n", me->firmwareName);
			printf(" OverSize of memory : %ld Bytes\n", me->firmwareSize);
			break;
		}

		uint32_t countdownSize = me->firmwareSize;
		uint32_t wBytes = 0U;
//		uint32_t pageCnt = 0U;

		printf("Memory Programming ...\n");
		printf(" File : %s\n", me->firmwareName);
		printf(" Size : %ld Bytes\n", me->firmwareSize);
		printf(" Address : %lX\n\n", me->newAddress);

		while (countdownSize > 0U)
		{
			if (countdownSize >= 512U)
			{
				sprintf(command, "AT+CFTRANTX=\"F:/%s\",%lu,%u", me->firmwareName, wBytes, 512U);
				wBytes += 512U;
				countdownSize -= 512U;
			}
			else
			{
				sprintf(command, "AT+CFTRANTX=\"F:/%s\",%lu,%lu", me->firmwareName, wBytes, countdownSize);
				wBytes += countdownSize;
				countdownSize = 0;
			}
			printf("Loading: %.2f%%\n", 100 * (double) wBytes / me->firmwareSize);
			if (SIM_sendATCommand(simPtr, command, "+CFTRANTX:", SIM_URC_ERROR, 1000) == THT_OK)
			{
//				HAL_FLASHEx_DoublePageProgram(
//						me->newAddress + (pageCnt++ * 512U),
//						(uint32_t*) (simPtr->buffer + 23));
			}
			else
			{
				break;
			}
		}

		HAL_FLASH_Lock();

		me->CRC32 = HAL_CRC_Calculate(&hcrc, (uint32_t*) me->newAddress, me->firmwareSize / 4);

		if (me->CRC32 == 0UL)
		{
			printf("Check CRC right!\n"
					"Download verified successfully\n\n\n");
			FOTA_SET_FLAG(me, FOTA_FLAG_UPOK);
		}
		else
		{
			printf("Check CRC wrong!\n"
					"Download verified unsuccessfully\nTry again\n\n\n");
			FOTA_SET_FLAG(me, FOTA_FLAG_CRCER);
		}

		counter++;
	}
	while ((counter < 3) && (FOTA_GET_FLAG(me, FOTA_FLAG_UPOK) == 0));

	memset(command, '\0', strlen(command));

	sprintf(command, "AT+FSDEL=\"/%s\"", me->firmwareName);
	SIM_sendATCommand(simPtr, command, SIM_URC_OK, SIM_URC_ERROR, 3000);

	if (FOTA_GET_FLAG(me, FOTA_FLAG_UPOK))
	{
		me->curAddress = me->newAddress;
		me->curVersion[0] = me->newVersion[0];
		me->curVersion[1] = me->newVersion[1];
		me->curVersion[2] = me->newVersion[2];
		return THT_OK;
	}
	else
	{
		return THT_ERROR;
	}
}

uint32_t FOTA_getCurrentAddress(void)
{
	return *(volatile uint32_t*) FOTA_EEPROM_CURRENT_ADDR;
}

void FOTA_getCurrentVersion(uint16_t pVersion[], size_t size)
{
	if (size != 3)
	{
		return;
	}

	pVersion[0] = *(volatile uint16_t*) FOTA_EEPROM_CURRENT_MAJOR;
	pVersion[1] = *(volatile uint16_t*) FOTA_EEPROM_CURRENT_MINOR;
	pVersion[2] = *(volatile uint16_t*) FOTA_EEPROM_CURRENT_PATCH;
}
void FOTA_gotoFirmware(uint32_t address)
{
	HAL_RCC_DeInit();    //Tắt hết ngoại vi xoá hết c�? ngắt

	HAL_DeInit();			//Xoá hết các Pending, đồng th�?i tắt System Tick

	SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk);

	__set_MSP(*((__IO uint32_t*) address));

	uint32_t jump_address = *((__IO uint32_t*) (address + 4));
	void (*reset_handler)(void) = (void*) jump_address;
	reset_handler();
}

THT_StatusTypeDef FOTA_readEPPROMs(FOTA_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Read section from EEPROM */
	uint32_t SRStoredInDataEEPROM = *(volatile uint8_t*) FOTA_EEPROM_SR;

	me->SR = SRStoredInDataEEPROM;

	return THT_OK;
}

THT_StatusTypeDef FOTA_writeEPPROMs(FOTA_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* Read section from EEPROM */
	uint32_t SRStoredInDataEEPROM = *(volatile uint8_t*) FOTA_EEPROM_SR;
	uint32_t curAddrStoredInDataEEPROM = *(volatile uint32_t*) FOTA_EEPROM_CURRENT_ADDR;
	uint32_t majorStoredInDataEEPROM = *(volatile uint16_t*) FOTA_EEPROM_CURRENT_MAJOR;
	uint32_t minorStoredInDataEEPROM = *(volatile uint16_t*) FOTA_EEPROM_CURRENT_MINOR;
	uint32_t patchStoredInDataEEPROM = *(volatile uint16_t*) FOTA_EEPROM_CURRENT_PATCH;

	HAL_FLASHEx_DATAEEPROM_Unlock();
	HAL_FLASHEx_DATAEEPROM_EnableFixedTimeProgram();

	if (SRStoredInDataEEPROM != me->SR)
	{
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_SR);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_SR, me->SR);
	}

	if (curAddrStoredInDataEEPROM != me->curAddress)
	{
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_CURRENT_ADDR);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_CURRENT_ADDR, me->curAddress);
	}

	if (majorStoredInDataEEPROM != me->curVersion[0])
	{
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_CURRENT_MAJOR);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_CURRENT_MAJOR, me->curVersion[0]);
	}

	if (minorStoredInDataEEPROM != me->curVersion[1])
	{
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_CURRENT_MINOR);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_CURRENT_MINOR, me->curVersion[1]);
	}

	if (patchStoredInDataEEPROM != me->curVersion[2])
	{
		HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_CURRENT_PATCH);
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
		FOTA_EEPROM_CURRENT_PATCH, me->curVersion[2]);
	}

	HAL_FLASHEx_DATAEEPROM_DisableFixedTimeProgram();
	HAL_FLASHEx_DATAEEPROM_Lock();
	return THT_OK;
}

