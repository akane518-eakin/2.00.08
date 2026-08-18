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
 *  Copyright 2016, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  api_Reset.c
 * Date Created:  Wed 28 Dec 2016 04:25:06 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>


#include "stm32f10x_rcc.h"



/*************************************************************************************************
* Function Name :	api_reset_source
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/12/16	W. Paul			Created
*************************************************************************************************/
uint8_t api_reset_source(uint8_t debug)
{
	// Local Variables
	static		uint8_t		source		=  0;
	
	// Code
//	*	@arg RCC_FLAG_PINRST: Pin reset
//	*	@arg RCC_FLAG_PORRST: POR/PDR reset
//	*	@arg RCC_FLAG_SFTRST: Software reset
//	*	@arg RCC_FLAG_IWDGRST: Independent Watchdog reset
//	*	@arg RCC_FLAG_WWDGRST: Window Watchdog reset
//	*	@arg RCC_FLAG_LPWRRST: Low Power reset
//	*
//	*	@retval The new state of RCC_FLAG (SET or RESET).
	
	if(source	== 0){
		if(		RCC_GetFlagStatus(RCC_FLAG_SFTRST)	){	source	=  1;	}
		else if(RCC_GetFlagStatus(RCC_FLAG_PORRST)	){	source	=  2;	}
		else if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST)	){	source	=  3;	}
		else if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)	){	source	=  4;	}
		else if(RCC_GetFlagStatus(RCC_FLAG_PINRST)	){	source	=  5;	}
		else if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST)	){	source	=  6;	}
		
		RCC_ClearFlag();							// Clear reset flags
	}
	
	if(debug){
		printf("\r\nLast Reset Source ... ");
		switch(source){
			case 1:		printf("Software ");    break;
			case 2:		printf("POR/PDR  ");    break;
			case 3:		printf("WWDog    ");    break;
			case 4:		printf("IWDog    ");    break;
			case 5:		printf("Pin      ");    break;
			case 6:		printf("LowPower ");    break;
			default:	printf("None");
		}
	}
	
	return(source);
}

/*************************************************************************************************
* Function Name : 	api_reset_now
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		02/08/17	W. Paul			Created
*************************************************************************************************/
void api_reset_now(void)
{
	NVIC_SystemReset();		/* Do a software Reset to enter the bootlaoder code */
	return;
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
