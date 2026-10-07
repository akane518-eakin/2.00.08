/**********************************************************************************************************
 *  Marturion Electronics Ltd
 *
 *  Knockmore Hill Business Park
 *  9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2017, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  csp_STM32_fsmc.h
 * Date Created:  Tue 16 May 2017 09:31:23 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _CSP_STM32_FSMC_H
#define _CSP_STM32_FSMC_H

#include <stdint.h>

#define Bank1_SRAM1_ADDR    ((uint32_t)0x60000000)
#define Bank1_SRAM2_ADDR    ((uint32_t)0x64000000)
#define Bank1_SRAM3_ADDR    ((uint32_t)0x68000000)
#define Bank1_SRAM4_ADDR    ((uint32_t)0x6C000000)

typedef enum{
	FSMC_OFF		=  0,
	FSMC_STARTUP,
	FSMC_HIGH_SPEED,
}FSMC_MODE_t;

void csp_FSMC_NE1_Config(uint8_t option);
void csp_FSMC_NE2_Config(void);



#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
