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
 * Filename    :  csp_S25FL0xx.c
 * Date Created:  Tue 04 Apr 2017 09:41:53 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include "stdint.h"

#include "csp_S25FL0xx.h"
#include "S25FL0xx.h"

#include "pcb_spi1.h"
#include "pcb_spi2.h"

#include "csp_STM32_uart1.h"
#include "csp_STM32_uart2.h"



/*********************************************************************************************************
 *		Local Variables
 ********************************************************************************************************/
uint8_t	sys_mem_buf[S25FL_4KB_SECTOR_SIZE];
uint8_t	sys_mem_init	=  0;


/*********************************************************************************************************
 ********************************************************************************************************/


/*
*********************************************************************************************************
* Function Name : csp_mem_config
* Description   : This function is used for configuring external FLASH
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			05/11/2021			Tim Barr		Original Created
*********************************************************************************************************
*/
void csp_mem_config(void)
{
	// Local Variables
	
	// Code
	if(sys_mem_init		>  0){
		return;
	}
	
	S25FL0xx_Config();
	
	sys_mem_init		=  1;
	
	return;
}


/*
*********************************************************************************************************
* Function Name : csp_mem_deconfig
* Description   : This function is used for deconfiguring external FLASH
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			05/11/2021			Tim Barr		Original Created
*********************************************************************************************************
*/
void csp_mem_deconfig(void)
{
	// Local Variables
	
	// Code
	PinSet(SPI_MEM_PWR, 1);
	
//	sys_mem_init		=  0;
	
	return;
}


/**********************************************************************************************************
* Function Name :	csp_mem_wr
* Description   : This function is used to write data to the flash chip
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	24/03/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "chip"
**********************************************************************************************************/
void csp_mem_wr(uint8_t *data_ptr, uint32_t addr, uint32_t nbytes)
{
	// Local Variables
	uint8_t		page_remainder;
	uint8_t		pages_2_write;
	uint16_t	p;
	uint16_t	byte_total;
	uint32_t	page_addr;
	uint8_t		chip;

	// Code
	csp_mem_config();
	
	chip	=  csp_S25FL0xx_chip_select(&addr,DONT_CORRECT);
	S25FL0xx_WriteEnable(chip);

	page_remainder = addr%S25FL_PAGE_SIZE;										// Is Data to be written to the middle of a Page?
	if(page_remainder > 0)														// Only write data in that page from start, more data will only
	{																			// overflow into the start of the same page otherwise.
		page_remainder = S25FL_PAGE_SIZE - page_remainder;						// How many Bytes left in the page
		if(page_remainder > nbytes) byte_total = nbytes;						// We wont be writing to the end of the current page
		else						byte_total = page_remainder;				// We will be writing to the end of the current page
		chip	=  csp_S25FL0xx_chip_select(&addr,DONT_CORRECT);
		S25FL0xx_WriteBytes(chip,addr, data_ptr, byte_total);					// Write the remaining Bytes
		nbytes 		-= byte_total;												// Remove the page remaining bytes from the total sum
		addr 		+= byte_total;
		(data_ptr) 	+= byte_total;												// Increment The data
	}

	pages_2_write = nbytes/S25FL_PAGE_SIZE;										// Find out how many full pages there are
	for(p=0; p<pages_2_write; p++){
		page_addr 	=  addr + (p*S25FL_PAGE_SIZE);								// Locate the address
		chip		=  csp_S25FL0xx_chip_select(&addr,DONT_CORRECT);
		S25FL0xx_WriteBytes(chip,page_addr, data_ptr, S25FL_PAGE_SIZE);			// Write the remainder data in the next page, 256 max at a time
		data_ptr += S25FL_PAGE_SIZE;											// Increment the pointer 1 page
		nbytes -= S25FL_PAGE_SIZE;												// Remove a page from the Bytes
	}

	/* If there is still Bytes left! This should not be a full page */
	if(nbytes > 0){
		chip	=  csp_S25FL0xx_chip_select(&addr,DONT_CORRECT);
		S25FL0xx_WriteBytes(chip,addr, data_ptr, nbytes);						// Write the remainder data in the next page
	}

	chip	=  csp_S25FL0xx_chip_select(&addr,DONT_CORRECT);
	S25FL0xx_WriteDisable(chip);

	return;
}


/**********************************************************************************************************
* Function Name :	csp_mem_rd
* Description   : This function is used to read data from the flash chip
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	24/03/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "chip"
**********************************************************************************************************/
void csp_mem_rd(uint8_t *data_ptr, uint32_t addr, uint32_t nbytes)
{
	// Local Variables
	uint8_t		chip;
	
	// Code
	csp_mem_config();
	
	chip	=  csp_S25FL0xx_chip_select(&addr,CORRECT);
	S25FL0xx_Read(chip,addr, data_ptr, nbytes);

	return;
}

/**********************************************************************************************************
* Function Name : csp_sys_mem_wr
* Description   : This function is used for system variable settings as it only used small sectors in this region.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		24/03/14	    	Pauric Lynch        Original Created
* 1.0.1		31/03/2016			William Paul		works now for any 4kb sector
**********************************************************************************************************/
void csp_sys_mem_wr(uint8_t *data_ptr, uint32_t addr, uint32_t nbytes)
{
	// Local Variables
	uint16_t	i;
	uint32_t	sector_start;
	uint32_t	sector_add;
	
	// Code
	csp_mem_config();
	
	if((addr &0x7fffff) >= 0x1ffff){
		printf("\r\nError - Sector size in this region is 64KB not 4KB");
	}

	sector_start	=  addr;
	sector_start	&= ~(S25FL_4KB_SECTOR_SIZE-1);
	sector_add	=  addr;
	sector_add	-= sector_start;

	csp_mem_rd(sys_mem_buf, sector_start, S25FL_4KB_SECTOR_SIZE);		// Read in the contents of the register
	for(i=sector_add; i<sector_add+nbytes; i++){
		sys_mem_buf[i] = *data_ptr;					// Move new Data into the array
		data_ptr++;
	}

	csp_erase_4kb_sectors(sector_start);
	csp_mem_wr(sys_mem_buf, sector_start, S25FL_4KB_SECTOR_SIZE);			// Now update the memory

	return;
}



/*
*********************************************************************************************************
* Function Name : csp_erase_64kb_sectors
* Description   : This function is used .
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	25/03/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "chip"
*********************************************************************************************************
*/
void csp_erase_64kb_sectors(uint32_t addr, uint8_t nSectors)
{
	// Local Variables
	uint8_t		chip;
	
	// Code
	csp_mem_config();

	chip	=  csp_S25FL0xx_chip_select(&addr,CORRECT);
	S25FL0xx_WriteEnable(chip);
	S25FL0xx_EraseMultipleSectors(chip,addr, nSectors);						// Erase the sectors

	S25FL0xx_WriteDisable(chip);
	return;
}


/**********************************************************************************************************
* Function Name : csp_erase_64kb_sectors
* Description   : This function is used .
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	25/03/14	    	Pauric Lynch        Original Created
* 2.0.0			11/9/17				William Paul		Add "chip"
**********************************************************************************************************/
void csp_erase_4kb_sectors(uint32_t addr)
{
	// Local Variables
	uint8_t		chip;
	
	// Code
	csp_mem_config();

	chip	=  csp_S25FL0xx_chip_select(&addr,CORRECT);
	S25FL0xx_WriteEnable(chip);

	S25FL0xx_4KBSectorErase(chip,addr);										// Erase the sectors
	S25FL0xx_WriteDisable(chip);

	return;
}


/**********************************************************************************************************
* Function Name : csp_mem_dump
* Description   : This function prints the memory contents of the flash.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	08/01/16	    	Pauric Lynch        Original Created
**********************************************************************************************************/
void csp_mem_dump(uint32_t addr, uint16_t nPages)
{
	// Local Variables
	uint32_t	i;
	uint32_t	j;
	uint32_t	mem_addr;

	// Code
	csp_mem_config();
	for(i=0; i<nPages; i++)
	{
		mem_addr =  addr + (i*S25FL_PAGE_SIZE);
		csp_mem_rd(sys_mem_buf, mem_addr, S25FL_PAGE_SIZE);
		for(j=0; j<S25FL_PAGE_SIZE; j++){
			uart1_putchar(sys_mem_buf[j]);
		}
	}

}

/*************************************************************************************************
* Function Name : 	csp_S25FL0xx_chip_select
* Description   : 	This Function works out which chip is being accessed depending on the address
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t csp_S25FL0xx_chip_select(uint32_t *address,uint8_t correct)
{
	// Local Variables
	uint8_t		chip;
	
	// Code
	csp_mem_config();
	
	if(*address > 0xFFFFFF){
		printf("\r\n S25FL0xx chip address too high  %x",*address);
	}

	if(*address < 0x800000){	chip	=  S25FL0xx_CHIP1_EN;	}
//	else{						chip	=  S25FL0xx_CHIP2_EN;
//		if(correct==CORRECT){
//			address -= 0x800000;
//		}
//	}

	return(chip);
}


/*
*********************************************************************************************************
*											End of csp_S25FL0xx.c
*********************************************************************************************************
*/
