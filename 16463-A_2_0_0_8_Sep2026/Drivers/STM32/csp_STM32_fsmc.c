/**********************************************************************************************************
 *  Marturion Electronics Ltd
 *
 *  Knockmore Hill Business Park
 *  9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2017, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  csp_STM32_fsmc.c
 * Date Created:  Tue 16 May 2017 09:28:56 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "csp_STM32_fsmc.h"
#include "stm32f10x_fsmc.h"


/*************************************************************************************************
* Function Name : 	csp_FSMC_NE1_Config
* Description   : 	This Function initialises the FSMC bus for SSD1963 LCD
* Arguments     : 	uint8_t option		fast or slow bus
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
void csp_FSMC_NE1_Config(uint8_t option)
{
	FSMC_NORSRAMInitTypeDef 		FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef 	FSMC_NORSRAM_Timing;

	if(option == FSMC_OFF){
		FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, DISABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, DISABLE);
	}
	else{
	  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	
	  	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, DISABLE);
		
		
		/* FSMC NOR/SRAM Init structure definition */
		FSMC_NORSRAMInitStructure.FSMC_Bank							= FSMC_Bank1_NORSRAM1;						// Select bank to be used to map NOR Flash memory
		FSMC_NORSRAMInitStructure.FSMC_MemoryType					= FSMC_MemoryType_NOR;                     	//Specifies the type of external memory attached to the corresponding memory bank

		FSMC_NORSRAMInitStructure.FSMC_DataAddressMux				= FSMC_DataAddressMux_Disable;              // Specifies whether the address and data values are multiplexed on data bus or not
		FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth				= FSMC_MemoryDataWidth_16b;                 // Specifies external memory device width

		FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode				= FSMC_BurstAccessMode_Disable;             // Enables or disables burst access mode for flash memory
		FSMC_NORSRAMInitStructure.FSMC_WrapMode						= FSMC_WrapMode_Disable;                    // Enables or disables the Wrapped burst access mode for Flash memory, valid only when accessing flash memory in burst mode

		FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait				= FSMC_AsynchronousWait_Disable;
		FSMC_NORSRAMInitStructure.FSMC_WaitSignal					= FSMC_WaitSignal_Disable;                  // Enables or disables the wait-state insertion via wait signal, valid for Flash memory access in burst mode
		FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity			= FSMC_WaitSignalPolarity_Low;              // Specifies wait signal polarity, valid only when accessing flash memory in burst mode
		FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive				= FSMC_WaitSignalActive_DuringWaitState;    // Specifies if the wait signal is asserted by the memory one clock cycle before the wait state or during the wait state

		FSMC_NORSRAMInitStructure.FSMC_WriteOperation				= FSMC_WriteOperation_Enable;               // Enables or disables the write operation in the selected bank by the FSMC
		FSMC_NORSRAMInitStructure.FSMC_WriteBurst					= FSMC_WriteBurst_Disable;                  // Enables or disables the write burst operation

		FSMC_NORSRAMInitStructure.FSMC_ExtendedMode					= FSMC_ExtendedMode_Disable;                // Only enable this if the rd timings are differnt from write timmings

		/* rd timings */
		if(option==FSMC_STARTUP){
			FSMC_NORSRAM_Timing.FSMC_AddressSetupTime		= 15;
			FSMC_NORSRAM_Timing.FSMC_AddressHoldTime		= 15;
			FSMC_NORSRAM_Timing.FSMC_DataSetupTime			= 255;
			FSMC_NORSRAM_Timing.FSMC_BusTurnAroundDuration	= 15;
			FSMC_NORSRAM_Timing.FSMC_CLKDivision			= 15;
			FSMC_NORSRAM_Timing.FSMC_DataLatency			= 15;
			FSMC_NORSRAM_Timing.FSMC_AccessMode				= FSMC_AccessMode_B;
		}
		else if(option==FSMC_HIGH_SPEED){
			FSMC_NORSRAM_Timing.FSMC_AddressSetupTime		= 0;
			FSMC_NORSRAM_Timing.FSMC_AddressHoldTime		= 1;
			FSMC_NORSRAM_Timing.FSMC_DataSetupTime			= 1;
			FSMC_NORSRAM_Timing.FSMC_BusTurnAroundDuration	= 0;
			FSMC_NORSRAM_Timing.FSMC_CLKDivision			= 0;
			FSMC_NORSRAM_Timing.FSMC_DataLatency			= 0;
			FSMC_NORSRAM_Timing.FSMC_AccessMode				= FSMC_AccessMode_B;
		}
		FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct		= &FSMC_NORSRAM_Timing;           /* Timing Parameters for write and read access if the  ExtendedMode is not used */
		FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct			= &FSMC_NORSRAM_Timing;           /* Timing Parameters for write access if the  ExtendedMode is used */

		FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
		FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);	/* FSMC 1.1 (ie NOR/SRAM Bank 1) is enabled */
	}
	return;


}


/*******************************************************************************
* Function Name  : csp_FSMC_NE2_Config
* Description    : Configures the Parallel interface (FSMC) for LCD(Parallel mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void csp_FSMC_NE2_Config(void)
{
	FSMC_NORSRAMInitTypeDef  		FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  	p;

/*-- FSMC Configuration ------------------------------------------------------*/
	p.FSMC_AddressSetupTime 		= 0;
	p.FSMC_AddressHoldTime 			= 0;
	p.FSMC_DataSetupTime 			= 1;
	p.FSMC_BusTurnAroundDuration 	= 0;
	p.FSMC_CLKDivision 				= 0;
	p.FSMC_DataLatency 				= 0;
	p.FSMC_AccessMode 				= FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank 					= FSMC_Bank1_NORSRAM2;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux 			= FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType 				= FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth 			= FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode 			= FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait 		= FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity 		= FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode 				= FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive 		= FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation 			= FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal 				= FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode 			= FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst 				= FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct 	= &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct 		= &p;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);

	return;
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
