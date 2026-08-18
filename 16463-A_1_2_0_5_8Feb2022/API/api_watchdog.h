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
 * Filename    :  api_watchdog.h
 * Date Created:  Wed 20 Sep 2017 09:22:38 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_WATCHDOG_CTRL_H
#define _API_WATCHDOG_CTRL_H

#include <stdint.h>
#include "csp_STM32_iwdg.h"

uint8_t api_watchdog_init(uint8_t en_dis);
void 	api_watchdog_reset(void);

void 	api_watchdog_reload(void);
uint8_t api_watchdog_buzzer_rd(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
