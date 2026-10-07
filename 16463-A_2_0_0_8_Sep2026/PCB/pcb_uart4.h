/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	pcb_uart4.h
 * Date Created :	Tue, 03 Jan 2012  04:59:30 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/
#ifndef _PCB_UART4_H
#define _PCB_UART4_H



#define	UART4_RX_BUF_EN				1		/* this should always be enabled */
#define	UART4_TX_BUF_EN				0

#define UART4_RX_FLOW_RTS_EN		0
#define UART4_TX_FLOW_CTS_EN		0

#define UART4_RTS_POLARITY_GO		0
#define UART4_CTS_POLARITY_GO		0

#define	UART4_BAUD					19200

#define	UART4_RX_BUFFER_SIZE		255	/*max 255*/
#define	UART4_RTS_STOP_TRIG			60
#define	UART4_RTS_GO_TRIG			30

#define	UART4_TX_BUFFER_SIZE		255	/*max 255*/
#define UART4_TX_TIMEOUT 			1000


#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


