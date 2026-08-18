/**********************************************************************************************************
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
 *                  Copyright 2010, Marturion Ltd, Lisburn, Co. Antrim, Northern Ireland
 *                                          All Rights Reserved
 *
 *
 *                                          csp_dac.h
 *
 * Filename    :  csp_dac.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the adc functions.
 * Compiler    :  GNU GCC
 * Target      :  STM32103
 * Version     :  Version 1.0.0
 *********************************************************************************************************/

/**********************************************************************************************************
 *                                           INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x.h"			/* CMSIS Cortex-M3 Device Peripheral Access Layer Header File. 			*/
#include "csp_STM32_dac.h"
#include "stm32f10x_dac.h"




/**********************************************************************************************************
 *********************************************************************************************************/


/*************************************************************************************************
 * Function Name :		init_DAC1
 * Description   :		This function is used initialise the DAC1 in the system.
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0		04/10/10	W. Paul		Created
 *
 ************************************************************************************************/
void init_DAC1(void)
{
// Local Variables
	DAC_InitTypeDef		DAC_InitStructure;
	GPIO_InitTypeDef 	GPIO_InitStructure;

// Code
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	GPIO_InitStructure.GPIO_Pin		=  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_AIN;
	//GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

// DAC channel1 Configuration
	DAC_InitStructure.DAC_Trigger							=  DAC_Trigger_T2_TRGO;
//	DAC_InitStructure.DAC_Trigger							=  DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration					=  DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude		=  DAC_TriangleAmplitude_4095;

	//	DAC_InitStructure.DAC_WaveGeneration				=  DAC_WaveGeneration_Noise;
	//	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude	=  DAC_LFSRUnmask_Bits11_0;

	//	DAC_InitStructure.DAC_WaveGeneration				=  DAC_WaveGeneration_Triangle;
	//	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude	=  DAC_TriangleAmplitude_4095;

	DAC_InitStructure.DAC_OutputBuffer						=  DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

//Set DAC data alignment
	DAC_SetChannel1Data(DAC_Align_12b_R, 0x100);				// Set DAC Channel1 DHR12R register
	//	DAC_SetDualChannelData(DAC_Align_12b_R, 0x100, 0x100);	// Set DAC dual channel DHR12RD register

	csp_DAC1_write(2048);	//half scale

	DAC_Cmd(DAC_Channel_1, ENABLE);				// Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is automatically connected to the DAC converter.
	DAC_DMACmd(DAC_Channel_1, ENABLE);			// Enable DMA for DAC Channel1

	return;
}


/*************************************************************************************************
 * Function Name :		csp_DAC1_write()
 * Description   :		This function is used to write a value to DAC 1.
 * Arguments     : 		uint8_t		dac_value
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer		Reason for Change
 * 0.1.0		09/07/12	A. Duignan		Created
 *
 ************************************************************************************************/
void csp_DAC1_write(uint16_t	dac_value)
{

// Code
	DAC_SetChannel1Data(DAC_Align_12b_R, dac_value);

	return;
}




/**********************************************************************************************************
 *********************************************************************************************************/
 //end of file
