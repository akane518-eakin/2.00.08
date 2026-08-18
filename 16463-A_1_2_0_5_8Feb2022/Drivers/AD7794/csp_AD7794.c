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
*                                          csp_AD7794.c
*
* Filename    :  csp_AD7794.c
* Programmer  :  Pauric Lynch / William Paul
* Description :
* Compiler    :  IAR
* Target      :  STM32F103
* Version     :  Version 1.1.0
**********************************************************************************************************
*/
/*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************/
#include "stdio.h"

#include "csp_AD7794.h"


#include "pcb_pins.h"
#include "pcb_spi1.h"
#include "pcb_spi2.h"

#include "csp_STM32_spi1.h"
#include "csp_STM32_spi2.h"

#include "csp_STM32_delay.h"
/*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************/


/*********************************************************************************************************
*                                           VARIABLES
*********************************************************************************************************/
uint8_t 	(*AD7794_spi)(uint8_t);
void 		(*AD7794_chip_select)(uint8_t);

uint16_t	AD7794_spi16(uint16_t data);
uint32_t 	AD7794_spi24(uint32_t data);


/*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************/
/**********************************************************************************************************
 * Function Name :	hal_AD7794_spi_fun_sel
 * Description   :	This function is used to link the spi module under use to the code contained within.
 * Arguments     :	pointer to spi function
 * Returns       :	None
 * Notes         :	None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 0.0.1        06/12/2012      Pauric Lynch        Original Created
 **********************************************************************************************************/
void AD7794_spi_fun_sel (   uint8_t (*spi_funct_ptr)(uint8_t),
                                void (*chip_select_ptr)(uint8_t)
                            )
{
// Code
    AD7794_spi			= spi_funct_ptr;
	AD7794_chip_select 	= chip_select_ptr;
    return;
}



/*
*********************************************************************************************************
* Function Name : AD7794_spi16
* Description   : This function is used to write an int16 on SPI.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	10/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
uint16_t AD7794_spi16(uint16_t data)
{
	union{
		uint8_t		a[2];
		uint16_t	b;
	}out,in;

// Code
	out.b	=  data;
	in.a[1]	=  AD7794_spi(	out.a[1]	);				// Send MSB Mode Register Settings
	in.a[0]	=  AD7794_spi(	out.a[0]	);				// Send LSB Mode Register Settings

	return(in.b);
}

/*
*********************************************************************************************************
* Function Name : AD7794_spi16
* Description   : This function is used to write an int16 on SPI.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	10/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
uint32_t AD7794_spi24(uint32_t data)
{
	union{
		uint8_t		a[4];
		uint32_t	b;
	}out,in;

// Code
	out.b	=  data;
	in.a[3]	=  0;
	in.a[2]	=  AD7794_spi(	out.a[2]	);				// Send LSB Mode Register Settings
	in.a[1]	=  AD7794_spi(	out.a[1]	);				// Send MSB Mode Register Settings
	in.a[0]	=  AD7794_spi(	out.a[0]	);				// Send LSB Mode Register Settings

	return(in.b);
}


/*
*********************************************************************************************************
* Function Name : ad7794_reg_wr8
* Description   : This function is used to write to the Config Register.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	11/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
void ad7794_reg_wr8(uint8_t chip,uint8_t reg, uint8_t data)
{

// Code
	AD7794_chip_select(chip);													// Enable the Chip
	AD7794_spi(reg);															// Select the CONFIG Register
	AD7794_spi(data);															// Write contents to the CONFIG Register
	AD7794_chip_select(~chip);													// Disable the Chip
	return;
}

/*
*********************************************************************************************************
* Function Name : ad7794_reg_wr16
* Description   : This function is used to write to the Config Register.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	11/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
void ad7794_reg_wr16(uint8_t chip,uint8_t reg, uint16_t data)
{

// Code
	AD7794_chip_select(chip);													// Enable the Chip
	AD7794_spi(reg);															// Select the CONFIG Register
	AD7794_spi16(data);															// Write contents to the CONFIG Register
	AD7794_chip_select(~chip);													// Disable the Chip
	return;
}

/*
*********************************************************************************************************
* Function Name : ad7794_reg_wr24
* Description   : This function is used to write to the Config Register.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	11/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
void ad7794_reg_wr24(uint8_t chip,uint8_t reg, uint32_t data)
{

// Code
	AD7794_chip_select(chip);													// Enable the Chip
	AD7794_spi(reg);															// Select the CONFIG Register
	AD7794_spi24(data);															// Write contents to the CONFIG Register
	AD7794_chip_select(~chip);													// Disable the Chip
	return;
}

/*
*********************************************************************************************************
* Function Name : ad7794_reg_rd8
* Description   : This function is reads the content of a register .
* Arguments     :	uint8_t chip => the selected device.
*					uint8_t addr => address of the data register.
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	06/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
uint8_t ad7794_reg_rd8(uint8_t chip, uint8_t reg)
{
	uint8_t data;

// Code
	AD7794_chip_select(chip);													// Enable the device
	AD7794_spi(reg| REG_RD);                                                   	// Select the Register to Read
	data = AD7794_spi(0xff);													// Send dummy Byte and collect register Content
	AD7794_chip_select(~chip);                                                  // Disable the Chip

    return(data);
}



/*
*********************************************************************************************************
* Function Name : ad7794_reg_rd16
* Description   : This function is used read bytes from a register.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	07/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
uint16_t ad7794_reg_rd16(uint8_t chip,uint8_t reg)
{
	uint16_t	data;

// Code
	AD7794_chip_select(chip);													// Enable the device
	AD7794_spi(reg|REG_RD);														// Select the Register to Read
	data	=  AD7794_spi16(0xffff);
	AD7794_chip_select(~chip);													// Disable the Chip

	return(data);
}

/*
*********************************************************************************************************
* Function Name : ad7794_reg_rd16
* Description   : This function is used read bytes from a register.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	07/12/12	    	Pauric Lynch     Original Created
*********************************************************************************************************
*/
uint32_t ad7794_reg_rd24(uint8_t chip,uint8_t reg)
{
	uint32_t	data;

// Code
	AD7794_chip_select(chip);													// Enable the device
	AD7794_spi(reg|REG_RD);														// Select the Register to Read
	data	=  AD7794_spi24(0x0055AAff);
	AD7794_chip_select(~chip);													// Disable the Chip

	return(data);
}



/*
*********************************************************************************************************
* Function Name : ad7794_Reset
* Description   : This function is used to reset a chip.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	10/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
void ad7794_Reset(uint8_t chip)
{
	// Code
	AD7794_chip_select(chip);													// Enable the Chip
	AD7794_spi(0xff);															// 32 clock cycles returns the ADC to its
	AD7794_spi(0xff);															// default state and resets the entire part
	AD7794_spi(0xff);
	AD7794_spi(0xff);
	AD7794_chip_select(~chip);													// Disable the Chip

	return;
}



/*
*********************************************************************************************************
* Function Name : ad7794_reg_print
* Description   : This function is used to test the ADC .
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	10/12/12	    	Pauric Lynch     	Original Created
*********************************************************************************************************
*/
void ad7794_reg_print(uint8_t chip)
{
	uint8_t		rd8;
	uint16_t	rd16;
	uint32_t	rd32;

// Code
	printf("\r\nAD7794 Reg");
	rd8		=  ad7794_reg_rd8(chip, REG_STATUS);
	printf("\r\n REG_STATUS     =  %02x",rd8);

	rd16	=  ad7794_reg_rd16(chip, REG_MODE);
	printf("\r\n REG_MODE       =  %04x",rd16);

	rd16	=  ad7794_reg_rd16(chip, REG_CONFIG);
	printf("\r\n REG_CONFIG     =  %04x",rd16);

	rd32	=  ad7794_reg_rd24(chip, REG_DATA);
	printf("\r\n REG_DATA       =  %02x",rd32);

	rd8		=  ad7794_reg_rd8(chip, REG_ID);
	printf("\r\n REG_ID         =  %02x",rd8);

	rd8		=  ad7794_reg_rd8(chip, REG_IO);
	printf("\r\n REG_IO         =  %02x",rd8);

	rd32	=  ad7794_reg_rd24(chip, REG_OFFSET);
	printf("\r\n REG_OFFSET     =  %02x",rd32);

	rd32	=  ad7794_reg_rd24(chip, REG_FULL_SCALE);
	printf("\r\n REG_FULL_SCALE =  %02x",rd32);

	printf("\r\nEnd");
	return;
}




/*
*********************************************************************************************************
* Function Name : ad7794_convert_single_start
* Description   : This function is used to start a single A2D convertion
* Arguments     : 	uint8_t chip	The selected Device
*					uint8_t ch		A2D channel
* Returns       : void
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	01/07/13	    	William Paul     	Original Created
*********************************************************************************************************
*/
void ad7794_convert_single_start(uint8_t chip,uint8_t ch)
{
	AD7794_chip_select(chip);														// Enable the Chip
	AD7794_spi(REG_CONFIG);
	AD7794_spi16(REG_CONF_UNIPOLAR | REG_CONF_GAIN1 | REG_CONF_REF_EXT_1 | REG_CONF_BUF | ch);


	AD7794_spi(REG_MODE);															// Select the MODE Register
	AD7794_spi16(REG_MODE_SINGLE | REG_MODE_R500);

	//leave CS active low
	return;
}

/*
*********************************************************************************************************
* Function Name : ad7794_convert_continuous_start
* Description   : This function is used to start a single A2D convertion
* Arguments     : 	uint8_t chip	The selected Device
*					uint8_t ch		A2D channel
* Returns       : void
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	01/07/13	    	William Paul     	Original Created
*********************************************************************************************************
*/
void ad7794_convert_continuous_start(uint8_t chip,uint8_t ch)
{

// Code
	AD7794_chip_select(chip);														// Enable the Chip

	AD7794_spi(REG_CONFIG);															// Select the CONFIG Register
	AD7794_spi16(REG_CONF_UNIPOLAR | REG_CONF_GAIN2 | REG_CONF_REF_EXT_1 | REG_CONF_BUF | ch);

	AD7794_spi(REG_MODE);															// Select the MODE Register
	AD7794_spi16(REG_MODE_CONTINUOUS | REG_MODE_R250);

	//leave CS active low
	return;
}

/*
*********************************************************************************************************
* Function Name : ad7794_convert_continuous_stop
* Description   : This function is used to start a single A2D convertion
* Arguments     : 	uint8_t chip	The selected Device
*					uint8_t ch		A2D channel
* Returns       : void
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	01/07/13	    	William Paul     	Original Created
*********************************************************************************************************
*/
void ad7794_convert_continuous_stop(uint8_t chip)
{
	uint8_t		pinhigh;
	uint16_t	cnt	=  0;

// Code
	do{
		pinhigh	=  ad7794_convert_status();
		cnt++;
	}while(pinhigh && cnt<1000);


	AD7794_spi(REG_DATA | REG_RD);						//stop continuous read
	AD7794_chip_select(~chip);

	return;
}

/*
*********************************************************************************************************
* Function Name : ad7794_convert_status
* Description   : This function is used check if the A2D convertion is finished
* Arguments     : void
* Returns       : uint8_t 	pin_status		0= finished
*											1= ADC in progress
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	01/07/13	    	William Paul     	Original Created
*********************************************************************************************************
*/
uint8_t ad7794_convert_status(void)
{
//	return( PinRead(SPI1_MISO) );				// Wait for Data RDY/MISO to go low		SPI1
	return( PinRead(SPI2_MISO) );				// Wait for Data RDY/MISO to go low		spi2
}

/*
*********************************************************************************************************
* Function Name : ad7794_convert_status
* Description   : This function is used check if the A2D convertion is finished
* Arguments     : void
* Returns       : uint8_t 	pin_status		0= finished
*											1= ADC in progress
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	01/07/13	    	William Paul     	Original Created
*********************************************************************************************************
*/
uint32_t ad7794_convert_value(uint8_t chip)
{
    uint32_t	result;
	uint8_t		pinhigh;
	uint16_t	cnt	=  0;

// Code
	do{
		pinhigh	=  ad7794_convert_status();
		cnt++;
	}while(pinhigh && cnt<1000);

	result	=  ad7794_reg_rd24(chip, REG_DATA);

	if(cnt>=1000){
		return(1);
	}

    return(result);
}

/*
*********************************************************************************************************
* Function Name : ad7794_convert_status
* Description   : This function is used check if the A2D convertion is finished
* Arguments     : void
* Returns       : uint8_t 	pin_status		0= finished
*											1= ADC in progress
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	01/07/13	    	William Paul     	Original Created
*********************************************************************************************************
*/
uint32_t ad7794_convert_continuous_value(uint8_t chip,uint8_t no_of_samples,uint8_t *sam_no,uint32_t *run_total)
{
	uint32_t	result;
	uint8_t		status;

// Code
	if(*sam_no < no_of_samples){
		do{
			status	=  ad7794_convert_status();
		}while(	status == 1	);								//wait for convertion to end

		if(*sam_no==0){
			AD7794_spi(REG_DATA | REG_RD | REG_CREAD);		//if first sample enable continuous read
		}

		*run_total	+= AD7794_spi24(0x55555555);				//get A2D value
		*sam_no		+= 1;
	}
	result	=  (*run_total)/(*sam_no);

    return(result);
}


/*
*********************************************************************************************************
* Function Name : ad7794_convert_single_wait
* Description   : This function is used to do one ADC conversion.
* Arguments     : 	uint8_t chip: 		The selected Device
*					uint8_t ch:			The A2D channel to read
* Returns       : 	uint32_t result: 	The ADC result
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	01/07/13	    	William Paul     	Original Created
*********************************************************************************************************
*/
uint32_t ad7794_convert_single_wait(uint8_t chip,uint8_t ch)
{
    uint32_t	result;
	uint8_t		pinhigh;

// Code
	ad7794_convert_single_start(chip, ch);

	do{
		pinhigh	=  ad7794_convert_status();
	}while(pinhigh);

	result	=  ad7794_convert_value(chip);

    return(result);
}

/*
*********************************************************************************************************
* Function Name : ad7794_convert_single_wait
* Description   : This function is used to do one ADC conversion.
* Arguments     : 	uint8_t chip: 		The selected Device
*					uint8_t ch:			The A2D channel to read
* Returns       : 	uint32_t result: 	The ADC result
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	01/07/13	    	William Paul     	Original Created
*********************************************************************************************************
*/
uint32_t ad7794_convert_continuous_wait(uint8_t chip,uint8_t ch,uint8_t no_of_samples)
{
	uint8_t		sam_no		=  0;
	uint32_t	run_total	=  0;
	uint8_t		A2D_ready;
	uint32_t	result;

// Code
	ad7794_convert_continuous_start(chip,ch);
	do
	{
		do
		{
			A2D_ready	=  ad7794_convert_status();
		}while(A2D_ready   == 1);
		result	=  ad7794_convert_continuous_value(chip,no_of_samples,&sam_no,&run_total);
	}while(sam_no < no_of_samples);

	ad7794_convert_continuous_stop(chip);

    return(result);
}

/*************************************************************************************************
* Function Name : 	ad7794_err_check
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		30/01/19	W. Paul			Created
*************************************************************************************************/
uint8_t ad7794_err_check(uint8_t chip)
{
	uint8_t		rd8;
	uint16_t	rd16;
//	uint32_t	rd32;

	uint8_t		result = 1;		//0=fail,1=pass

	rd8		=  ad7794_reg_rd8(chip, REG_STATUS);
	if(rd8 & 0x70){	result = 0;	}

	rd16	=  ad7794_reg_rd16(chip, REG_MODE);
	if(rd16 & 0x0D20){	result = 0;	}

	rd16	=  ad7794_reg_rd16(chip, REG_CONFIG);
	if(rd16 & 0x00C0){	result = 0;	}

//	rd32	=  ad7794_reg_rd24(chip, REG_DATA);

	rd8		=  ad7794_reg_rd8(chip, REG_ID);
	if(rd8 != 0x4F){	result = 0;	}

	rd8		=  ad7794_reg_rd8(chip, REG_IO);
	if(rd8){	result = 0;	}

//	rd32	=  ad7794_reg_rd24(chip, REG_OFFSET);
//	rd32	=  ad7794_reg_rd24(chip, REG_FULL_SCALE);

	return(result);
}


/*********************************************************************************************************
*********************************************************************************************************/