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
 * Filename    :  app_button.h
 * Date Created:  Mon 16 Oct 2017 01:29:39 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _APP_BUTTON_H
#define _APP_BUTTON_H

#include <stdint.h>


#define	BUT_HOLD				0x80
#define	BUT_HOLD_5S				0x40

#define	BUT_NONE				0x00
#define	BUT_ALARM				0x01
#define	BUT_PWR					0x02

#define	BUT_ALARM_HOLD			BUT_HOLD + BUT_ALARM
#define	BUT_PWR_HOLD			BUT_HOLD + BUT_PWR
#define	BUT_PWR_HOLD5S			BUT_HOLD_5S + BUT_PWR

#define	BUT_ALL					BUT_ALARM + BUT_PWR
#define	BUT_ALL_HOLD			BUT_ALARM + BUT_PWR + BUT_HOLD




#define	BUT_PRESS_AND_HOLD_DURATION		400
#define	BUT_PRESS_AND_HOLD_5S			5000
#define	BUT_PRESS_AND_HOLD_ERROR		7000

typedef union{
	uint8_t		a;
	struct{
		uint8_t	alarm	:1;
		uint8_t	pwr		:1;
		uint8_t	unused	:4;
		uint8_t	hold5s	:1;
		uint8_t	hold	:1;
	}bits;

}button_u;


uint8_t	app_button_handler(void);
void 	app_button_irq(void);
uint8_t app_button_hold_err(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
