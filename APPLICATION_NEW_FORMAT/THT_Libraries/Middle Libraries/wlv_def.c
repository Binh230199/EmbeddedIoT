/*
 * wlv_def.c
 *
 *  Created on: Apr 1, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "wlv_def.h"

#include <string.h>

/**
 * @brief  Copy substring until meets a desired token
 * @param[in]  src Pointer to source string.
 * @param[out] des Pointer to destination array stored substring.
 * @param[in]  size The size of string wanna to copy
 * 			@Note: if substring length is less than size, it's copy
 * 			substring length bytes, else it's copy size bytes.
 * @retval None
 */
void THT_copyStringUntilToken(const char *src, char des[], uint16_t size, const char *token)
{
	if (src == NULL || des == NULL || size == 0)
	{
		des = NULL;
		return;
	}

	if (token == NULL)
	{
		uint16_t srcSize = strlen(src);
		if (srcSize < size)
		{
			memcpy(des, src, srcSize);
			des[srcSize] = '\0';
		}
		else
		{
			memcpy(des, src, size);
			des[size - 1] = '\0';
		}
	}
	else
	{
		char *sPtr = strstr(src, token);

		if (sPtr != NULL)
		{
			if (sPtr - src < size)
			{
				memcpy(des, src, sPtr - src);
				des[sPtr - src] = '\0';
			}
			else
			{
				memcpy(des, src, size);
				des[size - 1] = '\0';
			}
		}
		else
		{
			des = NULL;
		}
	}
}

THT_StatusTypeDef THT_convertDoubleToHex(double d, uint8_t pBuffer[], size_t sizeMustSetIs8)
{
	if (pBuffer == NULL || sizeMustSetIs8 != sizeof(double))
	{
		return THT_ERROR;
	}

	memcpy(pBuffer, (uint8_t*) &d, sizeof(double));

	return THT_OK;
}

THT_StatusTypeDef THT_convertFloatToHex(double d, uint8_t pBuffer[], size_t sizeMustSetIs4)
{
	if (pBuffer == NULL || sizeMustSetIs4 != sizeof(float))
	{
		return THT_ERROR;
	}

	memcpy(pBuffer, (uint8_t*) &d, sizeof(float));

	return THT_OK;
}

uint8_t THT_Checksum8XOR(const uint8_t *array, size_t size)
{
	uint8_t value = 0;

	for (size_t i = 0; i < size; i++)
	{
		value ^= array[i];
	}
	return value;
}
