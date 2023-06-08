/*
 * sim7x00.c
 *
 *  Created on: Apr 14, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "sim7x00.h"
#include <stdbool.h>
#include "main.h"
uint8_t rx_flag = 0U;
uint8_t is_OK = 0U;
uint8_t g_rx_buffer[RX_LEN] = { 0 };
uint8_t counterTimeout = 0U;

void sim_UART_Init(void)
{
	/* Cho phép VĐK giao tiếp UART bằng DMA IDLE */
	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, g_rx_buffer, RX_LEN);
	((&hdma_usart2_rx)->Instance->CCR &= ~((0x1UL << (2U))));    //	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

uint8_t sim_getCounterTimeout(void)
{
	return counterTimeout;
}

void sim_setCounterTimeout(uint8_t value)
{
	counterTimeout = value;
}

/**
 * @brief Send AT command to module 4G
 * @param p_command: command must be included "\r\n"
 * @param response: desired response
 * @param timeout: timeout to exit if not received response data
 * @param is_OK: has OK in response data?
 * @param is_wait: need time to wait all data transfer to MCU
 * @retval Result of function
 */

static eSTATUS send_AT_cmd(char *p_command, char *p_response, uint32_t timeout, eAT is_wait)
{
	sim_UART_Init();
	eSTATUS e_answer = RET_WAIT;
	memset(g_rx_buffer, '\0', RX_LEN);
	printf(p_command);
	printf("\n");
	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, g_rx_buffer, RX_LEN);
	((&hdma_usart2_rx)->Instance->CCR &= ~((0x1UL << (2U))));
	char command[300] = { 0 };
	sprintf(command, "%s\r\n", p_command);
	HAL_UART_Transmit(&SIM_UART, (uint8_t*) command, strlen(command),
	HAL_MAX_DELAY);

	if (is_wait == AT_WAIT)
	{
		HAL_Delay(650);
	}

	uint32_t tick_start = HAL_GetTick();

	while (e_answer != RET_OK)
	{
		if (HAL_GetTick() - tick_start >= timeout)
		{
			e_answer = RET_TIMEOUT;
			counterTimeout++;
			printf(">> time out\n");
			break;
		}
		if (rx_flag == true)
		{
			if (strstr((char*) g_rx_buffer, p_response) != NULL)
			{
				e_answer = RET_OK;
				break;
			}
			else if (strstr((char*) g_rx_buffer, "ERROR") != NULL)
			{
				e_answer = RET_ERROR;
				break;
			}
		}
	}
	rx_flag = false;
	printf((char*) g_rx_buffer);
	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, g_rx_buffer, RX_LEN);
	return e_answer;
}
/**
 * @brief Receive and check OK in p_response data
 * @retval Result of function
 */
eCellState sim_rx_event(void)
{
	rx_flag = true;
	eCellState e_cellular_state = CELL_IDLE;

	/* Check event of module SIM */
	if (strstr((char*) g_rx_buffer, "+CMT: \"") != NULL)
	{
		printf("Incoming Msg");
		e_cellular_state = CELL_SMS;
	}
	else if (strstr((char*) g_rx_buffer, "+SIMCARD: NOT AVAILABLE") != NULL)
	{
		printf("Sim is removed");
		e_cellular_state = CELL_SIM_OFF;
	}
	else if (strstr((char*) g_rx_buffer, "+CPIN: READY") != NULL)
	{
		printf("Sim is plugin");
		e_cellular_state = CELL_SIM_ON;
	}
	printf("\n");
	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, g_rx_buffer, RX_LEN);
	return e_cellular_state;
}
/**
 * @brief Return received data in g_rx_buffer
 * @retval pointer to g_rx_buffer
 */
char* sim_get_g_rx_buffer(void)
{
	return (char*) g_rx_buffer;
}
/**
 * @brief Initialize 4G module and set up some parameters
 * @param power_port: port of power pin
 * @param power_pin: power pin
 * @retval Result of function
 */
void sim_init(GPIO_TypeDef *power_port, uint16_t power_pin)
{
	/* Bật nguồn bằng phần mềm */
	power_port->BSRR = (uint32_t) power_pin;    //	HAL_GPIO_WritePin(power_port, power_pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	power_port->BSRR = (uint32_t) power_pin << 16;    //HAL_GPIO_WritePin(power_port, power_pin, GPIO_PIN_RESET);
	HAL_Delay(17000);
	/* Cho phép VĐK giao tiếp UART bằng DMA IDLE */
	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, g_rx_buffer, RX_LEN);
	((&hdma_usart2_rx)->Instance->CCR &= ~((0x1UL << (2U))));    //	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
	/* Kiểm tra kết nối của module SIM bằng cách gửi lện AT */
	uint8_t count = 0;
	do
	{
		printf("Connecting\n");
		HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, g_rx_buffer, RX_LEN);
		count++;
	}
	while (send_AT_cmd("AT", "OK", 1000, AT_NO_WAIT) != RET_OK && count < 30);

	send_AT_cmd("ATE0", "OK", 9000, AT_WAIT);
	send_AT_cmd("ATI", "OK", 5000, AT_NO_WAIT);
	send_AT_cmd("AT+CPIN?", "READY", 5000, AT_NO_WAIT);
	/* Kiểm tra thẻ SIM đã được gắn hay chưa thì mới thực hiện những lệnh AT này */

	if (RET_OK == send_AT_cmd("AT+CPIN?", "READY", 5000, AT_NO_WAIT))
	{
		send_AT_cmd("AT+CREG?", "+CREG: 0,1", 5000, AT_NO_WAIT);
		send_AT_cmd("AT+CGREG?", "+CGREG: 0,1", 5000, AT_NO_WAIT);
		send_AT_cmd("AT+CTZU=1", "OK", 5000, AT_NO_WAIT);
		send_AT_cmd("AT+CNMP=39", "OK", 5000, AT_NO_WAIT);    //cũ là 2: auto, 39: GSM/WCDMA/LTE
		send_AT_cmd("AT+CPMS=\"SM\",\"SM\",\"SM\"", "OK", 5000, AT_NO_WAIT);
		send_AT_cmd("AT+CMGL=\"ALL\"", "OK", 15000, AT_NO_WAIT);
		send_AT_cmd("AT+CMGD=,4", "OK", 5000, AT_NO_WAIT);
		send_AT_cmd("AT+CMGF=1", "OK", 5000, AT_NO_WAIT);
		send_AT_cmd("AT+CNMI=1,2,0,0,0", "OK", 5000, AT_NO_WAIT);
	}
	send_AT_cmd("AT+FSCD=F:", "OK", 3000, AT_NO_WAIT);
	send_AT_cmd("AT+FSLS=2", "OK", 5000, AT_NO_WAIT);
}

void sim_hard_reset(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	/* Call UART to ready received data if module reset completely */
	HAL_UARTEx_ReceiveToIdle_DMA(&SIM_UART, g_rx_buffer, RX_LEN);
	((&hdma_usart2_rx)->Instance->CCR &= ~((0x1UL << (2U))));    //	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
	/* Start hard reset module */
	GPIOx->BSRR = (uint32_t) GPIO_Pin;    //HAL_GPIO_WritePin(RESET_Port, RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	GPIOx->BSRR = (uint32_t) GPIO_Pin << 16;    //	HAL_GPIO_WritePin(RESET_Port, RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);
}

/* Set local time by connect NTP server */
void sim_sync_local_time(char *time)
{
	send_AT_cmd("AT+CREG?", "+CREG: 0,1", 5000, AT_NO_WAIT);
	send_AT_cmd("AT+CPSI?", "OK", 5000, AT_NO_WAIT);

	uint8_t count = 0;
	do
	{
		switch (count)
		{
			case 0:
				send_AT_cmd("AT+CNTP=\"time1.google.com\",28", "OK", 5000, AT_NO_WAIT);
				break;
			case 1:
				send_AT_cmd("AT+CNTP=\"0.vn.pool.ntp.org\",28", "OK", 5000, AT_NO_WAIT);
				break;
			case 2:
				send_AT_cmd("AT+CNTP=\"0.asia.pool.ntp.org\",28", "OK", 5000, AT_NO_WAIT);
				break;
			default:
				break;
		}
		send_AT_cmd("AT+CNTP", "+CNTP: 0", 9000, AT_WAIT);
		count++;
	}
	while ((RET_OK != sim_get_time_zone(time)) && count < 3);
}
/**
 * @brief Read the voltage value of the power supply
 * @retval value of the power supply in float
 */
//float sim_get_supply_voltage() {    //+CBC: 3.812V
//	if (RET_OK == send_AT_cmd("AT+CBC", "OK", 3000, AT_NO_WAIT)) {
//		char *token = NULL;
//		token = strtok((char*) g_rx_buffer, " ");
//		token = strtok(NULL, "V");
//		return atof(token);
//	} else {
//		return 3.4001f;
//	}
//}
///**
// * @brief Read the temperature of the module
// * @retval temperature in int type
// */
//int8_t sim_get_temperature() {    //+CPMUTEMP: 24
//	send_AT_cmd("AT+CPMUTEMP", "OK", 3000, AT_NO_WAIT);
//	char *token = NULL;
//	token = strtok((char*) g_rx_buffer, " ");
//	token = strtok(NULL, "\r");
//	return atoi(token);
//}
/**
 * @brief Turn off power and module will off
 * @retval Result of function
 * @response:
 [2022-01-09_18:50:32:185]AT+CPOF

 [2022-01-09_18:50:32:185]OK
 [2022-01-09_18:50:58:991]
 [2022-01-09_18:50:58:991]RDY

 [2022-01-09_18:50:58:991]+CPIN: READY

 [2022-01-09_18:50:58:991]SMS DONE

 [2022-01-09_18:50:58:991]PB DONE
 */
eSTATUS sim_power_off(void)
{
	return send_AT_cmd("AT+CPOF", "OK", 2000, AT_NO_WAIT);
}
/**
 * @brief Reset module
 * @retval Result of function
 * @response
 "[2022-01-09_18:55:12:283]AT+CRESET

 [2022-01-09_18:55:12:283]OK
 [2022-01-09_18:55:36:867]
 [2022-01-09_18:55:36:867]RDY

 [2022-01-09_18:55:36:867]+CPIN: READY

 [2022-01-09_18:55:36:867]SMS DONE

 [2022-01-09_18:55:36:867]PB DONE"
 */
eSTATUS sim_reset_module(void)
{
	return send_AT_cmd("AT+CRESET", "OK", 2000, AT_NO_WAIT);
}
/**
 * @brief Module enter sleep mode
 * @retval Result of function
 */
eSTATUS sim_sleep_mode(eSleep eSleep)
{
	char buffer[13] = { 0 };
	sprintf(buffer, "AT+CSCLK=%d", eSleep);
	return send_AT_cmd(buffer, "OK", 2000, AT_NO_WAIT);
}

/*
 0 – minimum functionality
 1 – full functionality, online mode
 4 – disable phone both transmit and receive RF circuits
 5 – Factory Test Mode
 6 – Reset
 7 – Offline Mode
 */
eSTATUS sim_set_phone_function(ePhoneFunction mode)
{
	char buf[13] = { 0 };
	sprintf(buf, "AT+CFUN=%d", mode);
	return send_AT_cmd(buf, "OK", 5000, AT_WAIT);
}
eSTATUS sim_get_time_zone(char *time)
{    //\r\n+CCLK: "22/04/23,09:33:59+28"\r\n\r\nOK\r\n
	memset(time, '\0', strlen(time));
	eSTATUS stt;
	stt = send_AT_cmd("AT+CCLK?", "+28", 5000, AT_NO_WAIT);
	if (RET_OK == stt)
	{
		if (strstr((char*) g_rx_buffer, "80/0") == NULL)
		{
			memcpy(time, (char*) g_rx_buffer + 10, 17);
			return RET_OK;
		}
		else
		{
			return RET_ERROR;
		}
	}
	else
	{
		return stt;
	}
}
eSTATUS sim_set_local_time(char *TimeString)
{

	char command[35] = { 0 };    //AT+CCLK="08/11/26,10:15:00"
	sprintf(command, "AT+CCLK=\"%s\"", TimeString);
	return send_AT_cmd(command, "OK", 9000, AT_NO_WAIT);
}

eSTATUS sim_get_IMEI(char *IMEI)
{
	eSTATUS stt = RET_ERROR;
	uint8_t count = 0;

	do
	{
		memset(IMEI, '\0', strlen(IMEI));
		stt = send_AT_cmd("AT+SIMEI?", "OK", 3000, AT_NO_WAIT);
		if (RET_OK == stt)
		{
			char *token = NULL;
			memset(IMEI, '\0', strlen(IMEI));
			token = strtok((char*) g_rx_buffer, " ");
			token = strtok(NULL, "\r");
			memcpy(IMEI, token, 15);
		}
		count++;
	}
	while (strlen(IMEI) == 0 && count < 3);

	if (strlen(IMEI) == 0)
	{
		memcpy(IMEI, "000000000000000", 15);
	}
	return stt;
}

int sim_get_RSSI(void)
{
	int RSSI = 0;

	if (RET_OK == send_AT_cmd("AT+CSQ", "OK", 2000, AT_NO_WAIT))
	{
		char *token = NULL;
		token = strtok((char*) g_rx_buffer, ": ");
		token = strtok(NULL, ",");
		RSSI = atoi(token);
	}
	else
	{
		RSSI = 0;
	}
	return RSSI;
}

void sim_get_network(char *type)
{
	memset(type, '\0', strlen(type));
	send_AT_cmd("AT+CPSI?", "OK", 3000, AT_NO_WAIT);
	char *token = NULL;
	if (strstr((char*) g_rx_buffer, "LTE") || strstr((char*) g_rx_buffer, "WCDMA") || strstr((char*) g_rx_buffer, "GSM")
			|| strstr((char*) g_rx_buffer, "NO SERVICE"))
	{
		token = strtok((char*) g_rx_buffer, " ");
		token = strtok(NULL, ",");
		sprintf(type, "%s", token);
	}
	else
	{
		memcpy(type, "UNKNOWED\0\0", 10);
	}
}

eSTATUS sim_http_start(void)
{
	return send_AT_cmd("AT+HTTPINIT", "OK", 30000, AT_NO_WAIT);    //2000 -> 120000: GSM delay lâu hơn 2000ms, nên tăng thời gian timeout để chờ phản hồi
}
eSTATUS sim_http_stop(void)
{
	return send_AT_cmd("AT+HTTPTERM", "OK", 30000, AT_NO_WAIT);
}
eSTATUS sim_http_set_URL(char const *http_URL)
{
	char buffer[250] = { 0 };
	sprintf(buffer, "AT+HTTPPARA=\"URL\",\"%s\"", http_URL);
	return send_AT_cmd(buffer, "OK", 5000, AT_NO_WAIT);
}
eSTATUS sim_http_set_parameter(char *content_type)
{
	char buffer[99] = { 0 };
	sprintf(buffer, "AT+HTTPPARA=\"CONTENT\",\"%s\"", content_type);
	return send_AT_cmd(buffer, "OK", 5000, AT_NO_WAIT);
}
eSTATUS sim_http_use_method(eHTTPMethod method, uint16_t *error_code, uint32_t *length_of_data)
{
	char *token;
	char buffer[25] = { 0 };
	sprintf(buffer, "AT+HTTPACTION=%d", method);
	eSTATUS stt = send_AT_cmd(buffer, "+HTTPACTION:", 30000, AT_WAIT);
	if (RET_OK == stt)
	{
		token = strtok((char*) g_rx_buffer, ":");
		token = strtok(NULL, ",");
		token = strtok(NULL, ",");
		*error_code = atoi(token);
		token = strtok(NULL, ")");
		*length_of_data = atoi(token);
		if (*error_code == 200)
		{
			return RET_OK;
		}
	}
	return stt;
}

eSTATUS sim_http_upload_data(uint32_t size, uint16_t time, char *data)
{
	char buffer[250] = { 0 };
	sprintf(buffer, "AT+HTTPDATA=%ld,%d", size, time);
	eSTATUS stt = send_AT_cmd(buffer, "DOWNLOAD", 5000, AT_NO_WAIT);
	if (RET_OK == stt)
	{
		if (RET_OK == send_AT_cmd(data, "OK", 5000, AT_NO_WAIT))
		{
			return RET_OK;
		}
	}
	return stt;
}

eSTATUS sim_http_save_file(char *filename)
{
	char buffer[60] = { 0 };
	sprintf(buffer, "AT+HTTPREADFILE=\"%s\"", filename);
	return send_AT_cmd(buffer, "+HTTPREADFILE: 0", 30000, AT_NO_WAIT);
}

eSTATUS sim_http_read_content(char *pData, uint16_t start_position, uint16_t read_size)
{
	char buffer[35] = { 0 };
	sprintf(buffer, "AT+HTTPREAD=%d,%d", start_position, read_size);
	eSTATUS stt = send_AT_cmd(buffer, "+HTTPREAD:", 30000, AT_WAIT);    // sim anh thương 7600H phản hồi +HTTPREAD:0 chứ không phải +HTTPREAD: 0
//	[2022-05-05_14:46:18:967]+HTTPREAD: 0
//	[2022-05-05_14:46:18:967]D: DATA,142
//	[2022-05-05_14:46:18:967]{"FW":"https://core-asset.s3-ap-southeast-1.amazonaws.com/vrain2_60/firmware/8ec7fca9-20b8-4b13-a93a-e66b3e35e98f/vraincontroller_0.1.1.bin"}
			//sim trên datalogger 7600E trả về +HTTPREAD: 0
//	[2022-05-05_15:06:00:983]+HTTPREAD: 0
//	[2022-05-05_15:06:00:983]A,142
//	[2022-05-05_15:06:00:983]{"FW":"https://core-asset.s3-ap-southeast-1.amazonaws.com/vrain2_60/firmware/8ec7fca9-20b8-4b13-a93a-e66b3e35e98f/vraincontroller_0.1.1.bin"}
	if (RET_OK == stt)
	{
//		\r\n+HTTPREAD: 0\r\nD: DATA,256\r\n
		char *token = NULL;
		token = strtok((char*) g_rx_buffer, "+");
		token = strtok(NULL, "\n");
		token = strtok(NULL, "\n");
		token = strtok(NULL, "\n");
		memcpy(pData, token, read_size);
		token = NULL;
	}
	return stt;
}

eSTATUS sim_sendSms(char const *phone_number, char *msg)
{

	char buffer[200] = { 0 };
	eSTATUS stt = send_AT_cmd("AT+CMGF=1", "OK", 5000, AT_NO_WAIT);

	uint16_t sms_total_size = strlen(msg);

	if (sms_total_size < 160)
	{
		sprintf(buffer, "AT+CMGS=\"%s\"", phone_number);
		stt = send_AT_cmd(buffer, ">", 15000, AT_NO_WAIT);
		if (RET_OK == stt)
		{
			memset(buffer, '\0', 200);
			printf("%s%s\n", "Sim7x00: ", msg);
			sprintf(buffer, "%s%c", msg, 0x1A);
			stt = send_AT_cmd(buffer, "OK", 15000, AT_WAIT);
			if (stt != RET_OK)
			{
				printf("%d\tCan't send\n", __LINE__);
			}
			else
			{
				printf("%d\tSent\n", __LINE__);
			}
		}
	}
	else
	{
		uint8_t sms_segment_size = 0;
		uint8_t total_message = (sms_total_size / 146) + 1;    //146 là độ dài tối đa của 1 mảng tin nhắn

		for (size_t message_segment = 1; message_segment <= total_message; message_segment++)
		{
			if (sms_total_size < 146)
			{
				sms_segment_size = sms_total_size;
			}
			else
			{
				sms_segment_size = 146;
			}

			sprintf(buffer, "AT+CMGSEX=\"%s\",190,%d,%d\r", phone_number, message_segment, total_message);
			stt = send_AT_cmd(buffer, ">", 15000, AT_NO_WAIT);
			if (stt == RET_OK)
			{
				memset(buffer, '\0', 200);
				memcpy(buffer, msg + (message_segment - 1) * 146, sms_segment_size);
				buffer[sms_segment_size] = 0x1A;
				stt = send_AT_cmd(buffer, "OK", 15000, AT_WAIT);
				sms_total_size -= sms_segment_size;
			}
		}
	}

	return stt;
}
eSTATUS sim_sms_delete(void)
{
	return send_AT_cmd("AT+CMGD=,4", "OK", 5000, AT_NO_WAIT);
}

eSTATUS sim_call_USSD(char const *ussd, char *response)
{
	eSTATUS stt;
	char buffer[50] = { 0 };
	sprintf(buffer, "AT+CUSD=1,\"%s\",15", ussd);
	stt = send_AT_cmd(buffer, ",15\r\n", 15000, AT_NO_WAIT);
	if (RET_OK == stt)
	{    //+CUSD: 1,"Moi 01699741245 chon:
		memset(response, '\0', strlen(response));
		char *token = NULL;
		token = strtok((char*) g_rx_buffer, ",");
		token = strtok(NULL, "");
		strncpy(response, token, (strlen(token) - 5));    //5 ký tự cuối không quan trọng ...\r\n
	}
	sprintf(buffer, "AT+CUSD=2,\"\",15");
	send_AT_cmd(buffer, "OK", 10000, AT_NO_WAIT);
	return stt;
}
eSTATUS sim_call_hangup(void)
{
	return send_AT_cmd("AT+CHUP", "", 5000, AT_NO_WAIT);
}

eSTATUS sim_pbook_save_contact(char *phone_number, char *hinted_name)
{    //ghi đè lên số cũ
	if ((strlen(phone_number) <= 14) && (strlen(hinted_name) < 15))
	{
		char savePhoneBook[50] = { 0 };
		send_AT_cmd("AT+CPBS=\"ME\"", "OK", 2000, AT_NO_WAIT);
		sprintf(savePhoneBook, "AT+CPBW=1,\"%s\",129,\"%s\"", phone_number, hinted_name);    //ghi số mới vào vị trí 1
		return send_AT_cmd(savePhoneBook, "OK", 2000, AT_NO_WAIT);    //OK\r\n
	}
	else
	{
		return RET_ERROR;
	}
}
eSTATUS sim_pbook_read_contact(int order, char *phone_number, char *hinted_name)
{
	eSTATUS stt;
	char buffer[20] = { 0 };
	char *token = NULL;
	send_AT_cmd("AT+CPBS=\"ME\"", "OK", 1000, AT_NO_WAIT);    //AT+CPBS="ME"\r\nOK\r\n
	sprintf(buffer, "AT+CPBR=%d", order);
	stt = send_AT_cmd(buffer, "not found", 1000, AT_NO_WAIT);    //\r\n+CPBR: 1,"0969647577",129,""\r\n\r\nOK\r\n
	if (RET_OK == stt)
	{    //\r\nAT+CPBR=1\r\n+CME ERROR: not found\r\n
		memcpy(hinted_name, "not found", 9);
		memcpy(phone_number, "not found", 9);
	}
	else    //\r\n+CPBR: 1,"0969647577",129,""\r\n\r\nOK\r\n
	{    //or  \r\n+CPBR: 1,"+84969647577",129,""\r\n\r\nOK\r\n
		memset(phone_number, '\0', strlen(phone_number));
		memset(hinted_name, '\0', strlen(hinted_name));
		token = strtok((char*) g_rx_buffer, "\"");    //\r\n+CPBR: 1,
		token = strtok(NULL, "\"");    //+84969647577
		memcpy(phone_number, token, 13);    //+84969647577\0
		sprintf(hinted_name, "%s", "");
	}
	return stt;
}

void sim_pbook_get_own_number(char *phone_number)
{
	/*
	 [2022-10-06_15:15:28:241]AT+COPS=?

	 [2022-10-06_15:16:43:579]+COPS: (2,"Viettel","Viettel","45204",7),(1,"VIETTEL","VIETTEL","45204",2),(1,"VIETTEL","VIETTEL","45204",0),(1,"VN VINAPHONE","GPC","45202",7),(1,"Vietnamobile","VNMOBILE","45205",7),(1,"VN VINAPHONE","GPC","45202",0),(1,"VN Mobifone","Mobifone","45201",7),(1,"VN VINAPHONE","GPC","45202",2),(1,"Vietnamobile","VNMOBILE","45205",2),(1,"VN Mobifone","Mobifone","45201",2),,(0,1,2,3,4,5),(0,1,2)
	 */
	uint8_t count = 0;
	memset(phone_number, '\0', strlen(phone_number));
	char json_buffer[210] = { 0 };
	do
	{

		if (RET_OK == send_AT_cmd("AT+COPS?", "OK", 5000, AT_NO_WAIT))
		{
			char *token = NULL;
			if (strstr((char*) g_rx_buffer, "Viettel") != NULL || strstr((char*) g_rx_buffer, "VIETTEL") != NULL)
			{
				if (RET_OK == sim_call_USSD("*098#", json_buffer))
				{    //"Moi 01699741245 chon:

					token = strtok(json_buffer, " ");
					token = strtok(NULL, " ");    //01699741245
					sprintf(phone_number, "84%s", token + 1);

				}
			}
			else if (strstr((char*) g_rx_buffer, "Vina") != NULL || strstr((char*) g_rx_buffer, "VINA") != NULL)
			{
				if (RET_OK == sim_call_USSD("*110#", json_buffer))
				{    //"TB (VINACARD):914486786
					token = strtok(json_buffer, ":");
					token = strtok(NULL, "\r");    //01699741245
					if (strlen(token) < 12)
					{
						sprintf(phone_number, "84%s", token);
					}
				}
			}
			else if (strstr((char*) g_rx_buffer, "Mobi") != NULL || strstr((char*) g_rx_buffer, "Mobifone") != NULL)
			{
				if (RET_OK == sim_call_USSD("*0#", json_buffer))
				{    //"84914486786"
					if (strstr(json_buffer, "84") != NULL)
					{
						memcpy(phone_number, json_buffer + 1, strlen(json_buffer) - 2);
					}
				}
			}
			else
			{
				send_AT_cmd("AT+CNUM", "OK", 1000, AT_NO_WAIT);    //+CNUM: "","+84386599045",145
				if (strstr((char*) g_rx_buffer, "+84") != NULL)
				{
					token = strtok((char*) g_rx_buffer, ",");
					token = strtok(NULL, "\"");
					memcpy(phone_number, token + 1, 11);
				}
			}
		}
		count++;
		HAL_Delay(1000);
	}
	while (strlen(phone_number) < 11 && count < 3);

}
