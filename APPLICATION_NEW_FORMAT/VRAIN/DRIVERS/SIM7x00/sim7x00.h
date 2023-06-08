/*
 * sim7x00.h
 *
 *  Created on: Apr 14, 2022
 *      Author: HOANG VAN BINH
 *      Email: binhhv.23.1.99@gmail.com
 */

#ifndef INC_SIM7X00_H_
#define INC_SIM7X00_H_

/* Include library */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32l1xx_hal.h"
#include "logger.h"
#include "sim7x00_param.h"
#include "config.h"
/* Extern variable */
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
/* Define */
// Change here
#define SIM_UART	huart2
#define USART		USART2
#define RX_LEN		560

#define PHONE_LEN	13
#define REC_TIME_LEN 22
#define SMS_CONTENT_LEN	160
/* TypeDef ENUM */
typedef enum
{
	RET_OK, RET_ERROR, RET_TIMEOUT, RET_WAIT,
} eSTATUS;

typedef enum
{
	AT_NO_OK, AT_OK, AT_NO_WAIT, AT_WAIT,
} eAT;
/**
 * @brief  Command for cell module enumeration
 */
typedef enum
{
	CELL_IDLE, CELL_CALL, CELL_SMS, CELL_SIM_ON, CELL_SIM_OFF, CELL_SIM_OVER_VOLTAGE, CELL_SIM_UNDER_VOLTAGE,
} eCellState;
/* TypeDef STRUCT */
typedef struct
{
	char phone_number[PHONE_LEN];
	char received_time[REC_TIME_LEN];
	char content[SMS_CONTENT_LEN];
} SIM_SMS_Data_t;
void sim_UART_Init(void);
void sim_setCounterTimeout(uint8_t value);
uint8_t sim_getCounterTimeout(void);
eCellState sim_rx_event();
char* sim_get_g_rx_buffer();

void sim_init(GPIO_TypeDef *power_port, uint16_t power_pin);
void sim_sync_local_time(char *time);
eSTATUS sim_power_off();
void sim_hard_reset(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
eSTATUS sim_reset_module();
eSTATUS sim_sleep_mode(eSleep eSleep);
eSTATUS sim_set_phone_function(ePhoneFunction mode);
eSTATUS sim_get_time_zone(char *time);
eSTATUS sim_set_local_time(char *TimeString);
eSTATUS sim_auto_update_time_zone(eUpdateTimeZone mode);
float sim_get_supply_voltage();
int8_t sim_get_temperature();

eSTATUS sim_setImei(char *imei);
eSTATUS sim_get_IMEI(char *imei);
int sim_get_RSSI();
void sim_get_network(char *type);

eSTATUS sim_http_start();
eSTATUS sim_http_stop();
eSTATUS sim_http_set_URL(char const *http_URL);
eSTATUS sim_http_set_parameter(char *content_type);
eSTATUS sim_http_use_method(eHTTPMethod method, uint16_t *error_code, uint32_t *length_of_data);
eSTATUS sim_http_upload_data(uint32_t size, uint16_t time, char *data);
eSTATUS sim_http_save_file(char *filename);
eSTATUS sim_http_read_content(char *pData, uint16_t start_position, uint16_t read_size);

eSTATUS sim_sendSms(char const *phone_number, char *msg);
eSTATUS sim_sms_delete();

eSTATUS sim_call_USSD(char const *ussd, char *response);
eSTATUS sim_call_hangup();

eSTATUS sim_pbook_save_contact(char *phone_number, char *hinted_name);
eSTATUS sim_pbook_read_contact(int order, char *phone_number, char *hinted_name);
void sim_pbook_get_own_number(char *phone_number);

void sim_list_file();
#endif /* INC_SIM7X00_H_ */
