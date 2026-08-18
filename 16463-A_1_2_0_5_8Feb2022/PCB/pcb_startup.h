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
 * Filename    :  pcb_startup.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/
#ifndef _PCB_STARTUP_H
#define _PCB_STARTUP_H

#define	INT_VECT_OFFSET		0x5000

#define	STARTUP_BASIC	0
#define	STARTUP_FULLY	1

/**********************************************************************************************************
 *	FUNCTION PROTOTYPES
 **********************************************************************************************************/
void pcb_startup(uint8_t mode);




#endif

