/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	pcb_uart3.h
 * Date Created :	Tue, 03 Jan 2012  04:59:15 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/
#ifndef _PCB_UART3_H
#define _PCB_UART3_H



#define	UART3_RX_BUF_EN				1		/* this should always be enabled */
#define	UART3_TX_BUF_EN				1

#define UART3_RX_FLOW_RTS_EN		0
#define UART3_TX_FLOW_CTS_EN		0

#define UART3_RTS_POLARITY_GO		0
#define UART3_CTS_POLARITY_GO		0

#define	UART3_BAUD					115200

#define	UART3_RX_BUFFER_SIZE		255	/*max 255*/
#define	UART3_RTS_STOP_TRIG			60
#define	UART3_RTS_GO_TRIG			30

#define	UART3_TX_BUFFER_SIZE		255	/*max 255*/
#define UART3_TX_TIMEOUT 			1000

#define UART3_HOOK 					0

#if( UART3_HOOK == 1 )
void pcb_uart3_irq_rx_hook (void);
#endif

#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


