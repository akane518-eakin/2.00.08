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
 * Filename    :  csp_STM32_iwdg.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>


#include "stm32f10x.h"
#include "stm32f10x_dbgmcu.h"
#include "csp_STM32_iwdg.h"


/**********************************************************************************************************
* Function Name : watchdog_init
* Description   : This function is used to initialise the Watchdog timer.
* Arguments     : uint16_t timeout in mS
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	19/12/12	    	Pauric Lynch     Original Created
**********************************************************************************************************/
void watchdog_init(uint16_t timeout)
{
/* Local Variables */

/* Code */
#ifdef IAR_DEBUG
	//setup the debug register
	//stop the watchdog when in breakpoint
	//keep the debugger connected while in sleep, stopped & standby

	DBGMCU_Config(DBGMCU_STOP		,ENABLE);
	DBGMCU_Config(DBGMCU_STANDBY	,ENABLE);
	DBGMCU_Config(DBGMCU_SLEEP		,ENABLE);
	DBGMCU_Config(DBGMCU_IWDG_STOP	,ENABLE);
#endif


	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	//clock is 40kHz

	//			freq		min (0x000)	max(0xfff)
	//4		10 		kHz		0.1ms		409.6	ms
	//8		 5  	kHz		0.2ms		819.2	ms
	//16	 2.5	kHz		0.4ms		1.6384	s
	//32	 1.25	kHz		0.8ms		3.2768	s
	//64	 0.625	kHz		1.6ms		6.5535	s
	//128	 0.3125 kHz		3.2ms		13.104	s
	//256	 0.15625kHz		6.4ms		26.208	s

	//10seconds		/128	3125 (0xC35)

	IWDG_SetPrescaler(IWDG_Prescaler_128);
	IWDG_SetReload(timeout);
	IWDG_ReloadCounter();
	IWDG_Enable();

}


/*************************************************************************************************
 * Function Name :	watchdog_reload
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	23/12/16	W. Paul		Created
 *
 *************************************************************************************************/
void watchdog_reload(void)
{
	IWDG_ReloadCounter();
	return;
}

/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


