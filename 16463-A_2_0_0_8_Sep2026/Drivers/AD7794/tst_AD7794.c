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
 *  Copyright 2015, Marturion Ltd
 *  All Rights Reserved
 *
 *
 * Filename    :  tst_S6B1713.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include	"app_system.h"

#include "api_watchdog.h"
#include "app_pneumatic_ctrl.h"

#include "csp_AD7794.h"
#include "pcb_spi1.h"
#include "pcb_spi2.h"

#include "csp_STM32_uart.h"
#include "hal_STM32_uart.h"

/**********************************************************************************************************
**********************************************************************************************************/



/**********************************************************************************************************
* Function Name : 	tst_ADC_AD7794_menu
* Description   :
* Arguments     :	void
*					void
* Returns       : 	void
* Notes         :
*
* Version	Date d/m/Y		Programmer		Reason for Change
* 1.0.0		1/08/2011		W Paul			Original Created
**********************************************************************************************************/
uint8_t tst_ADC_AD7794_menu(void)
{
/* Local Variables */
	uint8_t				rec_status_u8;
	uint8_t				rec_char_u8;

	uint8_t				chip		=  AD7794_CHIP_EN;

	uint8_t				i;
	float				voltage;
	uint32_t			result;
	uint8_t				A2D_ready;
	uint16_t			delay_cnt;
	uint8_t 			sam_no;
	uint32_t 			run_total;

	uint32_t 			run_total_h[20];
	uint16_t 			delay_cnt_h[20];



/* Code */
	rec_char_u8 = debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){

		switch(rec_char_u8){
			case ' ':
				printf("\r\n");
				printf("\r\n****TST AD7794 menu****");
				printf("\r\nEsc  Exit Menu");
				printf("\r\n1    Single read wait");
				printf("\r\n2    Single read loop");
				printf("\r\n3    Continuous read wait");
				printf("\r\n4    Continuous read loop");
				printf("\r\nr    Print register values");
				printf("\r\nR    Chip reset");
				printf("\r\nD    Disable IRQ rd seq");
				printf("\r\nE    Enable IRQ rd seq");

				break;

			case 0x1b:
				return(0);
				//break;
			case 'D':
				app_pneumatics_IRQ_en_dis(0);	//disabled
				printf("\r\nAD7794 IRQ rd seq disabled");
				break;
			case 'E':
				app_pneumatics_IRQ_en_dis(1);	//enabled
				printf("\r\nAD7794 IRQ rd seq enabled");
				break;

			case '1':
				printf("\r\nSingle Read wait");
				for(i=0;i<6;i++){
					result = ad7794_convert_single_wait(chip,i);
					voltage = ((float)(result)/(float)(0xffffff))*AD7794_VREF_VALUE;
					printf("\r\nCH%d = %8d(0x%06x) %.6f",i,result,result,voltage);
					app_sys_watchdog_reload();
				}
				break;
			case '2':
				printf("\r\nSingle Read loop");
				for(i=0;i<6;i++){
					ad7794_convert_single_start(chip,i);
					do{
						A2D_ready	=  ad7794_convert_status();
					}while(A2D_ready   == 1);
					result	=  ad7794_convert_value(chip);
					voltage = ((float)(result)/(float)(0xffffff))*AD7794_VREF_VALUE;
					printf("\r\nCH%d = %8d(0x%06x) %.6f",i,result,result,voltage);
					app_sys_watchdog_reload();
				}
				break;
			case '3':
				printf("\r\nContinuous Read wait");
				for(i=0;i<6;i++){
					result	=  ad7794_convert_continuous_wait(chip,i,10);
					voltage = ((float)(result)/(float)(0xffffff))*AD7794_VREF_VALUE;
					printf("\r\nCH%d = %8d(0x%06x) %.6f",i,result,result,voltage);
					app_sys_watchdog_reload();
				}
				break;
			case '4':
				printf("\r\nContinuous Read loop");
				for(i=0;i<6;i++){
					ad7794_convert_continuous_start(chip,i);
					sam_no		=  0;
					run_total	=  0;
					do{
						do{
							A2D_ready	=  ad7794_convert_status();
						}while(A2D_ready   == 1);
						result	=  ad7794_convert_continuous_value(chip,10,&sam_no,&run_total);
						run_total_h[sam_no-1]	=  run_total;
						delay_cnt_h[sam_no-1]	=  delay_cnt;
					}while(sam_no < 10);
					ad7794_convert_continuous_stop(chip);
					voltage = ((float)(result)/(float)(0xffffff))*AD7794_VREF_VALUE;

					for(sam_no=0;sam_no<10;sam_no++){
						printf("\r\n%8d %5d ",run_total_h[sam_no],delay_cnt_h[sam_no]);
						if(sam_no > 0){
							printf("%8d",run_total_h[sam_no]-run_total_h[sam_no-1]);
						}
						else{
							printf("%8d",run_total_h[sam_no]);
						}
					}
					printf("\r\nCH%d = %8d(0x%06x) %.6f",i,result,result,voltage);
					printf("\r\n");
					app_sys_watchdog_reload();
				}
				break;

			case 'r':
				ad7794_reg_print(chip);
				break;
			case 'R':
				ad7794_Reset(chip);
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
