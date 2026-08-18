/*
*********************************************************************************************************
*                                           Marturion Ltd
*
*                                           Knockmore Hill Business Park
*                                           9 Ferguson Drive
*                                           Lisburn
*                                           Co. Antrim
*                                           Northern Ireland
*                                           BT28 2EX
*
*
*                  	Copyright 2012,Marturion Ltd, Lisburn, Co. Antrim, Northern Ireland
*                                          All Rights Reserved
*
*                                          csp_STM32_uart4.h
*
* Filename    :  csp_STM32_uart4.h
* Programmer  :  Pauric Lynch
* Description :  
* Compiler    :  IAR
* Target      :  STM32F103
* Version     :  Version 1.1.0
**********************************************************************************************************
*/
#ifndef _csp_STM32_uart4_H
#define _csp_STM32_uart4_H

/*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************/
#include "pcb_uart4.h"
#include "pcb_pins.h"

/*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************/

#define UART4_IRQ_PREM_PRI			0
#define UART4_IRQ_SUB_PRI			1
/*********************************************************************************************************
*                                           VARIABLES
*********************************************************************************************************/


/*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************/
void uart4_IRQ(void);
void uart4_1mS_timer(void);
void uart4_init_rx_buf(void);
void uart4_init_tx_buf(void);
uint8_t uart4_bytes2read(void);
void uart4_reset_counters(void);
void uart4_putchar(uint8_t tx_char_u8);
void uart4_config(uint32_t baud_rate_u32);
uint8_t uart4_getchar(uint16_t wait_time_u16,uint8_t *rec_status_u8);

#endif    //_csp_STM32_uart4_H
/*********************************************************************************************************
*                                           End of csp_STM32_uart4.h
*********************************************************************************************************/