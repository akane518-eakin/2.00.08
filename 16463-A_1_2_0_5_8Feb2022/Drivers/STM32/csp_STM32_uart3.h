/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	csp_STM32_uart3.h
 * Date Created :	Thu, 05 Jan 2012  04:33:47 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/
#ifndef _CSP_STM32_UART3_H
#define _CSP_STM32_UART3_H

/**********************************************************************************************************
 *	NOTES
 **********************************************************************************************************
#define mCSP_USART3_RTS_PIN_GO						GPIO_SetBits(			UART3_RTS_PORT, UART3_RTS_PIN)
#define mCSP_USART3_RTS_PIN_STOP					GPIO_ResetBits(			UART3_RTS_PORT, UART3_RTS_PIN)
#define mCSP_USART3_CTS_READ						GPIO_ReadInputDataBit(	UART3_CTS_PORT, UART3_CTS_PIN)


**********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 **********************************************************************************************************/
#include "pcb_uart3.h"
#include "pcb_pins.h"

/**********************************************************************************************************
 *  PRECOMPILE FUNCTION DEFINES
 **********************************************************************************************************/
#ifndef UART3_RX_BUF_EN
#error	UART3_RX_BUF_EN not defined in pcb_uart3.h
#endif
#ifndef UART3_TX_BUF_EN
#error	UART3_TX_BUF_EN not defined in pcb_uart3.h
#endif
#ifndef UART3_RX_FLOW_RTS_EN
#error	UART3_RX_FLOW_RTS_EN not defined in pcb_uart3.h
#endif
#ifndef UART3_TX_FLOW_CTS_EN
#error	UART3_TX_FLOW_CTS_EN not defined in pcb_uart3.h
#endif
#ifndef UART3_BAUD
#error	UART3_BAUD not defined in pcb_uart3.h
#endif

#if( UART3_RX_BUF_EN == 1)
	#ifndef UART3_RX_BUFFER_SIZE
	#error	UART3_RX_BUFFER_SIZE not defined in pcb_uart3.h
	#endif
#endif

#if( UART3_RX_BUF_EN == 1)
	#ifndef UART3_TX_BUFFER_SIZE
	#error	UART3_TX_BUFFER_SIZE not defined in pcb_uart3.h
	#endif
	#ifndef UART3_TX_TIMEOUT
	#error	UART3_TX_TIMEOUT not defined in pcb_uart3.h
	#endif
#endif

#if( UART3_RX_FLOW_RTS_EN == 1)
	#ifndef UART3_RTS_STOP_TRIG
	#error	UART3_RTS_STOP_TRIG not defined in pcb_uart3.h
	#endif
	#ifndef UART3_RTS_GO_TRIG
	#error	UART3_RTS_GO_TRIG not defined in pcb_uart3.h
	#endif
	#ifndef UART3_RTS_PORT
	#error  UART3_RTS_PORT not defined in pcb_pins.h
	#endif
	#ifndef UART3_RTS_PIN
	#error  UART3_RTS_PIN not defined in pcb_pins.h
	#endif
	#ifndef mCSP_USART3_RTS_PIN_HI
	#error  mCSP_USART3_RTS_PIN_HI not defined in pcb_pins.h
	#endif
	#ifndef mCSP_USART3_RTS_PIN_LOW
	#error  mCSP_USART3_RTS_PIN_LOW not defined in pcb_pins.h
	#endif
#endif

#if( UART3_TX_FLOW_CTS_EN == 1)
	#ifndef UART3_CTS_PORT
	#error  UART3_CTS_PORT not defined in pcb_pins.h
	#endif
	#ifndef UART3_CTS_PIN
	#error  UART3_CTS_PIN not defined in pcb_pins.h
	#endif
	#ifndef mCSP_USART3_CTS_READ
	#error  mCSP_USART3_CTS_READ not defined in pcb_pins.h
	#endif
#endif

#if(UART3_RTS_POLARITY_GO == 1)
#define mCSP_USART3_RTS_PIN_GO						mCSP_USART3_RTS_PIN_HI
#define mCSP_USART3_RTS_PIN_STOP					mCSP_USART3_RTS_PIN_LOW
#else
#define mCSP_USART3_RTS_PIN_GO						mCSP_USART3_RTS_PIN_LOW
#define mCSP_USART3_RTS_PIN_STOP					mCSP_USART3_RTS_PIN_HI
#endif

/**********************************************************************************************************
 *  DEFINES
 **********************************************************************************************************/
#define USART3_IRQ_PREM_PRI			0
#define USART3_IRQ_SUB_PRI			3

/**********************************************************************************************************
 *  VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 *	FUNCTION PROTOTYPES
 **********************************************************************************************************/
void	uart3_config(uint32_t baud_rate_u32);

void	uart3_init_rx_buf(void);
void 	uart3_init_tx_buf(void);

uint8_t uart3_getchar(uint16_t wait_time_u16,uint8_t *rec_status_u8);
void	uart3_putchar(uint8_t tx_char_u8);

void	uart3_IRQ(void);
void 	uart3_timer_control(void);

#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


