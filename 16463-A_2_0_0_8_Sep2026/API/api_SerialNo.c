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
 * Filename    :  api_SerialNo.c
 * Date Created:  Tue 19 Jul 2016 08:25:50 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include "stdio.h"
#include "stdint.h"
#include "string.h"


#include "api_SerialNo.h"
#include "hal_STM32_uart.h"

#include "csp_S25FL0xx.h"
#include "pcb_mem_map_flash.h"



/*************************************************************************************************
* Function Name : 	api_SerialNo_read
* Description   : 	This Function reads the unit serial number
* Arguments     : 	uint8_t *str
* Returns       : 	uint8_t 		1	= has been set
*									0	= has never been initialised
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		19/07/16		W. Paul			Created
*************************************************************************************************/
uint8_t api_SerialNo_read(uint8_t *str)
{
	SerialNo_t		my_sn;
	uint8_t			i;

	csp_mem_rd((uint8_t*)&my_sn, MEM_ADD_UNIT_SERIAL_NO, sizeof(SerialNo_t) );

	for(i=0;i<SN_LEN;i++){
		str[i]	=  my_sn.Serial_No[i];
	}

	if(my_sn.tamper == SN_POPULATED)	return(1);
	else								return(0);

}


/*************************************************************************************************
* Function Name : 	api_SerialNo_write
* Description   : 	This Function saves the serial number
* Arguments     : 	uint8_t *str
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		19/07/16		W. Paul			Created
*************************************************************************************************/
void api_SerialNo_write(uint8_t *str,uint8_t set_valid)
{
	SerialNo_t		my_sn;
	uint8_t			i;

	for(i=0;i<SN_LEN;i++){
		my_sn.Serial_No[i]	=  str[i];
	}

	if(set_valid){	my_sn.tamper	=  SN_POPULATED;	}
	else{			my_sn.tamper	=  0;				}

	csp_sys_mem_wr((uint8_t*)&my_sn, MEM_ADD_UNIT_SERIAL_NO, sizeof(SerialNo_t) );

	return;
}

/*************************************************************************************************
* Function Name : 	api_SerialNo_set
* Description   : 	This Function sets and saves the serial number
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		19/07/16		W. Paul			Created
*************************************************************************************************/
void api_SerialNo_set(void)
{
	uint8_t		serial_no[16];
	uint8_t		save_quit[2];

	memset(serial_no, 0x00, sizeof(serial_no));

	printf("\r\nEnter Unit Serial No:--------\b\b\b\b\b\b\b\b");
	BSP_get_str(serial_no, 8);

	printf("\r\nPress S to save or ESC -");
	BSP_get_str(&save_quit[0],1);
	if(save_quit[0] == 'S'){
		printf("\r\nSaving ..");
		api_SerialNo_write(serial_no, 1);
		printf("Done");
	}
	else{
		printf("\r\nExit (not Saved)");
	}


	return;
}

/*************************************************************************************************
* Function Name : 	api_SerialNo_print
* Description   : 	This Function printd the unit serial number
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		21/07/16	W. Paul			Created
*************************************************************************************************/
void api_SerialNo_print(void)
{
	uint8_t		my_sn_str[16];
	uint8_t		status;

	status	=  api_SerialNo_read(my_sn_str);

	printf("\r\nUnit SerialNo  ");
	if(status){	printf("%s",my_sn_str);	}
	else{		printf("*Not Avail*");	}

	return;
}


/*************************************************************************************************
* Function Name : 	api_ModelNo_read
* Description   : 	This Function reads the unit model number
* Arguments     : 	uint8_t *str
* Returns       : 	uint8_t 		1	= has been set
*									0	= has never been initialised
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/16		W. Paul			Created
*************************************************************************************************/
uint8_t api_ModelNo_read(uint8_t *str)
{
	ModelNo_t		my_mn;
	uint8_t			i;

	csp_mem_rd((uint8_t*)&my_mn, MEM_ADD_UNIT_MODEL_NO, sizeof(ModelNo_t) );

	for(i=0;i<MN_LEN;i++){
		str[i]	=  my_mn.Model_No[i];
	}

	if(my_mn.tamper == MN_POPULATED)	return(1);
	else								return(0);

}


/*************************************************************************************************
* Function Name : 	api_ModelNo_write
* Description   : 	This Function saves the serial number
* Arguments     : 	uint8_t *str
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/16		W. Paul			Created
*************************************************************************************************/
void api_ModelNo_write(uint8_t *str,uint8_t set_valid)
{
	ModelNo_t		my_mn;
	uint8_t			i;

	for(i=0;i<MN_LEN;i++){
		my_mn.Model_No[i]	=  str[i];
	}

	if(set_valid){	my_mn.tamper	=  MN_POPULATED;	}
	else{			my_mn.tamper	=  0;				}

	csp_sys_mem_wr((uint8_t*)&my_mn, MEM_ADD_UNIT_MODEL_NO, sizeof(ModelNo_t) );

	return;
}


/*************************************************************************************************
* Function Name : 	api_ModelNo_print
* Description   : 	This Function printd the unit serial number
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/16	W. Paul			Created
*************************************************************************************************/
void api_ModelNo_print(void)
{
	uint8_t		my_mn_str[16];
	uint8_t		status;

	status	=  api_ModelNo_read(my_mn_str);

	printf("\r\nUnit ModelNo  ");
	if(status){	printf("%s",my_mn_str);	}
	else{		printf("*Not Avail*");	}

	return;
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
