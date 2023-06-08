/*
 * sd.h
 *
 *  Created on: Apr 2, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_SD_H_
#define INC_SD_H_

#include "fatfs.h"

#include "fatfs_sd.h"
#include "midlib.h"

typedef struct
{
	SDCARD_HandleTypeDef parent;
	uint16_t dectectPin;
	GPIO_TypeDef *detectPort;
} SD_HandleTypeDef;

THT_StatusTypeDef SD_mount(void);
THT_StatusTypeDef SD_unmount(void);
THT_StatusTypeDef SD_createFile(const char *filename);
THT_StatusTypeDef SD_updateFile(const char *filename, const char *data, uint32_t size);
uint32_t SD_getFileSize(const char *filename);
THT_StatusTypeDef SD_createDir(const char *path);


#endif /* INC_SD_H_ */
