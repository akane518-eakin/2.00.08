/**********************************************************************************************************
 *  Marturion Ltd
 *
 *	Knockmore Hill Business Park
 *	9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2010, Marturion Ltd
 *  All Rights Reserved
 *
 *
 * Filename    :  csp_STM32_interrupts.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/
#ifndef _CSP_STM32_INTERRUPTS_H
#define _CSP_STM32_INTERRUPTS_H


/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include "pcb_interrupts.h"

/**********************************************************************************************************
 *	DEFINES
 *********************************************************************************************************/


/**********************************************************************************************************
 *	FUNCTION PROTOTYPES
 *********************************************************************************************************/

void	csp_interrupts_config(void);
void ext_interupt_endis(uint8_t line,uint8_t endis);

#if(EXT_INT0_EN == 1)
void csp_STM32_interrupt_EXTI0(uint8_t en_dis);
#endif

#if(EXT_INT1_EN == 1)
void csp_STM32_interrupt_EXTI1(uint8_t en_dis);
#endif

#if(EXT_INT2_EN == 1)
void csp_STM32_interrupt_EXTI2(uint8_t en_dis);
#endif

#if(EXT_INT3_EN == 1)
void csp_STM32_interrupt_EXTI3(uint8_t en_dis);
#endif

#if(EXT_INT4_EN == 1)
void csp_STM32_interrupt_EXTI4(uint8_t en_dis);
#endif

#if( (EXT_INT5_EN == 1)||(EXT_INT6_EN == 1)||(EXT_INT7_EN == 1)||(EXT_INT8_EN == 1)||(EXT_INT9_EN == 1) )
void csp_STM32_interrupt_EXTI9_5(uint8_t en_dis);
#endif

#if( (EXT_INT10_EN == 1)||(EXT_INT11_EN == 1)||(EXT_INT12_EN == 1)||(EXT_INT13_EN == 1)||(EXT_INT14_EN == 1)||(EXT_INT15_EN == 1) )
void csp_STM32_interrupt_EXTI15_10(uint8_t en_dis);
#endif

#endif

//end of file
