/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	pcb_uart1.h
 * Date Created :	Tue, 03 Jan 2012  04:58:33 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/
#ifndef _PCB_UART1_H
#define _PCB_UART1_H



#define	UART1_RX_BUF_EN			1		/* this should always be enabled */
#define	UART1_TX_BUF_EN			0

#define	UART1_RX_FLOW_RTS_EN		1
#define	UART1_TX_FLOW_CTS_EN		0

#define	UART1_RTS_POLARITY_GO		0
#define	UART1_CTS_POLARITY_GO		0

#define	UART1_BAUD				115200

#define	UART1_RX_BUFFER_SIZE		255	/*max 255*/
#define	UART1_RTS_STOP_TRIG			240
#define	UART1_RTS_GO_TRIG			30

#define	UART1_TX_BUFFER_SIZE		64	/*max 255*/
#define	UART1_TX_TIMEOUT 			20000

#define	UART1_HOOK				0

#if( UART1_HOOK == 1 )
void pcb_uart1_irq_rx_hook (void);
#endif

#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


