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
 * Filename    :  pcb_timer.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/
#ifndef _PCB_TIMER_H
#define _PCB_TIMER_H

/* enabling these bits will allow the uart to be compiled & initialised at start up */
#define TIMER1_EN		1	//75Hz timer
#define TIMER1_CH1_EN	1	//LED_PWR	75Hz 256 resolution
#define TIMER1_CH2_EN	1	//LED_BAT_R	75Hz 256 resolution
#define TIMER1_CH3_EN	1	//LED_BAT_G	75Hz 256 resolution
#define TIMER1_CH4_EN	1	//LED_ALARM	75Hz 256 resolution
#define TIMER1_INT_EN	1

#define TIMER2_EN		1	//8kHz	Audio
#define TIMER2_CH1_EN	0
#define TIMER2_CH2_EN	0
#define TIMER2_CH3_EN	0
#define TIMER2_CH4_EN	0
#define TIMER2_INT_EN	1	//audio interrupt

#define TIMER3_EN		1	//36kHz
#define TIMER3_CH1_EN	0
#define TIMER3_CH2_EN	0
#define TIMER3_CH3_EN	1	//fan pwm 							36kHz 100 resolution
#define TIMER3_CH4_EN	0
#define TIMER3_INT_EN	1

#define TIMER4_EN		1	//100Hz
#define TIMER4_CH1_EN	0
#define TIMER4_CH2_EN	0
#define TIMER4_CH3_EN	0
#define TIMER4_CH4_EN	0
#define TIMER4_INT_EN	1	//pneumatics ADC sample freq

#define TIMER8_EN		1	//100kHz
#define TIMER8_CH1_EN	1	//Proportional valve -Air			100Hz 1000 resolution
#define TIMER8_CH2_EN	1	//Proportional valve -O2			100Hz 1000 resolution
#define TIMER8_CH3_EN	1	//Proportional valve -Venturi		100Hz 1000 resolution
#define TIMER8_CH4_EN	0
#define TIMER8_INT_EN	0	//update the pwm values

#endif

