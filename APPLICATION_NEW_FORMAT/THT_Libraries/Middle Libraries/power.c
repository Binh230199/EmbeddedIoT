/*
 * power.c
 *
 *  Created on: Feb 14, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "power.h"

#include <stddef.h>

THT_StatusTypeDef POWER_init(POWER_HandleTypeDef *const me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->init)(me);
}

uint8_t POWER_getBatPercent(POWER_HandleTypeDef *me)
{
	if (me == NULL)
	{
		return 0;
	}

	return (*me->getPercent)(me);
}

float POWER_getBatVoltage(POWER_HandleTypeDef *me)
{
	if (me == NULL)
	{
		return 0;
	}

	return (*me->getVoltage)(me);
}

THT_StatusTypeDef POWER_getPowerState(POWER_HandleTypeDef *me)
{
	if (me == NULL)
	{
		return THT_ERROR;
	}

	return (*me->getPowerState)(me);
}
