/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	csp_STM32_uart5.c
 * Date Created :	Thu, 05 Jan 2012  04:35:33 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/


/**********************************************************************************************************
 *	INCLUDE FILES
 **********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "pcb_uart.h"





#if( UART5_EN == 1)
#include "pcb_pins.h"
#include "pcb_uart5.h"
#include "csp_STM32_uart.h"
#include "csp_STM32_uart5.h"

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"



/**********************************************************************************************************
 *	GLOBALS VARIABLES
 **********************************************************************************************************/
uint8_t    uart5_rx_buffer_a_u8_glb[UART5_RX_BUFFER_SIZE];
uint8_t    uart5_tx_buffer_a_u8_glb[UART5_TX_BUFFER_SIZE];

t_serial_st		st_uart5_rx_glb;
t_serial_st		st_uart5_tx_glb;


/**********************************************************************************************************
 **********************************************************************************************************/



/*************************************************************************************************
 * Function Name :		uart5_config
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart5_config(uint32_t baud_rate_u32)
{
/* Local Variables */
	USART_InitTypeDef	USART_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;

/* Code */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);			/* Enable high speed APB2 clock 72MHz */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);								/* UART5 Periph clock enable */


	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12;										/* Configure UART5 TX pin */
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2;										/* Configure UART5 RX pin */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

#if( UART5_TX_FLOW_CTS_EN == 1)															/* Enable UART5 CTS pin */
	GPIO_InitStructure.GPIO_Pin		= UART5_CTS_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;									/* Input Pull-Down */
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(UART5_CTS_PORT, &GPIO_InitStructure);
#endif
#if( UART5_RX_FLOW_RTS_EN == 1)															/* Enable UART5 RTS pin */
	GPIO_InitStructure.GPIO_Pin     = UART5_RTS_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP; 								/* Push-Pull Output */
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(UART5_RTS_PORT, &GPIO_InitStructure);

	mCSP_UART5_RTS_PIN_GO;			//initialise the RTS pin as go
#endif



	/* Configure USART Parameters */
	USART_InitStructure.USART_BaudRate 				= baud_rate_u32;					/* BaudRate = eg 57600 baud */
	USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;				/* Word Length = 8 Bits */
	USART_InitStructure.USART_StopBits 				= USART_StopBits_1;					/* One Stop Bit */
	USART_InitStructure.USART_Parity 				= USART_Parity_No;					/* No parity */
	USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;	/* Hardware flow control disabled (RTS and CTS signals)*/
	USART_InitStructure.USART_Mode 					= USART_Mode_Rx | USART_Mode_Tx; 	/* Receive and transmit enabled */
	USART_Init(UART5, &USART_InitStructure);											/* USART configuration */
	USART_Cmd(UART5, ENABLE);															/* Enable USARTx Peripheral */




#if( UART5_RX_BUF_EN == 1)
	uart5_init_rx_buf();
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);		/* Enable the UART_COM1 Receive interrupt: this interrupt is generated when the UART_COM1 receive data register is not empty */
#endif

#if( UART5_TX_BUF_EN == 1)
	uart5_init_tx_buf();
	// the interrupt is enabled when first byte is txed
#endif

/* Enable the USARTx Interupt */
#if (UART5_TX_BUF_EN == 1) || (UART5_RX_BUF_EN == 1)
	NVIC_InitTypeDef 	NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel 						=  UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=  UART5_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  UART5_IRQ_SUB_PRI;
	NVIC_InitStructure.NVIC_IRQChannelCmd					=  ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif


	return;

}

/*************************************************************************************************
 * Function Name :		uart5_init_rx_buf
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart5_init_rx_buf(void)
{
    /* Code */
	st_uart5_rx_glb.wr_index_u8				=  0;
	st_uart5_rx_glb.rd_index_u8				=  0;
	st_uart5_rx_glb.cnt_u8					=  0;
	st_uart5_rx_glb.flags.bits.overflow		=  0;
	st_uart5_rx_glb.flags.bits.empty		=  1;		//need to flag hi to show buf is empty
	st_uart5_rx_glb.flags.bits.flow_stop	=  0;
	st_uart5_rx_glb.timeout_timer_u16		=  0;
	memset(uart5_rx_buffer_a_u8_glb,0,sizeof uart5_rx_buffer_a_u8_glb);

	return;

}


/*************************************************************************************************
 * Function Name :		uart5_init_tx_buf
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart5_init_tx_buf(void)
{
/* Code */
	st_uart5_tx_glb.wr_index_u8				=  0;
	st_uart5_tx_glb.rd_index_u8				=  0;
	st_uart5_tx_glb.cnt_u8					=  0;
	st_uart5_tx_glb.flags.bits.overflow		=  0;
	st_uart5_tx_glb.flags.bits.empty		=  1;
	st_uart5_tx_glb.flags.bits.flow_stop	=  0;
	st_uart5_tx_glb.timeout_timer_u16		=  0;
	memset(uart5_tx_buffer_a_u8_glb,0,sizeof uart5_tx_buffer_a_u8_glb);

	return;

}


/*************************************************************************************************
 * Function Name :		uart5_IRQ
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart5_IRQ(void)
{
/* Local Variables */
  uint16_t rec_char_u16;

/* Code */
    if(USART_GetITStatus(UART5, USART_IT_FE) != RESET){
        USART_ClearITPendingBit(UART5, USART_IT_FE);
       	rec_char_u16 = USART_ReceiveData(UART5);
    }

    if(USART_GetITStatus(UART5, USART_IT_PE) != RESET){
        while(USART_GetFlagStatus(UART5, USART_FLAG_RXNE) == RESET){
        }
        USART_ClearITPendingBit(UART5, USART_IT_PE);
        rec_char_u16 = USART_ReceiveData(UART5);
    }

    if(USART_GetITStatus(UART5, USART_IT_ORE) != RESET){
        USART_ClearITPendingBit(UART5, USART_IT_ORE);
       	rec_char_u16 = USART_ReceiveData(UART5);
    }

    if(USART_GetITStatus(UART5, USART_IT_NE) != RESET){
        USART_ClearITPendingBit(UART5, USART_IT_NE);
        rec_char_u16 = USART_ReceiveData(UART5);
    }

	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET){
		rec_char_u16 = USART_ReceiveData(UART5);
		if(st_uart5_rx_glb.cnt_u8 < UART5_RX_BUFFER_SIZE){
			uart5_rx_buffer_a_u8_glb[st_uart5_rx_glb.wr_index_u8] = (uint8_t) rec_char_u16;
		   	if (++st_uart5_rx_glb.wr_index_u8 == UART5_RX_BUFFER_SIZE){
				st_uart5_rx_glb.wr_index_u8			=  0;
			}
		   	if (++st_uart5_rx_glb.cnt_u8 == UART5_RX_BUFFER_SIZE){
		      	st_uart5_rx_glb.flags.bits.overflow	=  1;
		   	}
		}
		else{
			st_uart5_rx_glb.flags.bits.overflow	=  1;
		}
#if( UART5_RX_FLOW_RTS_EN == 1)											/* Check if RX flow control enabled */
		if(st_uart5_rx_glb.cnt_u8 >= UART5_RTS_STOP_TRIG){
			st_uart5_rx_glb.flags.bits.flow_stop	=  1;
			mCSP_UART5_RTS_PIN_STOP;
		}
#endif
	}


	if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET){
		if(st_uart5_tx_glb.cnt_u8 > 0){
#if( UART5_TX_FLOW_CTS_EN == 1)
			if(mCSP_UART5_CTS_READ != UART5_CTS_POLARITY_GO){
				USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
				st_uart5_tx_glb.flags.bits.flow_stop	=  1;
			}
			else{
#endif
				st_uart5_tx_glb.flags.bits.empty		=  0;

				st_uart5_tx_glb.cnt_u8--;
				USART_SendData(UART5,uart5_tx_buffer_a_u8_glb[st_uart5_tx_glb.rd_index_u8]);
				if (++st_uart5_tx_glb.rd_index_u8 == UART5_TX_BUFFER_SIZE){
					st_uart5_tx_glb.rd_index_u8			=  0;
				}
#if( UART5_TX_FLOW_CTS_EN == 1)
			}
#endif
		}
		else{
			st_uart5_tx_glb.flags.bits.empty		=  1;
			USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
		}
	}

    return;
}

/*************************************************************************************************
 * Function Name :		uart5_timer_control
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart5_timer_control(void)
{
	if(st_uart5_rx_glb.timeout_timer_u16){
		st_uart5_rx_glb.timeout_timer_u16--;
	}
	if(st_uart5_tx_glb.timeout_timer_u16){
		st_uart5_tx_glb.timeout_timer_u16--;
	}

#if( UART5_TX_BUF_EN == 1)
	if(st_uart5_tx_glb.flags.bits.flow_stop == 1){				//CTS
		if(mCSP_UART5_CTS_READ == UART5_CTS_POLARITY_GO){
			USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
			st_uart5_tx_glb.flags.bits.flow_stop	=  0;
		}
	}
#endif
	return;
}


/*************************************************************************************************
 * Function Name :		uart5_getchar
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
uint8_t uart5_getchar(uint16_t wait_time_u16,uint8_t *rec_status_u8)
{
/* Local Variables */
	uint8_t 		data_byte_u8 	= 0;


	/* Initialise variable */
	*rec_status_u8	= 0;
	st_uart5_rx_glb.timeout_timer_u16	=  wait_time_u16;
	while(	(st_uart5_rx_glb.timeout_timer_u16 > 0) && (st_uart5_rx_glb.cnt_u8 == 0)	){

	}

	if(st_uart5_rx_glb.cnt_u8 != 0){
		data_byte_u8		= uart5_rx_buffer_a_u8_glb[st_uart5_rx_glb.rd_index_u8];
		*rec_status_u8		= 1;

		if (++st_uart5_rx_glb.rd_index_u8 == UART5_RX_BUFFER_SIZE){
			st_uart5_rx_glb.rd_index_u8=0;
		}
		USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);
		--st_uart5_rx_glb.cnt_u8;
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

#if( UART5_RX_FLOW_RTS_EN == 1)										/* Check if RX flow control enabled */
		if(st_uart5_rx_glb.cnt_u8 <= UART5_RTS_GO_TRIG){
			st_uart5_rx_glb.flags.bits.flow_stop	=  0;
			mCSP_UART5_RTS_PIN_GO;
		}
#endif

	}

   	return(data_byte_u8);

}


/*************************************************************************************************
 * Function Name :		uart5_putchar
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
#if( UART5_TX_BUF_EN == 0)
void uart5_putchar(uint8_t tx_char_u8)
{
/* Local Variables */
	uint8_t 	CTS_status		=  UART5_CTS_POLARITY_GO;

/* Code */

#if( UART5_TX_FLOW_CTS_EN == 1)
	st_uart5_tx_glb.timeout_timer_u16 	= UART5_TX_TIMEOUT;
	do{
		CTS_status = mCSP_UART5_CTS_READ;
	}while( (CTS_status != UART5_CTS_POLARITY_GO) && (st_uart5_tx_glb.timeout_timer_u16 ));
#endif

	/* Transmit data */
	if(CTS_status == UART5_CTS_POLARITY_GO){
		USART_SendData(UART5, (uint8_t) tx_char_u8);
		while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET){
		}
	}

	return;
}

#endif

/*************************************************************************************************
 * Function Name :		uart5_putchar
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
#if( UART5_TX_BUF_EN == 1)
void uart5_putchar(uint8_t tx_char_u8)
{
/* Local Variables */
	uint8_t 	CTS_status		=  UART5_CTS_POLARITY_GO;




#if( UART5_TX_FLOW_CTS_EN == 1)
	st_uart5_tx_glb.timeout_timer_u16 	= UART5_TX_TIMEOUT;
	do{
		CTS_status = mCSP_USART5_CTS_READ;					/* Read state of CTS pin */
	}while( (CTS_status != UART5_CTS_POLARITY_GO) && (st_uart5_tx_glb.timeout_timer_u16));
#endif

	while(st_uart5_tx_glb.cnt_u8 >= UART5_TX_BUFFER_SIZE){
		//wait until buffer is reduced
	}


	if(st_uart5_tx_glb.cnt_u8 < UART5_TX_BUFFER_SIZE){
		uart5_tx_buffer_a_u8_glb[st_uart5_tx_glb.wr_index_u8]	=  tx_char_u8;
		if (++st_uart5_tx_glb.wr_index_u8 == UART5_TX_BUFFER_SIZE){
			st_uart5_tx_glb.wr_index_u8	=  0;
		}
		USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
		st_uart5_tx_glb.cnt_u8++;
		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
		if (st_uart5_tx_glb.cnt_u8 == UART5_TX_BUFFER_SIZE){
		   	st_uart5_tx_glb.flags.bits.overflow	=  1;
		}
	}
	else{
		st_uart5_tx_glb.flags.bits.overflow	=  1;
	}

	//if buffer was empty then we need to restart interupt
	if(st_uart5_tx_glb.flags.bits.empty	==  1){
		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
	}

	return;
}
#endif


#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


