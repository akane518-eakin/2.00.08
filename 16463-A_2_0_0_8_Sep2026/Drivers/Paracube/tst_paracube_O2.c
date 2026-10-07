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
 * Filename    :  tst_paracube_O2.c
 * Date Created:  Wed 18 Oct 2017 01:11:00 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "csp_paracube_O2.h"
#include "tst_paracube_O2.h"

#include "csp_STM32_uart.h"
#include "hal_STM32_uart.h"

/**********************************************************************************************************
**********************************************************************************************************/



/*************************************************************************************************
* Function Name : 	tst_paracube_O2_menu
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/10/17	W. Paul			Created
*************************************************************************************************/
uint8_t tst_paracube_O2_menu(void)
{
/* Local Variables */
	uint8_t				rec_status_u8;
	uint8_t				rec_char_u8;
	uint8_t				val;
	float				O2_conc;

/* Code */
	rec_char_u8 = debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){

		switch(rec_char_u8){
			case ' ':
				printf("\r\n");
				printf("\r\n****TST Paracube O2 menu****");
				printf("\r\nEsc  Exit Menu");
				printf("\r\nd    debug tog");
				printf("\r\n0    Disable CRC");
				printf("\r\n1    ID");
				printf("\r\n2    Ver");
				printf("\r\n3    rd press comp");
				printf("\r\n4    Identity");
				printf("\r\n9    Restore Factory Calibration");
				printf("\r\nc    press comp off");
				printf("\r\nC    press comp on");
				printf("\r\nl    low cal point");
				printf("\r\nh    high cal point");
				printf("\r\np    1 point cal point");

				break;

			case 0x1b:
				return(0);
				//break;
			case 'd':
				val	= csp_paracube_debug(2);	//toggle
				printf("\r\nParacube debug %d",val);
				break;
			case '0':	csp_paracube_command(PARACUBE_CRC, 0);					break;
			case '1':	csp_paracube_command(PARACUBE_ID, 0);					break;
			case '2':	csp_paracube_command(PARACUBE_VER, 0);					break;
			case '3':	csp_paracube_command(PARACUBE_PRES_COMP_RD, 0);			break;
			case '4':	csp_paracube_command(PARACUBE_IDENTITY, 0);				break;
			case '9':	csp_paracube_command(PARACUBE_RESTORE_CAL, 0);			break;
			case 'c':
				printf("\r\nPressure comp off");
				csp_paracube_command(PARACUBE_PRES_COMP_SET, 0);
				break;
			case 'C':
				printf("\r\nPressure comp on");
				csp_paracube_command(PARACUBE_PRES_COMP_SET, 1);
				break;
			case 'l':
				printf("\r\nEnter the low calibration concentration  (3.1): ");
				O2_conc		=  BSP_get_float(3,1);
				csp_paracube_command(PARACUBE_2_POINT_CAL_LOW, O2_conc);
				break;
			case 'h':
				printf("\r\nEnter the High calibration concentration (3.1): ");
				O2_conc		=  BSP_get_float(3,1);
				csp_paracube_command(PARACUBE_2_POINT_CAL_HIGH, O2_conc);
				break;
			case 'p':
				printf("\r\nEnter the Single point calibration concentration (3.1): ");
				O2_conc		=  BSP_get_float(3,1);
				csp_paracube_command(PARACUBE_1_POINT_CAL, O2_conc);
				break;



			default:
					printf("\r\n Invalid Command");
					break;
		}
	}
	return(1);
}

/**********************************************************************************************************
**********************************************************************************************************/
//end of file
