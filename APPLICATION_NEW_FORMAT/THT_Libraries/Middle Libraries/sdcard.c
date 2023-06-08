/*
 * sdcard.c
 *
 *  Created on: Mar 27, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "sdcard.h"

#include <string.h>

extern uint8_t retUSER;

THT_StatusTypeDef SDCARD_init(SDCARD_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}
	retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

	(*me->sdMount)();
	(*me->sdCreateFile)(SDCARD_LOST_FILE);
	(*me->sdCreateFile)(SDCARD_TEST_FILE);
	(*me->sdUnmount)();

	return THT_OK;
}

THT_StatusTypeDef SDCARD_mount(SDCARD_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->sdMount)();
}

THT_StatusTypeDef SDCARD_unmount(SDCARD_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->sdUnmount)();
}

uint32_t SDCARD_getFileSize(SDCARD_HandleTypeDef *const me, const char *pFilename)
{
	if (me == NULL || pFilename == NULL)
	{
		return 0;
	}

	return (*me->sdGetFileSize)(pFilename);
}

THT_StatusTypeDef SDCARD_logData(SDCARD_HandleTypeDef *const me, const char *pFilename, const char *pData,
		uint16_t dataSize)
{
	if (me == NULL || pFilename == NULL || pData == NULL)
	{
		return THT_ERROR;
	}

	(*me->sdMount)();
	(*me->sdUpdateFile)(pFilename, pData, dataSize);
	(*me->sdUnmount)();

	return THT_OK;
}

THT_StatusTypeDef SDCARD_setCurrentFile(SDCARD_HandleTypeDef *const me, const char *pPathname)
{
	if (me == NULL || pPathname == NULL || (strlen(pPathname) != 15u) || (strstr(pPathname, "txt") == NULL))
	{
		return THT_ERROR;
	}

	char yearFolder[6] = { 0 };
	char monthFolder[9] = { 0 };

	memcpy(yearFolder, pPathname, 5);
	memcpy(monthFolder, pPathname, 8);
	memcpy(me->currentFile, pPathname, 15);

	(*me->sdMount)();
	(*me->sdCreateDir)(yearFolder);
	(*me->sdCreateDir)(monthFolder);
	(*me->sdCreateFile)(me->currentFile);
	(*me->sdUnmount)();

	return THT_OK;
}

