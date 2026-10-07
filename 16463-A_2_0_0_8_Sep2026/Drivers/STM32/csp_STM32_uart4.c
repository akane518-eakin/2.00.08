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
*                                          csp_STM32_uart4.c
*
* Filename    :  csp_STM32_uart4.c
* Programmer  :  Pauric Lynch
* Description :
* Compiler    :  IAR
* Target      :  STM32F103
* Version     :  Version 1.1.0
**********************************************************************************************************
*/
/*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************/
#include "csp_STM32_uart4.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "pcb_uart.h"

#if( UART4_EN == 1)

#include "pcb_pins.h"
#include "pcb_uart4.h"
#include "csp_STM32_uart.h"
#include "csp_STM32_uart4.h"
//#include "csp_STM32_system_cfg.h"

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
/*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************/


/*********************************************************************************************************
*                                           VARIABLES
*********************************************************************************************************/
uint8_t    uart4_rx_buffer_a_u8_glb[UART4_RX_BUFFER_SIZE];
uint8_t    uart4_tx_buffer_a_u8_glb[UART4_TX_BUFFER_SIZE];

t_serial_st	st_uart4_rx_glb;
t_serial_st	st_uart4_tx_glb;

/*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************/
/*********************************************************************************************************
* Function Name : uart4_config
* Description   : This function enables UART 4
* Arguments     : Baud Rate
* Returns       : None
* Notes         :
*
* Version		Date				Programmer          Reason for Change
* 1.0.0        	22/06/12            Pauric Lynch        Original Created
********************************************************************************************************/
void uart4_config(uint32_t baud_rate_u32)
{
/* Local Variables */
	USART_InitTypeDef	USART_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;

/* Code */
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);			/* Enable high speed APB2 clock 72MHz */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);								/* UART4 Periph clock enable */

	/* Configure the GPIO ports( UART4 Transmit and Receive Lines) */
	/* Configure the UART4_Tx as Alternate function Push-Pull */
	/* Configure UART4_Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure UART4_Rx as input floating */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

/* UART4 configuration ------------------------------------------------------*/
/* UART4 configured as follow:
    - BaudRate = 9600 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
*/
	USART_InitStructure.USART_BaudRate 			= UART4_BAUD;
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;
	USART_InitStructure.USART_Parity				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode				= USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the UART4 */
	USART_Init(UART4, &USART_InitStructure);

	/* Enable UART4 interrupt */
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
  	uart4_init_rx_buf();

  	/* Enable the UART4 */
	USART_Cmd(UART4, ENABLE);


	/* Enable the USARTx Interupt */
#if (UART4_TX_BUF_EN == 1) || (UART4_RX_BUF_EN == 1)
	NVIC_InitTypeDef 	NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel 					=  UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority		=  UART4_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  UART4_IRQ_SUB_PRI;
	NVIC_InitStructure.NVIC_IRQChannelCmd					=  ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

}// void uart4_config(uint32_t baud_rate_u32)


/*********************************************************************************************************
* Function Name : uart4_init_rx_buf
* Description   : This function initialises the serial buffer
* Arguments     : None
* Returns       : None
* Notes         :
*
* Version		Date				Programmer          Reason for Change
* 1.0.0        	22/06/12            Pauric Lynch        Original Created
********************************************************************************************************/
void uart4_init_rx_buf(void)
{
/* Code */
	st_uart4_rx_glb.wr_index_u8				=  0;
	st_uart4_rx_glb.rd_index_u8				=  0;
	st_uart4_rx_glb.cnt_u8					=  0;
	st_uart4_rx_glb.flags.bits.overflow		=  0;
	st_uart4_rx_glb.flags.bits.empty			=  1;		//need to flag hi to show buf is empty
	st_uart4_rx_glb.timeout_timer_u16			=  0;
	memset(uart4_rx_buffer_a_u8_glb,0,sizeof uart4_rx_buffer_a_u8_glb);

}// void uart4_init_rx_buf(void)


/*********************************************************************************************************
* Function Name : uart4_init_tx_buf
* Description   : This function initialises the serial tx buffer.
* Arguments     : None
* Returns       : None
* Notes         :
*
* Version		Date				Programmer          Reason for Change
* 1.0.0        	22/06/12            Pauric Lynch        Original Created
********************************************************************************************************/
void uart4_init_tx_buf(void)
{
/* Code */
	st_uart4_tx_glb.wr_index_u8				=  0;
	st_uart4_tx_glb.rd_index_u8				=  0;
	st_uart4_tx_glb.cnt_u8					=  0;
	st_uart4_tx_glb.flags.bits.overflow		=  0;
	st_uart4_tx_glb.flags.bits.empty			=  1;
	st_uart4_tx_glb.timeout_timer_u16			=  0;
	memset(uart4_tx_buffer_a_u8_glb,0,sizeof uart4_tx_buffer_a_u8_glb);

}// void uart4_init_tx_buf(void)

/*********************************************************************************************************
* Function Name :
* Description   : This function enables the selected chip
* Arguments     : None
* Returns       : None
* Notes         :
*
* Version		Date				Programmer          Reason for Change
* 1.0.0        	22/06/12            Pauric Lynch        Original Created
********************************************************************************************************/
void uart4_IRQ(void)
{
/* Local Variables*/
	uint16_t rec_char_u16;

/* Code */
	    if(USART_GetITStatus(UART4, USART_IT_FE) != RESET){		/* Check if a Frame error is signaled */
        USART_ClearITPendingBit(UART4, USART_IT_FE);	 		/* Clear the USART4 Frame error pending bit */
       	rec_char_u16 = USART_ReceiveData(UART4);				/* Read contents of UART */
    }

    if(USART_GetITStatus(UART4, USART_IT_PE) != RESET){		 /* If the UART4 detects a parity error */
        while(USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == RESET){
        }
        USART_ClearITPendingBit(UART4, USART_IT_PE);			/* Clear the USART3 Parity error pending bit */
        rec_char_u16 = USART_ReceiveData(UART4);				/* Read contents of USART */
    }

    if(USART_GetITStatus(UART4, USART_IT_ORE) != RESET){		 /* If a Overrun error is signaled */
        USART_ClearITPendingBit(UART4, USART_IT_ORE);			/* Clear the USART3 Overrun error is signaled */
       	rec_char_u16 = USART_ReceiveData(UART4);				/* Read contents of USART */
    }

    if(USART_GetITStatus(UART4, USART_IT_NE) != RESET){		/* If a Noise error is signaled by the card */
        USART_ClearITPendingBit(UART4, USART_IT_NE);			 /* Clear the USART3 Frame error pending bit */
        rec_char_u16 = USART_ReceiveData(UART4);				/* Read contents of USART */
    }

	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
		rec_char_u16 = USART_ReceiveData(UART4);				/* Read contents of USART */
		uart4_rx_buffer_a_u8_glb[st_uart4_rx_glb.wr_index_u8] = (uint8_t) rec_char_u16;
	   	if (++st_uart4_rx_glb.wr_index_u8 == UART4_RX_BUFFER_SIZE){
			st_uart4_rx_glb.wr_index_u8		=0;
		}
	   	if (++st_uart4_rx_glb.cnt_u8 == UART4_RX_BUFFER_SIZE){
	    	st_uart4_rx_glb.cnt_u8				=0;
	      	st_uart4_rx_glb.flags.bits.overflow	=1;
	   	}

#ifdef _UART_4_RX_FLOW_CTR											/* Check if RX flow control enabled */
		if(st_uart4_rx_glb.cnt_u8 >= RX_UART4_RTS_STOP_TRIG){		/* Check if buffer is empty */
			mBSP_UART_4_RTS_PIN_STOP();									/* Set RTS pin low */
		}
#endif

	}//if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){

}//


/**********************************************************************************************************
 * Function Name : uart4_getchar
 * Description   : This function is used to lift one byte of data from UART 1 port buffer.
 * Arguments     : uint16_t	wait_time_u16		in ms
 *				  uint8_t	*rec_status_u8 - 	0 - data present
 *												1 - no serial data received
 * Returns       : Received Byte
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		23/07/2010      Stephen Serplus     Original Created
 * 2.0.0		28/6/11			W. Paul				Merge getchar_wait and getchar_nowait
 * 2.1.0		28/6/11			W. Paul				Add OS semiphore
 **********************************************************************************************************/
uint8_t uart4_getchar(uint16_t wait_time_u16,uint8_t *rec_status_u8)
{
/* Local Variables */
	uint8_t 		data_byte_u8 	= 0;


	/* Initialise variable */
	*rec_status_u8	= 0;
	st_uart4_rx_glb.timeout_timer_u16 = 0;

	while(	(st_uart4_rx_glb.timeout_timer_u16 < wait_time_u16) && (st_uart4_rx_glb.cnt_u8 == 0)	){

	}

	if(st_uart4_rx_glb.cnt_u8 != 0){
		data_byte_u8		= uart4_rx_buffer_a_u8_glb[st_uart4_rx_glb.rd_index_u8];
		*rec_status_u8		= 1;

		if (++st_uart4_rx_glb.rd_index_u8 == UART4_RX_BUFFER_SIZE){
			st_uart4_rx_glb.rd_index_u8=0;
		}
		USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
		--st_uart4_rx_glb.cnt_u8;
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);


	}

   	return(data_byte_u8);

}


/**********************************************************************************************************
* Function Name : uart4_putchar
* Description   : This function sends 1 character out USART1.
* Arguments     : tx_char_u8 - character to send
* Returns       : None
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    18/11/2010      Philip Gillespie     Original Created
**********************************************************************************************************/
#if( UART4_TX_BUF_EN == 0)
void uart4_putchar(uint8_t tx_char_u8)
{
/* Local Variables */
	uint8_t 	tx_transmit_status_u8		=  1;			/* Status of slave device */

/* Code */

	/* Transmit data */
	if(tx_transmit_status_u8 == 1){
		USART_SendData(UART4, (uint8_t) tx_char_u8);
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET){} 	/* Loop until the end of transmission */
	}

	return;

}

#endif		//#if( UART4_TX_BUF_EN = 0)

/**********************************************************************************************************
 * Function Name : uart4_putchar
 * Description   : This function sends 1 character out USART1.
 * Arguments     : tx_char_u8 - character to send
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		18/11/2010      Philip Gillespie     Original Created
 **********************************************************************************************************/
#if( UART4_TX_BUF_EN == 1)
void uart4_putchar(uint8_t tx_char_u8)
{
/* Local Variables */
	uint8_t 	tx_transmit_status_u8		=  1;			/* Status of slave device */


	if(tx_transmit_status_u8 == 1){
		if(st_uart4_tx_glb.cnt_u8 < UART4_TX_BUFFER_SIZE){
			uart4_tx_buffer_a_u8_glb[st_uart4_tx_glb.wr_index_u8]	=  tx_char_u8;
			if (++st_uart4_tx_glb.wr_index_u8 == UART4_TX_BUFFER_SIZE){
				st_uart4_tx_glb.wr_index_u8	=  0;
			}
			USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
			st_uart4_tx_glb.cnt_u8++;
			USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
			if (st_uart4_tx_glb.cnt_u8 == UART4_TX_BUFFER_SIZE){
			   	st_uart4_tx_glb.flags.bits.overflow	=  1;
			}

		}
		else{
			st_uart4_tx_glb.flags.bits.overflow	=  1;
		}

		//if buffer was empty then we need to restart interupt
		if(st_uart4_tx_glb.flags.bits.empty	==  1){
			USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
		}
	}

	return;
}



#endif		//#if( UART4_TX_BUF_EN == 1)


#endif 	//#if( UART4_EN == 1)		whole file

/*********************************************************************************************************
* Function Name : uart4_1mS_timer
* Description   : This is incremented by the syst tick timer
* Arguments     : None
* Returns       : None
* Notes         :
*
* Version		Date				Programmer          Reason for Change
* 1.0.0        	25/07/12            Pauric Lynch        Original Created
********************************************************************************************************/
void uart4_1mS_timer(void)
{
	st_uart4_rx_glb.timeout_timer_u16++;
}




/*
*********************************************************************************************************
* Function Name : uart4_bytes2read
* Description   : This function is returns how many bytes are to be read from the serial port.
* Arguments     : None
* Returns       : uint8_t numOfBytes - Bytes to be read!
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	22/03/13	    	Pauric Lynch     Original Created
*********************************************************************************************************
*/
uint8_t uart4_bytes2read(void)
{
/* Code */
	return st_uart4_rx_glb.cnt_u8;

}


/*
*********************************************************************************************************
* Function Name : uart4_reset_counters
* Description   : This function reset the counters and indexes of the serial port.
* Arguments     : None
* Returns       : uint8_t numOfBytes - Bytes to be read!
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	22/03/13	    	Pauric Lynch     Original Created
*********************************************************************************************************
*/
void uart4_reset_counters(void)
{
/* Code */
	st_uart4_rx_glb.cnt_u8 = 0;
	st_uart4_rx_glb.wr_index_u8 = 0;
	st_uart4_rx_glb.rd_index_u8 = 0;

}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
