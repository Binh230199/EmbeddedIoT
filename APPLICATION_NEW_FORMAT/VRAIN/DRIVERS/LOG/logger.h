/*
 * logger.h
 *
 *  Created on: Jun 14, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef DRIVERS_LOG_LOGGER_H_
#define DRIVERS_LOG_LOGGER_H_

#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include "stm32l1xx_hal.h"

#define DEBUG_UART	huart3

extern UART_HandleTypeDef huart3;

#endif /* DRIVERS_LOG_LOGGER_H_ */
