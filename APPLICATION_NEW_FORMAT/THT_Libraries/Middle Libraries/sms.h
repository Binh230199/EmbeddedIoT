/*
 * sms.h
 *
 *  Created on: Mar 24, 2023
 *      Author: Hoang Van Binh (gmail: binhhv.23.1.99@gmail.com)
 */

#ifndef INC_SMS_H_
#define INC_SMS_H_

#include "sim.h"
#include "wlv_def.h"

#define SMS_NUMBER_SYNTAX			(15U)
/**
 * @brief SMS Syntaxs
 */
#define SMS_SYNTAX_DEBUG			("*DEBUG#")
#define SMS_SYNTAX_SEND				("*SEND#")
#define SMS_SYNTAX_SETCONFIG		("*SETCFG#")
#define SMS_SYNTAX_SETDEBUG			("*SETDB#")
#define SMS_SYNTAX_SETGATE			("*SETGATE#")
#define SMS_SYNTAX_SETID			("*SETID#")
#define SMS_SYNTAX_SETURL			("*SETURL#")
#define SMS_SYNTAX_SETTIME			("*SETTIME#")
#define SMS_SYNTAX_RESTORE			("*RESTORE#")
#define SMS_SYNTAX_RESET			("*RESET#")
#define SMS_SYNTAX_UPFW				("*UPFW#")
#define SMS_SYNTAX_RESEND			("*RESEND#")
#define SMS_SYNTAX_TESTWARN			("*TESTWARN#")
#define SMS_SYNTAX_SETFWS			("*SETFWS#")
#define SMS_SYNTAX_SETADMIN			("*SETAD#")
#define SMS_SYNTAX_SETTOKEN			("*SETTOKEN#")
#define SMS_SYNTAX_SETCHATID		("*SETCID#")

/**
 * @brief SMS Reply
 */
#define SMS_REPLY

/**
 * @brief SMS admin phone number
 */
#define SMS_ADPHONE_1				("84969647577") //Bình
#define SMS_ADPHONE_2				("84981449744") //số anh Huy 2
#define SMS_ADPHONE_3				("84982577702") //số anh Bin //84913491640 soos anh Huy
#define SMS_ADPHONE_4				("84905951124") //số anh Trường 1
#define SMS_ADPHONE_5				("84966519488") //số anh Trường 2
#define SMS_ADPHONE_6				("84914486786") //SĐT thầy Thanh
#define SMS_ADPHONE_7				("84862865768")

#define SMS_ADPHONE_ORDER_1			(1u) //Bình
#define SMS_ADPHONE_ORDER_2			(2u) //số anh Huy 2
#define SMS_ADPHONE_ORDER_3			(3u) //số anh Huy 1
#define SMS_ADPHONE_ORDER_4			(4u) //số anh Trường 1
#define SMS_ADPHONE_ORDER_5			(5u) //số anh Trường 2
#define SMS_ADPHONE_ORDER_6			(6u) //SĐT thầy Thanh
#define SMS_ADPHONE_ORDER_7			(7u) //số
#define SMS_DBPHONE_ORDER_8			(8u) //số debug
#define SMS_ADPHONE_ORDER_9			(9u) //số nhận SMS Warning 1
#define SMS_ADPHONE_ORDER_10		(10u) //số nhận SMS Warning 2
#define SMS_ADPHONE_ORDER_11		(11u) //số nhận SMS Warning 3
#define SMS_ADPHONE_ORDER_12		(12u) //số nhận SMS Warning 4
#define SMS_ADPHONE_ORDER_13		(13u) //số nhận SMS Warning 5
#define SMS_FLWPHONE_ORDER_14		(14u) //số trạm phát thanh

void SMS_process(void);
THT_StatusTypeDef SMS_checkNumber(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_checkSyntax(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_debug(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_send(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setConfig(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setMode(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setDebug(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setGate(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setID(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setURL(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setTime(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_restore(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_reset(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_update(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_resend(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setAdmin(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setToken(SIM_SMSTypeDef *const me);
THT_StatusTypeDef SMS_setChatID(SIM_SMSTypeDef *const me);

#endif /* INC_SMS_H_ */
