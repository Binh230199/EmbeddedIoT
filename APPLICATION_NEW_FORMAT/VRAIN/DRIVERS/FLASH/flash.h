/*
 * flash.h
 *
 *  Created on: Apr 14, 2022
 *      Author: HOANG VAN BINH
 *      Email: binhhv.23.1.99@gmail.com
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32l1xx_hal.h"

void flash_lock();
void flash_unlock();
void flash_erase(uint32_t page_address);
void flash_write_int(uint32_t address, int32_t int_data);
void flash_write_array(uint32_t address, char *arr_data, uint16_t length);
uint32_t flash_read_int(uint32_t address);
void flash_read_array(uint32_t address, char *arr_data, uint16_t length);

#endif /* INC_FLASH_H_ */
