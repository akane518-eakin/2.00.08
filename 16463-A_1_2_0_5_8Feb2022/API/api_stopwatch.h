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
 * Filename    :  api_stopwatch.h
 * Date Created:  Tue 12 Sep 2017 08:25:00 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_STOPWATCH_H
#define _API_STOPWATCH_H

#include <stdint.h>

#define		Stopwatch_Clk_Freq		1000		//incremented on 1ms timer

#define		STOPWATCH_VALID		0x11

typedef enum{
	STOPWATCH_MODE_START	= 1,
	STOPWATCH_MODE_STOP,
	STOPWATCH_MODE_RESET,
	STOPWATCH_MODE_START_STOP,
}STOPWATCH_MODE_enum;


typedef struct{
	STOPWATCH_MODE_enum		mode;
	uint8_t					cnt_en;
	uint16_t				cntToSeconds;
	uint32_t				seconds;
}api_Stopwatch_t;


extern api_Stopwatch_t		api_Stopwatch;

/*********************************************************************************************************
 *		Global Variables
 ********************************************************************************************************/
void		api_Stopwatch_mode_set(STOPWATCH_MODE_enum	mode);
uint8_t 	api_Stopwatch_running_read(void);
uint8_t		api_Stopwatch_read(uint16_t *day, uint8_t *hr, uint8_t *min, uint8_t *sec);
void		api_Stopwatch_IRQ(void);
uint8_t		api_Stopwatch_menu(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
