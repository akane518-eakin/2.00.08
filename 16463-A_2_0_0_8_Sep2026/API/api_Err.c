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
 * Filename    :  api_Err.c
 * Date Created:  Tue 04 Apr 2017 10:25:43 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include "stdio.h"
#include "stdint.h"

#include "stm32f10x.h"
#include "core_cm3.h"


/*
*********************************************************************************************************
* Function Name : MarturionFatalErrorHandler
* Description   : This function is used for
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y			Programmer			Reason for Change
* 1.0.0			23/11/2015			Tim Barr			Original Created
*********************************************************************************************************
*/
void MarturionFatalErrorHandler(char* sourceFile, unsigned long lineNumber, char* description)
{
	__disable_irq();
	while(1);
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
