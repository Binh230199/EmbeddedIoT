/*
 * sd.c
 *
 *  Created on: Apr 14, 2022
 *      Author: Hoang Van Binh
 */

#include "sd.h"

/**
 * @brief Mount SD card.
 * @param path: path want to mount
 * @retval Result of function
 */
uint8_t SD_Detect(void) {

	uint8_t status = 1;
	/* Check SD card detect pin */
	if (GPIO_PIN_SET == HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin)) {
		status = 0;
	}
	return status;
}

FRESULT sd_mount(const TCHAR *path) {
	static FATFS fatfs;
	static FRESULT res;

	res = f_mount(&fatfs, path, 0);
	if (res == FR_OK) {
		printf("%sMount SD OK\n", "SD: ");
	} else {
		printf("%sMount SD not OK\n", "SD: ");
	}
	return res;
}
/**
 * @brief Unmount SD card.
 * @param path: path want to mount
 * @retval Result of function
 */
FRESULT sd_unmount(const TCHAR *path) {
	FRESULT res;
	res = f_mount(NULL, path, 1);
	if (res == FR_OK) {
		printf("%sUnmount SD OK\n", "SD: ");
	} else {
		printf("%sUnmount SD not OK\n", "SD: ");
	}
	return res;
}
/**
 * @brief Create a new file.
 * @param filename: name of file (.txt)
 * @Note: Can inclue the directory. Ex: /DIR1/test.txt
 * @retval Result of function
 */
FRESULT sd_create_new_file(char *filename) {
	FIL fil;
	FRESULT res;
	FILINFO fno;

	res = f_stat(filename, &fno);
	if (res == FR_OK)    //file is existed
			{
		return res;
	} else {
		res = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
		if (res != FR_OK) {
			return res;
		} else {
			printf("%sFile %s created!\n", "SD: ", filename);
		}
		res = f_close(&fil);
	}
	return res;
}

/**
 * @brief Update new data into end of the file.
 * @param filename: name of file (.txt)
 * @param data: data want to update
 * @param size: size of data want to update
 * @retval Result of function
 */
FRESULT sd_update_file(char *filename, char *data, uint32_t size) {
	FIL file;
	FRESULT res;
	UINT bw;
	FILINFO fno;

	res = f_stat(filename, &fno);
	if (res != FR_OK) {
		return res;
	} else {
		res = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
		if (res != FR_OK) {
			return res;
		} else {
			res = f_lseek(&file, f_size(&file));
			res = f_write(&file, data, size, &bw);
			if (res != FR_OK) {
				return res;
			} else {
				printf("%sFile %s updated!\n", "SD: ", filename);
			}
		}
		res = f_close(&file);
	}
	return res;
}

/**
 * @brief Get size of file.
 * @param filename: name of file (.txt)
 * @retval Result of function
 */
uint32_t sd_get_file_size(char *filename) {
	FIL fil;

	uint32_t size = 0;
	if (f_open(&fil, filename, FA_READ) != FR_OK) {
		size = 0;
	} else {
		//			size = fno.fsize;
		size = fil.obj.objsize;
		f_close(&fil);
	}
	return size;
}
/**
 * @brief Create a new DIR.
 * @param dir: name of direction
 * @retval Result of function
 */
FRESULT sd_create_new_dir(char *path) {
	FILINFO fno;

	if (f_stat(path, &fno) == FR_OK)    //đã tồn tại DIR
			{
		printf("%sDIR existed\n", "SD: ");
		return FR_OK;
	}
	printf("%sDIR created\n", "SD: ");
	return f_mkdir(path);
}
//FRESULT sd_open_dir(char *path)
//{
//	DIR dp;
//	FILINFO fno;
//	FRESULT res;
//	res = f_stat(path, &fno);
//	if (res == FR_OK)    //đã tồn tại DIR
//	{
//		debug_println(eSD, "Open DIR OK");
//		return f_opendir(&dp, path);
//	}
//	debug_println(eSD, "Open DIR not OK");
//	return res;
//}
//FRESULT sd_close_dir()
//{
//	DIR dp;
//	debug_println(eSD, "DIR closed");
//	return f_closedir(&dp);
//}
