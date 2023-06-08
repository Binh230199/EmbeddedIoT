/*
 * config.h
 *
 *  Created on: Jun 14, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "stm32l1xx_hal.h"

#define NEW 1

extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern ADC_HandleTypeDef hadc;
#define LOG_UART		huart3
#define SIM_UART		huart2
#define SIM_DMA_UART	hdma_usart2_rx
#define SD_SPI			hspi2
#define BQ2429x_I2C		hi2c1

#define DEVICE_MODE_ADDRESS			0x0803FE00 //4 bytes
#define DEVICE_ID_ADDRESS			0x0803FE04 //12 bytes
#define DEVICE_RAIN_WARNING_ADDRESS	0x0803FE10 //4 bytes
#define DEVICE_BATT_WARNING_ADDRESS	0x0803FE14 //4 bytes
#define DEVICE_GATE_SEND_ADDRESS	0x0803FE18 //4 bytes
#define DEVICE_GATE_READ_ADDRESS	0x0803FE1C //4 bytes
#define DEVICE_SEND_PERIOD_ADDRESS	0x0803FE20 //4 bytes
#define DEVICE_READ_PERIOD_ADDRESS	0x0803FE24 //4 bytes
#define DEVICE_HTTP_URL_ADDRESS		0x0803FE28 //60 bytes
#define DEVICE_TOKEN_ADDRESS		0x0803FE64 //60 bytes
#define DEVICE_CHAT_ID_ADDRESS		0x0803FE94 //60 bytes

#define DEVICE_MODE_DEFAULT			(APP_DOMUA10V2)
#define DEVICE_ID_DEFAULT			"84914486786"
#define DEVICE_GATE_SEND_DEFAULT	"8088"
#define DEVICE_GATE_READ_DEFAULT	"7769"
#define DEVICE_SEND_PERIOD_DEFAULT	(10U)
#define DEVICE_READ_PERIOD_DEFAULT	(10U)
#define DEVICE_RAIN_WARNING_DEFAULT	(50U)
#define DEVICE_BATT_WARNING_DEFAULT	(50U)
#define DEVICE_HTTP_URL10_DEFAULT	"http://ws.vrain.vn/v2/vrain10"
#define DEVICE_HTTP_URL60_DEFAULT	"http://ws.vrain.vn/v2/vrain"
#define DEVICE_TELEGRAM_TOKEN_DEFAULT	"6164292395:AAFQuCJ8Db5t0LF1Nd3ijxQiIpsdIeuTRZo"
#define DEVICE_TELEGRAM_CHAT_ID_DEFAULT	"-853747679"

#define API_BUFFER_SIZE				(210U) /* Size of API array */
#define MAX_BAT_VOLTAGE				(4.05f)
#define MIN_BAT_VOLTATE				(3.3f)
#define NOISE_DELAY					(20000U)
#define SIM_WAKEUP_DELAY			(100U)
#define SIM_SLEEP_DELAY				(100U)
#define GET_WARNING_PERIOD			(3U)
#define START_OF_HOUR				(00U)
#define START_OF_DATE				(00U)
#define START_OF_MONTH				(01U)
#define START_OF_YEAR				(01U)
#define END_OF_HOUR					(59U)
#define END_OF_DATE					(23U)
#define END_OF_MONTH				((28U) || (29U) || (30U) || (31))
#define END_OF_YEAR					(12U)
#define LOST_FILE					"disconnect.txt"
#define RESOLUTION					(0.2f)

#define SYSTEM_ADMIN_PHONENUMBER_1	"+84862865768"
#define SYSTEM_ADMIN_PHONENUMBER_2	"+84969647577" //"+84386599045"
#define SYSTEM_ADMIN_PHONENUMBER_3	"+84905951124" //số anh Trường 1
#define SYSTEM_ADMIN_PHONENUMBER_4	"+84913491640" //số anh Huy 1
#define SYSTEM_ADMIN_PHONENUMBER_5	"+84981449744" //số anh Huy 2
#define SYSTEM_ADMIN_PHONENUMBER_6	"+84792774714" //số anh Trường 2 -> Số Th
#define SYSTEM_ADMIN_PHONENUMBER_7	"+84914486786" //SĐT thầy Thanh

#define SMS_RESET_SYNTAX			"*RESET#"
#define SMS_RESTORE_SYNTAX			"*RESTORE#"
#define SMS_SETID_SYNTAX			"*SETID#"
#define SMS_SETTIME_SYNTAX			"*SETTIME#"
#define SMS_SETGATE_SYNTAX			"*SETGATE#"
#define SMS_SETCONFIG_SYNTAX		"*SETCFG#"
#define SMS_SETURL_SYNTAX			"*SETURL#"
#define SMS_SETDEBUG_SYNTAX			"*SETDB#"
#define SMS_SEND_SYNTAX				"*SEND#"
#define SMS_DEBUG_SYNTAX			"*DEBUG#"
#define SMS_RESEND_SYNTAX			"*RESEND#"
#define SMS_UPFW_SYNTAX				"*UPFW#"
#define SMS_SET_DEVICE_MODE_SYNTAX	"*SETMODE#"
#define SMS_SET_TOKEN_SYNTAX		"*SETTOKEN#"
#define SMS_SET_CHAT_ID_SYNTAX		"*SETCID#"

#define RESPONSE_RESET				"VR2: Dang khoi dong lai! Cho 1 phut!"
#define RESPONSE_RESET_OK			"VR2: Thiet bi da khoi dong lai!"
#define RESPONSE_RESTORE			"VR2: Cau hinh da duoc dua ve mac dinh!"
#define RESPONSE_SETID				"VR2: Da set ID thanh cong! ID moi: "
#define RESPONSE_SETTIME			"VR2: Thoi gian cua thiet bi la: "
#define RESPONSE_SETCFG				"VR2: Da cau hinh thanh cong!"
#define RESPONSE_SETURL				"VR2: Da cau hinh URL thanh cong!"
#define RESPONSE_SETDB				"VR2: So debug da duoc cap nhat!"
#define RESPONSE_RESEND				"VR2: Thiet bi da xu ly xong!"
#define RESPONSE_SETMODE			"VR2: Che do hien tai la: "
#define RESPONSE_UPFW_OK			"VR2: Da cap nhat firmware moi thanh cong tai "
#define RESPONSE_UPFW_DOWNLOAD_F	"VR2: Tai firmware khong thanh cong!"
#define RESPONSE_UPFW_OLD_VERSION	"VR2: Chua co phien ban moi hoac phien ban khong hop le!"
#define RESPONSE_UPFW_CRC_FAIL		"VR2: Kiem tra CRC loi!"
#define RESPONSE_UPFW_WRITE_FAIL	"VR2: Ghi firmware khong thanh cong!"
#define RESPONSE_UPFW_BAD_SIGNAL	"VR2: Song yeu!"
#define RESPONSE_UPFW_GET_URL_F		"VR2: Lay duong dan firmware khong thanh cong"
#define RESPONSE_SETID_F			"VR2: ID bat dau la 84, dai 11-12 ky tu!"
#define RESPONSE_SETTIME_F			"VR2: Cap nhat thoi gian khong thanh cong!"
#define RESPONSE_SETGATE_F			"VR2: Vui long kiem tra lai cu phap!"
#define RESPONSE_SETCFG_F			"VR2: Khong the xac dinh duoc cac gia tri!"
#define RESPONSE_SETURL_F			"VR2: Kiem tra lai do dai URL (<60 ky tu)!"
#define RESPONSE_SETDB_F1			"VR2: Kiem tra lai cu phap. So debug bat dau la 84, dai 11 ky tu!"
#define RESPONSE_SETDB_F2			"VR2: Khong the luu duoc so dien thoai!"
#define RESPONSE_RESEND_F			"VR2: Loi khong the doc the nho"
#define RESPONSE_SETMODE_F			"VR2: Kiem tra lai cu phap"
#define APP_DEBUG					"VR2: "

#define TELEGRAM_TRUE_STRING			"true"
#define TELEGRAM_FALSE_STRING			"false"
#define TELEGRAM_NULL_STRING			"null"
#define TELEGRAM_METHOD_SEND_MSG		"sendMessage"

/*
 84969647577: Invalid SMS Sender.
 84969647577: Post fail 10 times.
 84969647577: SD card is disconnecting.
 84969647577: Battery low.
 84969647577: Set time error.
 */
#endif /* INC_CONFIG_H_ */
