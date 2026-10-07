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
 * Filename    :  pcb_spi1.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "pcb_spi1.h"

#ifdef	SPI1_EN
#include "csp_STM32_spi.h"
#include "csp_STM32_spi1.h"
#include "pcb_pins.h"


/**********************************************************************************************************
 *	File scope Vars
 *********************************************************************************************************/


/**********************************************************************************************************
 *********************************************************************************************************/


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
void spi1_cs_manager(uint8_t device_cs_u8)
{
/* Local Variables */
//	static uint8_t			last_active_spi_dev	=  0xff;

/* Code */
    switch(device_cs_u8)
    {
    	case S25FL0xx_CHIP1_DIS:
    	case S25FL0xx_CHIP2_DIS:
			PinSet(SPI1_CS_MEM0,1);
			PinSet(SPI1_CS_MEM1,1);
			break;
		case S25FL0xx_CHIP1_EN:
			PinSet(SPI1_CS_MEM1,1);	//disable other devices
			PinSet(SPI1_CS_MEM0,0);
			break;
		case S25FL0xx_CHIP2_EN:
			PinSet(SPI1_CS_MEM0,1);	//disable other devices
			PinSet(SPI1_CS_MEM1,0);
			break;

/*
		case S25FL0xx_CHIP1_EN:
			if(last_active_spi_dev != S25FL0xx_CHIP1_EN){
				//list all other chips on bus here as DIS
				PinSet(SPI1_CS_MEM1,1);
				//Configure the SPI bus here for this device
				spi1_config(SPI_MODE_0,10000000);
	            last_active_spi_dev =  S25FL0xx_CHIP1_EN;
			}
			// enable the spi device here
			PinSet(SPI1_CS_MEM0,0);
			break;
		case S25FL0xx_CHIP2_EN:
			if(last_active_spi_dev != S25FL0xx_CHIP2_EN){
				//list all other chips on bus here as DIS
				PinSet(SPI1_CS_MEM0,1);
				//Configure the SPI bus here for this device
				spi1_config(SPI_MODE_0,10000000);
	            last_active_spi_dev =  S25FL0xx_CHIP2_EN;
			}
			// enable the spi device here
			PinSet(SPI1_CS_MEM1,0);
			break;
*/
		default:
			printf("\r\n!Err! SPI1 CS man");
    }

  return;

}



#endif	//#ifdef	SPI1_EN

//end of file
