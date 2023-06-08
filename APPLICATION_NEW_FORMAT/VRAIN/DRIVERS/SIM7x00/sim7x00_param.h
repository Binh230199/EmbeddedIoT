/*
 * sim_param.h
 *
 *  Created on: May 29, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef DRIVERS_SIM7X00_SIM7X00_PARAM_H_
#define DRIVERS_SIM7X00_SIM7X00_PARAM_H_

typedef enum
{
	SIM_SLEEP_DISABLE, SIM_SLEEP_ENABLE,
} eSleep;

typedef enum
{
	MINI_MODE, FULL_MODE, FLIGHT_MODE = 4, TEST_MODE, RESET_MODE, OFFLINE_MODE,
} ePhoneFunction;

typedef enum
{
	TZ_DISABLE, TZ_ENABLE,
} eUpdateTimeZone;

typedef enum
{
	GET, POST, HEAD, DELETE,
} eHTTPMethod;

#endif /* DRIVERS_SIM7X00_SIM7X00_PARAM_H_ */
