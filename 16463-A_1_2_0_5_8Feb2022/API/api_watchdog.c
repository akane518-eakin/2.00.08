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
 * Filename    :  api_watchdog.c
 * Date Created:  Wed 20 Sep 2017 08:49:21 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>

#include "api_watchdog.h"
#include "csp_STM32_delay.h"
#include "pcb_pins.h"



/*************************************************************************************************
* Function Name : 	api_watchdog_init
* Description   : 	This Function initialies the watchdog circuit
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		20/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_watchdog_init(uint8_t en_dis)
{
	uint8_t		buz		=  0;

//	buz	=  api_watchdog_buzzer_rd();
//	if(buz){
//		api_watchdog_reload();
////		printf("\r\nBuz was Reset");
//	}
//
//
//	PinSet(WATCHDOG_EN,!en_dis);		//1= watchdog disabled
//	api_watchdog_reset();			//kick watchdog

	return(buz);
}


/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		20/09/17	W. Paul			Created
*************************************************************************************************/
void api_watchdog_reset(void)
{
//	uint32_t	sys_tic;
//
//#warning set back to 1
//	PinSet(WATCHDOG_BUZ_CTRL,1);	//low -> hi   /set back to 1
//	sys_tic	=  sys_tic_rd() + 50;
//
//	while(sys_tic_rd() < sys_tic);
//
//	PinSet(WATCHDOG_BUZ_CTRL,0);	//return to low
	return;
}

/*************************************************************************************************
* Function Name :	api_watchdog_reload
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		20/09/17	W. Paul			Created
*************************************************************************************************/
void api_watchdog_reload(void)
{
	static uint8_t tog	=  0;

	PinSet(WATCHDOG_INPUT,tog);	//return to low
	tog	^= 0x01;

	return;
}
/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		20/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_watchdog_buzzer_rd(void)
{
	if( PinRead(WATCHDOG_WD_BUZ)){	return(1);	}	//pin hi =  buzzer on
	else{							return(0);	}
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
