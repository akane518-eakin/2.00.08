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
 * Filename    :  csp_STM32_flash_eeprom.c
 * Programmer  :  William Paul
 * Description :  This module is used to emulate EEPROM mem on the flash of the STM32
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "glb_typedefs.h"
#include "csp_STM32_flash_eeprom.h"
#include "csp_STM32_uart.h"

#include "stm32f10x_flash.h"


/*********************************************************************************************************
*********************************************************************************************************/
uint16_t 		csp_STM32_FindValidPage(uint8_t Operation);
FLASH_Status 	csp_STM32_flash_eeprom_format(void);


/*********************************************************************************************************
*********************************************************************************************************/







/*********************************************************************************************************
 * Function Name : csp_STM32_flash_eeprom_rd
 * Description   : This function is used to read siz amount of data from src_addr to *dst_p
 * Arguments     :	void 		* dst_p		data destination pointer
 *					uint16_t 	src_addr	start address
 *					size_t 		siz			number of 16bit words to read
 * Returns       :	void
 * Notes         :
 *
 * Version		Date (d/m/y)	    Programmer          Reason for Change
 * 1.0.0        dd/mm/20yy          Paul Young          Original Created
 ********************************************************************************************************/
void csp_STM32_flash_eeprom_rd(void *dst_p, uint16_t src_addr, size_t siz)
{
	uint16_t		active_page;
	uint32_t		start_address;
	uint32_t		flash_address;
	uint16_t		i;
	uint16_t 		* data_p;

	data_p			=  (uint16_t*)dst_p;
	active_page		=  csp_STM32_FindValidPage(READ_FROM_VALID_PAGE);

	start_address	=  (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(active_page * PAGE_SIZE));

	flash_address	=  start_address + src_addr + 2;			// 2 ensures we do not write on status 16bit word

    for(i=0;i<siz; i=i+2){										// i=i+2 because read 2 bytes at a time
    	*data_p				= (*(uint16_t*)(flash_address));
		data_p++;
		flash_address		+= 2;
		if(flash_address >= (start_address + PAGE_SIZE) ){
			flash_address	=  start_address + 2;
		}
    }

	return;
}

/*********************************************************************************************************
 * Function Name : csp_STM32_flash_eeprom_wr
 * Description   : This function is used to read siz amount of data from src_addr to *dst_p
 * Arguments     :	void 		* src_p		data source pointer
 *					uint16_t 	dst_addr	start address
 *					size_t 		siz			number of 16bit words to read
 * Returns       :	void
 * Notes         :
 *
 * Version		Date (d/m/y)	    Programmer          Reason for Change
 * 1.0.0        dd/mm/20yy          Paul Young          Original Created
 ********************************************************************************************************/
void csp_STM32_flash_eeprom_wr(void * src_p, uint16_t dst_addr, size_t siz)
{
	uint16_t		active_page;
	uint32_t		start_address;
	uint32_t		flash_address;
	FLASH_Status 	f_status;
	uint16_t		i;
	uint16_t		data;
	uint16_t 		*data_p;

	data_p			=  (uint16_t*)src_p;
	active_page		=  csp_STM32_FindValidPage(WRITE_IN_VALID_PAGE);

	start_address	=  (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(active_page * PAGE_SIZE));

	flash_address	=  start_address + dst_addr + 2;			// 2 ensures we do not write on status 16bit word

    for(i=0;i<(PAGE_SIZE-2); i=i+2){							// i=i+2 because read 2 bytes at a time
		if(i<siz){
			f_status = FLASH_ProgramHalfWord(flash_address,*data_p);
		}
		else if(active_page == PAGE0){
			data		=  (*(uint16_t*)(flash_address + PAGE_SIZE));
			f_status 	= FLASH_ProgramHalfWord(flash_address,data);
		}
		else{
			data		=  (*(uint16_t*)(flash_address - PAGE_SIZE));
			f_status = FLASH_ProgramHalfWord(flash_address,data);
		}
		if (f_status != FLASH_COMPLETE){
			printf("\r\nEEPROM wr Error!!");
			while(1);
		}

		data_p++;
		flash_address	+= 2;
		if(flash_address >= (start_address + PAGE_SIZE) ){
			flash_address	=  start_address + 2;
		}
    }


	//erase old page
	if(active_page == PAGE0){
    	f_status = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
	}
	else{
    	f_status = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
	}
	if (f_status != FLASH_COMPLETE){
		printf("\r\nEEPROM wr erase Error!!");
		while(1);
	}

	//set valid page marker
	if(active_page == PAGE0){
    	f_status = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
	}
	else{
    	f_status = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
	}
	if (f_status != FLASH_COMPLETE){
		printf("\r\nEEPROM wr Vmarker Error!!");
		while(1);
	}

	return;
}

/**********************************************************************************************************
 * Function Name : csp_STM32_FindValidPage
 * Description   : Find valid Page for write or read operation
 * Arguments     : uint8_t Operation	This parameter can be one of the following values:
 *						READ_FROM_VALID_PAGE:	read operation from valid page
 *						WRITE_IN_VALID_PAGE:	write operation from valid page
 * Returns       : uint16_t	active_page;
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		25/08/11	William Paul	Original Created
 **********************************************************************************************************/
uint16_t csp_STM32_FindValidPage(uint8_t Operation)
{
	uint16_t		PageStatus0 = 6;
	uint16_t		PageStatus1 = 6;
	uint16_t		active_page;
	FLASH_Status 	f_status;

	PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);			/* Get Page0 actual status */
	PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);			/* Get Page1 actual status */

	/* Write or read operation */
	switch (Operation){
		case WRITE_IN_VALID_PAGE:														/* ---- Write operation ---- */
			if (	PageStatus1 == VALID_PAGE){		active_page	=  PAGE0;	}			/* Page1 valid therefore write to page 0*/
			else if(PageStatus0 == VALID_PAGE){		active_page	=  PAGE1;	}			/* Page0 valid therefore write to page 1*/
			else{									active_page	=  NO_VALID_PAGE;	}	/* No valid Page */
			break;
		case READ_FROM_VALID_PAGE:														/* ---- Read operation ---- */
			if(		PageStatus0 == VALID_PAGE){		active_page	=  PAGE0;	}			/* Page0 valid */
			else if(PageStatus1 == VALID_PAGE){		active_page	=  PAGE1;	}           /* Page1 valid */
			else{									active_page	=  NO_VALID_PAGE;	}	/* No valid Page */
			break;
		default:									active_page	=  PAGE0;				/* Page0 valid */
	}

	if(active_page == NO_VALID_PAGE){
		f_status	=  csp_STM32_flash_eeprom_format();									/* format EEprom flash pages */
		if (f_status != FLASH_COMPLETE){
			printf("\r\nEEPROM Format Error!!");
			while(1);
		}
		active_page	=  PAGE0;
	}



	return(active_page);
}


/**********************************************************************************************************
 * Function Name : csp_STM32_flash_eeprom_format
 * Description   : Erases PAGE0 and PAGE1 and writes VALID_PAGE header to PAGE0
 * Arguments     : None
 * Returns       : FLASH_Status		f_status
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		25/08/11	William Paul	Original Created
 **********************************************************************************************************/
FLASH_Status csp_STM32_flash_eeprom_format(void)
{
	FLASH_Status 	f_status = FLASH_COMPLETE;

	/* Erase Page0 */
	f_status = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
	if (f_status != FLASH_COMPLETE){
		return(f_status);
	}

	/* Set Page0 as VALID_PAGE*/
	f_status = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
	if (f_status != FLASH_COMPLETE){
		return(f_status);
	}


	/* Erase Page1 */
	f_status = FLASH_ErasePage(PAGE1_BASE_ADDRESS);


	return(f_status);
}


/**********************************************************************************************************
 * Function Name : 	csp_STM32_flash_eeprom_menu
 * Description   :
 * Arguments     :	void
 *					void
 * Returns       : 	void
 * Notes         :
 *
 * Version	Date d/m/Y		Programmer		Reason for Change
 * 1.0.0	25/08/2011		W Paul			Original Created
 **********************************************************************************************************/
void csp_STM32_flash_eeprom_menu(void)
{
/* Local Variables */
	uint8_t			rec_status_u8;
	uint8_t			rec_char_u8;
	uint8_t			loop_here	=  1;
	uint16_t		data[32];
	uint16_t		i;
	uint16_t		PageStatus0 = 6;
	uint16_t		PageStatus1 = 6;

/* Code */
	while(loop_here == 1){
		rec_char_u8 = debug_getchar(0,&rec_status_u8);
		if(rec_status_u8){

			switch(rec_char_u8){
				case ' ':
					printf("\r\n");
					printf("\r\n****EEPROM****");
					printf("\r\nEsc  Exit Menu");
					printf("\r\nr    test rd");
					printf("\r\nw    test wr");
					printf("\r\ns    read eeprom status");

					break;

				case ESC_CHAR:	loop_here = 0;						break;
				case 'r':
					csp_STM32_flash_eeprom_rd(data,20,32);
					for(i=0;i<32;i++){
						if(i%8 == 0){
							printf("\r\n%4x -",i);
						}
						printf("%4x ",data[i]);
					}
					break;
				case 'w':
					for(i=0;i<32;i++){
						if(i%8 == 0){
							printf("\r\n%4x -",i);
						}
						data[i]	=  i + 0x1200;
						printf("%4x ",data[i]);
					}
					csp_STM32_flash_eeprom_wr(data,20,32);
					break;
				case 's':
					PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);			/* Get Page0 actual status */
					PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);			/* Get Page1 actual status */
					printf("\r\nPage 0-%4x",PageStatus0);
					printf("\r\nPage 1-%4x",PageStatus1);
					break;
				default:
						printf("\r\n Invalid Command");
						break;
			}
		}
	}
	return;
}

/*********************************************************************************************************
*********************************************************************************************************/
//endof file
