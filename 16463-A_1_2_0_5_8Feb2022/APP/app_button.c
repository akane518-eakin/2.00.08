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
 * Filename    :  app_button.c
 * Date Created:  Mon 16 Oct 2017 01:28:27 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>

#include "app_button.h"
#include "app_system.h"
#include "pcb_pins.h"

uint32_t	button_hold_dur	=  0;
uint8_t		buttonDetectDurationTest	=  0;

/*************************************************************************************************
* Function Name : 	app_button_handler
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/10/17	W. Paul			Created
*************************************************************************************************/
uint8_t	app_button_handler(void)
{
 	button_u		status;
 	static uint8_t	last_status;

	status.a				=  0;

	status.bits.alarm		=  PinRead(BUT_1_ALARM);
	status.bits.pwr			=  PinRead(BUT_2_ON_OFF);

	if( status.a != last_status){								//if new button press is detected
		last_status	=  status.a;

		if(status.a == 0){				button_hold_dur	=  0;	}	//stop timer
		else{ 							button_hold_dur	=  1;	}	//start timer if new status

		if(button_hold_dur < BUT_PRESS_AND_HOLD_ERROR){
			buttonDetectDurationTest	=  0;		//clear error
		}
	}
	else if(button_hold_dur > BUT_PRESS_AND_HOLD_ERROR){
		buttonDetectDurationTest	=  1;
	}
	else if(button_hold_dur > BUT_PRESS_AND_HOLD_5S){
		status.bits.hold5s	=  1;
	}
	else if(button_hold_dur > BUT_PRESS_AND_HOLD_DURATION){		//press and hold reported
		status.bits.hold	=  1;
	}

	//if power button is held for 5s .. power down system
	if(status.a	== BUT_PWR_HOLD5S){
		system_shutdown(RESET_SYSTEM);
	}

 	return(status.a);
}

/*************************************************************************************************
* Function Name :	app_button_irq
* Description   : 	This Function is the button hold timer irq
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/10/17	W. Paul			Created
*************************************************************************************************/
void app_button_irq(void)
{
	if(button_hold_dur){			button_hold_dur++;			}
	return;
}


/*************************************************************************************************
* Function Name : 	app_button_hold_err
* Description   : 	This Function returns a high if a button has been pressed longer than x ms
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		02/05/18	W. Paul			Created
*************************************************************************************************/
uint8_t app_button_hold_err(void)
{
	return(buttonDetectDurationTest);
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
