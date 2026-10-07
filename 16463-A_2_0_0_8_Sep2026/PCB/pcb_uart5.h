/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	pcb_uart5.h
 * Date Created :	Tue, 03 Jan 2012  04:59:42 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/
#ifndef _PCB_UART5_H
#define _PCB_UART5_H



#define	UART5_RX_BUF_EN				1		/* this should always be enabled */
#define	UART5_TX_BUF_EN				0

#define UART5_RX_FLOW_RTS_EN		1
#define UART5_TX_FLOW_CTS_EN		0

#define UART5_RTS_POLARITY_GO		0
#define UART5_CTS_POLARITY_GO		0

#define	UART5_BAUD					115200

#define	UART5_RX_BUFFER_SIZE		255	/*max 255*/
#define	UART5_RTS_STOP_TRIG			200
#define	UART5_RTS_GO_TRIG			150

#define	UART5_TX_BUFFER_SIZE		255	/*max 255*/
#define UART5_TX_TIMEOUT 			1000

#define UART5_HOOK 					0

#if( UART5_HOOK == 1 )
void pcb_uart5_irq_rx_hook (void);
#endif

#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


