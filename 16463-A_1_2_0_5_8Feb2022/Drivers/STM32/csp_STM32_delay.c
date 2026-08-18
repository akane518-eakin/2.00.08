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
 * Filename    :  csp_STM32_delay.c
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

#include "csp_STM32_delay.h"


/**********************************************************************************************************
 *********************************************************************************************************/

uint32_t 		TimingDelay_u32;
uint32_t		const_inc_timer		=  1;





/*
*********************************************************************************************************
* Function Name : Decrement_TimingDelay
* Description   : This function is used to Decrements the TimingDelay variable.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    11/05/2010      Michael Kelly       Original Created
*********************************************************************************************************
*/
void Decrement_TimingDelay(void)		//int code
{
/* Code */
	if (TimingDelay_u32 != 0x00){
		TimingDelay_u32--;
	}
	const_inc_timer++;
	return;

}

/*
*********************************************************************************************************
* Function Name : Delay
* Description   : This function is used to Inserts a delay time.
* Arguments     : nCount: specifies the delay time length (time base 1 ms).
* Returns       : None
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    11/05/2010      Michael Kelly       Original Created
*********************************************************************************************************
*/
void Delay(uint32_t nCount)
{
/* Code */
	TimingDelay_u32 = nCount;
	while(TimingDelay_u32 != 0);

	return;

}
/*
*********************************************************************************************************
* Function Name : Delay
* Description   : This function is used to Inserts a delay time.
* Arguments     : nCount: specifies the delay time length (time base 1 ms).
* Returns       : None
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    11/05/2010      Michael Kelly       Original Created
*********************************************************************************************************
*/
uint32_t sys_tic_rd(void)
{
/* Code */
	return(const_inc_timer);
}

//end of file
