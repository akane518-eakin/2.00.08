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
 * Filename    :  csp_STM32_iwdg.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/
#ifndef _CSP_STM32_IWDG_H
#define _CSP_STM32_IWDG_H


void watchdog_init(uint16_t timeout);
void watchdog_reload(void);

#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


