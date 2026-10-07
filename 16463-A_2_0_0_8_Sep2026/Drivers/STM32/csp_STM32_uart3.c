/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	csp_STM32_uart3.c
 * Date Created :	Thu, 05 Jan 2012  04:30:59 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :
 *
 **********************************************************************************************************/


/**********************************************************************************************************
 *	INCLUDE FILES
 **********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "pcb_uart.h"





#if( UART3_EN == 1)
#include "pcb_pins.h"
#include "pcb_uart3.h"
#include "csp_STM32_uart.h"
#include "csp_STM32_uart3.h"

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"



/**********************************************************************************************************
 *	GLOBALS VARIABLES
 **********************************************************************************************************/
uint8_t    uart3_rx_buffer_a_u8_glb[UART3_RX_BUFFER_SIZE];
uint8_t    uart3_tx_buffer_a_u8_glb[UART3_TX_BUFFER_SIZE];

t_serial_st		st_uart3_rx_glb;
t_serial_st		st_uart3_tx_glb;


/**********************************************************************************************************
 **********************************************************************************************************/



/*************************************************************************************************
 * Function Name :		uart3_config
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart3_config(uint32_t baud_rate_u32)
{
/* Local Variables */
	USART_InitTypeDef	USART_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;

/* Code */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);			/* Enable GPIO Port B clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);								/* USART3 Periph clock enable */


	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;										/* Configure USART3 TX pin */
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_11;										/* Configure USART3 RX pin */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#if( UART3_TX_FLOW_CTS_EN == 1)															/* Enable USART3 CTS pin */
	GPIO_InitStructure.GPIO_Pin		= UART3_CTS_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;									/* Input Pull-Down */
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(UART3_CTS_PORT, &GPIO_InitStructure);
#endif
#if( UART3_RX_FLOW_RTS_EN == 1)															/* Enable USART3 RTS pin */
	GPIO_InitStructure.GPIO_Pin     = UART3_RTS_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP; 								/* Push-Pull Output */
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(UART3_RTS_PORT, &GPIO_InitStructure);

	mCSP_USART3_RTS_PIN_GO;			//initialise the RTS pin as go
#endif



	/* Configure USART Parameters */
	USART_InitStructure.USART_BaudRate 				= baud_rate_u32;					/* BaudRate = eg 57600 baud */
	USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;				/* Word Length = 8 Bits */
	USART_InitStructure.USART_StopBits 				= USART_StopBits_1;					/* One Stop Bit */
	USART_InitStructure.USART_Parity 				= USART_Parity_No;					/* No parity */
	USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;	/* Hardware flow control disabled (RTS and CTS signals)*/
	USART_InitStructure.USART_Mode 					= USART_Mode_Rx | USART_Mode_Tx; 	/* Receive and transmit enabled */
	USART_Init(USART3, &USART_InitStructure);											/* USART configuration */
	USART_Cmd(USART3, ENABLE);															/* Enable USARTx Peripheral */



#if( UART3_RX_BUF_EN == 1)
	uart3_init_rx_buf();
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		/* Enable the UART_COM3 Receive interrupt: this interrupt is generated when the UART_COM3 receive data register is not empty */
#endif

#if( UART3_TX_BUF_EN == 1)
	uart3_init_tx_buf();
	// the interrupt is enabled when first byte is txed
#endif

/* Enable the USARTx Interupt */
#if (UART3_TX_BUF_EN == 1) || (UART3_RX_BUF_EN == 1)
	NVIC_InitTypeDef 	NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel 						=  USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=  USART3_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  USART3_IRQ_SUB_PRI;
	NVIC_InitStructure.NVIC_IRQChannelCmd					=  ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif


	return;

}

/*************************************************************************************************
 * Function Name :		uart3_init_rx_buf
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart3_init_rx_buf(void)
{
/* Code */
	st_uart3_rx_glb.wr_index_u8				=  0;
	st_uart3_rx_glb.rd_index_u8				=  0;
	st_uart3_rx_glb.cnt_u8					=  0;
	st_uart3_rx_glb.flags.bits.overflow		=  0;
	st_uart3_rx_glb.flags.bits.empty		=  1;		//need to flag hi to show buf is empty
	st_uart3_rx_glb.flags.bits.flow_stop	=  0;
	st_uart3_rx_glb.timeout_timer_u16		=  0;
	memset(uart3_rx_buffer_a_u8_glb,0,sizeof uart3_rx_buffer_a_u8_glb);

	return;

}


/*************************************************************************************************
 * Function Name :		uart3_init_tx_buf
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart3_init_tx_buf(void)
{
/* Code */
	st_uart3_tx_glb.wr_index_u8				=  0;
	st_uart3_tx_glb.rd_index_u8				=  0;
	st_uart3_tx_glb.cnt_u8					=  0;
	st_uart3_tx_glb.flags.bits.overflow		=  0;
	st_uart3_tx_glb.flags.bits.empty		=  1;
	st_uart3_tx_glb.flags.bits.flow_stop	=  0;
	st_uart3_tx_glb.timeout_timer_u16		=  0;
	memset(uart3_tx_buffer_a_u8_glb,0,sizeof uart3_tx_buffer_a_u8_glb);

	return;

}


/*************************************************************************************************
 * Function Name :		uart3_IRQ
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart3_IRQ(void)
{
/* Local Variables */
  uint16_t rec_char_u16;

/* Code */
    if(USART_GetITStatus(USART3, USART_IT_FE) != RESET){
        USART_ClearITPendingBit(USART3, USART_IT_FE);
       	rec_char_u16 = USART_ReceiveData(USART3);
    }

    if(USART_GetITStatus(USART3, USART_IT_PE) != RESET){
        while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET){
        }
        USART_ClearITPendingBit(USART3, USART_IT_PE);
        rec_char_u16 = USART_ReceiveData(USART3);
    }

    if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET){
        USART_ClearITPendingBit(USART3, USART_IT_ORE);
       	rec_char_u16 = USART_ReceiveData(USART3);
    }

    if(USART_GetITStatus(USART3, USART_IT_NE) != RESET){
        USART_ClearITPendingBit(USART3, USART_IT_NE);
        rec_char_u16 = USART_ReceiveData(USART3);
    }

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		rec_char_u16 = USART_ReceiveData(USART3);
		if(st_uart3_rx_glb.cnt_u8 < UART3_RX_BUFFER_SIZE){
			uart3_rx_buffer_a_u8_glb[st_uart3_rx_glb.wr_index_u8] = (uint8_t) rec_char_u16;
		   	if (++st_uart3_rx_glb.wr_index_u8 == UART3_RX_BUFFER_SIZE){
				st_uart3_rx_glb.wr_index_u8			=  0;
			}
		   	if (++st_uart3_rx_glb.cnt_u8 == UART3_RX_BUFFER_SIZE){
		      	st_uart3_rx_glb.flags.bits.overflow	=  1;
		   	}
		}
		else{
			st_uart3_rx_glb.flags.bits.overflow	=  1;
		}
#if( UART3_RX_FLOW_RTS_EN == 1)											/* Check if RX flow control enabled */
		if(st_uart3_rx_glb.cnt_u8 >= UART3_RTS_STOP_TRIG){
			st_uart3_rx_glb.flags.bits.flow_stop	=  1;
			mCSP_USART3_RTS_PIN_STOP;
		}
#endif
#if( UART3_HOOK == 1)
        pcb_uart3_irq_rx_hook();
#endif
	}


	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET){
		if(st_uart3_tx_glb.cnt_u8 > 0){
#if( UART3_TX_FLOW_CTS_EN == 1)
			if(mCSP_USART3_CTS_READ != UART3_CTS_POLARITY_GO){
				USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
				st_uart3_tx_glb.flags.bits.flow_stop	=  1;
			}
			else{
#endif
				st_uart3_tx_glb.flags.bits.empty		=  0;

				st_uart3_tx_glb.cnt_u8--;
				USART_SendData(USART3,uart3_tx_buffer_a_u8_glb[st_uart3_tx_glb.rd_index_u8]);
				if (++st_uart3_tx_glb.rd_index_u8 == UART3_TX_BUFFER_SIZE){
					st_uart3_tx_glb.rd_index_u8			=  0;
				}
#if( UART1_TX_FLOW_CTS_EN == 1)
			}
#endif
		}
		else{
			st_uart3_tx_glb.flags.bits.empty		=  1;
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
	}

    return;
}

/*************************************************************************************************
 * Function Name :		uart3_timer_control
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart3_timer_control(void)
{
	if(st_uart3_rx_glb.timeout_timer_u16){
		st_uart3_rx_glb.timeout_timer_u16--;
	}
	if(st_uart3_tx_glb.timeout_timer_u16){
		st_uart3_tx_glb.timeout_timer_u16--;
	}

#if( UART3_TX_BUF_EN == 1)
	if(st_uart3_tx_glb.flags.bits.flow_stop == 1){				//CTS
		if(mCSP_USART3_CTS_READ == UART3_CTS_POLARITY_GO){
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
			st_uart3_tx_glb.flags.bits.flow_stop	=  0;
		}
	}
#endif
	return;
}


/*************************************************************************************************
 * Function Name :		uart3_getchar
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
uint8_t uart3_getchar(uint16_t wait_time_u16,uint8_t *rec_status_u8)
{
/* Local Variables */
	uint8_t 		data_byte_u8 	= 0;


	/* Initialise variable */
	*rec_status_u8	= 0;
	st_uart3_rx_glb.timeout_timer_u16	=  wait_time_u16;
	while(	(st_uart3_rx_glb.timeout_timer_u16 ) && (st_uart3_rx_glb.cnt_u8 == 0)	){

	}

	if(st_uart3_rx_glb.cnt_u8 != 0){
		data_byte_u8		= uart3_rx_buffer_a_u8_glb[st_uart3_rx_glb.rd_index_u8];
		*rec_status_u8		= 1;

		if (++st_uart3_rx_glb.rd_index_u8 == UART3_RX_BUFFER_SIZE){
			st_uart3_rx_glb.rd_index_u8=0;
		}
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
		--st_uart3_rx_glb.cnt_u8;
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

#if( UART3_RX_FLOW_RTS_EN == 1)										/* Check if RX flow control enabled */
		if(st_uart3_rx_glb.cnt_u8 <= UART3_RTS_GO_TRIG){
			st_uart3_rx_glb.flags.bits.flow_stop	=  0;
			mCSP_USART3_RTS_PIN_GO;
		}
#endif

	}

   	return(data_byte_u8);

}


/*************************************************************************************************
 * Function Name :		uart3_putchar
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
#if( UART3_TX_BUF_EN == 0)
void uart3_putchar(uint8_t tx_char_u8)
{
/* Local Variables */
		uint8_t 	CTS_status		=  UART3_CTS_POLARITY_GO;

/* Code */

#if( UART3_TX_FLOW_CTS_EN == 1)
	st_uart3_tx_glb.timeout_timer_u16 	= UART3_TX_TIMEOUT;
	do{
		CTS_status = mCSP_USART3_CTS_READ;					/* Read state of CTS pin */
	}while( (CTS_status != UART3_CTS_POLARITY_GO) && (st_uart3_tx_glb.timeout_timer_u16));
#endif

	/* Transmit data */
	if(tx_transmit_status_u8 == UART3_CTS_POLARITY_GO){
		USART_SendData(USART3, (uint8_t) tx_char_u8);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET){
		}
	}

	return;
}
#endif


/*************************************************************************************************
 * Function Name :		uart3_putchar
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	05/01/12	W. Paul		Created
 *
 *************************************************************************************************/
 #if( UART3_TX_BUF_EN == 1)
void uart3_putchar(uint8_t tx_char_u8)
{
/* Local Variables */
	uint8_t 	CTS_status		=  UART3_CTS_POLARITY_GO;



#if( UART3_TX_FLOW_CTS_EN == 1)
	st_uart3_tx_glb.timeout_timer_u16 	= UART3_TX_TIMEOUT;
	do{
		CTS_status = mCSP_USART3_CTS_READ;					/* Read state of CTS pin */
	}while( (CTS_status != UART3_CTS_POLARITY_GO) && (st_uart3_tx_glb.timeout_timer_u16));
#endif

	while(st_uart3_tx_glb.cnt_u8 >= UART3_TX_BUFFER_SIZE){
		//wait until buffer is reduced
	}

	if(st_uart3_tx_glb.cnt_u8 < UART3_TX_BUFFER_SIZE){
		uart3_tx_buffer_a_u8_glb[st_uart3_tx_glb.wr_index_u8]	=  tx_char_u8;
		if (++st_uart3_tx_glb.wr_index_u8 == UART3_TX_BUFFER_SIZE){
			st_uart3_tx_glb.wr_index_u8	=  0;
		}
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		st_uart3_tx_glb.cnt_u8++;
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
		if (st_uart3_tx_glb.cnt_u8 == UART3_TX_BUFFER_SIZE){
		   	st_uart3_tx_glb.flags.bits.overflow	=  1;
		}
	}
	else{
		st_uart3_tx_glb.flags.bits.overflow	=  1;
	}

	//if buffer was empty then we need to restart interupt
	if(st_uart3_tx_glb.flags.bits.empty	==  1){
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}

	return;
}
#endif


#endif
/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


