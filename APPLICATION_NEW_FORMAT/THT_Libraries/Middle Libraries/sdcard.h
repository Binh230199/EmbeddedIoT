/*
 * sdcard.h
 *
 *  Created on: Mar 27, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_SDCARD_H_
#define INC_SDCARD_H_

#include "fatfs.h"
#include "wlv_def.h"

typedef struct sdcard
{
	volatile uint32_t SR;
	char currentFile[16];
	THT_StatusTypeDef (*sdMount)(void);
	THT_StatusTypeDef (*sdUnmount)(void);
	THT_StatusTypeDef (*sdCreateFile)(const char *filename);
	THT_StatusTypeDef (*sdUpdateFile)(const char *filename, const char *data, uint32_t size);
	uint32_t (*sdGetFileSize)(const char *filename);
	THT_StatusTypeDef (*sdCreateDir)(const char *pathname);
} SDCARD_HandleTypeDef;

typedef THT_StatusTypeDef (*sdMount)(void);
typedef THT_StatusTypeDef (*sdUnmount)(void);
typedef THT_StatusTypeDef (*sdCreateFile)(const char *filename);
typedef THT_StatusTypeDef (*sdUpdateFile)(const char *filename, const char *data, uint32_t size);
typedef uint32_t (*sdGetFileSize)(const char *filename);
typedef THT_StatusTypeDef (*sdCreateDir)(const char *pathname);

#define SDCARD_SR_DTEC_Pos			(0U)
#define SDCARD_SR_DTEC_Msk			(0x1UL << SDCARD_SR_DTEC_Pos)
#define SDCARD_SR_DTEC				SDCARD_SR_DTEC_Msk
#define SDCARD_SR_WRS_Pos			(1U)
#define SDCARD_SR_WRS_Msk			(0x1UL << SDCARD_SR_WRS_Pos)
#define SDCARD_SR_WRS				SDCARD_SR_WRS_Msk
#define SDCARD_SR_RDS_Pos			(2U)
#define SDCARD_SR_RDS_Msk			(0x1UL << SDCARD_SR_RDS_Pos)
#define SDCARD_SR_RDS				SDCARD_SR_RDS_Msk

#define SDCARD_LOST_FILE			"/failed.txt"
#define SDCARD_TEST_FILE			"/test.txt"

#define SDCARD_IS_WRITE_OK(__ME__)	(SDCARD_logData((__ME__), SDCARD_TEST_FILE, "", 0) == THT_OK ? "OK" : "FAIL")

THT_StatusTypeDef SDCARD_init(SDCARD_HandleTypeDef *const me);
THT_StatusTypeDef SDCARD_mount(SDCARD_HandleTypeDef *const me);
THT_StatusTypeDef SDCARD_unmount(SDCARD_HandleTypeDef *const me);
uint32_t SDCARD_getFileSize(SDCARD_HandleTypeDef *const me, const char *pFilename);
THT_StatusTypeDef SDCARD_logData(SDCARD_HandleTypeDef *const me, const char *pFilename, const char *pData,
		uint16_t dataSize);
THT_StatusTypeDef SDCARD_setCurrentFile(SDCARD_HandleTypeDef *const me, const char *pPathname);

#endif /* INC_SDCARD_H_ */
