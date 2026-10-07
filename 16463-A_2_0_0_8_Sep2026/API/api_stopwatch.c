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
 * Filename    :  api_stopwatch.c
 * Date Created:  Tue 12 Sep 2017 08:24:13 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>

#include 	"api_stopwatch.h"

#include 	"csp_STM32_uart.h"
#include	"hal_STM32_uart.h"

/*********************************************************************************************************
 *		Global Variables
 ********************************************************************************************************/
api_Stopwatch_t		api_Stopwatch	=  {
						.mode			=  STOPWATCH_MODE_STOP,
						.cnt_en			=  0,
						.cntToSeconds	=  0,
						.seconds		=  0,
					};


/*********************************************************************************************************
 ********************************************************************************************************/


/*************************************************************************************************
* Function Name : 	api_Stopwatch_mode_set
* Description   : 	This Function sets the mode of the stopwatch function
* Arguments     : 	STOPWATCH_MODE_enum		mode
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
void api_Stopwatch_mode_set(STOPWATCH_MODE_enum	mode)
{
	api_Stopwatch.mode	=  mode;

	switch(api_Stopwatch.mode){
		case STOPWATCH_MODE_START:		api_Stopwatch.cnt_en		=  1;		break;
		case STOPWATCH_MODE_STOP:		api_Stopwatch.cnt_en		=  0;		break;
		case STOPWATCH_MODE_RESET:		api_Stopwatch.seconds		=  0;
										api_Stopwatch.cntToSeconds	=  0;		break;
		case STOPWATCH_MODE_START_STOP:
			if(api_Stopwatch.cnt_en){	api_Stopwatch.cnt_en	=  0;	}
			else{						api_Stopwatch.cnt_en	=  1;	}
			break;
		default:
			api_Stopwatch.mode		=  STOPWATCH_MODE_STOP;
	}
	return;
}

/*************************************************************************************************
* Function Name : 	api_Stopwatch_running_read
* Description   : 	This Function returns the mode of the stopwatch function
* Arguments     : 	void
* Returns       : 	STOPWATCH_MODE_enum		mode
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_Stopwatch_running_read(void)
{
	return(api_Stopwatch.cnt_en);
}

/*************************************************************************************************
* Function Name : 	api_Stopwatch_read
* Description   : 	This Function reads the stopwatch time
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_Stopwatch_read(uint16_t	*day, uint8_t *hr, uint8_t *min, uint8_t *sec)
{
	static uint8_t			last_mode		=  0xff;
	uint32_t				stopwatch_cur;
	static uint32_t			stopwatch_last	=  0xff;
	uint8_t					change			=  0;


	stopwatch_cur	=  api_Stopwatch.seconds;

	if(stopwatch_last != stopwatch_cur){
		stopwatch_last	=  stopwatch_cur;
		change	=  1;
	}

	*sec			=  (uint8_t)(stopwatch_cur % 60);
	stopwatch_cur	/= 60;
	*min			=  (uint8_t)(stopwatch_cur % 60);
	stopwatch_cur	/= 60;
	*hr				=  (uint8_t)(stopwatch_cur % 24);
	stopwatch_cur	/= 24;
	*day			=  (uint16_t)(stopwatch_cur);

	if(last_mode != api_Stopwatch_running_read() ){
		last_mode	= api_Stopwatch_running_read();
		change	=  1;
	}
	return(change);
}

/*************************************************************************************************
* Function Name : 	api_Stopwatch_IRQ
* Description   : 	This Function increments the cnt if the timer is enabled
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
void api_Stopwatch_IRQ(void)
{
	if(api_Stopwatch.cnt_en){
		if(++api_Stopwatch.cntToSeconds >= Stopwatch_Clk_Freq){
			api_Stopwatch.cntToSeconds	=  0;
			api_Stopwatch.seconds	+= 1;
		}
	}
	return;
}



/*************************************************************************************************
* Function Name : 	api_Stopwatch_menu
* Description   : 	This Function can be used to test the functionality of the timer
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_Stopwatch_menu(void)
{
	uint8_t			rec_status_u8;
	uint8_t			rec_char_u8;
	uint8_t			hr,min,sec	= 0;
	uint16_t		day	=  0;

	rec_char_u8 = debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){
		switch(rec_char_u8){
			case ' ':
				printf("\r\n -------------------------------");
				printf("\r\n api Stopwatch Menu");
				printf("\r\nEsc Exit Menu");
				printf("\r\n1   Start");
				printf("\r\n2   Stop");
				printf("\r\n3   Reset");
				printf("\r\n4   Start/Stop");
				printf("\r\n");
				break;

			case 0x1b:		return(0);
			case '1':	api_Stopwatch_mode_set(STOPWATCH_MODE_START);		printf("\rStart          \r\n");	break;
			case '2':	api_Stopwatch_mode_set(STOPWATCH_MODE_STOP);		printf("\rStop           \r\n");	break;
			case '3':	api_Stopwatch_mode_set(STOPWATCH_MODE_RESET);		printf("\rReset          \r\n");	break;
			case '4':	api_Stopwatch_mode_set(STOPWATCH_MODE_START_STOP);	printf("\rStart/Stop Tog \r\n");	break;
		}

	}
	if(api_Stopwatch_read(&day,&hr,&min,&sec) ){
		printf("\r%d  %d:%2d:%2d ",day,hr,min,sec);
	}

	return(1);
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
