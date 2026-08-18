/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	csp_STM32_uart2.h
 * Date Created :	Thu, 05 Jan 2012  04:33:54 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/
#ifndef _CSP_STM32_UART2_H
#define _CSP_STM32_UART2_H

/**********************************************************************************************************
 *	NOTES
 **********************************************************************************************************
#define mCSP_USART2_RTS_PIN_HI						GPIO_SetBits(			UART2_RTS_PORT, UART2_RTS_PIN)
#define mCSP_USART2_RTS_PIN_LOW						GPIO_ResetBits(			UART2_RTS_PORT, UART2_RTS_PIN)
#define mCSP_USART2_CTS_READ						GPIO_ReadInputDataBit(	UART2_CTS_PORT, UART2_CTS_PIN)


**********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 **********************************************************************************************************/
#include "pcb_uart2.h"
#include "pcb_pins.h"

/**********************************************************************************************************
 *  PRECOMPILE FUNCTION DEFINES
 **********************************************************************************************************/
#ifndef UART2_RX_BUF_EN
#error	UART2_RX_BUF_EN not defined in pcb_uart2.h
#endif
#ifndef UART2_TX_BUF_EN
#error	UART2_TX_BUF_EN not defined in pcb_uart2.h
#endif
#ifndef UART2_RX_FLOW_RTS_EN
#error	UART2_RX_FLOW_RTS_EN not defined in pcb_uart2.h
#endif
#ifndef UART2_TX_FLOW_CTS_EN
#error	UART2_TX_FLOW_CTS_EN not defined in pcb_uart2.h
#endif
#ifndef UART2_BAUD
#error	UART2_BAUD not defined in pcb_uart2.h
#endif

#if( UART2_RX_BUF_EN == 1)
	#ifndef UART2_RX_BUFFER_SIZE
	#error	UART2_RX_BUFFER_SIZE not defined in pcb_uart2.h
	#endif
#endif

#if( UART2_RX_BUF_EN == 1)
	#ifndef UART2_TX_BUFFER_SIZE
	#error	UART2_TX_BUFFER_SIZE not defined in pcb_uart2.h
	#endif
	#ifndef UART2_TX_TIMEOUT
	#error	UART2_TX_TIMEOUT not defined in pcb_uart2.h
	#endif
#endif

#if( UART2_RX_FLOW_RTS_EN == 1)
	#ifndef UART2_RTS_STOP_TRIG
	#error	UART2_RTS_STOP_TRIG not defined in pcb_uart2.h
	#endif
	#ifndef UART2_RTS_GO_TRIG
	#error	UART2_RTS_GO_TRIG not defined in pcb_uart2.h
	#endif
	#ifndef UART2_RTS_PORT
	#error  UART2_RTS_PORT not defined in pcb_pins.h
	#endif
	#ifndef UART2_RTS_PIN
	#error  UART2_RTS_PIN not defined in pcb_pins.h
	#endif
	#ifndef mCSP_USART2_RTS_PIN_HI
	#error  mCSP_USART2_RTS_PIN_HI not defined in pcb_pins.h
	#endif
	#ifndef mCSP_USART2_RTS_PIN_LOW
	#error  mCSP_USART2_RTS_PIN_LOW not defined in pcb_pins.h
	#endif
#endif

#if( UART2_TX_FLOW_CTS_EN == 1)
	#ifndef UART2_CTS_PORT
	#error  UART2_CTS_PORT not defined in pcb_pins.h
	#endif
	#ifndef UART2_CTS_PIN
	#error  UART2_CTS_PIN not defined in pcb_pins.h
	#endif
	#ifndef mCSP_USART2_CTS_READ
	#error  mCSP_USART2_CTS_READ not defined in pcb_pins.h
	#endif
#endif

#if(UART2_RTS_POLARITY_GO == 1)
#define mCSP_USART2_RTS_PIN_GO						mCSP_USART2_RTS_PIN_HI
#define mCSP_USART2_RTS_PIN_STOP					mCSP_USART2_RTS_PIN_LOW
#else
#define mCSP_USART2_RTS_PIN_GO						mCSP_USART2_RTS_PIN_LOW
#define mCSP_USART2_RTS_PIN_STOP					mCSP_USART2_RTS_PIN_HI
#endif

/**********************************************************************************************************
 *  DEFINES
 **********************************************************************************************************/
#define USART2_IRQ_PREM_PRI			0
#define USART2_IRQ_SUB_PRI			0

/**********************************************************************************************************
 *  VARIABLES
 **********************************************************************************************************/

/**********************************************************************************************************
 *	FUNCTION PROTOTYPES
 **********************************************************************************************************/
void	uart2_config(uint32_t baud_rate_u32);

void	uart2_init_rx_buf(void);
void 	uart2_init_tx_buf(void);

uint8_t uart2_getchar(uint16_t wait_time_u16,uint8_t *rec_status_u8);
void	uart2_putchar(uint8_t tx_char_u8);

void	uart2_IRQ(void);
void 	uart2_timer_control(void);

#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


