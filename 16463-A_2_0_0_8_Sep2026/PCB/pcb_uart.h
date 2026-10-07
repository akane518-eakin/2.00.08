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
 * Filename    :  pcb_uart.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/
#ifndef _PCB_UART_H
#define _PCB_UART_H

/* enabling these bits will allow the uart to be compiled & initialised at start up */
#define UART1_EN		0	//not used
#define UART2_EN		1	//USB
#define UART3_EN		0	//not used
#define UART4_EN		1	//O2 sensor
#define UART5_EN		0	//not used


#define	UART_PRINTF_PORT			2	//2	/* Which port does printf & scanf use */
#define	DUMP_PRINTF_PORT			100



#endif

