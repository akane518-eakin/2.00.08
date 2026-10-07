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
 * Filename    :  api_RTC.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/

/**********************************************************************************************************
 *                                           INCLUDE FILES
 **********************************************************************************************************/
#include 	<stdio.h>
#include	<stdlib.h>
#include	<stdint.h>
#include	<string.h>
#include	<time.h>

#include	"app_system.h"

#include 	"stm32f10x.h"			/* CMSIS Cortex-M3 Device Peripheral Access Layer Header File. 	*/
#include 	"api_RTC.h"

#include 	"csp_STM32_rtc.h"
#include 	"csp_STM32_uart.h"
#include	"hal_STM32_uart.h"		//for api_set_time
#include "csp_STM32_iwdg.h"



time_t	(*t_api_RTC_time_f)(		time_t *timer );
time_t	(*t_api_RTC_ALM_time_f)(	time_t *timer );

uint8_t no_days_in_month[12]	=  {31,28,31,30,31,30,31,31,30,31,30,31};
char	mon_str[12][4]			= {	"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0",
									"Jul\0", "Aug\0", "Sep\0", "Oct\0", "Nov\0", "Dec\0"	};
char	wday_str[7][5]			= {	"Sun\0", "Mon\0", "Tue\0", "Wed\0", "Thur\0", "Fri\0","Sat\0"};

/**********************************************************************************************************
 * Function Name : api_RTC_map
 * Description   : This function is used to
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		10/01/12	William Paul	Original Created
 **********************************************************************************************************/
void api_RTC_map(	time_t	(*t_api_RTC_time_ptr) (time_t *timer )	)
{
    t_api_RTC_time_f	 = t_api_RTC_time_ptr;

    return;
}


/**********************************************************************************************************
 * Function Name : time
 * Description   : This function is used to
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		10/01/12	William Paul	Original Created
 **********************************************************************************************************/
time_t time(time_t *timer )
{

	return t_api_RTC_time_f(timer);
}




/**********************************************************************************************************
 * Function Name : api_STM32_internal_RTC_test
 * Description   :
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		2/ 3/2011		William Paul		Update function
 * 1.0.1		25/6/2012		WTP					Tidy
 **********************************************************************************************************/
uint8_t api_STM32_internal_RTC_test(void)
{

/* Local Variables */
	uint8_t		rec_status_u8;
	uint8_t		rec_char_u8;

	uint32_t 	RTC_value_u32;
	time_t 		rawtime;
	struct tm * timeinfo;
/* Code */


	rec_char_u8 = debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){
		switch(rec_char_u8){
			case ' ':
				printf("\r\n -------------------------------");
				printf("\r\n api STM32 int RTC Test menu");
				printf("\r\nEsc Exit Menu");

				printf("\r\nr   Read  RTC Value(sec)");
				printf("\r\nw   Write RTC Value(sec)");

				printf("\r\nR   Read  RTC Value(time.h)");
				printf("\r\nW   Write RTC Value(time.h)");

				printf("\r\n");
				break;

			case 0x1b:
				return(0);

          	case 'r':
				RTC_value_u32   =  (uint32_t)time(0);
				printf("\r\nRTC value (now)  = %ld",RTC_value_u32);
				break;
			case 'w':
				printf("\r\nEnter RTC time in s:----------\b\b\b\b\b\b\b\b\b\b");
				RTC_value_u32	=  BSP_get_num(BASE10,10);
				time((time_t*)&RTC_value_u32);
				break;

			case 'R':
				rawtime	= time(0);
				timeinfo = localtime (&rawtime);
				printf("\n\rRTC time: %s %d%s%d %2d:%02d.%02d"	, wday_str[timeinfo->tm_wday]
																, timeinfo->tm_mday
																, mon_str[timeinfo->tm_mon]
																, timeinfo->tm_year + EPOCH_YEAR
																, timeinfo->tm_hour
																, timeinfo->tm_min
																, timeinfo->tm_sec			);
				break;
			case 'W':
				api_set_time(&rawtime);
				time(&rawtime);
				break;
		}

	}
	return(1);
}


/*************************************************************************************************
 * Function Name :	api_set_time()
 * Description   :	Set the RTC time
 * Arguments     : 	time_t	*raw_time
 * Returns       : 	void
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	21/06/2012	W. Paul		Created
 *
 *************************************************************************************************/
void api_set_time(time_t *rawtime)
{
	uint8_t				set_time_loop		=  1;
	uint8_t				set_time_state		=  0;
	uint8_t				uc_temp;
	struct tm 			timeinfo;

	while(set_time_loop){
		switch(set_time_state){
//YEAR
			case 0:
				printf("\n\rEnter Year\n\r ----\r:");
				timeinfo.tm_year	=  BSP_get_num(BASE10,4);
				set_time_state	+= 1;
				break;
//MONTH
			case 1:
				printf("\n\rEnter Month\n\r --\r:");
				timeinfo.tm_mon	=  BSP_get_num(BASE10,2);
				if((timeinfo.tm_mon>0)&&(timeinfo.tm_mon<=12)){
					set_time_state	+= 1;
					timeinfo.tm_mon--;		//change from 1-12 to 0-11
				}
				break;
//DATE
			case 2:
				printf("\n\rEnter Date\n\r --\r:");
				timeinfo.tm_mday	=  BSP_get_num(BASE10,2);
				uc_temp	=  no_days_in_month[timeinfo.tm_mon];
				if((timeinfo.tm_mon==1)&&(timeinfo.tm_year % 4 == 0)){
					uc_temp	+= 1;
				}
				if((timeinfo.tm_mday>0)&&(timeinfo.tm_mday<=uc_temp)){
					set_time_state	+= 1;
				}
				break;

//HOUR 24H
			case 3:
				printf("\n\rEnter Hour in 24H mode\n\r --\r:");
				timeinfo.tm_hour	=  BSP_get_num(BASE10,2);
				if(timeinfo.tm_hour <= 23){
					set_time_state	+= 1;
				}
				break;
//MIN
			case 4:
				printf("\n\rEnter minute\n\r --\r:");
				timeinfo.tm_min	=  BSP_get_num(BASE10,2);
				if(timeinfo.tm_min <= 59){
					set_time_state	+= 1;
				}
				break;
//SET TIME
			case 5:
				timeinfo.tm_year		-= EPOCH_YEAR;
				timeinfo.tm_sec		=  0;
				*rawtime	=  mktime ( &timeinfo );
				set_time_loop	=  0;
				break;
			default:
				set_time_state	=  0;
		}
		app_sys_watchdog_reload();
	}

	return;
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
