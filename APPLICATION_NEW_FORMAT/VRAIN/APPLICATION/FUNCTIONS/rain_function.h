/*
 * rain_function.h
 *
 *  Created on: Jun 14, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef APPLICATION_FUNCTIONS_RAIN_FUNCTION_H_
#define APPLICATION_FUNCTIONS_RAIN_FUNCTION_H_

#include "logger.h"
#include "fota.h"
#include "sd.h"
#include "flash.h"
#include "bq24298.h"
#include "config.h"
#include "stdbool.h"
#include "sim7x00.h"

typedef enum
{
	SD_NOT_DETECT, SD_DETECT
} Detect_StatusTypeDef;

typedef enum
{
	SD_WRITE_ERROR, SD_WRITE_OK
} Write_StatusTypeDef;

/**
 * @brief  Command for HTTP and SMS API enumeration
 */
typedef enum
{
	APP_RESULT, APP_SEND, APP_BATCAP, APP_WART, APP_GETCFG, APP_GETFW,
} APP_API_Command_t;

/**
 * @brief  Choose flash to read or write data enumeration
 */
typedef enum
{
	APP_FL_FOTA = (0x0001U << 0), APP_FL_CONFIG = (0x0001U << 1), APP_FL_RESET = (0x0001U << 2),
} APP_Flash_Write_t;

/**
 * @brief  Choose param to read data enumeration
 */
typedef enum
{
	APP_D_RAIN = (0x0001U << 0),
	APP_D_BATT = (0x0001U << 1),
	APP_D_NWTYPE = (0x0001U << 2),
	APP_D_SIGNAL = (0x0001U << 3),
	APP_D_POWER = (0x0001U << 4),
	APP_D_IMEI = (0x0001U << 5),
	APP_D_TIME_CURRENT = (0x0001U << 6),
	APP_D_PERIOD = (0x0001U << 7),
	APP_D_PHONE_NB = (0x0001U << 8),
	APP_D_SD = (0x0001U << 9),
} APP_Data_t;

/**
 * @brief  Choose param to read time enumeration
 */
typedef enum
{
	APP_T_RESET = (0x0001U << 0), APP_T_PERIOD = (0x0001U << 1), APP_T_CURRENT = (0x0001U << 2),
} APP_Time_Format_t;

/**
 * @brief  Select mode for device running
 */
typedef enum
{
	APP_DOMUA10V2, APP_DOMUA60V2,
} APP_Device_Mode;

typedef struct
{
	uint8_t last_hour;
	uint8_t last_minute;
	uint8_t last_second;
	uint8_t next_hour;
	uint8_t next_minute;
	uint8_t next_second;
	char string_time[20];
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	RTC_AlarmTypeDef alarm;
} APP_System_Time_t;

typedef struct
{
	APP_Device_Mode device_mode;
	uint8_t rain_warning;
	uint8_t batt_warning;
	uint8_t read_period;
	uint8_t send_period;
	char device_ID[13]; /* Device ID: 84969647577 hoặc 841699741245 *//* Phone Number: 0969647577 hoặc 01699741245 */
	char gate_send[5];
	char gate_read[5];
	char http_URL[61];
	char telegram_token[48];
	char telegram_chat_id[20];
} APP_Config_Data_t;

typedef struct
{
	uint8_t battery_percent;
	int16_t cell_signal;
	bool power_state;
	__IO uint16_t pulse_in_hour;
	__IO uint16_t pulse_in_period;
	__IO uint8_t pulse_in_period_10[6];
	__IO float rain_in_hour;
	__IO float rain_in_period_60;
	__IO float rain_in_period_10[6];
	__IO uint8_t element;
	char network_type[12];
	char http_time_period[23];
	char sms_time_period[17];
	char device_IMEI[16];
	char current_data_filename[16];
	char device_reset_time[20];
} APP_System_Data_t;

typedef struct
{
	__IO bool sendTime;
	__IO bool alarm;
	__IO bool w_batt;    //warning
	__IO bool w_rain;
	__IO bool rx_cplt;
	__IO bool sms_msg;
	__IO bool sim_disconnect;
	__IO bool call_coming;
	__IO bool http_enable_send_warning;
	__IO bool under_voltage;
	__IO bool readTime;
	FOTA_State_Update_t fota_state;
} APP_Flag_State_t;

void APP_Device_Init();
void APP_Device_Loop();
void enableSendAT();
void disableSendAT();
void sim_timeout(void);
void setAlarm(void);
void APP_get_SMS_info();
void APP_write_backup_data();
void readBKPdata();
void cellularEvent();
void reconnect_sim_card();
void processUrc_Sms();
void processHttp();
void sendRainWarning();
void APP_send_batt_warning(void);
#endif /* APPLICATION_FUNCTIONS_RAIN_FUNCTION_H_ */
