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
 * Filename    :  S25FL0xx.c
 * Date Created:  Tue 04 Apr 2017 09:48:17 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include "stdint.h"


#include "app_system.h"

#include "S25FL0xx.h"

#include "pcb_spi1.h"
#include "pcb_spi2.h"
#include "csp_STM32_uart2.h"
#include "csp_STM32_delay.h"

#include "glb_typedefs.h"
#include "csp_STM32_iwdg.h"

/**********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES
**********************************************************************************************************/
uint8_t 	(*S25FL0xx_spi)(uint8_t);
void 	(*S25FL0xx_chip_select)(uint8_t);



/*
*********************************************************************************************************
* Function Name : S25FL0xx_spi_fun_sel
* Description   : This function is used .
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	03/03/14	    	Pauric Lynch        Original Created
*********************************************************************************************************
*/
void S25FL0xx_spi_fun_sel(uint8_t(*spi_funct_ptr)(uint8_t), void (*chip_select_ptr)(uint8_t))
{
	S25FL0xx_spi			= spi_funct_ptr;
	S25FL0xx_chip_select 	= chip_select_ptr;

}



/*
*********************************************************************************************************
* Function Name : S25FL0xx_ReadStatusRegister
* Description   : This function is used .
* Arguments     : uint8_t chip_cs
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	03/03/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
uint8_t S25FL0xx_ReadStatusRegister(uint8_t chip_cs)
{
/* Local Variables */
	uint8_t status;

/* Code */
	S25FL0xx_chip_select(chip_cs);
	status = S25FL0xx_spi(RDSR);
	status = S25FL0xx_spi(DUMMY);
	S25FL0xx_chip_select(~chip_cs);

	return(status);
}



/*
*********************************************************************************************************
* Function Name : S25FL0xx_WriteEnable
* Description   : This function enable page wrtie to the flash .
* Arguments     : uint8_t chip_cs
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	27/02/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_WriteEnable(uint8_t chip_cs)
{
	S25FL0xx_WaitWriteComplete(chip_cs,1000);
	S25FL0xx_chip_select(chip_cs);
	S25FL0xx_spi(WREN);
	S25FL0xx_chip_select(~chip_cs);

	return;
}

/*
*********************************************************************************************************
* Function Name : S25FL0xx_WriteDisable
* Description   : This function enable page wrtie to the flash .
* Arguments     : uint8_t chip_cs
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	27/02/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_WriteDisable(uint8_t chip_cs)
{
	S25FL0xx_WaitWriteComplete(chip_cs,1000);
	S25FL0xx_chip_select(chip_cs);
	S25FL0xx_spi(WRDI);
	S25FL0xx_chip_select(~chip_cs);

	return;
}

/*
*********************************************************************************************************
* Function Name : S25FL0xx_EnableAllRegions
* Description   : This function allows all memory regions to be written to.
* Arguments     : uint8_t chip_cs
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	27/02/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_EnableAllRegions(uint8_t chip_cs)
{
	S25FL0xx_WriteEnable(chip_cs);

	S25FL0xx_WaitWriteComplete(chip_cs,1000);

	S25FL0xx_chip_select(chip_cs);
	S25FL0xx_spi(WRSR);
	S25FL0xx_spi(SR_WEL);				// Set the Write Enable Command
	S25FL0xx_chip_select(~chip_cs);

	return;
}

/*
*********************************************************************************************************
* Function Name : S25FL0xx_SWReset
* Description   : This function performs a sware reset
* Arguments     : uint8_t chip_cs
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	18/10/18	    	WPaul        Original Created
*********************************************************************************************************
*/
void S25FL0xx_SWReset(uint8_t chip_cs)
{

	S25FL0xx_chip_select(chip_cs);
	S25FL0xx_spi(S25FL_RSTEN);
	S25FL0xx_spi(S25FL_RST);				// Set the Write Enable Command
	S25FL0xx_chip_select(~chip_cs);

	Delay(2);

	return;
}

/*
*********************************************************************************************************
* Function Name : S25FL0xx_Config
* Description   : This function is enables the chip by unlocking all of the memory blocks.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	20/03/14	    	Pauric Lynch        Original Created
*********************************************************************************************************
*/
void S25FL0xx_Config(void)
{
	S25FL0xx_chip_select(~S25FL0xx_CHIP1_EN);		//disables all devices on the bus
	PinSet(SPI_MEM_PWR,0);				//0= power enabled

	Delay(50);

	S25FL0xx_SWReset(S25FL0xx_CHIP1_EN);

	return;
}


/*
*********************************************************************************************************
* Function Name : S25FL0xx_WaitWriteComplete
* Description   : This function is used .
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	27/02/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
uint8_t S25FL0xx_WaitWriteComplete(uint8_t chip_cs, uint32_t max_ms)
{
	uint8_t 	status;
	uint32_t	sys_tic;

	sys_tic		=  sys_tic_rd() + max_ms;

	S25FL0xx_chip_select(chip_cs);													// Select the Chip
	status = S25FL0xx_spi(RDSR);												// Send the read Register Command
	do{
		asm("nop");																// allow 1 cycle delay so we dont bombard the memory with requests
		status = S25FL0xx_spi(DUMMY);											// Read the state of the Status Register
	}while((status & SR_BUSY != 0)&&(sys_tic_rd()<sys_tic));					// Wait until the device is ready

	S25FL0xx_chip_select(~chip_cs);												// Disable the chip

	return(status& SR_BUSY);	//0 = not busy,
}


/*
*********************************************************************************************************
* Function Name : S25FL0xx_read
* Description   : This function is used read bytes from memory.
* Arguments     : 	uint8_t 	chip_cs
*					uint32_t	addr
*					uint8_t 	*data_ptr
*					uint32_t	nbytes
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	27/02/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_WriteBytes(uint8_t chip_cs, uint32_t addr, uint8_t *data_ptr, uint32_t nbytes)
{
	uint16_t i;

	addr	&= 0x7FFFFF;

	S25FL0xx_WaitWriteComplete(chip_cs,1000);				// Wait until chip is ready
	S25FL0xx_WriteEnable(chip_cs);						// A WREN command is required prior to writing the PP command. P.41 Datasheet
	S25FL0xx_WaitWriteComplete(chip_cs,1000);				// Wait until chip is ready
	S25FL0xx_chip_select(chip_cs);				// Select the Chip
	S25FL0xx_spi(PAGE_PROGRAM);					// Send the Read data Command
	S25FL0xx_spi((addr >> 16) & 0xff);			// Send the High byte
	S25FL0xx_spi((addr >> 8) & 0xff);			//
	S25FL0xx_spi(addr & 0xff);					// Send the Low byte

	for(i=0; i<nbytes; i++){
		S25FL0xx_spi(*data_ptr);				// Retreive the data
		data_ptr++;
	}

	S25FL0xx_chip_select(~chip_cs);				// Disable the chip

	return;
}



/*
*********************************************************************************************************
* Function Name : S25FL0xx_read
* Description   : This function is used read bytes from memory.
* Arguments     : 	uint8_t		chip_cs
*					uint32_t	addr
*					uint8_t 	*data_ptr
*					uint32_t	nbytes
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	27/02/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_Read(uint8_t chip_cs, uint32_t addr, uint8_t *data_ptr, uint32_t nbytes)
{
	uint16_t i;

	addr	&= 0x7FFFFF;

	S25FL0xx_WaitWriteComplete(chip_cs,1000);				// Wait until chip is ready
	S25FL0xx_chip_select(chip_cs);				// Select the Chip
	S25FL0xx_spi(READ);							// Send the Read data Command
	S25FL0xx_spi((addr >> 16) & 0xff);			// Send the High byte
	S25FL0xx_spi((addr >> 8) & 0xff);			//
	S25FL0xx_spi(addr & 0xff);					// Send the Low byte

	for(i=0; i<nbytes; i++)
	{
		*data_ptr = S25FL0xx_spi(DUMMY);		// Retrieve the data
		data_ptr++;
	}
	S25FL0xx_chip_select(~chip_cs);				// Disable the chip

	return;
}


/*
*********************************************************************************************************
* Function Name :	S25FL0xx_4KBSectorErase
* Description   : This function is used to erase a 4kB sector
* Arguments     : 	uint8_t chip_cs
*					uint32_t addr
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	27/02/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_4KBSectorErase(uint8_t chip_cs, uint32_t addr)
{
	uint8_t	finished;
	addr	&= 0x7FFFFF;

	S25FL0xx_WaitWriteComplete(chip_cs,1000);
	S25FL0xx_WriteEnable(chip_cs);
	S25FL0xx_WaitWriteComplete(chip_cs,1000);				// Wait until chip is ready
	S25FL0xx_chip_select(chip_cs);				// Select the Chip
	S25FL0xx_spi(SECTOR_ERASE);					// Send the Read data Command
	S25FL0xx_spi((addr >> 16) & 0xff);			// Send the High byte
	S25FL0xx_spi((addr >> 8) & 0xff);			//
	S25FL0xx_spi(addr & 0xff);					// Send the Low byte
	S25FL0xx_chip_select(~chip_cs);
	do{
		finished	=  S25FL0xx_WaitWriteComplete(chip_cs,1000);											// Takes a while to complete
	}while(finished);

	return;
}


/*
*********************************************************************************************************
* Function Name : S25FL0xx_BulkErase
* Description   : This function sets all the bits within the entire memory array to logic 1s.
* Arguments     : uint8_t chip_cs
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	21/03/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_BulkErase(uint8_t chip_cs)
{
	S25FL0xx_WriteEnable(chip_cs);						// Must be done before every Write
	S25FL0xx_WaitWriteComplete(chip_cs,1000);				// Wait until chip is ready
	S25FL0xx_chip_select(chip_cs);						// Select the Chip
	S25FL0xx_spi(BULK_ERASE);							// Send the Bulk Erase Command
	S25FL0xx_chip_select(~chip_cs);						// Release the device

	return;
}


/*
*********************************************************************************************************
* Function Name : S25FL0xx_BulkErase
* Description   : This function sets all the bits within the entire memory array to logic 1s.
* Arguments     :	uint8_t chip_cs
*					uint32_t addr None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	21/03/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_64KBSectorErase(uint8_t chip_cs,uint32_t addr)
{
	uint8_t	finished;
	addr	&= 0x7FFFFF;

	S25FL0xx_WriteEnable(chip_cs);
	S25FL0xx_WaitWriteComplete(chip_cs,1000);											// Wait until chip is ready
	S25FL0xx_chip_select(chip_cs);								// Select the Chip
	S25FL0xx_spi(BULK_ERASE_64);											// Send the Read data Command
	S25FL0xx_spi((addr >> 16) & 0xff);										// Send the High byte
	S25FL0xx_spi((addr >> 8) & 0xff);										//
	S25FL0xx_spi(addr & 0xff);												// Send the Low byte
	S25FL0xx_chip_select(~chip_cs);
	do{
		finished	=  S25FL0xx_WaitWriteComplete(chip_cs,1000);											// Takes a while to complete
	}while(finished);

	return;
}

/*
*********************************************************************************************************
* Function Name : S25FL0xx_ReadAndPrint
* Description   : This function is used read bytes from memory.
* Arguments     : 	uint8_t chip_cs
*					uint32_t addr
*					uint32_t nbytes
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	27/02/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_ReadAndPrint(uint8_t chip_cs,uint32_t addr, uint32_t nbytes)
{
/* Local Variables */
	uint8_t data;
	uint16_t i;

	addr	&= 0x7FFFFF;

/* Code */
	S25FL0xx_WaitWriteComplete(chip_cs,1000);											// Wait until chip is ready
	S25FL0xx_chip_select(chip_cs);								// Select the Chip
	S25FL0xx_spi(READ);														// Send the Read data Command
	S25FL0xx_spi((addr >> 16) & 0xff);										// Send the High byte
	S25FL0xx_spi((addr >> 8) & 0xff);										//
	S25FL0xx_spi(addr & 0xff);												// Send the Low byte

	for(i=0; i<nbytes; i++)
	{
		data = S25FL0xx_spi(DUMMY);											// Retreive the data
		uart2_putchar(data);
	}
	S25FL0xx_chip_select(~chip_cs);								// Disable the chip

	return;
}



/*
*********************************************************************************************************
* Function Name :	S25FL0xx_EraseMultipleSectors
* Description   : This function is used to erase multiple sectors on a single chip
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	24/03/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "uint8_t chip_cs"
*********************************************************************************************************
*/
void S25FL0xx_EraseMultipleSectors(uint8_t chip_cs,uint32_t start_addr, uint8_t num_of_sectors)
{
/* Local Variables */
	uint8_t		s;
	uint32_t	sector_addr;

/* Code */
	printf("\r\nErasing Memory Sectors...\r\n");
	for(s=0; s<num_of_sectors; s++)
	{
		sector_addr = start_addr + (s*SECTOR_SIZE);						// Get the sector Address
		S25FL0xx_64KBSectorErase(chip_cs,sector_addr);							// Erase the sector
		printf("\rSector %d of %d erased  ",s+1, num_of_sectors);

		app_sys_watchdog_reload();
	}

	return;

}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
