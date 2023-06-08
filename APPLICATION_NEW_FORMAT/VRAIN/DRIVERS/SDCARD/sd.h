/*
 * sd.h
 *
 *  Created on: Apr 14, 2022
 *      Author: HOANG VAN BINH
 *      Email: binhhv.23.1.99@gmail.com
 */

#ifndef INC_SD_H_
#define INC_SD_H_

#include "fatfs.h"
#include "fatfs_sd.h"
#include "stm32l1xx_hal.h"
#include "logger.h"

#define SIZE_USE_FOR_FGETS 180 //512
uint8_t SD_Detect(void);
FRESULT sd_mount(const TCHAR *path);
FRESULT sd_unmount(const TCHAR *path);
FRESULT sd_create_new_file(char *filename);
FRESULT sd_update_file(char *filename, char *data, uint32_t size);
uint32_t sd_get_file_size(char *filename);
FRESULT sd_create_new_dir(char *path);
//FRESULT sd_open_dir(char *path);
//FRESULT sd_close_dir();
#endif /* INC_SD_H_ */
