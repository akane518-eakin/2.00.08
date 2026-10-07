/**********************************************************************************************************
 *  Marturion Ltd
 *
 *	Knockmore Hill Business Park
 *	9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2010, Marturion Ltd
 *  All Rights Reserved
 *
 *
 * Filename    :  pcb_spi1.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 *********************************************************************************************************/


/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>


#include "pcb_spi1.h"

#if( SPI1_EN == 1)
#include "csp_STM32_spi.h"
#include "csp_STM32_spi1.h"

#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

/**********************************************************************************************************
 *********************************************************************************************************/


/**********************************************************************************************************
 * Function Name : spi1_config
 * Description   : This function is used initialise the SPI channel 1.
 * Arguments     : None
 * Returns       : None
 * Notes         : All ports,pins and clock are initialsied
 *
 * Version		Date d/m/y	    Programmer          	Reason for Change
 * 1.0.0		31/08/2010      Stephen Serplus       	Original Created
 * 1.0.1		10/11/2010		Philip Gillespie		Added APB2 high speed bus enable clock
 *														Added SPI MOSI configuration
 *														Added SPI MISO configuration
 *********************************************************************************************************/
void spi1_config(uint8_t mode_u8,uint32_t max_speed_u32)
{
/* Local Variables */
	static uint8_t				spi1_setup_u8		=  0;
	static uint8_t				last_spi1_mode_u8	=  0xff;
	static uint32_t				last_spi1_speed_u32	=  0;

	uint8_t						i;
	uint8_t						change_u8			=  0;

	GPIO_InitTypeDef			GPIO_InitStructure;
	RCC_ClocksTypeDef			RCC_Clocks;
	static SPI_InitTypeDef		spi1_curr_config_glb=	{
													        SPI_Direction_2Lines_FullDuplex,	/* Specifies SPI undirectional or direction data mode */
													        SPI_Mode_Master,				 	/* SPI operating mode */
													        SPI_DataSize_8b,				 	/* SPI data size */
													        SPI_CPOL_Low,					 	/* Serial clock steady state */
													        SPI_CPHA_1Edge,					 	/* Clock active edge for bit capture */
													        SPI_NSS_Soft,				 	 	/* Specifies whether NSS signal is managed by HW NSS pin or SW using SSI bit */
													        SPI_BaudRatePrescaler_2,		 	/* Baud Rate prescaler value used to configure TX and Rx clock */
													        SPI_FirstBit_MSB,				 	/* Specifies whether data transfer starts from MSB or LSB bit */
													    };

/* Code */
	if(spi1_setup_u8	== 0){
		spi1_setup_u8	=  1;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

		GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_5  |	//SPI1 CLK
	    GPIO_Pin_6  |	//SPI1 MISO
	    GPIO_Pin_7;		//SPI1 MOSI
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}

	/* Configure SPI 1 */
	if(last_spi1_mode_u8 != mode_u8){
		last_spi1_mode_u8 	=  mode_u8;
		switch(mode_u8){
			case SPI_MODE_0:	spi1_curr_config_glb.SPI_CPOL	=  SPI_CPOL_Low;	spi1_curr_config_glb.SPI_CPHA	=  SPI_CPHA_1Edge;		break;
			case SPI_MODE_1:	spi1_curr_config_glb.SPI_CPOL	=  SPI_CPOL_Low;	spi1_curr_config_glb.SPI_CPHA	=  SPI_CPHA_2Edge;		break;
			case SPI_MODE_2:	spi1_curr_config_glb.SPI_CPOL	=  SPI_CPOL_High;	spi1_curr_config_glb.SPI_CPHA	=  SPI_CPHA_1Edge;		break;
			case SPI_MODE_3:	spi1_curr_config_glb.SPI_CPOL	=  SPI_CPOL_High;	spi1_curr_config_glb.SPI_CPHA	=  SPI_CPHA_2Edge;		break;
		}
		change_u8			=  1;
	}
	if(last_spi1_speed_u32 != max_speed_u32){
		RCC_GetClocksFreq(&RCC_Clocks);
		last_spi1_speed_u32	=  RCC_Clocks.PCLK2_Frequency / 2;	//spi2/3 are from PCLK1		spi1 is from PCCLK2
		for(i=0;i<8;i++){
			if(last_spi1_speed_u32 <= max_speed_u32 ){
				i	=  10;
			}
			else{
				last_spi1_speed_u32	/= 2;
			}
		}
		spi1_curr_config_glb.SPI_BaudRatePrescaler	=  i<<3;
		last_spi1_speed_u32		=  max_speed_u32;
		change_u8				=  1;

	}

 	if(change_u8){
		SPI_Cmd(SPI1, DISABLE);
		SPI_I2S_DeInit(SPI1);

    	SPI_Init(SPI1, &spi1_curr_config_glb);

		SPI_Cmd(SPI1, ENABLE);
	}
	return;
}



/**********************************************************************************************************
 * Function Name : spi1_rdwr
 * Description   : This function is used to read and write to SPI devices on channel 1.
 * Arguments     : uint8_t wr_byte_u8 - device for chips select
 * Returns       : uint8_t - the read value
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		31/08/2010      Stephen Serplus     Original Created
 *********************************************************************************************************/
uint8_t spi1_rdwr(uint8_t wr_byte_u8)
{
/* Code */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);		/* Loop while DR register in not empty */
	SPI_I2S_SendData(SPI1, wr_byte_u8);									/* Send byte through the SPI1 peripheral */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);	/*  Wait to receive a byte*/
	return SPI_I2S_ReceiveData(SPI1);									/* Return the byte read from the SPI bus */

}

#endif //#if( SPI1_EN == 1)

//end of file
