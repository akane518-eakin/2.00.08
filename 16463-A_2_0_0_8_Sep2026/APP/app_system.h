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
 * Filename    :  app_system.h
 * Date Created:  Wed 04 Oct 2017 08:21:05 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _APP_SYSTEM_H
#define	_APP_SYSTEM_H

typedef enum{
	DONT_RESET_SYSTEM	=  0,
	RESET_SYSTEM		=  1,	
}RESET_SYS_t;

void system_shutdown(RESET_SYS_t reset_system);
void app_sys_watchdog_reload(void);



#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
