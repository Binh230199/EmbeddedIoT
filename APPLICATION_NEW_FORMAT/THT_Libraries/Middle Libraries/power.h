/*
 * power.h
 *
 *  Created on: Feb 14, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef MIDDLEWARE_LIB_POWER_H_
#define MIDDLEWARE_LIB_POWER_H_

#include <stdint.h>
#include <stdbool.h>

#include "wlv_def.h"

typedef struct power
{
	volatile uint32_t SR;
	float voltage;
	uint8_t percent;
	THT_StatusTypeDef (*init)(struct power *const me);
	uint8_t (*getPercent)(struct power *const me);
	float (*getVoltage)(struct power *const me);
	THT_StatusTypeDef (*getPowerState)(struct power *const me);
} POWER_HandleTypeDef;

typedef THT_StatusTypeDef (*powerInit)(struct power *const me);
typedef uint8_t (*powerGetPercent)(struct power *const me);
typedef float (*powerGetVoltage)(struct power *const me);
typedef THT_StatusTypeDef (*powerGetPowerState)(struct power *const me);

#define POWER_SR_SOURCE_Pos			(0U)
#define POWER_SR_SOURCE_Msk			(0x1UL << POWER_SR_SOURCE_Pos)
#define POWER_SR_SOURCE				POWER_SR_SOURCE_Msk
#define POWER_SR_WARNEN_Pos			(1U)
#define POWER_SR_WARNEN_Msk			(0x1UL << POWER_SR_WARNEN_Pos)
#define POWER_SR_WARNEN				POWER_SR_WARNEN_Msk
#define POWER_SR_LOWBAT_Pos			(2U)
#define POWER_SR_LOWBAT_Msk			(0x1UL << POWER_SR_LOWBAT_Pos)
#define POWER_SR_LOWBAT				POWER_SR_LOWBAT_Msk

/**
 *		POWER flag definitions
 */
#define POWER_FLAG_SOURCE			POWER_SR_SOURCE
#define POWER_FLAG_WARNEN			POWER_SR_WARNEN
#define POWER_FLAG_LOWBAT			POWER_SR_LOWBAT

#define POWER_GET_FLAG(__ME__, __FLAG__)		\
			((((__ME__)->SR) & (__FLAG__)) == (__FLAG__))

#define POWER_SET_FLAG(__ME__, __FLAG__)		\
			(((__ME__)->SR) |= (__FLAG__))

#define POWER_CLR_FLAG(__ME__, __FLAG__)		\
			(((__ME__)->SR) &= ~(__FLAG__))

#define POWER_IS_LOW_BATTERY(__ME__)		((__ME__)->percent < 50u)

#define POWER_IS_ENABLE_WARNING(__ME__)		((__ME__)->percent > 70u)

THT_StatusTypeDef POWER_init(POWER_HandleTypeDef *const me);
uint8_t POWER_getBatPercent(POWER_HandleTypeDef *const me);
float POWER_getBatVoltage(POWER_HandleTypeDef *me);
THT_StatusTypeDef POWER_getPowerState(POWER_HandleTypeDef *const me);

#endif /* MIDDLEWARE_LIB_POWER_H_ */
