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
 *  Copyright 2018, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  api_RTC.h
 * Date Created:  Tue 23 Jan 2018 03:09:58 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_RTC_H
#define _API_RTC_H

#include <time.h>

//normal epoch year is 		01/Jan/1970
//which gives a max date of 19/Jan/2038		//0x7fffffff seconds later
//amd a min date of			13/Dec/1901		//0x7fffffff seconds before

//if we add a mutiple of 28 years to this we get a working EPOCH (in terms of dates day of week ,etc)
//number of multiples		0(0years)		1(28years)		2(56years)		3(84years)
//so base year is			01/Jan/1970		01/Jan/1998		01/Jan/2026		01/Jan/2054
//min is					19/Jan/2038		19/Jan/2066		19/Jan/2094		19/Jan/2132
//max is					13/Dec/1901		13/Dec/1929		13/Dec/1957		13/Dec/1985

//	https://www.epochconverter.com/

//this code adds one multiple of 28

#define	EPOCH_YEAR			1900+28	//USED TO GET THE CORRECT YEAR
#define	TIME_MAX_YEAR		2038+28-1	//One year less because all of (2038+28) is not available

//minimum time allowed in this software
#define	TIME_MIN			631152000	//	1/Jan/2018	seconds since 1/Jan/1998

extern uint8_t 	no_days_in_month[12];
extern char		mon_str[12][4];
extern char		wday_str[7][5];


/*********************************************************************************************************
 ********************************************************************************************************/
void		api_RTC_map(		time_t	(*t_api_RTC_time_ptr)(		time_t *timer )	);

time_t 		time(				time_t *timer );
uint8_t 	api_STM32_internal_RTC_test(void);
void 		api_set_time(	time_t *rawtime);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
