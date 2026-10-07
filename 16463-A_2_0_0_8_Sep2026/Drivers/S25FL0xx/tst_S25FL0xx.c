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
 *  Copyright 2016, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  tst_S25FL0xx.c
 * Date Created:  Thu 17 Mar 2016 08:14:48 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/




/**********************************************************************************************************
 *                                           INCLUDE FILES
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "app_system.h"

#include "tst_S25FL0xx.h"
#include "csp_S25FL0xx.h"
#include "S25FL0xx.h"

#include "hal_STM32_uart.h"
#include "csp_STM32_uart.h"
#include "csp_STM32_iwdg.h"

#include "csp_STM32_spi.h"
#if(PCB_SEL	== PCB_17475_B_01)
#include "csp_STM32_spi1.h"
#include "pcb_spi1.h"
#elif(PCB_SEL	== PCB_17475_B_02)
#include "csp_STM32_spi2.h"
#include "pcb_spi2.h"
#endif	// PCB_SEL

/**********************************************************************************************************
 *                                           COMPILER DEFINES
 **********************************************************************************************************/

/**********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************/

void tst_S25FL0xx_fill_mem_page(void);
void tst_S25FL0xx_del_mem_page(void);
void tst_S25FL0xx_del_mem_chip(void);
void tst_S25FL0xx_read_mem_page(void);


//void tst_S25FL0xx_raw_output(void);
//void tst_S25FL0xx_raw_input(void);


/**********************************************************************************************************
 **********************************************************************************************************/

/*************************************************************************************************
 * Function Name :	tst_S25FL0xx_read_mem_page
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.0.1	16/02/11	W. Paul		Original Created
 * 0.1.0	14/03/12	W. Paul		Update
 *
 *************************************************************************************************/
void tst_S25FL0xx_read_mem_page(void)
{
	uint8_t		loop_u8		=  1;
	uint8_t		state_u8		=  0;

	uint32_t		start_add;
	uint16_t		no_bytes;
	uint8_t		data[0x10];
	uint16_t		i;
	uint8_t		j;



	while(loop_u8){
		switch(state_u8)
		{
			case 0:
				printf("\n\rEnter Start Address : 0x--------\b\b\b\b\b\b\b\b");
				start_add			=  BSP_get_num(BASE16,8);
				state_u8	+= 1;
				break;
			case 1:
				printf("\n\rEnter Number of Bytes to read : 0x----\b\b\b\b");
				no_bytes	=  BSP_get_num(BASE16,4);
				state_u8	+= 1;
				break;
			case 2:
				for(i=0;i<no_bytes;){
					csp_mem_rd(data, start_add + i, sizeof(data));
					printf("\n\r%8x   ",start_add + i);
					for(j=0;j<0x10;j++){
						printf("%2x ",data[j]);
						if(++i >= no_bytes){
							j	= 0x10;
						}
					}
				}
				printf("\n\rDone");
				loop_u8	=  0;
		}
		app_sys_watchdog_reload();
	}
	return;
}

/*************************************************************************************************
 * Function Name :	tst_S25FL0xx_fill_mem_page
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	14/03/12	W. Paul		Created
 *
 *************************************************************************************************/
void tst_S25FL0xx_fill_mem_page(void)
{
	uint8_t		loop_u8		=  1;
	uint8_t		state_u8		=  0;

	uint32_t		start_add;
	uint8_t		fill_val;
	uint16_t		no_bytes;
	uint32_t		number;

	uint32_t		i;
	uint8_t		data_a_u8[0x10];

	while(loop_u8){
		switch(state_u8)
		{
			case 0:
				printf("\n\rEnter Start Address : 0x--------\b\b\b\b\b\b\b\b");
				start_add			=  BSP_get_num(BASE16,8);
				state_u8	+= 1;
				break;
			case 1:
				printf("\n\rEnter Number of Bytes to fill : 0x----\b\b\b\b");
				no_bytes	=  BSP_get_num(BASE16,4);
				state_u8	+= 1;
				break;
			case 2:
				printf("\n\rEnter fill value : 0x--\b\b");
				fill_val	=  BSP_get_num(BASE16,2);
				memset(data_a_u8,fill_val,sizeof(data_a_u8));
				state_u8	+= 1;
				printf("\n\r");
				break;

			case 3:
				for(i=no_bytes;i>0; ){
					if(i > sizeof(data_a_u8)){	number	=  sizeof(data_a_u8);	}
					else{						number	=  i;			}

				//	csp_mem_wr( 	data_a_u8, start_add, number);
					csp_sys_mem_wr(	data_a_u8, start_add, number);
					i				-= number;
					start_add		+= number;
					printf("\r%d        ",i);

				}

				printf("\n\rDone");
				loop_u8	=  0;
				break;
		}
		app_sys_watchdog_reload();
	}
	return;
}


/*************************************************************************************************
 * Function Name :	tst_S25FL0xx_del_mem_page
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	14/03/12	W. Paul		Created
 *
 *************************************************************************************************/
void tst_S25FL0xx_del_mem_page(void)
{
	uint8_t		loop_u8		=  1;
	uint8_t		state_u8		=  0;

	uint32_t		add;
	uint8_t		type;

	while(loop_u8){
		switch(state_u8)
		{
			case 0:
				printf("\n\rEnter a del Address : 0x--------\b\b\b\b\b\b\b\b");
				add			=  BSP_get_num(BASE16,8);
				state_u8	+= 1;
				break;
			case 1:
				printf("\n\r1- 4kB erase  2-64kB erase: -\b");
				type	=  BSP_get_num(BASE10,1);
				if((type == 1)||(type == 2)){
					state_u8	+= 1;
				}
				break;
			case 2:
				if(type == 1)	csp_erase_4kb_sectors(add);
				else			csp_erase_64kb_sectors(add,1);

				printf("\r\nDone");
				loop_u8		=  0;
				break;
		}
		app_sys_watchdog_reload();
	}
	return;
}


/*************************************************************************************************
 * Function Name :	tst_S25FL0xx_del_mem_chip
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	14/03/12	W. Paul		Created
 *
 *************************************************************************************************/
void tst_S25FL0xx_del_mem_chip(void)
{
	uint8_t		loop_u8		=  1;
	uint8_t		state_u8		=  0;
	uint8_t		type;
	uint8_t		chip		=  0;
	uint8_t		finished;


	while(loop_u8){
		switch(state_u8)
		{
			case 0:
				printf("\n\rWhich Chip: -\b");
				chip	=  BSP_get_num(BASE10,1);
				state_u8	+= 1;
			case 1:
				printf("\n\rEnter 5 to continue: -\b");
				type	=  BSP_get_num(BASE10,1);
				if(type == 5){	state_u8	+= 1;	}
				else{
					printf("\r\nErase Abort");
					loop_u8		=  0;
				}
				break;
			case 2:
				printf("\r\n");
				S25FL0xx_BulkErase(chip);
				state_u8	+= 1;
				break;
			case 3:
				finished	=  S25FL0xx_WaitWriteComplete(chip,1000);
				printf(".");
				if(finished == 0){
					printf(" Done");
					loop_u8		=  0;
				}
				break;
		}
		app_sys_watchdog_reload();
	}
	return;
}

/*************************************************************************************************
* Function Name : 	tst_S25FL0xx_rwe_mem_chip
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/11/16	W. Paul			Created
*************************************************************************************************/
void tst_S25FL0xx_rwe_mem_chip(void)
{
	uint8_t		loop_u8		=  1;
	uint8_t		state_u8		=  0;

	uint32_t		add;
	uint8_t		res;

	while(loop_u8){
		switch(state_u8)
		{
			case 0:
				printf("\n\rEnter a test Address : 0x--------\b\b\b\b\b\b\b\b");
				add			=  BSP_get_num(BASE16,8);
				state_u8	+= 1;
				break;
			case 1:
				res = tst_S25FL0xx_rwe(add,1);

				printf("\r\nDone %d",res);
				loop_u8		=  0;
				break;
		}
		app_sys_watchdog_reload();
	}
	return;

}

/*************************************************************************************************
* Function Name : 	tst_S25FL0xx_rwe
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/11/16	W. Paul			Created
*************************************************************************************************/
uint8_t tst_S25FL0xx_rwe(uint32_t add,uint8_t debug_output)
{
    uint8_t		i;
	uint8_t		res		=  0;
	uint8_t		res1	=  0;
	uint8_t		rd_array[33];
	uint8_t		wr_array[33];



	for(i=0;i<sizeof(wr_array);i++){
		wr_array[i]	=  (uint8_t)((i*34) + 89);
	}
	csp_mem_wr(wr_array, add, sizeof(wr_array));
	if(debug_output){	printf("\r\nWr known data");	}

	csp_mem_rd(rd_array, add, sizeof(rd_array));
	if(debug_output){	printf("\r\nRd back data");		}

	res	= memcmp((char const *)wr_array,(char const *)rd_array, sizeof(rd_array));
	if(res==0){	res	=  1;	}
	else{		res	=  0;	}
	if(debug_output){	printf("\r\nCmp data .. %d",res);   }

	if(debug_output){	printf("\r\nErase data");	}
	csp_erase_4kb_sectors(add);

	csp_mem_rd(rd_array, add, sizeof(rd_array));
	if(debug_output){	printf("\r\nRd back data");	}

	memset(wr_array,0xff,sizeof(wr_array));
	res1	= memcmp((char const *)wr_array,(char const *)rd_array, sizeof(rd_array));
	if(res1==0){	res1	=  1;	}
	else{			res1	=  0;	}
	if(debug_output){	printf("\r\nCheck erase .. %d",res1);   }

	if((res == 1)&&(res1 == 1)){	return(1);	}
	else{							return(0);	}
}

/*************************************************************************************************
 * Function Name :	tst_S25FL0xx_raw_output
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	12/08/13	W. Paul		Created
 *
 *************************************************************************************************/
/*
void tst_S25FL0xx_raw_output(void)
{
	uint8_t 		device_u8;
	uint16_t 		start_page;
	uint16_t 		end_page;
	uint16_t 		page;
	uint16_t 		add;
	uint8_t 		read_status_u8;
	uint8_t 		i;
	uint8_t 		data[0x10];

	printf("\n\rEnter Device No:-\b");
	device_u8	=  BSP_get_num(BASE10,1);
	printf("\r\nEnter Start Page (dec):----\b\b\b\b");
	start_page		=  BSP_get_num(BASE10,4);
	printf("\r\nEnter End Page   (dec):----\b\b\b\b");
	end_page		=  BSP_get_num(BASE10,4);


	for(page=start_page;page<=end_page;page++){
		for(add=0;add<0x420;add=add+0x10){
			hal_AT45_rd(device_u8,page,add,0x10,data,&read_status_u8);
			for(i=0;i<0x10;i++){
				printf("%c",data[i]);
			}
		}
	}
	printf("\r\nFinished");

	return;
}
*/

/*************************************************************************************************
 * Function Name :	tst_S25FL0xx_raw_input
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	12/08/13	W. Paul		Created
 *
 *************************************************************************************************/
/*
void tst_S25FL0xx_raw_input(void)
{
	uint8_t 		device_u8;
	uint16_t 		start_page;
	uint16_t 		end_page;
	uint16_t 		page;
	uint16_t 		add;
	uint8_t 		write_status_u8;
	uint8_t 		i;
	uint8_t 		data[0x10];
	uint8_t 		rec_status_u8;

	printf("\n\rEnter Device No:-\b");
	device_u8	=  BSP_get_num(BASE10,1);
	printf("\r\nEnter Start Page (dec):----\b\b\b\b");
	start_page		=  BSP_get_num(BASE10,4);
	printf("\r\nEnter End Page   (dec):----\b\b\b\b");
	end_page		=  BSP_get_num(BASE10,4);


	for(page=start_page;page<=end_page;page++){
		printf("\r\nP%d %3x",page,add);
		for(add=0;add<0x420;add=add+0x10){
			for(i=0;i<0x10;i++){
				do{
					data[i] = debug_getchar(10,&rec_status_u8);
				}while(rec_status_u8 == 0);
			}
			hal_AT45_wr(device_u8, page,add ,0x10, data, &write_status_u8);
			printf("\b\b\b%3x",add);
		}
	}
	printf("\r\nFinished");

	return;
}
*/



/*************************************************************************************************
 * Function Name :	hal_AT45DBxxx_test_menu
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	14/03/12	W. Paul		Created
 *
 *************************************************************************************************/
uint8_t hal_S25FL0xx_test_menu(void)
{
	uint8_t			rec_status_u8;
	uint8_t			rec_char_u8;

	rec_char_u8 = debug_getchar(10,&rec_status_u8);
	if(rec_status_u8){
		switch(rec_char_u8){
			case ' ':
				printf("\r\n -------------------------------");
				printf("\r\n TST S25FL0xx menu");
				printf("\r\nEsc Exit Menu");
				printf("\r\nC   Config mem");
				printf("\r\nr   read a serial mem page");
				printf("\r\nf   fill a serial mem page");
				printf("\r\nd   erase a serial mem page");
				printf("\r\n+   erase serial mem chip");
				printf("\r\nt   test mem r,w,e");
			//	printf("\r\nO   raw output");
			//	printf("\r\nI   raw input");
				break;
			case 0x1b:	//ESC
				return(0);	//leave this menu


			case 'r':	tst_S25FL0xx_read_mem_page();		break;
			case 'f':	tst_S25FL0xx_fill_mem_page();		break;
			case 'd':	tst_S25FL0xx_del_mem_page();		break;
			case '+':	tst_S25FL0xx_del_mem_chip();		break;
			case 't':	tst_S25FL0xx_rwe_mem_chip();		break;
		//	case 'O':	tst_S25FL0xx_raw_output();		break;
		//	case 'I':	tst_S25FL0xx_raw_input();		break;
			case 'C':	S25FL0xx_Config();					break;
			case '!':
#if(PCB_SEL	== PCB_17475_B_01)
					spi1_cs_manager(0xff);
					spi1_config(SPI_MODE_0,10000000);
#elif(PCB_SEL	== PCB_17475_B_02)
					spi2_cs_manager(0xff);
					spi2_config(SPI_MODE_0,10000000);
#endif	// PCB_SEL
					break;
		}
	}
	return(1);
}



/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
