/*
 * battery.c
 *
 *  Created on: Apr 2, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "battery.h"

/**
 * @brief BATTERY Initialization Function
 * @param me [in] Baterry handle
 * @retval THT Status
 */
THT_StatusTypeDef BATTERY_init(BATTERY_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	me->hadc->Instance = ADC1;
	me->hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
	me->hadc->Init.Resolution = ADC_RESOLUTION_12B;
	me->hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc->Init.ScanConvMode = ADC_SCAN_DISABLE;
	me->hadc->Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc->Init.LowPowerAutoWait = ADC_AUTOWAIT_UNTIL_DATA_READ;
	me->hadc->Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_IDLE_PHASE;
	me->hadc->Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	me->hadc->Init.ContinuousConvMode = ENABLE;
	me->hadc->Init.NbrOfConversion = 1;
	me->hadc->Init.DiscontinuousConvMode = DISABLE;
	me->hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc->Init.DMAContinuousRequests = DISABLE;
	if (HAL_ADC_Init(me->hadc) != HAL_OK)
	{
		return THT_ERROR;
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_18;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
	if (HAL_ADC_ConfigChannel(me->hadc, &sConfig) != HAL_OK)
	{
		return THT_ERROR;
	}
	uint8_t bqConfig[8] = { 0x37, 0x1B, 0x44, 0x10, 0xB7, 0x8C, 0x73, 0x4B };

	return BQ_init(&me->bq24298, bqConfig, sizeof(bqConfig));
}

/**
 * @brief BATTERY Get power state
 * @param me [in] Baterry handle
 * @retval WLV Status
 */
THT_StatusTypeDef BATTERY_getPowerState(BATTERY_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return BQ_getPowerState(&me->bq24298);
}

/**
 * @brief Get battery in percent format
 * @param me [in] Baterry handle
 * @retval Battery percent
 */
uint8_t BATTERY_getBatteryPercent(BATTERY_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* 1. Calculates battery voltage */
	me->parent.voltage = 0.0f;
	uint16_t adcValue = 0U;
	HAL_GPIO_WritePin(me->batteryEnPort, me->batteryEnPin, GPIO_PIN_SET);
	HAL_ADC_Start(me->hadc);

	adcValue = HAL_ADC_GetValue(me->hadc);

	me->parent.voltage = ((float) ((adcValue * 3.3f) / 4096U) * 5U);

	HAL_ADC_Stop(me->hadc);

	/* 2. Calculates percent from voltage */

	me->parent.percent = (uint8_t) (((me->parent.voltage - BATTERY_MIN_VOLTAGE)
			/ (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) * 100U);
	if (me->parent.percent > 100U)
	{
		me->parent.percent = 100U;
	}
	else if (me->parent.percent < 0U)
	{
		me->parent.percent = 0U;
	}

	HAL_GPIO_WritePin(me->batteryEnPort, me->batteryEnPin, GPIO_PIN_RESET);

	return me->parent.percent;
}

/**
 * @brief Get battery voltage
 * @param me [in] Baterry handle
 * @retval battery voltage (V)
 */
float BATTERY_getBatteryVoltage(BATTERY_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	/* 1. Calculates battery voltage */
	me->parent.voltage = 0.0f;
	uint16_t adcValue = 0U;

	HAL_ADC_Start(me->hadc);

	adcValue = HAL_ADC_GetValue(me->hadc);

	me->parent.voltage = ((float) ((adcValue * 3.3f) / 4096U) * 5U);

	HAL_ADC_Stop(me->hadc);

	return me->parent.voltage;
}
