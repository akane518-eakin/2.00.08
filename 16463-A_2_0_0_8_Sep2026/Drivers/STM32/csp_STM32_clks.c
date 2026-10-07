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
 * Filename    :  csp_STM32_clks.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 *********************************************************************************************************/


/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>


#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dbgmcu.h"

#include "misc.h"
#include "system_stm32f10x.h"
#include "csp_STM32_iwdg.h"

#include "csp_STM32_clks.h"


/**********************************************************************************************************
 *********************************************************************************************************/
void init_SysTick_Configuration(void);






/**********************************************************************************************************
 * Function Name : STM32_startup
 * Description   : This function is used to initialise the STM32
 * Arguments     : None
 * Returns       : None
 * Notes         : All ports,pins and clock are initialsied
 *
 * Version		Date d/m/y	    Programmer          	Reason for Change
 * 1.0.0		19/07/2011      William Paul	       	Original Created
 *
 *********************************************************************************************************/
void STM32_startup(uint32_t int_vect_offset)
{

	SystemInit();
	SystemCoreClockUpdate();

	init_SysTick_Configuration();

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, int_vect_offset);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);			/* Configure the Priority Group to 2 bits */

    return;
}




/*
*********************************************************************************************************
* Function Name : init_SysTick_Configuration
* Description   : This function is used to Configure a SysTick Base time to 1 ms || 10ms || 100ms.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    11/05/2010      Philip Gillespie       Original Created
*********************************************************************************************************
*/

void init_SysTick_Configuration(void)
{
/* Code */
	NVIC_SetPriority(SysTick_IRQn, 0x02);					/* Configure the SysTick handler priority */
  	SysTick->CTRL	|= SysTick_CTRL_ENABLE;					/* Enable the SysTick Counter */
  	SysTick->VAL	= (uint32_t)0x0;						/* Clear the SysTick Counter */


#if SYSTEM_TICK_FREQ	== SYSTEM_TICK_1MS
	if (SysTick_Config(SystemCoreClock / 1000)){			/* Setup SysTick Timer for 1 msec interrupts  */
		while (1);											/* Capture error */
	}
#elif SYSTEM_TICK_FREQ	== SYSTEM_TICK_10MS
	if (SysTick_Config((SystemCoreClock) / 100)){			/* Setup SysTick Timer for 10 msec interrupts  */
		while (1);											/* Capture error */
	}
#elif SYSTEM_TICK_FREQ	== SYSTEM_TICK_100MS
  	if (SysTick_Config((SystemCoreClock) / 10)){			/* Setup SysTick Timer for 100 msec interrupts  */
   		while (1);											/* Capture error */
  	}
#else
	#warning system tic not set
#endif


	return;
}


/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


