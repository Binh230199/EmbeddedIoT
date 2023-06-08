/*
 * bq2429x.c
 *
 *  Created on: May 4, 2023
 *      Author: Hoàng Văn Bình
 */

#include "bq2429x.h"
#include <stdio.h>
/**
 * @brief BQ24298 Read single register
 * @param me [in] BQ24298 handle
 * @param address [in] register address
 * @param value [out] pointer to uint8_t that used to store the value of register
 * @retval WLV Status
 */
static THT_StatusTypeDef BQ_readRegister(BQ24298_HandleTypeDef *const me, uint8_t address, uint8_t *value)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	HAL_StatusTypeDef res = HAL_OK;

	res = HAL_I2C_Master_Transmit(me->hi2c, BQ_I2C_ADDRESS, &address, 1, 100);

	if (res == HAL_OK)
	{
		res = HAL_I2C_Master_Receive(me->hi2c, BQ_I2C_ADDRESS, value, 1, 100);

		if (res == HAL_OK)
		{
			return THT_OK;
		}
	}

	return THT_ERROR;
}

/**
 * @brief BQ24298 Read single register
 * @param me [in] BQ24298 handle
 * @param address [in] register address
 * @param data [in] the value want to be set to register
 * @retval WLV Status
 */
static THT_StatusTypeDef BQ_writeRegister(BQ24298_HandleTypeDef *const me, uint8_t address, uint8_t data)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	uint8_t txBuffer[2] = { address, data };

	HAL_StatusTypeDef res = HAL_OK;

	res = HAL_I2C_Master_Transmit(me->hi2c, BQ_I2C_ADDRESS, txBuffer, sizeof(txBuffer), 100);

	if (res == HAL_OK)
	{
		res = HAL_I2C_Master_Receive(me->hi2c, BQ_I2C_ADDRESS, txBuffer, sizeof(txBuffer), 100);

		if (res == HAL_OK)
		{
			return THT_OK;
		}
	}

	return THT_ERROR;
}

/**
 * @brief BQ24298 Initialize module. Write configuration parameters to registers.
 * @param me [in] BQ24298 handle
 * @param pRegister [in] pointer to uint8_t array that stored the configuration value.
 * @param size [in] number of register
 * @retval WLV Status
 */
THT_StatusTypeDef BQ_init(BQ24298_HandleTypeDef *const me, uint8_t *pRegister, size_t size)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	for (size_t i = 0; i < size; i++)
	{
		BQ_writeRegister(me, i, *(pRegister + i));
	}

	uint8_t value = 0;
	for (size_t i = 0; i < size; i++)
	{
		BQ_readRegister(me, i, &value);
		printf("%02x ", value);
	}
	printf("\n");

	return THT_OK;
}

/**
 * @brief BQ24298 Read status of the module.
 * @param me [in] BQ24298 handle
 * @retval WLV Status
 */
THT_StatusTypeDef BQ_readStatus(BQ24298_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return BQ_readRegister(me, REG_SYSTEMSTATUS, &me->status);
}

/**
 * @brief BQ24298 Read fault of the module.
 * @param me [in] BQ24298 handle
 * @retval WLV Status
 */
THT_StatusTypeDef BQ_readFault(BQ24298_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return BQ_readRegister(me, REG_NEWFAULT, &me->status);
}

/**
 * @brief BQ24298 Read the power state.
 * @param me [in] BQ24298 handle
 * @retval WLV Status Return THT_ERROR if no power input, return THT_OK if power is plugin
 */
THT_StatusTypeDef BQ_getPowerState(BQ24298_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	if (THT_OK == BQ_readStatus(me))
	{
		if ((me->status & BQ_SYSTEMSTATUS_PG_Msk) == BQ_SYSTEMSTATUS_PG_GOOD)
		{
			return THT_OK;
		}
	}

	return THT_ERROR;
}
