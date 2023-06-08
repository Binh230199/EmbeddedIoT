/*
 * bq24298.h
 *
 *  Created on: Apr 14, 2022
 *      Author: HOANG VAN BINH
 *      Email: binhhv.23.1.99@gmail.com
 */

#ifndef INC_BQ24298_H_
#define INC_BQ24298_H_

#include "string.h"
#include "stdbool.h"
#include "stm32l1xx_hal.h"
#include "logger.h"
/* Register 00h */
#define BQ2429X_INPUT_SRC_CTRL			0x00
#define BQ2429X_INPUT_SELE		  		0x37  	// 4.36V

/* Register 01h */
#define BQ2429X_CONFIG					0x01
#define	BQ2429X_OTG_SELE				0x1B		// OTG disable, Charge Battery, SYS_MIN = 3.5V, BOOST_LIM = 1.5A

/* Register 02h */
#define BQ2429X_ICHG_CTRL				0x02
#define BQ2429X_ICHG_SELE				0x20 //44		// Cũ 0x60 ICHG = 2048mA, mới 0x20 ICHG = 1024mA

/* Register 03h */
#define BQ2429X_IPRE_ITER_CTRL			0x03
#define BQ2429X_IPRE_ITER_SELE			0x10	// IPRECH: 128mA

/* Register 04h */
#define BQ2429X_VREG_CTRL				0x04
#define BQ2429X_VREG_SELE				0xBB //B7		// Cu: Default: 3.872V Moi 4.06 9A, Mới nhất A2 4.249V

/* Register 05h */
#define BQ2429X_WD_CTRL					0x05
#define BQ2429X_WD_SELE					0x8C		// Cũ 0x8C Enable termination, Disabled for I2C Watchdog Timer Limit, Fast Charge Timer 12h
//mới 0x88

/* Register 06h */
#define BQ2429X_BOOST_TREG_CTRL			0x06
#define BQ2429X_BOOST_TREG_SELE			0x73

/* Register 07h */
#define BQ2429X_MISC_OPER_CTRL			0x07
#define BQ2429X_MISC_OPER_SELE			0x4B

/* Register 08h */
#define BQ2429X_STATUS					0x08
#define BQ2429X_STAT76_1				0		// Unknown (no input, or DPDM detection incomplete)
#define BQ2429X_STAT76_2				1		// USB hot
#define BQ2429X_STAT76_3				2		// Adapter port
#define BQ2429X_STAT76_4				3		// OTG
#define BQ2429X_STAT54_1				0		// Not Charging
#define BQ2429X_STAT54_2				1		// Pre-charge (<VBATLOWV)
#define BQ2429X_STAT54_3				2		// Fast Charging
#define BQ2429X_STAT54_4				3		// Charge Termination Done
#define BQ2429X_STAT3_1					0		// Not DPM
#define BQ2429X_STAT3_2					1		// VINDPM or IINDPM
#define BQ2429X_STAT2_1					0		// Not Power Good
#define BQ2429X_STAT2_2					1		// Power Good
#define BQ2429X_STAT1_1					0		// Normal
#define BQ2429X_STAT1_2					1		// In Thermal Regulation
#define BQ2429X_STAT0_1					0		// Not in VSYSMIN regulation (BAT > VSYSMIN)
#define BQ2429X_STAT0_2					1		// In VSYSMIN regulation (BAT <VSYSMIN)

/* Register 09h */
#define BQ2429X_FAULT					0x09
#define BQ2429X_FAULT7					1		// Watchdog timer expiration
#define BQ2429X_FAULT6					1		// VBUS overloaded in OTG, or VBUS OVP, or battery is too low (any conditions that cannot start boost function)
#define BQ2429X_FAULT54_1				1		// Input fault (OVP or bad source)
#define BQ2429X_FAULT54_2				2		// Thermal shutdown
#define BQ2429X_FAULT54_3				3		// Charge Timer Expiration
#define BQ2429X_FAULT3					1		// Battery OVP
#define BQ2429X_FAULT1					1		// Cold Note: Cold temperature threshold is different based on device operates in buck or boost mode
#define BQ2429X_FAULT0					1		// Hot Note: Hot temperature threshold is different based on device operates in buck or boost mode

/* Other */
#define I2C_DELA_MAX					100					// 100ms: timeout read/write i2c with bq2429x
#define BQ2429X_I2C_ADDR        		0xD6

extern I2C_HandleTypeDef hi2c1;

//

bool bq2429x_updateBit(void);
void bq2429x_status(void);
void bq2429x_fault(void);
bool bq2429x_setInputLimit(uint8_t regVal);
bool bq2429x_powerConfig(uint8_t regVal);
bool bq2429x_chargeCurrentCtrl(uint8_t regVal);
bool bq2429x_ipreIterCtrl(uint8_t regVal);
bool bq2429x_chargeVoltCtrl(uint8_t regVal);
bool bq2429x_chargeTimerCtrl(uint8_t regVal);
bool bq2429x_boostCtrl(uint8_t regVal);
bool bq2429x_miscCtrl(uint8_t regVal);
int bq2429x_read(uint8_t devAdd, uint8_t addMemory, uint8_t sizeof_buffer);
bool bq2429x_write(uint8_t devAdd, uint8_t addMemory, uint8_t data, uint8_t sizeof_buffer);

#endif /* INC_BQ24298_H_ */
