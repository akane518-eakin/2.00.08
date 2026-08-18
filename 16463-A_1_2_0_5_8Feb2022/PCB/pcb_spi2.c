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
 * Filename    :  pcb_spi2.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "pcb_spi2.h"


#ifdef	SPI2_EN
#include "csp_STM32_spi.h"
#include "csp_STM32_spi2.h"
#include "pcb_pins.h"




/**********************************************************************************************************
 * Function Name : spi_1_cs_manager
 * Description   : This function is used to control the chip selects for the SPI devices on channel 2.
 * Arguments     : uint8_t device_cs_u8 - device for chips select
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		31/08/2010      Stephen Serplus     Original Created
 *********************************************************************************************************/
void spi2_cs_manager(uint8_t device_cs_u8)
{
/* Local Variables */
	static uint8_t			last_active_spi_dev	=  0xff;
/* Code */
	switch(device_cs_u8){
		case AD7794_CHIP_DIS:
		case PATIENT_ADC_DIS:
		case PATIENT_EE_ADC_DIS:
			PinSet(	SPI2_CS_ADC,1);
			PinSet(	SPI2_CS_PRESURE_ADC,1);
			PinSet(	SPI2_CS_PRESURE_EE,1);

			break;

		case AD7794_CHIP_EN:
			if(last_active_spi_dev != AD7794_CHIP_EN){
				//list all other chips on bus here as DIS
				PinSet(	SPI2_CS_PRESURE_ADC,1);
				PinSet(	SPI2_CS_PRESURE_EE,1);
				//Configure the SPI bus here for this device
				spi2_config(SPI_MODE_0,1000000);
	            last_active_spi_dev =  AD7794_CHIP_EN;
			}
			// enable the spi device here
			PinSet(	SPI2_CS_ADC,0);
			break;
		case PATIENT_ADC_EN:
			if(last_active_spi_dev != PATIENT_ADC_EN){
				//list all other chips on bus here as DIS
				PinSet(	SPI2_CS_ADC,1);
				PinSet(	SPI2_CS_PRESURE_EE,1);
				//Configure the SPI bus here for this device
				spi2_config(SPI_MODE_1,5000000);
	            last_active_spi_dev =  PATIENT_ADC_EN;
			}
			// enable the spi device here
			PinSet(	SPI2_CS_PRESURE_ADC,0);
			break;
		case PATIENT_EE_ADC_EN:
			if(last_active_spi_dev != PATIENT_EE_ADC_EN){
				//list all other chips on bus here as DIS
				PinSet(	SPI2_CS_ADC,1);
				PinSet(	SPI2_CS_PRESURE_ADC,1);
				//Configure the SPI bus here for this device
				spi2_config(SPI_MODE_0,5000000);
	            last_active_spi_dev =  PATIENT_EE_ADC_EN;
			}
			// enable the spi device here
			PinSet(	SPI2_CS_PRESURE_EE,0);
			break;

		default:
			printf("\r\n!Err! SPI2 CS man");
	}

	return;

}


#endif	//#ifdef	SPI2_EN

//end of file
