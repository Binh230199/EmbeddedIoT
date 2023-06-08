/*
 * logger.c
 *
 *  Created on: Jun 14, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "logger.h"

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&DEBUG_UART, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
	return ch;
}
