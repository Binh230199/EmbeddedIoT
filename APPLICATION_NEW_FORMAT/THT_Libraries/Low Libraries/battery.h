/*
 * battery.h
 *
 *  Created on: Apr 2, 2023
 *      Author: Hoàng Văn Bình
 */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "wlv_def.h"
#include "bq2429x.h"
#include "power.h"
/**
 * @brief  BATTERY handle Structure definition
 */
typedef struct
{
	POWER_HandleTypeDef parent;
	BQ24298_HandleTypeDef bq24298;
	ADC_HandleTypeDef *hadc;
	GPIO_TypeDef *batteryEnPort;
	uint16_t batteryEnPin;
} BATTERY_HandleTypeDef;

/* Upper battery threshold */
#define BATTERY_MAX_VOLTAGE		(4.2f)
/* Lower battery threshold */
#define BATTERY_MIN_VOLTAGE		(3.3f)

THT_StatusTypeDef BATTERY_init(BATTERY_HandleTypeDef *const me);
THT_StatusTypeDef BATTERY_getPowerState(BATTERY_HandleTypeDef *const me);
uint8_t BATTERY_getBatteryPercent(BATTERY_HandleTypeDef *const me);
float BATTERY_getBatteryVoltage(BATTERY_HandleTypeDef *const me);

#endif /* INC_BATTERY_H_ */

/************************ (C) COPYRIGHT THTECH *****END OF FILE****/
