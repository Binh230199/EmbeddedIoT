/*
 * dev_config.h
 *
 *  Created on: Mar 26, 2023
 *      Author: Hoàng Văn Bình
 *      Gmail: binhhv.23.1.99@gmail.com
 */

#ifndef INC_DEV_CONFIG_H_
#define INC_DEV_CONFIG_H_

#include "wlv_def.h"

#define CFG_GATE_SIZE		5U
#define CFG_DEVICE_ID_SIZE	13U
#define CFG_HTTP_URL_SIZE	80U

typedef enum
{
	CFG_MODE_10, CFG_MODE_60
} CFG_ModeTypeDef;

typedef struct
{
	CFG_ModeTypeDef mode;
	char deviceID[CFG_DEVICE_ID_SIZE];
	char gateSend[CFG_GATE_SIZE];
	char gateRead[CFG_GATE_SIZE];
	char httpURL[CFG_HTTP_URL_SIZE];
} CFG_HandleTypeDef;

#define CFG_GATE_READ_DEFAULT	("7769")
#define CFG_GATE_SEND_DEFAULT	("8088")
#define CFG_HTTP_URL_DEFAULT_VER10	("http://ws.vrain.vn/v2/vrain10")
#define CFG_HTTP_URL_DEFAULT_VER60	("http://ws.vrain.vn/v2/vrain")

#define CFG_EEPROM_MODE				(THT_CONFIG_BASE)
#define CFG_EEPROM_DEVICE_ID		(CFG_EEPROM_MODE + 1)
#define CFG_EEPROM_GATE_READ		(CFG_EEPROM_DEVICE_ID + 12)
#define CFG_EEPROM_GATE_SEND		(CFG_EEPROM_GATE_READ + 4)
#define CFG_EEPROM_HTTP_URL			(CFG_EEPROM_GATE_SEND + 4)

#define CFG_GET_HEADER_STRING(__ME__)		(((__ME__)->mode == CFG_MODE_10) ? "DOMUA10V2" : "DOMUAVRAINV2")
#define CFG_GET_RESULT_KQD_STRING(__ME__)	(((__ME__)->mode == CFG_MODE_10) ? "KTTV KQD" : "KQD")
#define CFG_GET_START_HOUR(__ME__)			(((__ME__)->mode == CFG_MODE_10) ? 0 : 19)
#define CFG_HTTP_URL_DEFAULT(__ME__)		(((__ME__)->mode == CFG_MODE_10) ? CFG_HTTP_URL_DEFAULT_VER10 : CFG_HTTP_URL_DEFAULT_VER60)
#define CFG_PERIOD_SEND_DEFAULT(__ME__)		(((__ME__)->mode == CFG_MODE_10) ? 10 : 60)
#define CFG_PERIOD_READ_DEFAULT(__ME__)		(((__ME__)->mode == CFG_MODE_10) ? 10 : 60)

THT_StatusTypeDef CFG_setMode(CFG_HandleTypeDef *const me, CFG_ModeTypeDef mode);
THT_StatusTypeDef CFG_setDeviceID(CFG_HandleTypeDef *const me, const char *pID, uint8_t size);
THT_StatusTypeDef CFG_setGate(CFG_HandleTypeDef *const me, const char *pGSend, const char *pGRead, uint8_t gsSize,
		uint8_t grSize);
THT_StatusTypeDef CFG_setURL(CFG_HandleTypeDef *const me, const char *pURL, uint8_t size);
char* CFG_getDeviceID(CFG_HandleTypeDef *const me);
char* CFG_getGateSend(CFG_HandleTypeDef *const me);
char* CFG_getGateRead(CFG_HandleTypeDef *const me);
char* CFG_getURL(CFG_HandleTypeDef *const me);
THT_StatusTypeDef CFG_readEEPROM(CFG_HandleTypeDef *const me);
THT_StatusTypeDef CFG_writeEEPROM(CFG_HandleTypeDef *const me);

#if TEST
THT_StatusTypeDef 	CFG_test(CFG_HandleTypeDef * const me);
#endif

#endif /* INC_DEV_CONFIG_H_ */
