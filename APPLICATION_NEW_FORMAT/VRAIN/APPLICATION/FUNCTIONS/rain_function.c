/*
 * rain_function.c
 *
 *  Created on: Jun 14, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "rain_function.h"

extern uint8_t g_rx_buffer[RX_LEN];

APP_Config_Data_t g_config = { 0 };
APP_System_Time_t g_time = { 0 };
APP_System_Data_t g_data = { 0 };
APP_Flag_State_t g_flag = { false, false, true, false, false, false, false,
false, true, false, false, FOTA_NO_UPDATE };
FOTA_Firmawre_Infor_t g_firmware = { 0 };
FOTA_Version_t g_current_ver = { 0 };
FOTA_Version_t g_new_ver = { 0 };
SIM_SMS_Data_t g_sms = { 0 };

char json_buffer[API_BUFFER_SIZE] = { 0 };
char firmware_URL[200] = { 0 };

uint16_t post_fail_counter = 0;    //Sử dụng để đếm số lần gửi dữ liệu không thành công, nếu 2 lần thì khởi động lại moduel SIM.

/* set mode:
 * AUTO: device update itself
 * SMS: set by SMS
 */
typedef enum
{
	APP_SET_AUTO, APP_SET_MANUAL,
} APP_Mode_Set_Param_t;
/* Private function */
static void APP_set_alarm(uint8_t minute);
void setAlarm(void);
static void APP_debug_data(void);
static void APP_resend_data(void);
static void APP_send_data(void);
static void APP_restore_device(void);
static void APP_reset_device(void);
static void APP_set_HTTP_URL(APP_Mode_Set_Param_t mode);
static void APP_set_admin_number(void);
static bool APP_set_config(APP_Mode_Set_Param_t mode);
static void APP_set_gate_number(APP_Mode_Set_Param_t mode);
void RTC_SetTime(void);
static void setTime(APP_Mode_Set_Param_t mode);
static APP_Device_Mode setDeviceMode(APP_Mode_Set_Param_t mode);
static char* setDeviceID(APP_Mode_Set_Param_t mode);
static void APP_update_firmware(void);
static bool ota_download_firmware(void);
static bool ota_get_new_firmware(void);
static bool ota_compare_version(void);
static eSTATUS APP_upload_data(void);
static void APP_upload_last_data(void);
static void APP_create_SMS_msg(APP_API_Command_t cmd);
static void APP_create_HTTP_json(APP_API_Command_t cmd);
static uint8_t APP_get_battery_percent(void);
static bool APP_get_power_state(void);
static int16_t APP_get_signal(void);
static char* APP_get_network_type(void);
static char* APP_get_device_IMEI(void);
static void getTime(APP_Time_Format_t time_format);
static void getDeviceData(APP_Data_t data);
static bool APP_check_SMS_sender();
static uint8_t APP_process_SMS_msg();
static void APP_create_new_data_file();
static void APP_write_flash(APP_Flash_Write_t w_flash);
static void readFlash(APP_Flash_Write_t r_flash);

/* NEW UPDATE VER 1.7 - Add TELEGRAM API */
/* telegram */
typedef struct
{
	char *method;
	char *parse_mode;
	char *disable_web_page_preview;
	char *disable_notification;
	char *reply_to_message_id;
} Telegram_HandleTypeDef;

static void APP_set_token(APP_Mode_Set_Param_t mode);
static void APP_set_chat_id(APP_Mode_Set_Param_t mode);
static eSTATUS APP_telegram_send_msg(Telegram_HandleTypeDef *const me, char *msg);

Telegram_HandleTypeDef g_tele = { .disable_notification = TELEGRAM_FALSE_STRING, .disable_web_page_preview =
TELEGRAM_FALSE_STRING, .method = TELEGRAM_METHOD_SEND_MSG, .parse_mode = "HTML", .reply_to_message_id =
TELEGRAM_NULL_STRING };

uint8_t getNextMinute(void)
{
	if ((((g_time.time.Minutes / g_config.read_period) + 1) * g_config.read_period) > 59)
	{
		return 0;
	}
	else
	{
		return ((g_time.time.Minutes / g_config.read_period) + 1) * g_config.read_period;
	}
}

uint8_t getNextHour(void)
{
	if (g_time.next_minute != 0)
	{
		return g_time.time.Hours;
	}
	else
	{
		if (g_time.time.Hours + 1 > 23)
		{
			return 0;
		}
		else
		{
			return g_time.time.Hours + 1;
		}
	}
}

void APP_Device_Init(void)
{
	HAL_GPIO_WritePin(MCU_GPIO_Port, MCU_Pin, GPIO_PIN_SET);
	/* Cấu hình nguồn thì mới khởi tạo SIM được */
	bq2429x_updateBit();
	/* Đọc thông số cấu hình và trạng thái của cập nhật firmware từ FLASH */
	readFlash(APP_FL_CONFIG | APP_FL_FOTA);
	/* Reset module SIM */
	sim_hard_reset(RESET_GPIO_Port, RESET_Pin);
	/* Khởi động lại module SIM */
	enableSendAT();

	sim_init(PWRKEY_GPIO_Port, PWRKEY_Pin);

	sim_sync_local_time(g_time.string_time);

	/* Cập nhật thời gian cho thiết bị */
	setTime(APP_SET_AUTO);

	/* Lấy dấu thời gian khởi động lại của thiết bị */
	getTime(APP_T_RESET);
	/* Đọc giá trị các tham số của thiết bị */
	getDeviceData(APP_D_BATT | APP_D_NWTYPE | APP_D_SIGNAL | APP_D_POWER | APP_D_IMEI);
	readBKPdata();

	/* Cập nhật ID cho thiết bị */
	setDeviceID(APP_SET_AUTO);

	/* Kiểm tra trạng thái sau khi cập nhật chương trình từ xa */
	char msg[60] = { 0 };
	switch (g_flag.fota_state)
	{
		case FOTA_SUCCESS:
			sprintf(msg, "%s%lX", RESPONSE_UPFW_OK, g_firmware.current_address);
			sim_sendSms(g_sms.phone_number, msg);
			break;
		case FOTA_WRITE_ERROR:
			sim_sendSms(g_sms.phone_number, RESPONSE_UPFW_WRITE_FAIL);
			break;
		case FOTA_CRC_ERROR:
			sim_sendSms(g_sms.phone_number, RESPONSE_UPFW_CRC_FAIL);
			break;
		case FOTA_RESET:
			sim_sendSms(g_sms.phone_number, RESPONSE_RESET_OK);
			break;
		default:
			break;
	}
	if (g_flag.fota_state != FOTA_NO_UPDATE)
	{
		flash_unlock();
		flash_erase(FOTA_STATE_UPDATE);
		flash_lock();
	}

	/* Cài báo thức để đọc dữ liệu */
	setAlarm();

	disableSendAT();
	/* Khởi tạo thẻ nhớ và tạo các file */
	sd_mount("/");
	/* Create a file to store data post failed */
	sd_create_new_file(LOST_FILE);
	sd_create_new_file("test.txt");
	/* Create a root direction */
	char dir_year[6] = { 0 };
	sprintf(dir_year, "/20%02d", g_time.date.Year);
	sd_create_new_dir(dir_year);
	/* Create a sub-dir */
	char dir_month[9] = { 0 };
	sprintf(dir_month, "/20%02d/%02d", g_time.date.Year, g_time.date.Month);
	sd_create_new_dir(dir_month);
	/* Create a file in day per day */
	char dir_day[16] = { 0 };    // /2022/05/23.txt
	sprintf(dir_day, "/20%02d/%02d/%02d.txt", g_time.date.Year, g_time.date.Month, g_time.date.Date);
	memcpy(g_data.current_data_filename, dir_day, 15);
	sd_create_new_file(dir_day);
	sd_unmount("/");

	APP_write_flash(APP_FL_CONFIG);
	bq2429x_updateBit();
	/* Kết thúc quá trình khởi động thiết bị */
	HAL_GPIO_WritePin(MCU_GPIO_Port, MCU_Pin, GPIO_PIN_RESET);
}

/*
 * @brief: Kéo chân DTR lên logic thấp, cho phép module SIM nhận lệnh AT từ VĐK
 */
void enableSendAT(void)
{
	DTR_GPIO_Port->BSRR = (uint32_t) DTR_Pin << 16;    //HAL_GPIO_WritePin(DTR_GPIO_Port, DTR_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GSM_GPIO_Port, GSM_Pin, GPIO_PIN_SET);
	HAL_Delay(SIM_WAKEUP_DELAY);
}
/*
 * @brief: Kéo chân DTR xuống logic cao, đồng thời gửi lệnh vô hiệu hoá UART
 * của module SIM, đưa module SIM vào trạng thái Sleep.
 */
void disableSendAT(void)
{
	HAL_UART_Transmit(&SIM_UART, (uint8_t*) "AT+CSCLK=1\r\n", strlen("AT+CSCLK=1\r\n"), HAL_MAX_DELAY);
	HAL_Delay(SIM_SLEEP_DELAY);
	HAL_GPIO_WritePin(GSM_GPIO_Port, GSM_Pin, GPIO_PIN_RESET);
	DTR_GPIO_Port->BSRR = (uint32_t) DTR_Pin;    // << 16;    //HAL_GPIO_WritePin(DTR_GPIO_Port, DTR_Pin, GPIO_PIN_RESET);
}
void sim_timeout(void)
{
	if (sim_getCounterTimeout() >= 5)
	{
		printf("Reset module SIM: time out %d times\n", sim_getCounterTimeout());
		/* Reset module SIM */
		sim_hard_reset(RESET_GPIO_Port, RESET_Pin);

		sim_init(PWRKEY_GPIO_Port, PWRKEY_Pin);

		sim_sync_local_time(g_time.string_time);

		sim_setCounterTimeout(0);
	}
}
static char* setDeviceID(APP_Mode_Set_Param_t mode)
{
	if (mode == APP_SET_AUTO)
	{
		sim_pbook_get_own_number(g_config.device_ID);
		if (strlen(g_config.device_ID) < 11)
		{
			flash_read_array(DEVICE_ID_ADDRESS, g_config.device_ID, 12);
		}
	}
	else if (mode == APP_SET_MANUAL)
	{
		/* SMS content: *SETID#8496964xxxx */
		if ((strlen(g_sms.content) == 18) || (strlen(g_sms.content) == 19))
		{
			char *token = NULL;
			token = strtok(g_sms.content, "#");
			token = strtok(NULL, "");
			if (token != NULL)
			{
				memset(g_config.device_ID, '\0', sizeof(g_config.device_ID));
				sprintf(g_config.device_ID, "%s", token);
				char msg[50] = { 0 };
				sprintf(msg, "%s%s", RESPONSE_SETID, g_config.device_ID);
				sim_sendSms(g_sms.phone_number, msg);
				memset(msg, '\0', 50);
			}
		}
		else
		{
			sim_sendSms(g_sms.phone_number, RESPONSE_SETID_F);
		}
	}
	else
	{
		flash_read_array(DEVICE_ID_ADDRESS, g_config.device_ID, 12);
	}
	return g_config.device_ID;
}

static APP_Device_Mode setDeviceMode(APP_Mode_Set_Param_t mode)
{
	if (mode == APP_SET_MANUAL)
	{
		/* SMS content: *SETMODE#DOMUA10V2 or *SETMODE#DOMUA60V2 */
		if (strlen(g_sms.content) == 18)
		{
			char msg[40] = { 0 };

			if (strstr(g_sms.content, "DOMUA60V2") != NULL)
			{
				g_config.send_period = 60;
				g_config.read_period = 60;
				g_config.device_mode = APP_DOMUA60V2;
				memset(g_config.http_URL, '\0', 61);
				memcpy(g_config.http_URL, DEVICE_HTTP_URL60_DEFAULT, strlen(DEVICE_HTTP_URL60_DEFAULT));
				sprintf(msg, "%s%s", RESPONSE_SETMODE, "DOMUA60V2");
				sim_sendSms(g_sms.phone_number, msg);
			}
			else if (strstr(g_sms.content, "DOMUA10V2") != NULL)
			{
				g_config.send_period = 10;
				g_config.read_period = 10;
				g_config.device_mode = APP_DOMUA10V2;
				memset(g_config.http_URL, '\0', 61);
				memcpy(g_config.http_URL, DEVICE_HTTP_URL10_DEFAULT, strlen(DEVICE_HTTP_URL10_DEFAULT));
				sprintf(msg, "%s%s", RESPONSE_SETMODE, "DOMUA10V2");
				sim_sendSms(g_sms.phone_number, msg);
			}
			else
			{
				sim_sendSms(g_sms.phone_number,
				RESPONSE_SETMODE_F);
			}
		}
	}
	else
	{
		g_config.device_mode = flash_read_int(DEVICE_MODE_ADDRESS);
	}
	/* Cài báo thức lại để đọc dữ liệu sau khi set mode mới */
	setAlarm();
	return g_config.device_mode;
}

void RTC_SetTime(void)
{
	RTC_DateTypeDef Date = { 0 };
	RTC_TimeTypeDef Time = { 0 };
	g_time.date.Year = atoi(g_time.string_time);
	g_time.date.Month = atoi(g_time.string_time + 3);
	g_time.date.Date = atoi(g_time.string_time + 6);
	g_time.time.Hours = atoi(g_time.string_time + 9);
	g_time.time.Minutes = atoi(g_time.string_time + 12);
	g_time.time.Seconds = atoi(g_time.string_time + 15);

	Time.Hours = RTC_ByteToBcd2(g_time.time.Hours);
	Time.Minutes = RTC_ByteToBcd2(g_time.time.Minutes);
	Time.Seconds = RTC_ByteToBcd2(g_time.time.Seconds);
	Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	Time.StoreOperation = RTC_STOREOPERATION_RESET;
	HAL_RTC_SetTime(&hrtc, &Time, RTC_FORMAT_BCD);
	Date.WeekDay = 0x01;
	Date.Month = RTC_ByteToBcd2(g_time.date.Month);
	Date.Date = RTC_ByteToBcd2(g_time.date.Date);
	Date.Year = RTC_ByteToBcd2(g_time.date.Year);
	HAL_RTC_SetDate(&hrtc, &Date, RTC_FORMAT_BCD);
}

static void setTime(APP_Mode_Set_Param_t mode)
{
	if (RET_OK == sim_get_time_zone(g_time.string_time))
	{
		RTC_SetTime();
	}
	HAL_RTC_GetDate(&hrtc, &g_time.date, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &g_time.time, RTC_FORMAT_BIN);

	if (mode == APP_SET_MANUAL)
	{
		sim_sync_local_time(g_time.string_time);
		if (strlen(g_time.string_time) < 17)
		{
			memcpy(g_time.string_time, g_sms.received_time, 17);
			sim_set_local_time(g_sms.received_time);
		}
		RTC_SetTime();

		HAL_Delay(1000);

		getTime(APP_T_CURRENT);

		char msg[60] = { 0 };    //VR2: Thoi gian cua thiet bi la: 10:47:19 11-11-2022.
		sprintf(msg, "%s%s", RESPONSE_SETTIME, g_time.string_time);
		sim_sendSms(g_sms.phone_number, msg);

		if (strstr(g_data.device_reset_time, "2000") != NULL)
		{
			getTime(APP_T_RESET);
			printf("Update reset time: %s\n", g_data.device_reset_time);
		}

		if (g_time.date.Year == 0 || g_time.date.Month == 0 || g_time.date.Date == 0)
		{
			HAL_RTC_DeInit(&hrtc);
			HAL_Delay(1000);

			hrtc.Instance = RTC;
			hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
			hrtc.Init.AsynchPrediv = 127;
			hrtc.Init.SynchPrediv = 255;
			hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
			hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
			hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
			if (HAL_RTC_Init(&hrtc) != HAL_OK)
			{

			}

			if (RET_OK == sim_get_time_zone(g_time.string_time))
			{
				RTC_SetTime();
			}
		}

		setAlarm();
	}

}

static void APP_set_gate_number(APP_Mode_Set_Param_t mode)
{
	if (mode == APP_SET_MANUAL)
	{
		/* SMS content: *SETGATE#8088,7769 */
		if (strlen(g_sms.content) == 18)
		{
			memcpy(g_config.gate_send, g_sms.content + 9, 4);
			memcpy(g_config.gate_read, g_sms.content + 14, 4);
			char msg[60] = { 0 };
			sprintf(msg, "Vr2: Gate send: %s, Gate read: %s", g_config.gate_send, g_config.gate_read);
			sim_sendSms(g_sms.phone_number, msg);
			memset(msg, '\0', 60);
		}
		else
		{
			sim_sendSms(g_sms.phone_number, RESPONSE_SETGATE_F);
		}
	}
	else
	{
		flash_read_array(DEVICE_GATE_READ_ADDRESS, g_config.gate_read, 4);
		flash_read_array(DEVICE_GATE_SEND_ADDRESS, g_config.gate_send, 4);
	}
}

static bool APP_set_config(APP_Mode_Set_Param_t mode)
{
	uint8_t pers = 0;
	uint8_t perm = 0;
	uint8_t wart = 0;
	bool param_is_ok = false;
	if (mode == APP_SET_MANUAL)
	{
		if (strlen(g_sms.content) < 10)
		{
			sim_sendSms(g_sms.phone_number, RESPONSE_SETCFG_F);
		}
		else
		{
			char *token = NULL;
			token = strtok(g_sms.content, " ");

			for (int i = 0; i < 3; i++)
			{
				token = strtok(NULL, " ");

				if (strstr(token, "PERS") != NULL)
				{
					token = strtok(NULL, " ");
					pers = atoi(token);
				}
				else if (strstr(token, "PERM") != NULL)
				{
					token = strtok(NULL, " ");
					perm = atoi(token);
				}
				else if (strstr(token, "WART") != NULL)
				{
					token = strtok(NULL, " ");
					wart = atoi(token);
				}
			}
			if (pers < perm)
			{
				sim_sendSms(g_sms.phone_number, "Kiem tra lai gia tri (pers >= perm)");
				param_is_ok = false;
			}
			else
			{
				sim_sendSms(g_sms.phone_number,
				RESPONSE_SETCFG);
				if ((pers != g_config.send_period) || (perm != g_config.read_period) || (wart != g_config.rain_warning))
				{
					g_config.send_period = pers;
					g_config.read_period = perm;
					g_config.rain_warning = wart;
					param_is_ok = true;
				}
			}
			token = NULL;
		}
	}
	else if (mode == APP_SET_AUTO)
	{
		if (((g_time.time.Hours % GET_WARNING_PERIOD) == 0) && (g_time.time.Minutes < 5))
		{
			APP_create_HTTP_json(APP_GETCFG);
			APP_upload_data();
			sim_sync_local_time(g_time.string_time);
			setTime(APP_SET_AUTO);
		}
	}
	return param_is_ok;
}

static void APP_set_admin_number(void)
{
	if (strlen(g_sms.content) == 18 || strlen(g_sms.content) == 19)
	{
		char phone_number[13] = { 0 };

		strcpy(phone_number, g_sms.content + 7);

		if (strlen(phone_number) == 11 || strlen(phone_number) == 12)
		{
			char temp[14] = { 0 };
			sprintf(temp, "+%s", phone_number);

			if (RET_OK == sim_pbook_save_contact(temp, ""))
			{
				sim_sendSms(g_sms.phone_number,
				RESPONSE_SETDB);
				sim_sendSms(temp, RESPONSE_SETDB);
			}
			else
			{
				sim_sendSms(g_sms.phone_number,
				RESPONSE_SETDB_F2);
			}
		}
	}
	else
	{
		sim_sendSms(g_sms.phone_number, RESPONSE_SETDB_F1);
	}
}

static void APP_set_HTTP_URL(APP_Mode_Set_Param_t mode)
{
	if (mode == APP_SET_MANUAL)
	{
		/* SMS content: *SETURL#http//... */
		if (strlen(g_sms.content) > 10 || strlen(g_sms.content) < 69)
		{
			memset(g_config.http_URL, '\0', sizeof(g_config.http_URL));
			strcpy(g_config.http_URL, g_sms.content + 8);
			sim_sendSms(g_sms.phone_number, RESPONSE_SETURL);
		}
		else
		{
			sim_sendSms(g_sms.phone_number, RESPONSE_SETURL_F);
		}
	}
	else if (mode == APP_SET_AUTO)
	{
		flash_read_array(DEVICE_HTTP_URL_ADDRESS, g_config.http_URL, 60);
	}
}

static void APP_reset_device(void)
{
	sim_sendSms(g_sms.phone_number, RESPONSE_RESET);
	APP_write_flash(APP_FL_RESET);
	NVIC_SystemReset();
}

static void APP_restore_device(void)
{
	setDeviceID(APP_SET_AUTO);

	memset(g_config.http_URL, '\0', 60);

	if (g_config.device_mode == APP_DOMUA60V2)
	{
		g_config.read_period = 60;
		g_config.send_period = 60;
		sprintf(g_config.http_URL, "%s", DEVICE_HTTP_URL60_DEFAULT);
	}
	else if (g_config.device_mode == APP_DOMUA10V2)
	{
		g_config.read_period = DEVICE_READ_PERIOD_DEFAULT;
		g_config.send_period = DEVICE_SEND_PERIOD_DEFAULT;
		sprintf(g_config.http_URL, "%s", DEVICE_HTTP_URL10_DEFAULT);
	}
	g_config.rain_warning = DEVICE_RAIN_WARNING_DEFAULT;
	g_config.batt_warning = DEVICE_BATT_WARNING_DEFAULT;
	sprintf(g_config.gate_read, "%s", DEVICE_GATE_READ_DEFAULT);
	sprintf(g_config.gate_send, "%s", DEVICE_GATE_SEND_DEFAULT);
	sim_sendSms(g_sms.phone_number, RESPONSE_RESTORE);
}

static void APP_send_data(void)
{
	getDeviceData(APP_D_BATT | APP_D_NWTYPE | APP_D_SIGNAL | APP_D_POWER | APP_D_PERIOD);
	APP_create_HTTP_json(APP_RESULT);
	if (APP_upload_data() != RET_OK)
	{
		APP_create_SMS_msg(APP_RESULT);
		sim_sendSms(g_config.gate_send, json_buffer);
	}
	APP_create_SMS_msg(APP_SEND);
	sim_sendSms(g_sms.phone_number, json_buffer);
	g_data.pulse_in_period = 0;
	g_data.rain_in_period_60 = 0;
	for (int i = 0; i <= g_data.element; i++)
	{
		g_data.rain_in_period_10[i] = 0.0f;
		g_data.pulse_in_period_10[i] = 0;
	}
	g_data.element = 0;
	APP_write_backup_data();
}

static void APP_resend_data(void)
{
	/* SMS content: *RESEND#00:00-05:00 28-04-2022 */
	if (strlen(g_sms.content) == 30)
	{
		char file[16] = { 0 };
		uint8_t begin_hour = 0;
		uint8_t end_hour = 0;
		char *token = NULL;
		token = strtok(g_sms.content, "#");    //*RESEND
		token = strtok(NULL, ":");    //00
		begin_hour = atoi(token);
		token = strtok(NULL, "-");    //00
		token = strtok(NULL, ":");    //05
		end_hour = atoi(token);
		token = strtok(NULL, " ");    //00
		token = strtok(NULL, "-");		//28-04-2022
		uint8_t day = atoi(token);
		token = strtok(NULL, "-");
		uint8_t month = atoi(token);
		token = strtok(NULL, "");
		uint16_t year = atoi(token);
		sprintf(file, "/%d/%02d/%02d.txt", year, month, day);
		token = NULL;

		sd_mount("/");
		FIL fil;
		FILINFO fno;

		if (FR_OK == f_stat(file, &fno))
		{
			if (FR_OK == f_open(&fil, file, FA_READ))
			{
				memset(json_buffer, '\0', strlen(json_buffer));
				char find_info[5] = { 0 };
				bool accept_post = false;
				while (f_gets(json_buffer, API_BUFFER_SIZE, &fil) != NULL)
				{
					/* Find start string */
					sprintf(find_info, "%d:", begin_hour);
					if ((strstr(json_buffer, find_info) != NULL))
					{
						accept_post = true;
					}
					/* Send all string from start string to end string or end of file */
					if (accept_post == true)
					{
						APP_upload_data();
					}
					/* Find end string */
					sprintf(find_info, "%d:", end_hour);
					if ((strstr(json_buffer, find_info) != NULL))
					{
						break;
					}
					memset(json_buffer, '\0', strlen(json_buffer));
				}
				sim_sendSms(g_sms.phone_number,
				RESPONSE_RESEND);
				f_close(&fil);
			}
		}
		token = NULL;
	}
	else
	{
		sim_sendSms(g_sms.phone_number, "VR2: Kiem tra lai cu phap");
	}
}

static void APP_set_token(APP_Mode_Set_Param_t mode)
{
	if (mode == APP_SET_MANUAL)
	{
		/* SMS content: *SETTOKEN#<46chars> */
		if (strlen(g_sms.content) > 10 || strlen(g_sms.content) < 60)
		{
			memset(g_config.telegram_token, '\0', 48);
			strcpy(g_config.telegram_token, g_sms.content + 10);
			sim_sendSms(g_sms.phone_number, g_config.telegram_token); /* Phan hoi lai token */
		}
		else
		{
			sim_sendSms(g_sms.phone_number, "Kiem tra lai token");
		}
	}
	else if (mode == APP_SET_AUTO)
	{
		flash_read_array(DEVICE_TOKEN_ADDRESS, g_config.telegram_token, 48);
	}
}
static void APP_set_chat_id(APP_Mode_Set_Param_t mode)
{
	if (mode == APP_SET_MANUAL)
	{
		/* SMS content: *SETCID#<max 20 chars> */
		if (strlen(g_sms.content) > 8 || strlen(g_sms.content) < 28)
		{
			memset(g_config.telegram_chat_id, '\0', 20);
			strcpy(g_config.telegram_chat_id, g_sms.content + 8);
			g_config.telegram_chat_id[19] = '\0';
			sim_sendSms(g_sms.phone_number, g_config.telegram_chat_id); /* Phan hoi lai token */
		}
		else
		{
			sim_sendSms(g_sms.phone_number, "Kiem tra lai chat id");
		}
	}
	else if (mode == APP_SET_AUTO)
	{
		flash_read_array(DEVICE_TOKEN_ADDRESS, g_config.telegram_chat_id, 20);
	}
}

typedef struct
{
	Detect_StatusTypeDef IsDetect;
	Write_StatusTypeDef IsWrite;
} SD_DataTypeDef;
SD_DataTypeDef sdcData = { 0 };
Detect_StatusTypeDef SD_IsDetect(void)
{
	sdcData.IsDetect = SD_Detect();
	return sdcData.IsDetect;
}

Write_StatusTypeDef SD_IsWrite(void)
{
	if (SD_DETECT != SD_IsDetect())
	{
		sdcData.IsWrite = SD_WRITE_ERROR;
	}
	else
	{
		sd_mount("/");
		if (FR_OK == sd_update_file("test.txt", "", strlen("")))
		{
			sdcData.IsWrite = SD_WRITE_OK;
		}
		else
		{
			sdcData.IsWrite = SD_WRITE_ERROR;
		}
		sd_unmount("/");
	}

	return sdcData.IsWrite;
}
static void APP_debug_data(void)
{
	getDeviceData(APP_D_BATT | APP_D_NWTYPE | APP_D_SIGNAL | APP_D_POWER | APP_D_TIME_CURRENT);

	char msg[450] = { 0 };
	sprintf(msg, "%s\n"
			"-Normal reset: %s\n"
			"-LastAlarm: %02d:%02d:%02d\n"
			"-NextAlarm: %02d:%02d:00\n"
			"-TimeNow: %s\n"
			"-OWNNUM: %s, DBNUM: %s\n"
			"-IMEI: %s\n"
			"-GS: %s, GR: %s\n"
			"-PW: %s\n"
			"-Bat: %d%%\n"
			"-URL: %s\n"
			"-PSend: %d\n"
			"-StartHour: %d\n"
			"-Warning: %d\n"
			"-Resolu: 0.2\n"
			"-AppAdd: %lX\n"
			"-FWVer: %d.%d.%d\n"
			"-SDCard: Write %s\n"
			"-Signal: %ddBm\n"
			"-Nwtype: %s\n"
			"-DateMfe: 15/07/2022", g_config.device_mode == APP_DOMUA10V2 ? "DOMUA10V2" : "DOMUA60V2",
			g_data.device_reset_time, g_time.last_hour, g_time.last_minute, g_time.last_second, g_time.next_hour,
			g_time.next_minute, g_time.string_time, g_config.device_ID, g_sms.phone_number + 1, g_data.device_IMEI,
			g_config.gate_send, g_config.gate_read, g_data.power_state == true ? "Good" : "Bad", g_data.battery_percent,
			g_config.http_URL, g_config.send_period, g_config.device_mode == APP_DOMUA60V2 ? 19 : 00,
			g_config.rain_warning, g_firmware.current_address, g_current_ver.major, g_current_ver.minor,
			g_current_ver.patch, SD_IsWrite() == SD_WRITE_OK ? "Ok" : "Fail", g_data.cell_signal, g_data.network_type);

	sim_sendSms(g_sms.phone_number, msg);

}

static void APP_update_firmware(void)
{
	if (sim_get_RSSI() < 10)
	{
		sim_sendSms(g_sms.phone_number,
		RESPONSE_UPFW_BAD_SIGNAL);
		return;
	}
	if (ota_get_new_firmware() != true)
	{
		sim_sendSms(g_sms.phone_number,
		RESPONSE_UPFW_GET_URL_F);
		return;
	}
	if (ota_compare_version() != true)
	{
		sim_sendSms(g_sms.phone_number,
		RESPONSE_UPFW_OLD_VERSION);
		return;
	}
	if (ota_download_firmware() != true)
	{
		sim_sendSms(g_sms.phone_number,
		RESPONSE_UPFW_DOWNLOAD_F);
		return;
	}
	APP_write_flash(APP_FL_FOTA);
	NVIC_SystemReset();
}

static bool ota_compare_version(void)
{
	g_current_ver.major = flash_read_int(
	FOTA_CURRENT_VER_MAJOR_ADDRESS);
	g_current_ver.minor = flash_read_int(
	FOTA_CURRENT_VER_MINOR_ADDRESS);
	g_current_ver.patch = flash_read_int(
	FOTA_CURRENT_VER_PATCH_ADDRESS);

	if ((g_current_ver.major == 0xFFFF) & (g_current_ver.minor == 0xFFFF) & (g_current_ver.patch == 0xFFFF))
	{
		g_current_ver.major = 0;
		g_current_ver.minor = 0;
		g_current_ver.patch = 0;
	}
	if (g_new_ver.major > g_current_ver.major)
	{
		if (((g_current_ver.patch == 0) && (g_new_ver.patch == 1))
				|| ((g_current_ver.patch == 1) && (g_new_ver.patch == 0)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (g_new_ver.major < g_current_ver.major)
	{
		return false;
	}
	else
	{
		if (g_new_ver.minor > g_current_ver.minor)
		{
			if (((g_current_ver.patch == 0) && (g_new_ver.patch == 1))
					|| ((g_current_ver.patch == 1) && (g_new_ver.patch == 0)))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (g_new_ver.minor < g_current_ver.minor)
		{
			return false;
		}
		else
		{
			return false;
		}
	}
}

static bool ota_get_new_firmware(void)
{
	APP_create_HTTP_json(APP_GETFW);
	if (RET_OK == APP_upload_data())
	{
		return true;
	}
	return false;
}

static bool ota_download_firmware(void)
{
	if (RET_OK == sim_http_start())
	{
		if (RET_OK == sim_http_set_URL(firmware_URL))
		{
			uint16_t error_code = 0;
			if (RET_OK == sim_http_use_method(GET, &error_code, &g_firmware.size_of_firmware))
			{
				if (RET_OK == sim_http_save_file(g_firmware.name_firmware))
				{
					sim_http_stop();

					return true;
				}
			}
		}
		sim_http_stop();
	}
	return false;
}

static void APP_set_alarm(uint8_t minute)
{
	if (minute > END_OF_HOUR)
	{
		minute = START_OF_HOUR;
	}
	RTC_AlarmTypeDef sAlarm = { 0 };
	sAlarm.AlarmTime.Hours = 0x0;
	sAlarm.AlarmTime.Minutes = RTC_ByteToBcd2(minute);
	sAlarm.AlarmTime.Seconds = 0x0;
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 0x1;
	sAlarm.Alarm = RTC_ALARM_A;
	bool rtcAlarmState = true;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
		rtcAlarmState = false;
	}
	char timeReport[50] = { 0 };
	HAL_RTC_GetAlarm(&hrtc, &g_time.alarm, RTC_ALARM_A, RTC_FORMAT_BIN);
	sprintf(timeReport, "Next alarm (calc): %d:%d (%d:%d)\n", g_time.alarm.AlarmTime.Hours,
			g_time.alarm.AlarmTime.Minutes, g_time.next_hour, g_time.next_minute);
	printf(timeReport);
	if (rtcAlarmState == false)
	{
		sim_sendSms("84969647577", timeReport);
		rtcAlarmState = true;
	}
}
void setAlarm(void)
{
	HAL_RTC_GetTime(&hrtc, &g_time.time, RTC_FORMAT_BIN);
	g_time.next_minute = getNextMinute();
	g_time.next_hour = getNextHour();

	APP_set_alarm(g_time.next_minute);
}

void APP_get_SMS_info(void)
{
//\r\n+CMT: \"+84969647577\",\"\",\"22/04/23,10:36:27+28\"\r\n*DEBUG#\r\n
//\r\n+CMT: "57535756","","22/04/26,11:45:54+28"\r\nYeu cau khong thanh cong. DV chi ap dung cho thue bao dang hoat dong hai chieu. Chi tiet LH 198 (mien phi).
//\r\n+CMT: "71826566","","22/04/26,11:49:54+28"\r\n<#> 869168 is your Grab Activation Code (GAC). It expires in 2 minutes. Do not share it with anyone.
//\r\n+CMT: "7065676966797975","","22/04/26,12:00:45+28"\r\n<#> 36011 is your Facebook codeLaz+nxCarLW
	/*
	 [2022-04-27_15:40:52:162]+CMT: "+84969647577","","22/04/27,15:40:50+28"
	 [2022-04-27_15:40:52:162]Quy khach da thanh toan 110.000d cuoc DV Di dong so 862865768. So tien thua 110.000d se duoc chuyen sang thang sau. De tra cuu hoa don dien tu va lua cho
	 [2022-04-27_15:40:52:398]
	 [2022-04-27_15:40:52:398]+CMT: "+84969647577","","22/04/27,15:40:50+28"
	 [2022-04-27_15:40:52:398]n KM goi, nhan tin, Internet danh rieng cho Quy khach, vui long truy cap ung dung My Viettel tai: https://myvt.page.link/tcts hoac bam goi *098#. LH 198
	 [2022-04-27_15:40:52:598]
	 [2022-04-27_15:40:52:598]+CMT: "+84969647577","","22/04/27,15:40:50+28"
	 [2022-04-27_15:40:52:598](0d).
	 */

	char *token = NULL;
	char *p_g_rx_buffer = sim_get_g_rx_buffer();
	printf("%s%s\n", "VR2: ", p_g_rx_buffer);
	memset(g_sms.phone_number, '\0', 12);
	memset(g_sms.received_time, '\0', REC_TIME_LEN);
	memset(g_sms.content, '\0', SMS_CONTENT_LEN);

	token = strtok(p_g_rx_buffer, "\"");    //"\r\n+CMT: "
	token = strtok(NULL, "\"");    //"+84969647577"

	if ((strlen(token) == 12) && (strstr(token, "+84") != NULL))
	{
		memcpy(g_sms.phone_number, token, 12);    //"+84969647577"
		token = strcpy(token, p_g_rx_buffer + 27);    //"22/04/23,10:36:27+28\"\r\n*DEBUG#\r\n"
		token = strtok(token, "\"");    //"22/04/23,10:36:27+28"
		memcpy(g_sms.received_time, token, REC_TIME_LEN - 1);    //"22/04/23,10:36:27+28\0"
	}
	token = strtok(NULL, "\n");    //"\r"
	token = strtok(NULL, "\r");    //"*DEBUG#"
	memcpy(g_sms.content, token, strlen(token));
	printf("%s%s, %s, %s\n", "VR2: ", g_sms.phone_number, g_sms.received_time, g_sms.content);
}

static bool APP_check_SMS_sender(void)
{
	bool check_sender = false;

	if ((strstr(g_sms.phone_number, SYSTEM_ADMIN_PHONENUMBER_1) != NULL)
	/*|| (strstr(g_sms.phone_number, SYSTEM_ADMIN_PHONENUMBER_2) != NULL)*/
	|| (strstr(g_sms.phone_number, SYSTEM_ADMIN_PHONENUMBER_3) != NULL)
			|| (strstr(g_sms.phone_number, SYSTEM_ADMIN_PHONENUMBER_4) != NULL)
			|| (strstr(g_sms.phone_number, SYSTEM_ADMIN_PHONENUMBER_5) != NULL)
			|| (strstr(g_sms.phone_number, SYSTEM_ADMIN_PHONENUMBER_6) != NULL)
			|| (strstr(g_sms.phone_number, SYSTEM_ADMIN_PHONENUMBER_7) != NULL))
	{
		check_sender = true;
	}
	else
	{
		char phoneNbCheck[14] = { 0 };    //lỗi định dạng
		char nameCheck[15] = { 0 };
		sim_pbook_read_contact(1, phoneNbCheck, nameCheck);

		if (strstr(g_sms.phone_number, phoneNbCheck) != NULL)
		{
			check_sender = true;
		}
	}
	return check_sender;
}

static uint8_t APP_process_SMS_msg(void)
{
	uint8_t need_to_write_flash = false;

	if (strstr(g_sms.content, SMS_SETID_SYNTAX) != NULL)
	{
		setDeviceID(APP_SET_MANUAL);
		need_to_write_flash = true;
	}
	else if (strstr(g_sms.content, SMS_DEBUG_SYNTAX) != NULL)
	{
		APP_debug_data();
		need_to_write_flash = false;
	}
	else if (strstr(g_sms.content, SMS_SEND_SYNTAX) != NULL)
	{
		APP_send_data();
		need_to_write_flash = false;
	}
	else if (strstr(g_sms.content, SMS_RESTORE_SYNTAX) != NULL)
	{
		APP_restore_device();
		need_to_write_flash = true;
	}
	else if (strstr(g_sms.content, SMS_RESET_SYNTAX) != NULL)
	{
		APP_reset_device();
		need_to_write_flash = false;
	}
	else if (strstr(g_sms.content, SMS_SETDEBUG_SYNTAX) != NULL)
	{
		APP_set_admin_number();
		need_to_write_flash = false;
	}
	else if (strstr(g_sms.content, SMS_SETTIME_SYNTAX) != NULL)
	{
		setTime(APP_SET_MANUAL);
		need_to_write_flash = false;
	}
	else if (strstr(g_sms.content, SMS_SETGATE_SYNTAX) != NULL)
	{
		APP_set_gate_number(APP_SET_MANUAL);
		need_to_write_flash = true;
	}
	else if (strstr(g_sms.content, SMS_SETCONFIG_SYNTAX) != NULL)
	{
		APP_set_config(APP_SET_MANUAL);
		need_to_write_flash = true;
	}
	else if (strstr(g_sms.content, SMS_SETURL_SYNTAX) != NULL)
	{
		APP_set_HTTP_URL(APP_SET_MANUAL);
		need_to_write_flash = true;
	}
	else if (strstr(g_sms.content, SMS_RESEND_SYNTAX) != NULL)
	{
		APP_resend_data();
		need_to_write_flash = false;
	}
	else if (strstr(g_sms.content, SMS_UPFW_SYNTAX) != NULL)
	{
		APP_update_firmware();
		need_to_write_flash = false;
	}
	else if (strstr(g_sms.content, SMS_SET_DEVICE_MODE_SYNTAX) != NULL)
	{
		setDeviceMode(APP_SET_MANUAL);
		need_to_write_flash = true;
	}
	else if (strstr(g_sms.content, SMS_SET_TOKEN_SYNTAX) != NULL)
	{
		APP_set_token(APP_SET_MANUAL);
		need_to_write_flash = true;
	}
	else if (strstr(g_sms.content, SMS_SET_CHAT_ID_SYNTAX) != NULL)
	{
		APP_set_chat_id(APP_SET_MANUAL);
		need_to_write_flash = true;
	}
	else
	{    //không phải là một trong những cú pháp trên
		need_to_write_flash = false;
	}
	return need_to_write_flash;
}

void processUrc_Sms(void)
{
	if (g_flag.sms_msg == true)
	{
		enableSendAT();
		if (APP_check_SMS_sender() == true)
		{
			if (APP_process_SMS_msg() == true)
			{
				APP_write_flash(APP_FL_CONFIG);
			}
		}
		else
		{
			char msg[220] = { 0 };
			snprintf(msg, 220, "%s: Invalid SMS:\n%s\n%s", g_config.device_ID, g_sms.phone_number, g_sms.content);
			APP_telegram_send_msg(&g_tele, msg);
		}
		memset(sim_get_g_rx_buffer(), '\0', RX_LEN);
		sim_sms_delete();
		g_flag.sms_msg = false;
		disableSendAT();
	}
}

static eSTATUS APP_telegram_send_msg(Telegram_HandleTypeDef *const me, char *msg)
{
	sim_http_stop();

	uint16_t error_code = 0;
	uint32_t len_of_data = 0;
	eSTATUS post_status = RET_ERROR;
	if (RET_OK != sim_http_start())
	{
		printf("HTTP start failed\n");
	}
	else
	{
		char tele_url[200] = { 0 };
		snprintf(tele_url, 200, "https://api.telegram.org/bot%s/sendMessage", g_config.telegram_token);
		if (RET_OK == sim_http_set_URL(tele_url))
		{
			sim_http_set_parameter("application/json");
			char json_data[500] = { 0 };
			snprintf(json_data, 500, "{"
					"\"text\":\"%s\","
					"\"parse_mode\":\"%s\","
					"\"disable_web_page_preview\":%s,"
					"\"disable_notification\":%s,"
					"\"reply_to_message_id\":%s,"
					"\"chat_id\": \"%s\"}", msg, me->parse_mode, me->disable_web_page_preview, me->disable_notification,
					me->reply_to_message_id, g_config.telegram_chat_id);

			if (RET_OK == sim_http_upload_data(strlen(json_data), 10000, json_data))
			{
				post_status = sim_http_use_method(POST, &error_code, &len_of_data);
			}
		}
		sim_http_stop();
	}
	if (error_code == 200 || post_status == RET_OK)
	{
		printf("%sSent OKE\n", "Tele: ");
		return RET_OK;
	}
	printf("%sSent failed\n", "Tele: ");
	return RET_ERROR;
}

static eSTATUS APP_upload_data(void)
{
	sim_http_stop();
	uint16_t error_code = 0;
	uint32_t len_of_data = 0;
	eSTATUS post_status = RET_ERROR;
	if (RET_OK != sim_http_start())
	{
		printf("HTTP start failed\n");
	}
	else
	{
		if (RET_OK == sim_http_set_URL(g_config.http_URL))
		{
			sim_http_set_parameter("application/json");
			if (RET_OK == sim_http_upload_data(strlen(json_buffer), 10000, json_buffer))
			{
				post_status = sim_http_use_method(POST, &error_code, &len_of_data);
				if (len_of_data > 0)
				{
					char response[200] = { 0 };
					sim_http_read_content(response, 0, len_of_data);
					char *token = NULL;
					if (strstr(response, "WART") != NULL)    //{"WART":50}
					{
						token = strtok(response, ":");
						token = strtok(NULL, "}");
						uint8_t thres = atoi(token);
						if (thres != g_config.rain_warning)
						{
							g_config.rain_warning = thres;
							APP_write_flash(APP_FL_CONFIG);
						}
					}
					if (strstr(response, "FW") != NULL)    //{"FW":"https://core-asset.s3-ap-southeast-1.amazonaws.com/vrain2_60/firmware/5d0b7f30-792d-44de-9fa0-069b60038e3b/vraincontroller_1.4.0.bin"}
					{
						memset(firmware_URL, '\0', 200);
						token = strtok(response, ":");
						token = strtok(NULL, "\"");    //"https://core-asset.s3-ap-southeast-1.amazonaws.com/vrain2_60/firmware/5d0b7f30-792d-44de-9fa0-069b60038e3b/vraincontroller_1.4.0.bin"
						memcpy(firmware_URL, token, len_of_data - 10);
						char version[200] = { 0 };
						memcpy(version, firmware_URL, strlen(firmware_URL));
						token = strtok(version, "/");
						while (token != NULL)
						{
							token = strtok(NULL, "/");
							if (strstr(token, ".bin") != NULL)
							{
								memset(g_firmware.name_firmware, '\0', 60);
								memcpy(g_firmware.name_firmware, token, strlen(token));
								char p_version[60] = { 0 };
								memcpy(p_version, g_firmware.name_firmware, strlen(g_firmware.name_firmware));
								token = strtok(p_version, "_");
								token = strtok(NULL, ".");
								g_new_ver.major = atoi(token);
								token = strtok(NULL, ".");
								g_new_ver.minor = atoi(token);
								token = strtok(NULL, ".");
								g_new_ver.patch = atoi(token);
							}
						}
						memset(version, '\0', 200);
					}
					token = NULL;
				}
			}
		}
		sim_http_stop();
	}
	if (error_code == 200 || post_status == RET_OK)
	{
		printf("%sPost OKE\n", "VR2: ");
		return RET_OK;
	}
	printf("%sPost failed\n", "VR2: ");
	return RET_ERROR;
}

static void APP_upload_last_data(void)
{
	FIL fil;
	static uint32_t p_read = 0;
	uint32_t size_of_file = 0;

	if (FR_OK == f_open(&fil, LOST_FILE, FA_READ))
	{
		size_of_file = fil.obj.objsize;
		memset(json_buffer, '\0', strlen(json_buffer));
		f_lseek(&fil, p_read);

		while (f_gets(json_buffer, API_BUFFER_SIZE, &fil) != NULL)
		{
			if (RET_OK != APP_upload_data())
			{
				break;    //sẽ nhảy đến hàm f_close() đầu tiên
			}
			p_read += strlen(json_buffer);
			memset(json_buffer, '\0', strlen(json_buffer));
		}
		f_close(&fil);

		if (p_read == size_of_file)
		{
			f_unlink(LOST_FILE);
			f_open(&fil, LOST_FILE, FA_CREATE_ALWAYS | FA_WRITE);
			f_close(&fil);
			p_read = 0;
		}
	}
}

void processHttp(void)
{

	/* Đến thời gian đo lượng mưa, chu kỳ đo có thể là 10, 20, 30, 60
	 * Kiểm tra bằng cách lấy số phút hiện tại chia cho chu kỳ đo phải bằng 0 */
	if (g_flag.readTime == true)
	{
		enableSendAT();
		/* Trong mỗi chu kỳ đọc phải tăng chỉ số phần tử dữ liệu lên 1 đơn vị */
		g_data.element++;
		/* Biến này được sử dụng cho bộ 60, nếu đến chu kỳ đo thì cũng là chu kỳ gửi
		 * nên reset lại biến đếm xung cảm biến */
		g_data.pulse_in_period = 0;
		/* Cài đặt báo thức cho chu kỳ đo tiếp theo */
		setAlarm();
		/* Với bộ 10 phút: chu kỳ gửi có thể là 10, 20, 30, 60 nên số lần gửi trong một tiếng
		 * lần lượt là 6, 3, 2, 1 lần.
		 * Với bộ 60 phút: chu kỳ gửi là 60 phút một lần, nên thời gian đo cũng là thời gian gửi*/
		if (g_flag.sendTime == true)
		{
			/* Khi vào thời điểm gửi, biến element đã tăng 1 đơn vị trước đó, nên phải giảm đi 1 */
			g_data.element--;
			/* Tạo mới file data nếu nó thoả mãn điều kiện là ngày mới */
			APP_create_new_data_file();
			/* Đọc dữ liệu các tham số của thiết bị */
			getDeviceData(APP_D_BATT | APP_D_NWTYPE | APP_D_SIGNAL | APP_D_POWER | APP_D_PERIOD);
			APP_create_HTTP_json(APP_RESULT);

			sd_mount("/");
			sd_update_file(g_data.current_data_filename, json_buffer, strlen(json_buffer));
			if (APP_upload_data() != RET_OK)
			{
				printf("Failed %d time(s)\n", ++post_fail_counter);

				if (strlen(json_buffer) > 160)
				{
					sd_update_file(LOST_FILE, json_buffer, strlen(json_buffer));
				}
				APP_create_SMS_msg(APP_RESULT);
				sim_sendSms(g_config.gate_send, json_buffer);
			}
			/* Sau khi gửi data thì phải xoá giá trị về 0 hết */
			g_data.rain_in_period_60 = 0.0f;
			for (int i = 0; i <= g_data.element; i++)
			{
				g_data.rain_in_period_10[i] = 0.0f;
				g_data.pulse_in_period_10[i] = 0;
			}
			g_data.element = 0;
			/* Với lượng mưa trong 1 tiếng để cảnh báo thì phải đến giờ tròn (phút 00) mới reset */
			if (g_time.time.Minutes == START_OF_HOUR)
			{
				g_flag.http_enable_send_warning = true;
				g_data.pulse_in_hour = 0;
				g_data.rain_in_hour = 0.0f;
			}

			APP_set_config(APP_SET_AUTO);

			//Code mới: xem lại , khi module SIM đã tắt do pin yếu, có nguồn sẽ bật lại, lúc này postFailed có thể >= 2 thì nó sẽ reset lại thêm một lần nữa...
			if (post_fail_counter >= 2 /* && g_flag.under_voltage == false*/)
			{
				HAL_UART_Transmit(&SIM_UART, (uint8_t*) "AT+CPOF\r\n", strlen("AT+CPOF\r\n"), HAL_MAX_DELAY);
				//Khi post failed 2 lần thì reset module SIM, bởi vì reset vdk và module SIM sợ các cấu hình không giữ lại được
				HAL_Delay(20000);
				enableSendAT();
				sim_init(PWRKEY_GPIO_Port, PWRKEY_Pin);
				sim_sync_local_time(g_time.string_time);

				char failed_sms_buffer[50] = { 0 };
				snprintf(failed_sms_buffer, 50, "%s: Post failed %u times", g_config.device_ID, post_fail_counter);
				g_tele.disable_notification = TELEGRAM_TRUE_STRING;
				if (RET_OK == APP_telegram_send_msg(&g_tele, failed_sms_buffer))
				{
					post_fail_counter = 0;
				}
			}
			/* Upload old data in lost file */
			if ((sd_get_file_size(LOST_FILE) > 0) && (strstr(g_data.network_type, "NO SERVICE") == NULL))
			{
				APP_upload_last_data();
			}
			sd_unmount("/");

			g_flag.sendTime = false;

			static bool power_source_warning = false;
			if (g_data.power_state == false && power_source_warning == true)
			{
				char failed_sms_buffer[50] = { 0 };
				snprintf(failed_sms_buffer, 50, "%s: Power is off", g_config.device_ID);
				g_tele.disable_notification = TELEGRAM_TRUE_STRING;
				if (RET_OK == APP_telegram_send_msg(&g_tele, failed_sms_buffer))
				{
					power_source_warning = false;
				}
			}
			else if (g_data.power_state == true && power_source_warning == false)
			{
				char failed_sms_buffer[50] = { 0 };
				snprintf(failed_sms_buffer, 50, "%s: Power is on", g_config.device_ID);
				g_tele.disable_notification = TELEGRAM_TRUE_STRING;
				if (RET_OK == APP_telegram_send_msg(&g_tele, failed_sms_buffer))
				{
					power_source_warning = true;
				}
			}

			static bool sd_warning = true;
			if (SD_IsWrite() == SD_WRITE_ERROR && sd_warning == true)
			{
				char failed_sms_buffer[50] = { 0 };
				snprintf(failed_sms_buffer, 50, "%s: SD is write fail", g_config.device_ID);
				g_tele.disable_notification = TELEGRAM_TRUE_STRING;
				if (RET_OK == APP_telegram_send_msg(&g_tele, failed_sms_buffer))
				{
					sd_warning = false;
				}
			}
		}
		//cho chắc cú, thì set alarm thêm lần nữa
		setAlarm();

		g_flag.readTime = false;
		disableSendAT();
	}
}

void sendRainWarning(void)
{
	/* Send warning rainfall */
	if ((g_flag.w_rain == true) && (g_flag.http_enable_send_warning == true))
	{
		char msg[60] = { 0 };
		enableSendAT();
		APP_create_HTTP_json(APP_WART);
		if (APP_upload_data() != RET_OK)
		{
			APP_create_SMS_msg(APP_WART);
			sim_sendSms(g_config.gate_send, json_buffer);
			snprintf(msg, 60, "%s: Send rain warning fail (%.2f)", g_config.device_ID, g_data.rain_in_hour);
		}
		else
		{
			g_flag.w_rain = false;
			g_flag.http_enable_send_warning = false;
			snprintf(msg, 60, "%s: Send rain warning done (%.2f)", g_config.device_ID, g_data.rain_in_hour);
		}
		g_tele.disable_notification = TELEGRAM_FALSE_STRING;
		APP_telegram_send_msg(&g_tele, msg);
		disableSendAT();
	}
}

void APP_send_batt_warning(void)
{
	/* %pin > 50 và điện lưới đang cắm mới cho phép được cảnh báo lại */
	if ((g_data.battery_percent > DEVICE_BATT_WARNING_DEFAULT) && (g_data.power_state == true))
	{
		g_flag.w_batt = true;
	}
	/* Pin thấp hơn ngưỡng và được phép cảnh báo thì gửi cảnh báo */
	if ((g_data.battery_percent < DEVICE_BATT_WARNING_DEFAULT) && (g_flag.w_batt == true))
	{
		char msg[60] = { 0 };
		APP_create_HTTP_json(APP_BATCAP);
		if (APP_upload_data() != RET_OK)
		{
			APP_create_SMS_msg(APP_BATCAP);
			sim_sendSms(g_config.gate_send, json_buffer);
			snprintf(msg, 60, "%s: Send battery warning fail (%u%%)", g_config.device_ID, g_data.battery_percent);
		}
		else
		{
			snprintf(msg, 60, "%s: Send battery warning done (%u%%)", g_config.device_ID, g_data.battery_percent);
			g_flag.w_batt = false;
		}
		g_tele.disable_notification = TELEGRAM_FALSE_STRING;
		APP_telegram_send_msg(&g_tele, msg);
	}
}

void cellularEvent(void)
{
	switch (sim_rx_event())
	{
		case CELL_SMS:
			APP_get_SMS_info();
			g_flag.sms_msg = true;
			break;
		case CELL_SIM_OFF:
			HAL_UART_Transmit(&SIM_UART, (uint8_t*) "AT+CSCLK=1\r\n", strlen("AT+CSCLK=1\r\n"), HAL_MAX_DELAY);
			g_flag.sim_disconnect = true;
			break;
		default:
			break;
	}
	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, g_rx_buffer, RX_LEN);
}

void reconnect_sim_card(void)
{
	if (g_flag.sim_disconnect == true)
	{
		enableSendAT();
		sim_set_phone_function(MINI_MODE);
		HAL_Delay(1000);    //chờ 1s để sim tự kết nối lại
		sim_set_phone_function(FULL_MODE);
		HAL_Delay(9000);
		if (strstr(APP_get_network_type(), "NO SERVICE") == NULL)
		{
			g_flag.sim_disconnect = false;
		}
		else
		{
			HAL_Delay(3000);    //nếu mà sim tháo ra không lắp lại, thì nó sẽ sáng đèn, chờ 3 giây cho nó sáng lên, sau đó gặp lệnh disable bên dưới nó sẽ tắt được đèn netlight
		}
		disableSendAT();
	}
}

void APP_write_backup_data(void)
{
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, g_time.time.Hours);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, g_time.date.Date);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, g_data.pulse_in_hour);
	if (g_config.device_mode == APP_DOMUA60V2)
	{
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, g_data.pulse_in_period);
	}
	else if (g_config.device_mode == APP_DOMUA10V2)
	{
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, g_data.element);
		//xxx: ver4.1 sử dụng switch case, ver 5.0 sử dụng vòng lặp for
		for (size_t i = 0; i <= g_data.element; i++)
		{
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5 + i, g_data.pulse_in_period_10[i]);
		}
	}
}

void readBKPdata(void)
{
//phải so sánh thời gian chủ có trùng ngày giờ hay không, nếu trùng thì mới đếm tiếp
	if (g_time.time.Hours == HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0)
			&& g_time.date.Date == HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1))
	{
		g_data.pulse_in_hour = HAL_RTCEx_BKUPRead(&hrtc,
		RTC_BKP_DR2);
		if (g_config.device_mode == APP_DOMUA60V2)
		{
			g_data.pulse_in_period = HAL_RTCEx_BKUPRead(&hrtc,
			RTC_BKP_DR3);
		}
		else if (g_config.device_mode == APP_DOMUA10V2)
		{
			g_data.element = HAL_RTCEx_BKUPRead(&hrtc,
			RTC_BKP_DR4);
			//xxx: ver4.1 sử dụng switch case, ver 5.0 sử dụng vòng lặp for
			for (size_t i = 0; i <= g_data.element; i++)
			{
				g_data.pulse_in_period_10[i] = HAL_RTCEx_BKUPRead(&hrtc,
				RTC_BKP_DR5 + i);
			}
		}
	}
	else
	{    //nếu khác giờ và ngày thì đếm lại từ đầu, vì đã quá thời gian của chu kỳ
		g_data.pulse_in_period = 0;
		g_data.pulse_in_hour = 0;
	}

	g_data.rain_in_hour = g_data.pulse_in_hour * RESOLUTION;
	g_data.rain_in_period_60 = g_data.pulse_in_period * RESOLUTION;
	for (int i = 0; i <= g_data.element; i++)
	{
		g_data.rain_in_period_10[i] = g_data.pulse_in_period_10[i] * RESOLUTION;
	}
}

static void APP_write_flash(APP_Flash_Write_t w_flash)
{
	flash_unlock();

	if (w_flash & APP_FL_CONFIG)
	{
		flash_erase(DEVICE_MODE_ADDRESS);
		flash_write_int(DEVICE_MODE_ADDRESS, g_config.device_mode);
		flash_write_array(DEVICE_ID_ADDRESS, g_config.device_ID, 12);
		flash_write_int(DEVICE_RAIN_WARNING_ADDRESS, g_config.rain_warning);
//		flash_write_int(DEVICE_BATT_WARNING_ADDRESS, g_config.batt_warning);
		flash_write_array(DEVICE_GATE_SEND_ADDRESS, g_config.gate_send, 4);
		flash_write_array(DEVICE_GATE_READ_ADDRESS, g_config.gate_read, 4);
		flash_write_int(DEVICE_SEND_PERIOD_ADDRESS, g_config.send_period);
		flash_write_int(DEVICE_READ_PERIOD_ADDRESS, g_config.read_period);
		flash_write_array(DEVICE_HTTP_URL_ADDRESS, g_config.http_URL, 60);
		flash_write_array(DEVICE_TOKEN_ADDRESS, g_config.telegram_token, 48);
		flash_write_array(DEVICE_CHAT_ID_ADDRESS, g_config.telegram_chat_id, 20);
	}
	if (w_flash & APP_FL_FOTA)
	{
		flash_erase(FOTA_FLAG_NEED_UPDATE);
		flash_write_int(FOTA_FLAG_NEED_UPDATE, 0x01);
		flash_write_int(FOTA_CURRENT_ADDRESS, g_firmware.current_address);
		flash_write_int(FOTA_CURRENT_VER_MAJOR_ADDRESS, g_current_ver.major);
		flash_write_int(FOTA_CURRENT_VER_MINOR_ADDRESS, g_current_ver.minor);
		flash_write_int(FOTA_CURRENT_VER_PATCH_ADDRESS, g_current_ver.patch);
		flash_write_int(FOTA_NEW_VER_MAJOR_ADDRESS, g_new_ver.major);
		flash_write_int(FOTA_NEW_VER_MINOR_ADDRESS, g_new_ver.minor);
		flash_write_int(FOTA_NEW_VER_PATCH_ADDRESS, g_new_ver.patch);
		flash_erase(FOTA_STATE_UPDATE);
		flash_write_int(FOTA_FIRMWARE_SIZE, g_firmware.size_of_firmware);
		flash_write_array(FOTA_PHONE_REQUEST, g_sms.phone_number, 12);
		flash_write_array(FOTA_FILENAME, g_firmware.name_firmware, 60);
	}
	if (w_flash & APP_FL_RESET)
	{
		flash_erase(FOTA_STATE_UPDATE);
		flash_write_int(FOTA_STATE_UPDATE, FOTA_RESET);
		flash_write_array(FOTA_PHONE_REQUEST, g_sms.phone_number, 12);
	}

	flash_lock();
}

void readFlash(APP_Flash_Write_t r_flash)
{
	if (r_flash & APP_FL_CONFIG)
	{
		memset(g_config.device_ID, '\0', 13);    //841699741245
		memset(g_config.gate_send, '\0', 5);
		memset(g_config.gate_read, '\0', 5);
		memset(g_config.http_URL, '\0', 61);
		memset(g_config.telegram_token, '\0', 48);
		memset(g_config.telegram_chat_id, '\0', 20);

		g_config.device_mode = flash_read_int(DEVICE_MODE_ADDRESS);
		g_config.rain_warning = flash_read_int(
		DEVICE_RAIN_WARNING_ADDRESS);
		g_config.send_period = flash_read_int(
		DEVICE_SEND_PERIOD_ADDRESS);
		g_config.read_period = flash_read_int(
		DEVICE_READ_PERIOD_ADDRESS);
		flash_read_array(DEVICE_ID_ADDRESS, g_config.device_ID, 12);
		flash_read_array(DEVICE_GATE_SEND_ADDRESS, g_config.gate_send, 4);
		flash_read_array(DEVICE_GATE_READ_ADDRESS, g_config.gate_read, 4);
		flash_read_array(DEVICE_HTTP_URL_ADDRESS, g_config.http_URL, 60);

		if (g_config.device_mode == 0x00)
		{
			g_config.device_mode = APP_DOMUA10V2;
		}
		if (g_config.device_ID[0] == 0x00)
		{
			memcpy(g_config.device_ID, DEVICE_ID_DEFAULT, 12);
		}
		if (g_config.gate_send[0] == 0x00) /* gate_send[0] = '8' */
		{
			memcpy(g_config.gate_send, DEVICE_GATE_SEND_DEFAULT, 4);
		}
		if (g_config.gate_read[0] == 0x00) /* gate_read[0] = '7' */
		{
			memcpy(g_config.gate_read, DEVICE_GATE_READ_DEFAULT, 4);
		}
		if ((g_config.http_URL[0] == 0x00) && (g_config.device_mode == APP_DOMUA10V2)) /* http_URL[0] = 'h' */
		{
			memcpy(g_config.http_URL, DEVICE_HTTP_URL10_DEFAULT, strlen(DEVICE_HTTP_URL10_DEFAULT));
		}
		else if ((g_config.http_URL[0] == 0x00) && (g_config.device_mode == APP_DOMUA60V2))
		{
			memcpy(g_config.http_URL, DEVICE_HTTP_URL60_DEFAULT, strlen(DEVICE_HTTP_URL60_DEFAULT));
		}

		if (g_config.telegram_token[0] == 0x00) /* telegram_token[0] = '\0' */
		{
			memcpy(g_config.telegram_token, DEVICE_TELEGRAM_TOKEN_DEFAULT, strlen(DEVICE_TELEGRAM_TOKEN_DEFAULT));
		}

		if (g_config.telegram_chat_id[0] == 0x00) /* telegram_chat_id[0] = '\0' */
		{
			memcpy(g_config.telegram_chat_id, DEVICE_TELEGRAM_CHAT_ID_DEFAULT, strlen(DEVICE_TELEGRAM_CHAT_ID_DEFAULT));
		}

		if (g_config.rain_warning == 0x00)
		{
			g_config.rain_warning = DEVICE_RAIN_WARNING_DEFAULT;
		}
		if (((g_config.read_period == 0x00) || (g_config.send_period == 0x00))
				&& (g_config.device_mode == APP_DOMUA10V2))
		{
			g_config.read_period = DEVICE_READ_PERIOD_DEFAULT;
			g_config.send_period = DEVICE_SEND_PERIOD_DEFAULT;
		}
		else if (((g_config.read_period == 0x00) || (g_config.send_period == 0x00))
				&& (g_config.device_mode == APP_DOMUA60V2))
		{
			g_config.read_period = 60;
			g_config.send_period = 60;
		}
	}

	if (r_flash & APP_FL_FOTA)
	{
		g_firmware.current_address = flash_read_int(
		FOTA_CURRENT_ADDRESS);
		g_current_ver.major = flash_read_int(
		FOTA_CURRENT_VER_MAJOR_ADDRESS);
		g_current_ver.minor = flash_read_int(
		FOTA_CURRENT_VER_MINOR_ADDRESS);
		g_current_ver.patch = flash_read_int(
		FOTA_CURRENT_VER_PATCH_ADDRESS);
		g_flag.fota_state = flash_read_int(FOTA_STATE_UPDATE);
		flash_read_array(FOTA_PHONE_REQUEST, g_sms.phone_number, 12);
	}
}

static void APP_create_HTTP_json(APP_API_Command_t cmd)
{
	memset(json_buffer, '\0', strlen(json_buffer));

	switch (cmd)
	{
		case APP_RESULT:
			/* Bộ 60 phút */
			if (g_config.device_mode == APP_DOMUA60V2)
			{
				sprintf(json_buffer,
						"{\"DOMUAVRAINV2\":{\"CMD\":\"RESULT\",\"ID\":\"%s\",\"IMEI\":\"%s\",\"TIME\":\"%s\",\"VAL\":%.1f,\"BATT\":%d,\"POWR\":%d,\"SIGS\":%d,\"NWTYPE\":\"%s\"}}\n",
						g_config.device_ID, g_data.device_IMEI, g_data.http_time_period, g_data.rain_in_period_60,
						g_data.battery_percent, g_data.power_state, g_data.cell_signal, g_data.network_type);
			}
			/* Bộ 10 phút */
			else if (g_config.device_mode == APP_DOMUA10V2)
			{
				//xxx: ver4.1 sử dụng switch case cho toàn bộ chuỗi dữ liệu, ver 5.0 sử dụng switch case cho mỗi data
				char vals[35] = { 0 };    //10.5,32.0,0.0,12.4,15.7,0.0

				switch (g_data.element)
				{
					case 0:
						sprintf(vals, "%.1f", g_data.rain_in_period_10[0]);
						break;
					case 1:
						sprintf(vals, "%.1f,%.1f", g_data.rain_in_period_10[0], g_data.rain_in_period_10[1]);
						break;
					case 2:
						sprintf(vals, "%.1f,%.1f,%.1f", g_data.rain_in_period_10[0], g_data.rain_in_period_10[1],
								g_data.rain_in_period_10[2]);
						break;
					case 3:
						sprintf(vals, "%.1f,%.1f,%.1f,%.1f", g_data.rain_in_period_10[0], g_data.rain_in_period_10[1],
								g_data.rain_in_period_10[2], g_data.rain_in_period_10[3]);
						break;
					case 4:
						sprintf(vals, "%.1f,%.1f,%.1f,%.1f,%.1f", g_data.rain_in_period_10[0],
								g_data.rain_in_period_10[1], g_data.rain_in_period_10[2], g_data.rain_in_period_10[3],
								g_data.rain_in_period_10[4]);
						break;
					case 5:
						sprintf(vals, "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", g_data.rain_in_period_10[0],
								g_data.rain_in_period_10[1], g_data.rain_in_period_10[2], g_data.rain_in_period_10[3],
								g_data.rain_in_period_10[4], g_data.rain_in_period_10[5]);
						break;
					default:
						break;
				}

				sprintf(json_buffer,
						"{\"DOMUAVRAINV2\":{\"CMD\":\"RESULT\",\"ID\":\"%s\",\"IMEI\":\"%s\",\"TIME\":\"%s\",\"VALS\":[%s],\"BATT\":%d,\"POWR\":%d,\"SIGS\":%d,\"NWTYPE\":\"%s\"}}\n",
						g_config.device_ID, g_data.device_IMEI, g_data.http_time_period, vals, g_data.battery_percent,
						g_data.power_state, g_data.cell_signal, g_data.network_type);
			}
			break;
		case APP_BATCAP:
			sprintf(json_buffer, "{\"DOMUAVRAINV2\":{\"CMD\":\"BATCAP\",\"ID\":\"%s\",\"IMEI\":\"%s\",\"VAL\":%d}}",
					g_config.device_ID, g_data.device_IMEI, g_data.battery_percent);
			break;
		case APP_GETCFG:
			sprintf(json_buffer, "{\"DOMUAVRAINV2\":{\"CMD\":\"GETCFG\",\"ID\":\"%s\",\"IMEI\":\"%s\"}}",
					g_config.device_ID, g_data.device_IMEI);
			break;
		case APP_GETFW:
			sprintf(json_buffer, "{\"DOMUAVRAINV2\":{\"CMD\":\"GETFW\",\"ID\":\"%s\",\"IMEI\":\"%s\"}}",
					g_config.device_ID, g_data.device_IMEI);
			break;
		case APP_WART:
			sprintf(json_buffer, "{\"DOMUAVRAINV2\":{\"CMD\":\"WART\",\"ID\":\"%s\",\"IMEI\":\"%s\",\"VAL\":%.1f}}",
					g_config.device_ID, g_data.device_IMEI, g_data.rain_in_hour);
			break;
		default:
			break;
	}
}

static void APP_create_SMS_msg(APP_API_Command_t cmd)
{
	memset(json_buffer, '\0', strlen(json_buffer));
	char vals[35] = { 0 };

	switch (g_data.element)
	{
		case 0:
			sprintf(vals, "%.1f", g_data.rain_in_period_10[0]);
			break;
		case 1:
			sprintf(vals, "%.1f,%.1f", g_data.rain_in_period_10[0], g_data.rain_in_period_10[1]);
			break;
		case 2:
			sprintf(vals, "%.1f,%.1f,%.1f", g_data.rain_in_period_10[0], g_data.rain_in_period_10[1],
					g_data.rain_in_period_10[2]);
			break;
		case 3:
			sprintf(vals, "%.1f,%.1f,%.1f,%.1f", g_data.rain_in_period_10[0], g_data.rain_in_period_10[1],
					g_data.rain_in_period_10[2], g_data.rain_in_period_10[3]);
			break;
		case 4:
			sprintf(vals, "%.1f,%.1f,%.1f,%.1f,%.1f", g_data.rain_in_period_10[0], g_data.rain_in_period_10[1],
					g_data.rain_in_period_10[2], g_data.rain_in_period_10[3], g_data.rain_in_period_10[4]);
			break;
		case 5:
			sprintf(vals, "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f", g_data.rain_in_period_10[0], g_data.rain_in_period_10[1],
					g_data.rain_in_period_10[2], g_data.rain_in_period_10[3], g_data.rain_in_period_10[4],
					g_data.rain_in_period_10[5]);
			break;
		default:
			break;
	}
	switch (cmd)
	{
		case APP_RESULT:
			if (g_config.device_mode == APP_DOMUA60V2)
			{
				sprintf(json_buffer, "DOMUA KQD %s %.1f", g_data.sms_time_period, g_data.rain_in_period_60);
			}
			else if (g_config.device_mode == APP_DOMUA10V2)
			{
				sprintf(json_buffer, "DOMUA KTTV KQD %s %s", g_data.sms_time_period, vals);
			}
			break;
		case APP_SEND:

			if (g_config.device_mode == APP_DOMUA60V2)
			{
				if (g_time.time.Minutes == START_OF_HOUR)
				{
					sprintf(json_buffer, "DOMUAVRAINV2 KQD %s %.1f &PhoneNumber=%s&Bat=%d&Powr=%d&SIGS=%d",
							g_data.sms_time_period, g_data.rain_in_period_60, g_config.device_ID,
							g_data.battery_percent, g_data.power_state, g_data.cell_signal);
				}
				else
				{
					sprintf(json_buffer, "DOMUAVRAINV2 KQD %s %.1f &PhoneNumber=%s&Bat=%d&Powr=%d&SIGS=%d",
							g_data.sms_time_period, g_data.rain_in_period_60, g_config.device_ID,
							g_data.battery_percent, g_data.power_state, g_data.cell_signal);
				}
			}
			else if (g_config.device_mode == APP_DOMUA10V2)
			{

				sprintf(json_buffer, "DOMUA10V2 KQD %s %s &PhoneNumber=%s&Bat=%d&Powr=%d&SIGS=%d",
						g_data.sms_time_period, vals, g_config.device_ID, g_data.battery_percent, g_data.power_state,
						g_data.cell_signal);
			}
			break;
		case APP_WART:
			sprintf(json_buffer, "DOMUAVRAINV2:CMD:WART,ID:%s,IMEI:%s,VAL:%.2f", g_config.device_ID, g_data.device_IMEI,
					g_data.rain_in_hour);
			break;
		case APP_BATCAP:
			sprintf(json_buffer, "DOMUAVRAINV2:CMD:BATCAP,ID:%s,VAL:%d", g_config.device_ID, g_data.battery_percent);
			break;
		default:
			break;
	}
	printf("%s%s\n", "VR2: ", json_buffer);
}

static uint8_t APP_get_battery_percent(void)
{
	uint16_t adc_value = 0U;
	float voltage = 0.0f;

	HAL_GPIO_WritePin(TP_OUT_GPIO_Port, TP_OUT_Pin, GPIO_PIN_SET);
	HAL_Delay(250);
	uint32_t tickStart = HAL_GetTick();
	while (HAL_OK != HAL_ADC_Start(&hadc))
	{
		if (HAL_GetTick() - tickStart >= 1000)
		{
			break;
		}
	}
	for (int i = 0; i < 5; i++)
	{
		adc_value += HAL_ADC_GetValue(&hadc);
		HAL_Delay(50);
	}
	adc_value /= 5;
	voltage = ((float) adc_value / 4095) * 3.3;
	g_data.battery_percent =
			(uint8_t) ((((voltage * 2) - MIN_BAT_VOLTATE) / (MAX_BAT_VOLTAGE - MIN_BAT_VOLTATE)) * 100);
	HAL_ADC_Stop(&hadc);

	/* Calibrate percent: 0 - 100% */
	if (g_data.battery_percent > 100)
	{
		g_data.battery_percent = 100;
	}
	else if (g_data.battery_percent < 0)
	{
		g_data.battery_percent = 0;
	}
	return g_data.battery_percent;
}

static bool APP_get_power_state(void)
{
	uint8_t state = false;
	char arr[10] = { 0 };
	arr[8] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_STATUS, 1);
	state = (arr[8] >> 2) & 0x01;
	if (state == BQ2429X_STAT2_1)
	{
		g_data.power_state = false;
	}
	else if (state == BQ2429X_STAT2_2)
	{
		g_data.power_state = true;
	}
	return g_data.power_state;
}

static int16_t APP_get_signal(void)
{
	uint8_t RSSI = sim_get_RSSI();

	if (RSSI > 0 && RSSI <= 31)
	{
		g_data.cell_signal = (2 * RSSI) - 113;
	}
	else
	{
		g_data.cell_signal = 0;
	}
	return g_data.cell_signal;
}

static char* APP_get_network_type(void)
{
	sim_get_network(g_data.network_type);
	return g_data.network_type;
}

static char* APP_get_device_IMEI(void)
{
	sim_get_IMEI(g_data.device_IMEI);
	return g_data.device_IMEI;
}

static void getTime(APP_Time_Format_t time_format)
{
	if (time_format & APP_T_RESET)
	{    //strlen("18:45:00 03-07-2022") = 19
		g_time.time.Hours == END_OF_DATE ?
				(g_time.next_hour = START_OF_DATE) : (g_time.next_hour = g_time.time.Hours + 1);
		g_time.last_hour = g_time.time.Hours;
		g_time.last_minute = g_time.time.Minutes;
		g_time.last_second = g_time.time.Seconds;
		sprintf(g_data.device_reset_time, "%02d:%02d:%02d %02d-%02d-20%02d", g_time.time.Hours, g_time.time.Minutes,
				g_time.time.Seconds, g_time.date.Date, g_time.date.Month, g_time.date.Year);
	}

	HAL_RTC_GetDate(&hrtc, &g_time.date, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &g_time.time, RTC_FORMAT_BIN);
	if (time_format & APP_T_PERIOD)
	{    //10:11-10:59 25-04-2022
		if (g_time.time.Minutes == START_OF_HOUR)
		{    //strlen("18:47-19:00 03-07-2022") = 22
			sprintf(g_data.http_time_period, "%02d:%02d-%02d:%02d %02d-%02d-20%02d", g_time.last_hour,
					g_time.last_minute, g_time.time.Hours, g_time.time.Minutes, g_time.date.Date, g_time.date.Month,
					g_time.date.Year);

			sprintf(g_data.sms_time_period,
					"%02d%02d%02d %02d%02d %02d%02d",    //strlen("240422 1500 1525") = 16
					g_time.date.Date, g_time.date.Month, g_time.date.Year, g_time.last_hour, g_time.last_minute,
					g_time.time.Hours, g_time.time.Minutes);
		}
		else
		{    //18:47-18:51 03-07-2022
			sprintf(g_data.http_time_period, "%02d:%02d-%02d:%02d %02d-%02d-20%02d", g_time.time.Hours,
					g_time.last_minute, g_time.time.Hours, g_time.time.Minutes, g_time.date.Date, g_time.date.Month,
					g_time.date.Year);

			sprintf(g_data.sms_time_period, "%02d%02d%02d %02d%02d %02d%02d", g_time.date.Date, g_time.date.Month,
					g_time.date.Year, g_time.time.Hours, g_time.last_minute, g_time.time.Hours, g_time.time.Minutes);
		}
		g_time.last_hour = g_time.time.Hours;
		g_time.last_minute = g_time.time.Minutes;
		g_time.last_second = g_time.time.Seconds;
	}

	if (time_format & APP_T_CURRENT)
	{    //strlen("18:55:00 03-07-2022") = 20
		sprintf(g_time.string_time, "%02d:%02d:%02d %02d-%02d-20%02d", g_time.time.Hours, g_time.time.Minutes,
				g_time.time.Seconds, g_time.date.Date, g_time.date.Month, g_time.date.Year);
	}
}

static void getDeviceData(APP_Data_t data)
{
	if ((data & APP_D_BATT) && (APP_get_power_state() == false))
	{
		APP_get_battery_percent();
	}
	else
	{
		g_data.battery_percent = 100U;    //Update 19/10/2022
	}
	if (data & APP_D_NWTYPE)
	{
		APP_get_network_type();
	}
	if (data & APP_D_SIGNAL)
	{
		APP_get_signal();
	}
	if (data & APP_D_POWER)
	{
		APP_get_power_state();
	}
	if (data & APP_D_IMEI)
	{
		APP_get_device_IMEI();
	}
	if (data & APP_D_TIME_CURRENT)
	{
		getTime(APP_T_CURRENT);
	}
	if (data & APP_D_PERIOD)
	{
		getTime(APP_T_PERIOD);
	}
}

static void APP_create_new_data_file(void)
{
	if (g_time.time.Hours == START_OF_DATE)
	{
		char dir_file_name[16] = { 0 };
		sd_mount("/");

		if ((g_time.date.Date == START_OF_MONTH))
		{
			/* Create the year dir */
			if ((g_time.date.Month == START_OF_YEAR))
			{
				sprintf(dir_file_name, "/20%02d", g_time.date.Year);
				sd_create_new_dir(dir_file_name);
				memset(dir_file_name, '\0', 16);
			}
			/* Create the month dir */
			sprintf(dir_file_name, "/20%02d/%02d", g_time.date.Year, g_time.date.Month);
			sd_create_new_dir(dir_file_name);
			memset(dir_file_name, '\0', 16);
		}
		/* Create the day file and copy filename to current file */
		sprintf(dir_file_name, "/20%02d/%02d/%02d.txt", g_time.date.Year, g_time.date.Month, g_time.date.Date);
		memcpy(g_data.current_data_filename, dir_file_name, 16 - 1);
		sd_create_new_file(dir_file_name);
		sd_unmount("/");
	}
}
