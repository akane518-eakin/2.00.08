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
 * Filename    :  app_debug.c
 * Date Created:  Tue 04 Apr 2017 08:58:53 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "app_pneumatic_ctrl.h"
#include "app_patient_pressure.h"
#include "app_UI.h"
#include "app_selfcheck.h"
#include "app_system.h"

#include "api_STM32_uniqueID.h"
#include "api_IO.h"
#include "api_RTC.h"
#include "api_leds.h"
#include "api_audio.h"
#include "api_stopwatch.h"
#include "api_SerialNo.h"
#include "api_STM32_touchscreen.h"
#include "tst_S25FL0xx.h"
#include "tst_AD7794.h"
#include "tst_LCD.h"
#include "tst_LTC2943.h"
#include "tst_paracube_O2.h"
#include "Language.h"

#include "csp_STM32_uart.h"
#include "hal_STM32_uart.h"
#include "fonts.h"

#include "csp_S25FL0xx.h"

#include "pcb_mem_map_flash.h"

void menu_access(uint8_t *rec_char_u8,uint8_t BY1,uint8_t BY2,uint8_t BY3,uint8_t *res);

/*************************************************************************************************
* Function Name : 	app_debug_handler
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	07/01/16		W. Paul			Created
*************************************************************************************************/
void app_debug_handler(uint8_t *debug_mode)
{
	static uint8_t		init			=  0;
	uint8_t				rec_status_u8;
	uint8_t				rec_char_u8;
	static uint8_t		menu_pass		=  0;
	static uint8_t		which_menu		=  0;
	uint8_t				ret;
	static uint8_t		esc_cnt			=  0;
	uint8_t				fault			=  0;
//	static uint32_t		sys_tic_print	=  0;

	if(*debug_mode != 1){
		uart_debug_fun_sel(DUMP_PRINTF_PORT);	//stop outputting all debug printf
		init	=  0;
	}
	else{	//0= packet mode, 1= hyperterminal menu
		if(init == 0){
			init	=  1;
			uart_debug_fun_sel(UART_PRINTF_PORT);
		}

		switch(which_menu){
			case 0:														break;
			case 1:		ret	=  hal_S25FL0xx_test_menu();				break;
			case 2:		ret	=  api_audio_menu();						break;
			case 3:		ret	=  api_Stopwatch_menu();					break;
			case 5:		ret	=  tst_ADC_AD7794_menu();					break;
			case 6:		ret	=  tst_LCD_menu();							break;
			case 7:		ret	=  api_LED_test();							break;
			case 8:		ret	=  tst_LTC2943_menu();						break;
			case 9:		ret	=  api_STM32_internal_RTC_test();			break;
			case 10:	ret	=  tst_paracube_O2_menu();					break;
			case 11:	ret	=  app_selfcheck_test_menu();				break;
			case 12:	ret	=  app_UI_mode_availability_set_menu();		break;
		}
		if(ret == 0){	which_menu = 0;	}


		if(which_menu == 0){
			rec_char_u8	= debug_getchar(0,&rec_status_u8);
			if(rec_status_u8){
				if(menu_pass==0){
					menu_access(&rec_char_u8,'M','A','R',&menu_pass);
				}
				if(rec_char_u8 != 0x1b){	esc_cnt	=  0;	}

				switch(rec_char_u8){
					case ' ':
						printf("\r\n");
						printf("\r\n-------------------------------");
						printf("\r\n           Test Menu");
						printf("\r\n-------------------------------");
						api_STM32_uniqueID_print();
						api_SerialNo_print();
						printf("\r\n-------------------------------");
						app_pneumatics_O2_sensor_print();
						app_pneumatics_O2_min_print();
						app_pneumatics_O2_max_print();
						app_pneumatics_Neb_flow_min_print();
						printf("\r\n-------------------------------");
						printf("\r\n");
						printf("\r\nc   Calibrate Touch Screen");
						printf("\r\nr   Selfcheck Results");
						printf("\r\nO   Select O2 Sensor");
						printf("\r\nN   Set Nebuliser Flow Rate");
						printf("\r\n7   Calibrate Paracube O2 Sensor");
						printf("\r\n8   Set min O2 conc");
						printf("\r\n9   Set max O2 conc");
						printf("\r\nU   Unlock PWM minimum");
						break;
					case 0x1b:
						if(++esc_cnt == 2){
							esc_cnt		=  0;
							*debug_mode	=  0;
							printf("\r\nApp Comms Mode");
						}
						break;
					case 'r':
					//	app_selfcheck_print_result();	//text
						app_selfcheck_print_status();	//binary
						break;
					case 'c':
						app_UI_screen(UI_TOUCH_CALIBRATE);
						break;
					case 'O':
						if(app_Pneumatics.O2_sensor_sel > (APP_PNEUMATIC_O2_SENSOR_enum)1){app_Pneumatics.O2_sensor_sel	=  (APP_PNEUMATIC_O2_SENSOR_enum)1;	}
						app_Pneumatics.O2_sensor_sel	^= 0x01;

						app_pneumatics_O2_sensor_print();
						app_pneumatics_parameters_wr();
						break;
					case '7':
						printf("\r\nCalibrate O2 Paracube Sensor");
						app_Pneumatics.Cal_status	=  O2_CAL_START;
						break;
					case '8':
						do{
							printf("\r\nEnter the min O2 concentration (0 < conc <= 21):--%%\b\b\b");
							app_Pneumatics.O2_conc_min	=	BSP_get_num(BASE10,2);
							if((app_Pneumatics.O2_conc_min>0)&&(app_Pneumatics.O2_conc_min<=21)){
								fault	=  0;
							}
							else{
								printf("%% - Not valid");
								fault	=  1;
							}
						}while(fault);
						app_pneumatics_O2_min_print();
						app_pneumatics_parameters_wr();
						break;
					case '9':
						do{
							printf("\r\nEnter the max O2 concentration (0 < conc <= 100):---%%\b\b\b");
							app_Pneumatics.O2_conc_max	=	BSP_get_num(BASE10,3);
							if((app_Pneumatics.O2_conc_max>0)&&(app_Pneumatics.O2_conc_max<=100)){
								fault	=  0;
							}
							else{
								printf("%% - Not valid");
								fault	=  1;
							}
						}while(fault);
						app_pneumatics_O2_max_print();
						app_pneumatics_parameters_wr();
						break;
					case 'N':
						do{
							printf("\r\nEnter the Nebuliser fLow rate (0 < conc <= 9.99):-.--L/min\b\b\b\b\b\b\b\b\b");
							app_Pneumatics.flow_rate_nebuliser	=  BSP_get_float(1,2);
							if((app_Pneumatics.flow_rate_nebuliser >= 0)&&(app_Pneumatics.flow_rate_nebuliser <= 9.99)){
								fault	=  0;
							}
							else{
								printf("L/min - Not valid");
								fault	=  1;
							}
						}while(fault);
						app_pneumatics_Neb_flow_min_print();
						app_pneumatics_parameters_wr();
						break;
					case 'U':
						pwm_calibration.calib_status	=  CALIB_FAILED;
						api_calibrate_pwm_wr();
						break;
					default:
						printf("Invalid Character\r\n");
						break;
				}

				if(menu_pass==1){
					switch(rec_char_u8){
						case ' ':
							printf("\r\n*   modes of operation available");
							printf("\r\nm   Flash Mem Menu");
							printf("\r\na   Audio Menu");
							printf("\r\nw   Stopwatch Menu");
							printf("\r\no   IO test Menu");
							printf("\r\nl   LCD test Menu");
							printf("\r\n1   AD7794 test Menu");
							printf("\r\n2   LEDs test Menu");
							printf("\r\n3   fuel guage test Menu");
							printf("\r\n4   Paracube O2");
							printf("\r\n5   Selfcheck Test");
							printf("\r\nt   RTC");
							printf("\r\ns   Set Serial No");
							printf("\r\np   Debug PID");
							printf("\r\nP   Debug Patient Pressure");
							printf("\r\n}   Debug O2");
							printf("\r\n{   Debug touchscreen output");
							printf("\r\n)   Print Calibration Info");
							printf("\r\n(   Print flow ADC Info");
							printf("\r\n$   Enter temperature calibration lines");
							printf("\r\n");

							break;

						case 'm':	which_menu	=  1;		break;
						case 'a':	which_menu	=  2;		break;
						case 'w':	which_menu	=  3;		break;
						case '1':	which_menu	=  5;		break;
						case 'l':	which_menu	=  6;		break;
						case '2':	which_menu	=  7;		break;
						case '3':	which_menu	=  8;		break;
						case 't':	which_menu	=  9;		break;
						case '4':	which_menu	=  10;		break;
						case '5':	which_menu	=  11;		break;
						case '*':	which_menu	=  12;		break;

						case 'o':	api_IO_test();			break;
						case '{':	api_touch_debug(2);		break;
						case 0x1b:
							menu_pass	=  0;
							break;
						case '!':
							printf("\r\n 5V sig = %f",io_status_st_glb.Test_5V);
							printf("\r\nTemperature = %f",io_status_st_glb.Temperature);
							break;
						case 's':
							api_SerialNo_set();
							break;
						case 'p':
							app_pneumatics_pid_debug(2);
							break;
						case 'P':
							app_PP_debug(2);
							break;
						case '}':
							app_pneumatics_o2_debug(2);
							break;
						case 'L':
							Lanugage_test();
							break;
						case 'F':
							Fonts_test();
							break;
						case ')':
							api_calibrate_cmd(CAL_FLOW_O2_a		,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_O2_b		,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_O2_c		,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_O2_d		,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_O2_e		,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_AIR_a	,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_AIR_b	,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_AIR_c	,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_AIR_d	,CAL_PRINT);
							api_calibrate_cmd(CAL_FLOW_AIR_e	,CAL_PRINT);
							api_calibrate_cmd(CAL_SENSOR_O2		,CAL_PRINT);
							api_calibrate_cmd(CAL_SENSOR_PP		,CAL_PRINT);
							break;
						case '(':
							printf("ADC (02  flow) %d ",io_status_st_glb.ADC_Pressure_O2);
							printf("ADC (Air flow) %d ",io_status_st_glb.ADC_Pressure_Air);
							break;
						case '$':
							printf("\r\nEnter the O2 gradient (5 d.p.): -");
							app_Pneumatics_caltemp.O2_m			=	0.0 - BSP_get_float(3, 5);
							printf("\r\nEnter the O2 offset: ");
							app_Pneumatics_caltemp.O2_c			=	BSP_get_float(5, 1);
							printf("\r\nEnter the Air gradient (5 d.p.): -");
							app_Pneumatics_caltemp.air_m		=	0.0 - BSP_get_float(3, 5);
							printf("\r\nEnter the Air offset: ");
							app_Pneumatics_caltemp.air_c		=	BSP_get_float(5, 1);
							
							printf("\r\nEnter the O2 gradient: %.5f", app_Pneumatics_caltemp.O2_m);
							printf("\r\nEnter the O2 offset: %.1f", app_Pneumatics_caltemp.O2_c);
							printf("\r\nEnter the Air gradient: %.5f", app_Pneumatics_caltemp.air_m);
							printf("\r\nEnter the Air offset: %.1f", app_Pneumatics_caltemp.air_c);
							printf("\r\nConfirm: y/n");
							fault					=  0;
							while(fault				== 0){
								app_sys_watchdog_reload();
								rec_char_u8	= debug_getchar(0,&rec_status_u8);
								if(rec_status_u8){
									switch(rec_char_u8){
										case 'y':
											fault	=  1;
											app_Pneumatics_caltemp.valid		=  PNEUMATICS_VALID;
											csp_sys_mem_wr((uint8_t*)&app_Pneumatics_caltemp, MEM_ADD_CALIBATION + 0x600, sizeof(app_Pneumatics_caltemp_t));
											printf("\r\nSettings saved");
											break;
										case 'x':
											fault	=  1;
											printf("\r\nSettings discarded");
											break;
										default:
											printf("\r\nInvalid Character");
									}
								}
							}
							break;
						default:
							printf("Invalid Character\r\n");
							break;
					}
				}
			}
		}
		
//		if(sys_tic_rd()		>  sys_tic_print){
//			sys_tic_print	+= 100;
//			api_print_status((TERMINAL_PRINT_enum)1);
//		}
	}
	return;
}




/*************************************************************************************************
* Function Name :		menu_access
* Description   :		Checks for the sequence BY1,BY2,BY3 in the correct order.
*						if correct it sets (*res) hi
* Arguments     : 		uint8_t 	*rec_char_u8
*						uint8_t 	BY1
*						uint8_t 	BY2
*						uint8_t 	BY3
*						uint8_t 	*res
* Returns       :
* Notes         : 		None Listed
*
* Version	Date d/m/y	Programmer	Reason for Change
* 0.1.0		04/01/11	W. Paul		Created
**************************************************************************************************/
void menu_access(uint8_t *rec_char_u8,uint8_t BY1,uint8_t BY2,uint8_t BY3,uint8_t *res)
{
	static uint8_t		char_his_a_u8[3]	=  {0,0,0};
	static uint8_t		char_his_index_u8	=  0;

	if( (*rec_char_u8 == BY1) || (*rec_char_u8 == BY2) || (*rec_char_u8 == BY3)){
		char_his_a_u8[char_his_index_u8++] = *rec_char_u8;				// Place received contents into test menu array.
		if(char_his_index_u8 >= 3){
			char_his_index_u8	=  0;

			if( (char_his_a_u8[0] == BY1) && (char_his_a_u8[1] == BY2) && (char_his_a_u8[2] == BY3) ){
				*res 				=  1;																/* Enable Test menu Flag */
				*rec_char_u8		=  ' ';											/* Set serial character to Space Bar to enter Test Menu */
			}
		}
	}
	else{
		char_his_index_u8	= 0;					// Initialise index to 0
		memset(char_his_a_u8,0,sizeof(char_his_a_u8));		// Initialise array
	}

	return;
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
