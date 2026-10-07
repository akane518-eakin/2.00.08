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
*                   Copyright 2010, Marturion Ltd, Lisburn, Co. Antrim, Northern Ireland
*                                          All Rights Reserved
*
*                                          bsp_dma.c
*
* Filename    :  csp_STM32_dma.c
* Programmer  :  William Paul
* Description :  This module containes the GPIO initialisation function.
* Compiler    :  GNU GCC Ride 7 Version 1.26.10.0130
* Target      :  STM32103
* ST Library  :  Version 1.0.0
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x.h"			/* CMSIS Cortex-M3 Device Peripheral Access Layer Header File. 			*/
//#include "bsp_system_cfg.h"
#include "stm32f10x_DMA.h"

#include "csp_STM32_dma.h"

uint8_t		dma2_1_clk_en	=  0;
uint8_t		dma2_3_clk_en	=  0;

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*************************************************************************************************
* Function Name : 	csp_STM32_DMA2_CH1_config_m2m
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		27/11/18	W. Paul			Created
*************************************************************************************************/
void csp_STM32_DMA2_CH1_config_m2m(uint32_t Sadd, uint32_t Dadd,uint32_t n)
{
	DMA_InitTypeDef		DMA_InitStructure;

// Code
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	dma2_1_clk_en	=  1;

	DMA_Cmd(DMA2_Channel1, DISABLE);
	DMA_DeInit(DMA2_Channel1);

	DMA_InitStructure.DMA_PeripheralBaseAddr		=  (uint32_t)Sadd;				//source address
	DMA_InitStructure.DMA_MemoryBaseAddr			=  (uint32_t)Dadd;				//destination
	DMA_InitStructure.DMA_DIR						=  DMA_DIR_PeripheralSRC;			//peripheral is source
	DMA_InitStructure.DMA_PeripheralInc				=  DMA_PeripheralInc_Disable;		//peripheral address increment 		disable
	DMA_InitStructure.DMA_MemoryInc					=  DMA_MemoryInc_Disable;			//memory     address increment 		disable
	DMA_InitStructure.DMA_PeripheralDataSize		=  DMA_PeripheralDataSize_HalfWord;	//peripheral data in 16bit format
	DMA_InitStructure.DMA_MemoryDataSize			=  DMA_MemoryDataSize_HalfWord;		//memory     data in 16bit format
	DMA_InitStructure.DMA_Mode						=  DMA_Mode_Normal;					//circular mode enabled
	DMA_InitStructure.DMA_Priority					=  DMA_Priority_High;				//priority of DMA
	DMA_InitStructure.DMA_M2M						=  DMA_M2M_Enable;					//

	//note DMA transfer is limited to 65535 bytes so loop for larger transfers
	do{
		if(n > 0xffff){		DMA_InitStructure.DMA_BufferSize	=  0xffff;	}	//no of bytes to move
		else{				DMA_InitStructure.DMA_BufferSize	=  n;	}
		n	-= DMA_InitStructure.DMA_BufferSize;

		DMA_Init(DMA2_Channel1, &DMA_InitStructure);
		DMA_ITConfig(DMA2_Channel1, DMA_IT_TC, ENABLE);
		DMA_ClearFlag(DMA2_FLAG_TC1);

		DMA_Cmd(DMA2_Channel1, ENABLE);						//start DMA
		while(DMA_GetFlagStatus(DMA2_FLAG_TC1) == RESET);	//wait until transfer is finished
		DMA_Cmd(DMA2_Channel1, DISABLE);					//end DMA	(can't restart dma without disabling previous DMA)
	}while(n);

	DMA_DeInit(DMA2_Channel1);

	if(dma2_3_clk_en == 0){	//check not being used by chanel 3
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, DISABLE);
		dma2_1_clk_en	=  0;
		dma2_3_clk_en	=  0;
	}

	return;
}

/*
*********************************************************************************************************
* Function Name : csp_STM32_DMA2_CH3_config
* Description   : This function is used to initialise DMA2 CH3
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		   08/10/10     William Paul     Original Created
*********************************************************************************************************
*/
void csp_STM32_DMA2_CH3_config(uint32_t buf_address, uint32_t buf_len)
{
// Local Variables
	DMA_InitTypeDef            DMA_InitStructure;

// Code
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	dma2_3_clk_en	=  1;

	DMA_Cmd(DMA2_Channel3, DISABLE);
	DMA_DeInit(DMA2_Channel3);

	DMA_InitStructure.DMA_PeripheralBaseAddr		=  DAC_DHR12R1_Address;				//peripheral address
	DMA_InitStructure.DMA_MemoryBaseAddr			=  buf_address;						//mem address
	DMA_InitStructure.DMA_DIR						=  DMA_DIR_PeripheralDST;			//peripheral is destination
	DMA_InitStructure.DMA_BufferSize 				=  buf_len;							//no of bytes to move
	DMA_InitStructure.DMA_PeripheralInc				=  DMA_PeripheralInc_Disable;		//peripheral address increment 		disable
	DMA_InitStructure.DMA_MemoryInc					=  DMA_MemoryInc_Enable;			//memory     address increment 		enable
	DMA_InitStructure.DMA_PeripheralDataSize		=  DMA_PeripheralDataSize_HalfWord;	//peripheral data in 16bit format
	DMA_InitStructure.DMA_MemoryDataSize			=  DMA_MemoryDataSize_HalfWord;		//memory     data in 16bit format
	DMA_InitStructure.DMA_Mode						=  DMA_Mode_Circular;				//circular mode enabled
	DMA_InitStructure.DMA_Priority					=  DMA_Priority_High;				//priority of DMA
	DMA_InitStructure.DMA_M2M						=  DMA_M2M_Disable;					//
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

//	DMA_Cmd(DMA2_Channel3, ENABLE);


	return;
}




/*************************************************************************************************
* Function Name : 	csp_STM32_DMA2_CH3_deconfig
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		27/11/18	W. Paul			Created
*************************************************************************************************/
void csp_STM32_DMA2_CH3_deconfig(void)
{
	DMA_Cmd(DMA2_Channel3, DISABLE);
	DMA_DeInit(DMA2_Channel3);

	if(dma2_1_clk_en == 0){		//check not being used by chanel 1
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, DISABLE);
		dma2_1_clk_en	=  0;
		dma2_3_clk_en	=  0;
	}
	return;
}

/*
*********************************************************************************************************
*                                           End of bsp_dma.c
*********************************************************************************************************
*/

