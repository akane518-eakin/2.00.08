/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	csp_STM32_uart5.h
 * Date Created :	Thu, 05 Jan 2012  04:46:11 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/
#ifndef _CSP_STM32_UART5_H
#define _CSP_STM32_UART5_H

/**********************************************************************************************************
 *	NOTES
 **********************************************************************************************************
#define mCSP_USART5_RTS_PIN_GO						GPIO_SetBits(			UART5_RTS_PORT, UART5_RTS_PIN)
#define mCSP_USART5_RTS_PIN_STOP					GPIO_ResetBits(			UART5_RTS_PORT, UART5_RTS_PIN)
#define mCSP_USART5_CTS_READ						GPIO_ReadInputDataBit(	UART5_CTS_PORT, UART5_CTS_PIN)


**********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 **********************************************************************************************************/
#include "pcb_uart5.h"
#include "pcb_pins.h"

/**********************************************************************************************************
 *  PRECOMPILE FUNCTION DEFINES
 **********************************************************************************************************/
#ifndef UART5_RX_BUF_EN
#error	UART5_RX_BUF_EN not defined in pcb_uart5.h
#endif
#ifndef UART5_TX_BUF_EN
#error	UART5_TX_BUF_EN not defined in pcb_uart5.h
#endif
#ifndef UART5_RX_FLOW_RTS_EN
#error	UART5_RX_FLOW_RTS_EN not defined in pcb_uart5.h
#endif
#ifndef UART5_TX_FLOW_CTS_EN
#error	UART5_TX_FLOW_CTS_EN not defined in pcb_uart5.h
#endif
#ifndef UART5_BAUD
#error	UART5_BAUD not defined in pcb_uart5.h
#endif

#if( UART5_RX_BUF_EN == 1)
	#ifndef UART5_RX_BUFFER_SIZE
	#error	UART5_RX_BUFFER_SIZE not defined in pcb_uart5.h
	#endif
#endif

#if( UART5_RX_BUF_EN == 1)
	#ifndef UART5_TX_BUFFER_SIZE
	#error	UART5_TX_BUFFER_SIZE not defined in pcb_uart5.h
	#endif
	#ifndef UART5_TX_TIMEOUT
	#error	UART5_TX_TIMEOUT not defined in pcb_uart5.h
	#endif
#endif

#if( UART5_RX_FLOW_RTS_EN == 1)
	#ifndef UART5_RTS_STOP_TRIG
	#error	UART5_RTS_STOP_TRIG not defined in pcb_uart5.h
	#endif
	#ifndef UART5_RTS_GO_TRIG
	#error	UART5_RTS_GO_TRIG not defined in pcb_uart5.h
	#endif
	#ifndef UART5_RTS_PORT
	#error  UART5_RTS_PORT not defined in pcb_pins.h
	#endif
	#ifndef UART5_RTS_PIN
	#error  UART5_RTS_PIN not defined in pcb_pins.h
	#endif
	#ifndef mCSP_UART5_RTS_PIN_HI
	#error  mCSP_UART5_RTS_PIN_HI not defined in pcb_pins.h
	#endif
	#ifndef mCSP_UART5_RTS_PIN_LOW
	#error  mCSP_UART5_RTS_PIN_LOW not defined in pcb_pins.h
	#endif
#endif

#if( UART5_TX_FLOW_CTS_EN == 1)
	#ifndef UART5_CTS_PORT
	#error  UART5_CTS_PORT not defined in pcb_pins.h
	#endif
	#ifndef UART5_CTS_PIN
	#error  UART5_CTS_PIN not defined in pcb_pins.h
	#endif
	#ifndef mCSP_UART5_CTS_READ
	#error  mCSP_UART5_CTS_READ not defined in pcb_pins.h
	#endif
#endif

#if(UART5_RTS_POLARITY_GO == 1)
#define mCSP_UART5_RTS_PIN_GO					mCSP_UART5_RTS_PIN_HI
#define mCSP_UART5_RTS_PIN_STOP					mCSP_UART5_RTS_PIN_LOW
#else
#define mCSP_UART5_RTS_PIN_GO					mCSP_UART5_RTS_PIN_LOW
#define mCSP_UART5_RTS_PIN_STOP					mCSP_UART5_RTS_PIN_HI
#endif

/**********************************************************************************************************
 *  DEFINES
 **********************************************************************************************************/
#define UART5_IRQ_PREM_PRI			0
#define UART5_IRQ_SUB_PRI			3

/**********************************************************************************************************
 *  VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 *	FUNCTION PROTOTYPES
 **********************************************************************************************************/
void	uart5_config(uint32_t baud_rate_u32);

void	uart5_init_rx_buf(void);
void 	uart5_init_tx_buf(void);

uint8_t uart5_getchar(uint16_t wait_time_u16,uint8_t *rec_status_u8);
void	uart5_putchar(uint8_t tx_char_u8);

void	uart5_IRQ(void);
void 	uart5_timer_control(void);

#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


