/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	pcb_uart2.h
 * Date Created :	Tue, 03 Jan 2012  04:59:03 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/
#ifndef _PCB_UART2_H
#define _PCB_UART2_H



#define	UART2_RX_BUF_EN				1		/* this should always be enabled */
#define	UART2_TX_BUF_EN				0

#define UART2_RX_FLOW_RTS_EN		1
#define UART2_TX_FLOW_CTS_EN		0

#define UART2_RTS_POLARITY_GO		0
#define UART2_CTS_POLARITY_GO		0

#define	UART2_BAUD					115200

#define	UART2_RX_BUFFER_SIZE		500	/*max 65535*/
#define	UART2_RTS_STOP_TRIG			450
#define	UART2_RTS_GO_TRIG			30

#define	UART2_TX_BUFFER_SIZE		64	/*max 255*/
#define UART2_TX_TIMEOUT 			1000

#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


