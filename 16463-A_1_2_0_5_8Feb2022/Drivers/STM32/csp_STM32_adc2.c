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
 * Filename    :  csp_STM32_ADC2.c
 * Programmer  :  Philip Gillespie
 * Description :  This module is used for all the adc functions.
 *
 **********************************************************************************************************/


/**********************************************************************************************************
 *			INCLUDE FILES
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>


#include "csp_STM32_ADC2.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"




/**********************************************************************************************************
 *		COMPILER DEFINES
 **********************************************************************************************************/


/**********************************************************************************************************
 *		GLOBALS VARIABLES
 **********************************************************************************************************/


/**********************************************************************************************************
 *		LOCAL VARIABLES
 **********************************************************************************************************/
uint8_t	adc2_channel_setup;

/**********************************************************************************************************
 *		LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************/


/**********************************************************************************************************
 **********************************************************************************************************/


/**********************************************************************************************************
 * Function Name : csp_ADC2_config
 * Description   : This function is used to initalise the ADC2
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
void csp_ADC2_config(void)
{
/* Local Variables */
	ADC_InitTypeDef		ADC_InitStructure;

/* Code */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);							/* PCLK2 is the APB2 clock ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);		/* Enable ADC2 clock so that we can talk to it */
	ADC_DeInit(ADC2);											/* Put everything back to power-on defaults */

	/* ADC2 Configuration */
	ADC_InitStructure.ADC_Mode 					= ADC_Mode_Independent;		/* Configures the ADC to operate in independent or dual mode */
	ADC_InitStructure.ADC_ScanConvMode 			= DISABLE;					/* Specifies whether the conversion is performed in Scan (multichannels) or Single (one channel) mode */
	ADC_InitStructure.ADC_ContinuousConvMode 	= DISABLE;		  			/* Don't do continuous conversions - do them on demand */
	ADC_InitStructure.ADC_ExternalTrigConv 		= ADC_ExternalTrigConv_None;/* Start conversion by software, not an external trigger */
	ADC_InitStructure.ADC_DataAlign 			= ADC_DataAlign_Right;		/* Conversions are 12 bit - put them in the lower 12 bits of the result */
	ADC_InitStructure.ADC_NbrOfChannel 			= 1;						/* Say how many channels would be used by the sequencer */
	ADC_Init(ADC2, &ADC_InitStructure);										/* Now do the setup */

	ADC_Cmd(ADC2, ENABLE);													/* Enable ADC2 */
	ADC_ResetCalibration(ADC2);												/* Enable ADC2 reset calibaration register */
	while(ADC_GetResetCalibrationStatus(ADC2));						 		/* Check the end of ADC2 reset calibration register */
	ADC_StartCalibration(ADC2);												/* Start ADC2 calibaration */
	while(ADC_GetCalibrationStatus(ADC2));									/* Check the end of ADC2 calibration */

	return;

}


/*
*********************************************************************************************************
* Function Name : csp_ADC2_SetupWaitRead
* Description   : This function is used to read an ADC channel from ADC 2
* Arguments     : uint8_t channel
* Returns       : uint16_t
* Notes         : This function:
*				   > Configures for the selected ADC regular channel its corresponding rank in the sequencer and its sample time
*				   > Enables the selected ADC software start conversion.
*				   > Checks whether the specified ADC flag (End Of Conversion) is set or not.
*				   > Returns the last ADCx conversion result data for regular channel.
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    11/11/2010      Philip Gillespie       Original Created
*********************************************************************************************************
*/
uint16_t csp_ADC2_SetupWaitRead(uint8_t adc_channel_u8)
{
	adc2_channel_setup  =  adc_channel_u8;
	ADC_RegularChannelConfig(ADC2, adc_channel_u8, 1, ADC_SampleTime_239Cycles5);	 /* Start the conversion */
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);								 		/* Wait until conversion completion */
	while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);				 		/* Get the conversion value		*/

	return ADC_GetConversionValue(ADC2);
}

/*
*********************************************************************************************************
* Function Name : csp_ADC2_Read
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
uint16_t csp_ADC2_Read(void)
{
	while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);		// Wait until conversion completion
  	return (ADC_GetConversionValue(ADC2) );						// Get the conversion value
}

/*
*********************************************************************************************************
* Function Name : csp_ADC2_ReadSetup
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
void csp_ADC2_Setup(uint8_t adc_channel_u8)
{
    adc2_channel_setup  =  adc_channel_u8;
  	ADC_RegularChannelConfig(ADC2, adc_channel_u8, 1, ADC_SampleTime_239Cycles5);	 	// Setup the conversion
  	ADC_SoftwareStartConvCmd(ADC2, ENABLE);												// Start the conversion

  	return;
}


/**********************************************************************************************************
 *			End of csp_STM32_ADC2.c
 **********************************************************************************************************/
