/*
 * sd.c
 *
 *  Created on: Apr 2, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "sd.h"

#include <string.h>
#include <stdio.h>

FRESULT res;
FILINFO fno;

/**
 * @brief Mount SD card.
 * @param path: path want to mount
 * @retval Result of function
 */
THT_StatusTypeDef SD_mount(void)
{
	res = f_mount(&USERFatFS, USERPath, 0);

	if (res != FR_OK)
	{
		return THT_ERROR;
	}

	return THT_OK;
}
/**
 * @brief Unmount SD card.
 * @param hsd: handle
 * @retval Result of function
 */
THT_StatusTypeDef SD_unmount(void)
{
	res = f_mount(NULL, USERPath, 1);

	if (res != FR_OK)
	{
		return THT_ERROR;
	}

	return THT_OK;
}
/**
 * @brief Create a new file.
 * @param filename: name of file (.txt)
 * @Note: Can inclue the directory. Ex: /DIR1/test.txt
 * @retval Result of function
 */
THT_StatusTypeDef SD_createFile(const char *filename)
{
	if (filename == NULL)
	{
		return THT_ERROR;
	}

	res = f_stat(filename, &fno);

	if (res == FR_OK)
	{    //file is existed
		printf(" >> %s existed\n", filename);
		return THT_ERROR;
	}
	else
	{
		res = f_open(&USERFile, filename, FA_CREATE_ALWAYS | FA_WRITE);

		f_close(&USERFile);
	}

	if (res != FR_OK)
	{
		printf(" >> %s open failed\n", filename);

		return THT_ERROR;
	}
	else
	{
		printf(" >> %s created!\n", filename);

		return THT_OK;
	}
}
/**
 * @brief Update new data into end of the file.
 * @param filename: name of file (.txt)
 * @param data: data want to update
 * @param size: size of data want to update
 * @retval Result of function
 */
THT_StatusTypeDef SD_updateFile(const char *filename, const char *data, uint32_t size)
{
	if (filename == NULL)
	{
		return THT_ERROR;
	}

	UINT bw;

	res = f_stat(filename, &fno);

	if (res != FR_OK)
	{
		printf(" >> %s not existed\n", filename);
		return THT_ERROR;
	}
	else
	{
		res = f_open(&USERFile, filename, FA_OPEN_EXISTING | FA_READ | FA_WRITE);

		if (res != FR_OK)
		{
			printf(" >> %s open failed\n", filename);

			return THT_ERROR;
		}
		else
		{
			f_lseek(&USERFile, f_size(&USERFile));
			res = f_write(&USERFile, data, size, &bw);
		}

		f_close(&USERFile);
	}

	if (res != FR_OK)
	{
		printf(" >> %s write failed\n", filename);
		return THT_ERROR;
	}
	else
	{
		printf(" >> %s updated!\n", filename);

		return THT_OK;
	}
}
/**
 * @brief Get size of file.
 * @param filename: name of file (.txt)
 * @retval Result of function
 */
uint32_t SD_getFileSize(const char *filename)
{
	if (filename == NULL)
	{
		return 0;
	}

	uint32_t size = 0;
	if (f_open(&USERFile, filename, FA_READ) != FR_OK)
	{
		size = 0;
	}
	else
	{
		//			size = fno.fsize;
		size = USERFile.obj.objsize;
		f_close(&USERFile);
	}

	printf(" >> size(%s) is %lu\n", filename, size);

	return size;
}
/**
 * @brief Create a new DIR.
 * @param dir: name of direction
 * @retval Result of function
 */
THT_StatusTypeDef SD_createDir(const char *path)
{
	if (path == NULL)
	{
		return THT_ERROR;
	}
	if (f_stat(path, &fno) == FR_OK)
	{    //đã tồn tại DIR
		printf(" >> %s existed\n", path);

		return THT_ERROR;
	}
	else
	{
		res = f_mkdir(path);
	}

	if (res != FR_OK)
	{
		printf(" >> %s create fail\n", path);

		return THT_ERROR;
	}
	else
	{
		printf(" >> %s created\n", path);

		return THT_OK;
	}
}

