/*
 * fota.h
 *
 *  Created on: Jun 14, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef APPLICATION_FOTA_FOTA_H_
#define APPLICATION_FOTA_FOTA_H_

#include "stdio.h"

/*
 * @brief  FOTA State structures definition
 */
typedef enum
{
	FOTA_NO_UPDATE = 0x00,
	FOTA_SUCCESS,
	FOTA_BAD_SIGNAL,
	FOTA_DOWNLOAD_ERROR,
	FOTA_FILE_ERROR,
	FOTA_WRITE_ERROR,
	FOTA_CRC_ERROR,
	FOTA_VERSION_UNNEW,
	FOTA_RESET,
} FOTA_State_Update_t;

/*
 * @brief  FOTA Version Structure definition
 */
typedef struct
{
	uint16_t major;
	uint16_t minor;
	uint16_t patch;
} FOTA_Version_t;

/*
 * @brief  FOTA Firmware informations Structure definition
 */
typedef struct
{
	uint32_t size_of_firmware;
	uint32_t current_address;
	uint32_t new_address;
	char name_firmware[60];
} FOTA_Firmawre_Infor_t;

#define FOTA_SIZE_OF_APPLICATION(SIZE)	((SIZE) * 1024)
#define FOTA_SIZE_OF_PAGE				(256)
#define FOTA_BOOTLOADER_START_ADDRESS	0x08000000
#define FOTA_OTA_START_ADDRESS			0x08002000
#define FOTA_APP1_START_ADDRESS			0x08007800
#define FOTA_APP2_START_ADDRESS			0x08023800

#define FOTA_FLAG_NEED_UPDATE			0x0803FC00 //4 bytes
#define FOTA_CURRENT_ADDRESS			0x0803FC04 //4 bytes
#define FOTA_CURRENT_VER_MAJOR_ADDRESS	0x0803FC08 //4 bytes
#define FOTA_CURRENT_VER_MINOR_ADDRESS	0x0803FC0C //4 bytes
#define FOTA_CURRENT_VER_PATCH_ADDRESS	0x0803FC10 //4 bytes
#define FOTA_NEW_VER_MAJOR_ADDRESS		0x0803FC14 //4 bytes
#define FOTA_NEW_VER_MINOR_ADDRESS		0x0803FC18 //4 bytes
#define FOTA_NEW_VER_PATCH_ADDRESS		0x0803FC1C //4 bytes

#define FOTA_STATE_UPDATE				0x0803FD00
#define FOTA_FIRMWARE_SIZE				0x0803FD04
#define FOTA_PHONE_REQUEST				0x0803FD08
#define FOTA_FILENAME					0x0803FD14

#endif /* APPLICATION_FOTA_FOTA_H_ */
