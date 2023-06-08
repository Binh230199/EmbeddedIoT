/*
 * bq24298.c
 *
 *  Created on: Apr 19, 2022
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#include "bq24298.h"

uint8_t buffer[2];
char arr[10];
HAL_StatusTypeDef ret;
bool bq2429x_updateBit(void)
{
	if (bq2429x_setInputLimit(BQ2429X_INPUT_SELE) != true)
		return 0;
	if (bq2429x_powerConfig(BQ2429X_OTG_SELE) != true)
		return 0;
	if (bq2429x_chargeCurrentCtrl(BQ2429X_ICHG_SELE) != true)
		return 0;
	if (bq2429x_ipreIterCtrl(BQ2429X_IPRE_ITER_SELE) != true)
		return 0;
	if (bq2429x_chargeVoltCtrl(BQ2429X_VREG_SELE) != true)
		return 0;
	if (bq2429x_chargeTimerCtrl(BQ2429X_WD_SELE) != true)
		return 0;
	if (bq2429x_boostCtrl(BQ2429X_BOOST_TREG_SELE) != true)
		return 0;
	if (bq2429x_miscCtrl(BQ2429X_MISC_OPER_SELE) != true)
		return 0;
	arr[8] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_STATUS, 1);
	arr[9] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_FAULT, 1);
	char regValStr[100];
	sprintf(regValStr, "%x-%x-%x-%x-%x-%x-%x-%x-%x-%x", arr[0], arr[1], arr[2],
			arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9]);
//	debug_println("\n************** Check charging function **************\n");
	printf("%s%s\n", "bq2429: ", regValStr);
	return 1;
}
void bq2429x_status(void)
{
	uint8_t status = 0;
	arr[8] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_STATUS, 1);
//	debug_println(eBQ2429, "\n************** Bq2429x status **************\n");

// Input source status
	status = (arr[8] >> 6) & 0x03;
	if (status == BQ2429X_STAT76_1)
		printf("%sInput source: Unknown\n", "bq2429: ");
	else if (status == BQ2429X_STAT76_2)
		printf("%sInput source: USB host\n", "bq2429: ");
	else if (status == BQ2429X_STAT76_3)
		printf("%sInput source: Adapter port\n", "bq2429: ");
	else if (status == BQ2429X_STAT76_4)
		printf("%sInput source: OTG\n", "bq2429: ");
	// Charging status
	status = (arr[8] >> 4) & 0x03;
	if (status == BQ2429X_STAT54_1)
		printf("%sCharging: Not Charging\n", "bq2429: ");
	else if (status == BQ2429X_STAT54_2)
		printf("%sCharging: Pre-charge\n", "bq2429: ");
	else if (status == BQ2429X_STAT54_3)
		printf("%sCharging: Fast Charging\n", "bq2429: ");
	else if (status == BQ2429X_STAT54_4)
		printf("%sCharging: Charge Termination Done\n", "bq2429: ");
	// Dynamic Power Management
	status = (arr[8] >> 3) & 0x01;
	if (status == BQ2429X_STAT3_1)
		printf("%sDynamic Power Management: Not DPM\n", "bq2429: ");
	else if (status == BQ2429X_STAT3_2)
		printf("%sDynamic Power Management: VINDPM or IINDPM\n", "bq2429: ");
	// PG
	status = (arr[8] >> 2) & 0x01;
	if (status == BQ2429X_STAT2_1)
		printf("%sPG: Not Power Good\n", "bq2429: ");
	else if (status == BQ2429X_STAT2_2)
		printf("%sPG: Power Good\n", "bq2429: ");
	// Thermal
	status = (arr[8] >> 1) & 0x01;
	if (status == BQ2429X_STAT1_1)
		printf("%sThermal: Normal\n", "bq2429: ");
	else if (status == BQ2429X_STAT1_2)
		printf("%sThermal: In Thermal Regulation\n", "bq2429: ");
	// Vsys
	status = arr[8] & 0x01;
	if (status == BQ2429X_STAT0_1)
		printf("%sVsys: Not in VSYSMIN regulation (BAT > VSYSMIN)\n",
				"bq2429: ");
	else if (status == BQ2429X_STAT0_2)
		printf("%sVsys: In VSYSMIN regulation (BAT < VSYSMIN)\n", "bq2429: ");
}
void bq2429x_fault(void)
{
	uint8_t fault = 0;
	arr[9] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_FAULT, 1);
//	debug_println(eBQ2429, "\n************** Bq2429x fault **************\n");
	if (arr[9] == 0x00)
		printf("%sBq2429x fault: Not fault\n", "bq2429: ");
	else
	{
		// Watchdog fault
		fault = (arr[9] >> 7) & 0x01;
		if (fault == BQ2429X_FAULT7)
			printf("%sWatchdog Watchdog: Watchdog timer expiration\n",
					"bq2429: ");
		// OTG fault
		fault = (arr[9] >> 6) & 0x01;
		if (fault == BQ2429X_FAULT6)
			printf(
					"%sOTG fault: VBUS overloaded in OTG, or VBUS OVP, or battery is too low\n",
					"bq2429: ");
		// Charging fault
		fault = (arr[9] >> 4) & 0x03;
		if (fault == BQ2429X_FAULT54_1)
			printf("%sCharging fault: Input fault\n", "bq2429: ");
		else if (fault == BQ2429X_FAULT54_2)
			printf("%sCharging fault: Thermal shutdown\n", "bq2429: ");
		else if (fault == BQ2429X_FAULT54_3)
			printf("%sCharging fault: Charge Timer Expiration\n", "bq2429: ");
		// Battery fault
		fault = (arr[9] >> 3) & 0x01;
		if (fault == BQ2429X_FAULT3)
			printf("%sBattery fault: Battery OVP\n", "bq2429: ");
		// NTC fault
		fault = (arr[9] >> 1) & 0x01;
		if (fault == BQ2429X_FAULT1)
			printf(
					"%sNTC fault: Cold temperature threshold is different based on device operates in buck or boost mode\n",
					"bq2429: ");
		// NTC fault
		fault = arr[9] & 0x01;
		if (fault == BQ2429X_FAULT0)
			printf(
					"%sNTC fault: Hot temperature threshold is different based on device operates in buck or boost mode\n",
					"bq2429: ");
	}
}
bool bq2429x_setInputLimit(uint8_t regVal)
{
	arr[0] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_INPUT_SRC_CTRL, 1);
	if (arr[0] != regVal)
		return bq2429x_write(BQ2429X_I2C_ADDR, BQ2429X_INPUT_SRC_CTRL, regVal,
				2);
	else
		return true;
}
bool bq2429x_powerConfig(uint8_t regVal)
{
	arr[1] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_CONFIG, 1);
	if (arr[1] != regVal)
		return bq2429x_write(BQ2429X_I2C_ADDR, BQ2429X_CONFIG, regVal, 2);
	else
		return true;
}
bool bq2429x_chargeCurrentCtrl(uint8_t regVal)
{
	arr[2] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_ICHG_CTRL, 1);
	if (arr[2] != regVal)
		return bq2429x_write(BQ2429X_I2C_ADDR, BQ2429X_ICHG_CTRL, regVal, 2);
	else
		return true;
}
bool bq2429x_ipreIterCtrl(uint8_t regVal)
{
	arr[3] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_IPRE_ITER_CTRL, 1);
	if (arr[3] != regVal)
		return bq2429x_write(BQ2429X_I2C_ADDR, BQ2429X_IPRE_ITER_CTRL, regVal,
				2);
	else
		return true;
}
bool bq2429x_chargeVoltCtrl(uint8_t regVal)
{
	arr[4] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_VREG_CTRL, 1);
	if (arr[4] != regVal)
		return bq2429x_write(BQ2429X_I2C_ADDR, BQ2429X_VREG_CTRL, regVal, 2);
	else
		return true;
}
bool bq2429x_chargeTimerCtrl(uint8_t regVal)
{
	arr[5] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_WD_CTRL, 1);
	if (arr[5] != regVal)
		return bq2429x_write(BQ2429X_I2C_ADDR, BQ2429X_WD_CTRL, regVal, 2);
	else
		return true;
}
bool bq2429x_boostCtrl(uint8_t regVal)
{
	arr[6] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_BOOST_TREG_CTRL, 1);
	if (arr[6] != regVal)
		return bq2429x_write(BQ2429X_I2C_ADDR, BQ2429X_BOOST_TREG_CTRL, regVal,
				2);
	else
		return true;
}
bool bq2429x_miscCtrl(uint8_t regVal)
{
	arr[7] = bq2429x_read(BQ2429X_I2C_ADDR, BQ2429X_MISC_OPER_CTRL, 1);
	if (arr[7] != regVal)
		return bq2429x_write(BQ2429X_I2C_ADDR, BQ2429X_MISC_OPER_CTRL, regVal,
				2);
	else
		return true;
}
int bq2429x_read(uint8_t devAdd, uint8_t addMemory, uint8_t sizeof_buffer)
{
	buffer[0] = addMemory;
	ret = HAL_I2C_Master_Transmit(&hi2c1, devAdd, buffer, sizeof_buffer,
	I2C_DELA_MAX);
	if (ret == HAL_OK)
		HAL_I2C_Master_Receive(&hi2c1, devAdd, buffer, sizeof_buffer,
		I2C_DELA_MAX);
	return buffer[0];
}
bool bq2429x_write(uint8_t devAdd, uint8_t addMemory, uint8_t data,
		uint8_t sizeof_buffer)
{
	buffer[0] = addMemory;
	buffer[1] = data;
	ret = HAL_I2C_Master_Transmit(&hi2c1, devAdd, buffer, sizeof_buffer,
	I2C_DELA_MAX);
	if (ret == HAL_OK)
	{
		HAL_I2C_Master_Receive(&hi2c1, devAdd, buffer, sizeof_buffer,
		I2C_DELA_MAX);
		return true;
	}
	else
		return false;
}
