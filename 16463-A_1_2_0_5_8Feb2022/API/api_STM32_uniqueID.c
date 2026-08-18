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
 * Filename    :  api_STM32_uniqueID.c
 * Date Created:  Tue 04 Apr 2017 09:04:32 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
 #include "stdio.h"
#include "stdint.h"
#include "string.h"

#include "api_STM32_uniqueID.h"

/*************************************************************************************************
* Function Name : 	api_STM32_uniqueID_read
* Description   : 	This Function reads the unSTM32 uniqueID
* Arguments     : 	uint8_t *str
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		6/10/16		W. Paul			Created
*************************************************************************************************/
void api_STM32_uniqueID_read(uint8_t *str)
{
	uint8_t		*uniqueID_ptr	=  (uint8_t*)(0x1FFFF7E8);
	uint8_t		i;

	for(i=0;i<12;i++){
		str[i]	=  *uniqueID_ptr;
		uniqueID_ptr++;
	}
	str[13]	= 0x00;

	return;
}

/*************************************************************************************************
* Function Name : 	api_STM32_uniqueID_print
* Description   : 	This Function reads the unSTM32 uniqueID
* Arguments     : 	uint8_t *str
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		6/10/16		W. Paul			Created
*************************************************************************************************/
void api_STM32_uniqueID_print(void)
{
	uint8_t				uniqueID[13];
	uint8_t				i;

	api_STM32_uniqueID_read(uniqueID);

	printf("\r\nSTM UniqueID  - ");
	for(i=0;i<12;i++){
		printf("%02x",	uniqueID[i]);
	}

	return;
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
