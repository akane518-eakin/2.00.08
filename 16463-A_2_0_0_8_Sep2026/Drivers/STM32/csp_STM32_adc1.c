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
 * Filename    :  csp_STM32_ADC1.c
 * Programmer  :  Philip Gillespie
 * Description :  This module is used for all the adc functions.
 *
 **********************************************************************************************************/


/**********************************************************************************************************
 *			INCLUDE FILES
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include	"app_system.h"

#include "csp_STM32_ADC1.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "csp_STM32_iwdg.h"

/**********************************************************************************************************
 *		COMPILER DEFINES
 **********************************************************************************************************/


/**********************************************************************************************************
 *		GLOBALS VARIABLES
 **********************************************************************************************************/


/**********************************************************************************************************
 *		LOCAL VARIABLES
 **********************************************************************************************************/
uint8_t     adc1_channel_setup;

/**********************************************************************************************************
 *		LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************/


/**********************************************************************************************************
 **********************************************************************************************************/


/**********************************************************************************************************
 * Function Name : csp_ADC1_config
 * Description   : This function is used to initalise the ADC1
 * Arguments     : None
 * Returns       : None
 * Notes         : This function:
 *					> Defines the ADC clock divider. This clock is derived from the APB2 clock (PCLK2).
 *					> Enables or disables the High Speed APB (APB2) peripheral clock.
 *					> Deinitializes the ADCx peripheral registers to their default reset values
 *					> Initialises the ADC registers with user settings.
 *
 *				You must set the ADC Prescaler value depending on the frequency of the analogue signal you are
 *				sampling.  From Nyquists sampling theorem - [Fsample >= 2(Fsignal)].
 *				ADC is clocked from the 72MHz APB2 bus and pre-scalers of 2,4,6,8 are available.
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		11/11/2010      Philip Gillespie    Original Created
 **********************************************************************************************************/
void csp_ADC1_config(void)
{
/* Local Variables */
	ADC_InitTypeDef		ADC_InitStructure;

/* Code */
	app_sys_watchdog_reload();

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);										// PCLK2 is the APB2 clock ADCCLK = PCLK2/6 = 72/6 = 12MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);					// Enable ADC1 clock so that we can talk to it
	ADC_DeInit(ADC1);														// Put everything back to power-on defaults

	/* ADC1 Configuration */
	ADC_InitStructure.ADC_Mode 					= ADC_Mode_Independent;		// Configures the ADC to operate in independent or dual mode
	ADC_InitStructure.ADC_ScanConvMode 			= DISABLE;					// Specifies whether the conversion is performed in Scan (multichannels) or Single (one channel) mode
	ADC_InitStructure.ADC_ContinuousConvMode 	= DISABLE;		  			// Don't do contimuous conversions - do them on demand
	ADC_InitStructure.ADC_ExternalTrigConv 		= ADC_ExternalTrigConv_None;// Start conversion by software, not an external trigger
	ADC_InitStructure.ADC_DataAlign 			= ADC_DataAlign_Right;		// Conversions are 12 bit - put them in the lower 12 bits of the result
	ADC_InitStructure.ADC_NbrOfChannel 			= 1;						// Say how many channels would be used by the sequencer
	ADC_Init(ADC1, &ADC_InitStructure);										// Now do the setup

	ADC_TempSensorVrefintCmd(ENABLE);										// Enable the Internal Temperature Sensor

	ADC_Cmd(ADC1, ENABLE);													// Enable ADC1
	ADC_ResetCalibration(ADC1);												// Enable ADC1 reset calibaration register
	while(ADC_GetResetCalibrationStatus(ADC1));						 		// Check the end of ADC1 reset calibration register
	ADC_StartCalibration(ADC1);												// Start ADC1 calibaration
	while(ADC_GetCalibrationStatus(ADC1));									// Check the end of ADC1 calibration

	return;

}


/*
*********************************************************************************************************
* Function Name : csp_ADC1_Read
* Description   : This function is used to read an ADC channel from ADC 1
* Arguments     : uint8_t channel
* Returns       : uint16_t
* Notes         : This function:
*					> Read the ADC
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0			4/09/2017		William Paul		Original Created
*********************************************************************************************************
*/
uint16_t csp_ADC1_Read(void)
{
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);		// Wait until conversion completion
  	return (ADC_GetConversionValue(ADC1) );						// Get the conversion value
}

/*
*********************************************************************************************************
* Function Name : csp_ADC1_ReadSetup
* Description   : This function is used to setup an ADC channel from ADC 1
* Arguments     : uint8_t channel
* Returns       : uint16_t
* Notes         : This function:
*				   	> Configures for the selected ADC regular channel
*				   	> Enables the selected ADC software start conversion.
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0			4/09/2017		William Paul		Original Created
*********************************************************************************************************
*/
void csp_ADC1_Setup(uint8_t adc_channel_u8)
{
    adc1_channel_setup  =  adc_channel_u8;
  	ADC_RegularChannelConfig(ADC1, adc_channel_u8, 1, ADC_SampleTime_239Cycles5);	 	// Setup the conversion
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);												// Start the conversion
//  	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);								// Wait until conversion completion

  	return;
}

/*
*********************************************************************************************************
* Function Name : csp_ADC1_SetupWaitRead
* Description   : This function is used to read an ADC channel from ADC 1
* Arguments     : uint8_t channel
* Returns       : uint16_t
* Notes         : This function:
*				   > Configures for the selected ADC regular channel
*				   > Enables the selected ADC software start conversion.
*				   > Wait for (End Of Conversion)
*				   > Returns the last ADCx conversion result data for regular channel.
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0			4/09/2017		William Paul		Original Created
*********************************************************************************************************
*/
uint16_t csp_ADC1_SetupWaitRead(uint8_t adc_channel_u8)
{
    adc1_channel_setup  =  adc_channel_u8;
	ADC_RegularChannelConfig(ADC1, adc_channel_u8, 1, ADC_SampleTime_28Cycles5);	// Setup the conversion
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);											// Start the conversion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);							// Wait until conversion completion

	return ADC_GetConversionValue(ADC1);											// Get the conversion value
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
