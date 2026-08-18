/**********************************************************************************************************
 *	Marturion Electronics Ltd
 *
 *	Knockmore Hill Business	Park
 *	9 Ferguson Drive
 *	Lisburn
 *	Co.	Antrim
 *	Northern Ireland
 *	BT28 2EX
 *
 *	Copyright 2017,	Marturion Electronics Ltd
 *	All	Rights Reserved
 *
 * Filename	   :  api_IO.c
 * Date	Created:  Thu 13 Jul 2017 03:43:53 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "alg_pid.h"

#include "app_system.h"
#include "app_pneumatic_ctrl.h"

#include "api_IO.h"
#include "api_watchdog.h"

#include "api_calibrate.h"
#include "csp_S25FL0xx.h"

#include "hal_STM32_uart.h"
#include "csp_STM32_uart.h"
#include "csp_STM32_iwdg.h"
#include "csp_STM32_timer3.h"

#include "csp_paracube_O2.h"

#include "pcb_spi2.h"
#include "pcb_pins.h"
#include "alg_av.h"

/*********************************************************************************************************
 *		Local Variables
 ********************************************************************************************************/
IO_STATUS_t				io_status_st_glb;


/**********************************************************************************************************
 **********************************************************************************************************/


/**********************************************************************************************************
 * Function	Name :	api_pwm_Air
 * Description	 :	This function is used to set the pwm for the Air proportional valve
 * Arguments	 : uint16_t value		0-1000
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		6/09/17	William	Paul	Original Created
 **********************************************************************************************************/
void api_pwm_Air(uint16_t value)
{
	if(value > PWM_AIR_MAX){	value =  PWM_AIR_MAX;	}
	io_status_st_glb.pwm_des_Air	=  value;

//	timer updated in api_update_IRQ
//	timer8_ch1_config(io_status_st_glb.pwm_Air);

	return;
}
void api_pwm_Air_IRQ(uint16_t value)
{
	if(value > PWM_AIR_MAX){	value =  PWM_AIR_MAX;	}
	io_status_st_glb.pwm_des_Air	=  value;

//	timer updated in api_update_IRQ
//	timer8_ch1_config(io_status_st_glb.pwm_Air);

	return;
}

/**********************************************************************************************************
 * Function	Name :	api_pwm_O2
 * Description	 :	This function is used to set the pwm for the O2 proportional valve
 * Arguments	 : uint16_t value		0-1000
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		6/09/17	William	Paul	Original Created
 **********************************************************************************************************/
void api_pwm_O2(uint16_t value)
{
	if(value > PWM_O2_MAX){	value =  PWM_O2_MAX;	}
	io_status_st_glb.pwm_des_O2	=  value;

//	timer updated in api_update_IRQ
//	timer8_ch2_config(io_status_st_glb.pwm_O2);

	return;
}
void api_pwm_O2_IRQ(uint16_t value)
{
	if(value > PWM_O2_MAX){	value =  PWM_O2_MAX;	}
	io_status_st_glb.pwm_des_O2	=  value;

//	timer updated in api_update_IRQ
//	timer8_ch2_config(io_status_st_glb.pwm_O2);

	return;
}

/**********************************************************************************************************
 * Function	Name :	api_pwm_Venturi_Flow
 * Description	 :	This function is used to set the pwm for the Venturi proportional valve
 * Arguments	 : uint16_t value		0-1000
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		6/09/17	William	Paul	Original Created
 **********************************************************************************************************/
void api_pwm_VenturiFlow(uint16_t value)
{
	if(value > PWM_VENTURI_MAX){	value =  PWM_VENTURI_MAX;	}
	io_status_st_glb.pwm_des_VenturiFlow	=  value;

//	timer updated in api_update_IRQ
//	timer8_ch3_config(io_status_st_glb.pwm_VenturiFlow);

	return;
}
void api_pwm_VenturiFlow_IRQ(uint16_t value)
{
	if(value > PWM_VENTURI_MAX){	value =  PWM_VENTURI_MAX;	}
	io_status_st_glb.pwm_des_VenturiFlow	=  value;

//	timer updated in api_update_IRQ
//	timer8_ch3_config(io_status_st_glb.pwm_VenturiFlow);

	return;
}




/**********************************************************************************************************
 * Function	Name :	api_valve_nebuliser
 * Description	 :	This function is used to switch the	'nebuliser' valve 'on_off'
 * Arguments	 : Valve_Nebuliser_enum		on_off		0-1
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 8/10/12	William	Paul	Original Created
 **********************************************************************************************************/
void api_valve_nebuliser(Valve_Nebuliser_enum 	on_off )
{
	if(on_off == VALVE_NEBLISER_TOG){		io_status_st_glb.valve_nebuliser	^= 0x01;				}
	else if(on_off >= VALVE_NEBLISER_ON){	io_status_st_glb.valve_nebuliser	=  VALVE_NEBLISER_ON;	}
	else{									io_status_st_glb.valve_nebuliser	=  VALVE_NEBLISER_OFF;	}

	PinSet(SOL_VALVE_NEBULIZER,(uint8_t)io_status_st_glb.valve_nebuliser);

	return;
}

/**********************************************************************************************************
 * Function	Name :	api_valve_O2_calibrate
 * Description	 :	This function is used to switch the	'O2_calibrate' valve 'on_off'
 * Arguments	 : Valve_O2_Cal_enum	 on_off		0-1
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 8/10/12	William	Paul	Original Created
 **********************************************************************************************************/
void api_valve_O2_calibrate(Valve_O2_Cal_enum on_off )
{
	if(on_off == VALVE_O2_CAL_TOG){		io_status_st_glb.valve_O2_calibrate	^= 0x01;				}
	else if(on_off >= VALVE_O2_CAL_ON){	io_status_st_glb.valve_O2_calibrate	=  VALVE_O2_CAL_ON;		}
	else{ 								io_status_st_glb.valve_O2_calibrate	=  VALVE_O2_CAL_OFF;	}

	PinSet(SOL_VALVE_O2_CAL,(uint8_t)io_status_st_glb.valve_O2_calibrate);

	return;
}

/**********************************************************************************************************
 * Function	Name :	api_valve_Venturi_En
 * Description	 :	This function is used to switch the	'Venturi_En' valve 'on_off'
 * Arguments	 : Valve_Venturi_En_enum	 en_dis		0-1
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 8/10/12	William	Paul	Original Created
 **********************************************************************************************************/
void api_valve_Venturi_En(Valve_Venturi_En_enum en_dis )
{
	if(en_dis == VALVE_Venturi_TOG){		io_status_st_glb.valve_venturi_en	^= 0x01;				}
	else if(en_dis >= VALVE_Venturi_EN){	io_status_st_glb.valve_venturi_en	=  VALVE_Venturi_EN;	}
	else{									io_status_st_glb.valve_venturi_en	=  VALVE_Venturi_DIS;	}

	PinSet(SOL_VALVE_VENTURI_EN,(uint8_t)io_status_st_glb.valve_venturi_en);

	return;
}


/**********************************************************************************************************
 * Function	Name :	api_io_fan
 * Description	 :	This function is used to switch the	fan on off
 * Arguments	 : uint8_t	 fanspeed		0-100
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 8/10/12	William	Paul	Original Created
 **********************************************************************************************************/
uint8_t api_io_fan(uint8_t fanspeed )
{
	if(fanspeed == 0xff){			return(io_status_st_glb.fan);	}
	if(		fanspeed == 0 ){		fanspeed	=  0;				}
	else if(fanspeed == 1 ){		fanspeed	=  PWM_FAN_HI;		}
	else if(fanspeed == 2 ){
		if(io_status_st_glb.fan){	fanspeed	=  0;				}
		else{						fanspeed	=  PWM_FAN_HI;		}
	}

	if(fanspeed > PWM_FAN_MAX){		fanspeed =  PWM_FAN_MAX;		}
	io_status_st_glb.fan	=  fanspeed;

	timer3_ch3_config(io_status_st_glb.fan);
	return(io_status_st_glb.fan);
}


/**********************************************************************************************************
 * Function	Name :	api_io_rd_AirSupply
 * Description	 :	This function is used to read the 'AirSupply' pressure io
 * Arguments	 : void
 * Returns		 : uint8_t	state
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 8/10/12	William	Paul	Original Created
 **********************************************************************************************************/
uint8_t api_io_rd_AirSupply(uint8_t renew)
{
	static	uint32_t	air_renew_timeout	=  0;
	
	if(air_renew_timeout					>  sys_tic_rd()){
		renew								=  0;
	}
	
	if(renew){
		io_status_st_glb.io_PressureAir		=  PinRead(IO_PRESSURE_AIR);
		if(io_status_st_glb.io_PressureAir	== 0){
			air_renew_timeout				=  sys_tic_rd() + 3000;
		}
	}
	
	return(io_status_st_glb.io_PressureAir);
}

/**********************************************************************************************************
 * Function	Name :	api_io_rd_O2Supply
 * Description	 :	This function is used to read the 'O2Supply' pressure io
 * Arguments	 : void
 * Returns		 : uint8_t	state
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 8/10/12	William	Paul	Original Created
 **********************************************************************************************************/
uint8_t api_io_rd_O2Supply(uint8_t renew)
{
	static	uint32_t	o2_renew_timeout	=  0;
	
	if(o2_renew_timeout						>  sys_tic_rd()){
		renew								=  0;
	}
	
	if(renew){
		io_status_st_glb.io_PressureO2		=  PinRead(IO_PRESSURE_O2);
		if(io_status_st_glb.io_PressureO2	== 0){
			o2_renew_timeout				=  sys_tic_rd() + 3000;
		}
	}
	
	return(io_status_st_glb.io_PressureO2);
}


/*************************************************************************************************
* Function Name :	api_io_safety_cutout
* Description   : 	This Function enables / disables the safety cutout option
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		21/09/17	W. Paul			Created
*************************************************************************************************/
void api_io_safety_cutout(Safty_Cutout_enum en_dis)
{
	if(en_dis == 2){	io_status_st_glb.safety_cutout	^= 0x01;				}
	else if(en_dis){	io_status_st_glb.safety_cutout	=  SAFTY_CUTOUT_EN;		}	//1= cutout enabled
	else{				io_status_st_glb.safety_cutout	=  SAFTY_CUTOUT_DIS;	}

	PinSet(SAFTY_CUTOUT,io_status_st_glb.safety_cutout);

	return;
}


/*************************************************************************************************
* Function Name :	api_io_power_sys
* Description   : 	This Function enables / disables power to the whole system
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		21/09/17	W. Paul			Created
*************************************************************************************************/
void api_io_power_sys(uint8_t en_dis)
{
	if(en_dis){	PinSet(PWR_LATCH,1);	}	//1= system powered on
	else{		PinSet(PWR_LATCH,0);	}

	return;
}

/*************************************************************************************************
* Function Name :	api_io_power_5V_sensor
* Description   : 	This Function enables / disables power to the whole system
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		21/09/17	W. Paul			Created
*************************************************************************************************/
void api_io_power_5V_sensor(uint8_t en_dis)
{
	if(en_dis){	io_status_st_glb.sensor_5V	=  1;	}	//1= 5V powered on
	else{		io_status_st_glb.sensor_5V	=  0;	}

	PinSet(PWR_5VEN,io_status_st_glb.sensor_5V);
	return;
}


/**********************************************************************************************************
 * Function	Name : api_print_status
 * Description	 : This	function is	used to
 * Arguments	 : None
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		09/10/12	William	Paul	Original Created
 **********************************************************************************************************/
void api_print_status(TERMINAL_PRINT_enum print_mode)
{
	switch(print_mode){
		case TERMINAL_LINE:
			printf("\r\n%d,"						,sys_tic_rd()						);
			
			printf("%.1f,"							,io_status_st_glb.Flow_Air				);
			
			printf("%d,"							,io_status_st_glb.ADC_Pressure_Air		);
			
			printf("%.1f,"							,io_status_st_glb.Flow_O2				);
			
			printf("%d,"							,io_status_st_glb.ADC_Pressure_O2		);
			
			printf("%.2f,"							,io_status_st_glb.Temperature			);
			
			printf("%d,%.2f,%.2f,%d,%.2f,"			,app_Pneumatics.O2_conc_target
													,app_Pneumatics.O2_conc_actual
													,app_Pneumatics.pid_conc_output
													,app_Pneumatics.flow_rate_target
													,app_Pneumatics.flow_rate_actual		);
													
			printf("%d,"							,io_status_st_glb.pwm_Air				);
			printf("%d"								,io_status_st_glb.pwm_O2				);
			
			break;
			
		case TERMINAL_FULL_SCREEN:
			printf("\r\n");
			printf("%d,"							,io_status_st_glb.pwm_Air		);
			printf("%d"								,io_status_st_glb.pwm_O2		);
			
			break;
			
		case TERMINAL_FAST:
			printf("\r\n");
			
			printf("%3d,"					,io_status_st_glb.pwm_Air			);
		
			printf("%d,%f,%d,"				,io_status_st_glb.ADC_Pressure_Air_raw
											,io_status_st_glb.ADC_Pressure_Air_compensated
											,io_status_st_glb.ADC_Pressure_Air	);
			
			printf("%.1f,"					,io_status_st_glb.Flow_Air			);
			
			printf("%d,"					,sys_tic_rd());
			
			printf("%f,"					,io_status_st_glb.Temperature		);
			
			break;
	}

	return;
}

/**********************************************************************************************************
 * Function	Name :	api_IO_test
 * Description	 :	This function is used to test IO functions
 * Arguments	 : None
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 8/10/12	William Paul	Original Created
 **********************************************************************************************************/
void api_IO_test(void)
{
	// Local Variables
	uint8_t				rec_status_u8;
	uint8_t				rec_char_u8;
	uint8_t				loop_here				=  1;
	uint8_t				result;
	static float		cal_pt_low,cal_pt_high;
	uint8_t				device;
	uint16_t			pwm_value;
	static TERMINAL_PRINT_enum		print_mode	=  TERMINAL_FULL_SCREEN;
	static uint32_t		sys_tic					=  0;
	static uint8_t		test_mode				=  0;
	static uint8_t		test_cnt				=  0;
	static uint16_t		test_restart			=  0;
	
	// Code
	while(loop_here){
		csp_paracube_handler();
		rec_char_u8	= debug_getchar(0,&rec_status_u8);
		if(rec_status_u8){
			switch(rec_char_u8){
				case ' ':
					printf("\r\n");
					printf("\r\n****TST	IO menu****");
					printf("\r\nEsc	 Exit Menu");
					printf("\r\np    print mode");
					printf("\r\n0123 Calibration seq");
					printf("\r\n4    Calibration vars");
					printf("\r\n");
					printf("\r\nqQ   Valve Nebuliser");
					printf("\r\nwW   Valve O2 calibration");
					printf("\r\neE   Valve Venturi En ");
					printf("\r\nazA  PWM Air");
					printf("\r\nsxS  PWM O2");
					printf("\r\ndcD  PWM Venturi");
					printf("\r\nvV   Safety CutOut");
					printf("\r\nb    buzzer reset");
					printf("\r\nnN   Sensor 5Ven");
					printf("\r\n!    Valve 1sec increment/decrement");
					printf("\r\nf    Fan");
					printf("\r\n");
					break;

				case 0x1b:
					loop_here =	0;
					break;
				case '!':
					if(test_mode == 0){	test_mode =  1;	}
					else{				test_mode =  0;	}
					test_cnt			=  0;
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_MATH);
					app_pneumatic_set_flow(0, 0);
					printf("\r\nTest Mode = %d",test_mode);
					break;

 				case 'p':
 					if(++print_mode > TERMINAL_FAST){
 						print_mode	=  TERMINAL_DISABLED;
 					}
 					break;

				case 'q':	api_valve_nebuliser(VALVE_NEBLISER_OFF);	break;
				case 'Q':	api_valve_nebuliser(VALVE_NEBLISER_ON);		break;
				case 'w':	api_valve_O2_calibrate(VALVE_O2_CAL_OFF);	break;
				case 'W':	api_valve_O2_calibrate(VALVE_O2_CAL_ON);	break;
				case 'e':	api_valve_Venturi_En(VALVE_Venturi_DIS);	break;
				case 'E':	api_valve_Venturi_En(VALVE_Venturi_EN);		break;
			//air pwm
				case 'a':
					pwm_value	=  io_status_st_glb.pwm_Air;
					pwm_value	+= 1;
					api_pwm_Air(pwm_value);
					break;
				case 'z':
					pwm_value	=  io_status_st_glb.pwm_Air;
					if(pwm_value>1){	pwm_value	-= 1;	}
					else{				pwm_value	=  0;	}
					api_pwm_Air(pwm_value);
					break;
				case 'A':
					printf("\r\nEnter AIR PWM :-----\b\b\b\b\b");
					pwm_value	=  BSP_get_num(BASE10,5);
					api_pwm_Air(pwm_value);
					break;
			//O2 pwm
				case 's':
					pwm_value	=  io_status_st_glb.pwm_O2;
					pwm_value	+= 1;
					api_pwm_O2(pwm_value);
					break;
				case 'x':
					pwm_value	=  io_status_st_glb.pwm_O2;
					if(pwm_value>1){	pwm_value	-= 1;	}
					else{				pwm_value	=  0;	}
					api_pwm_O2(pwm_value);
					break;
				case 'S':
					printf("\r\nEnter O2 PWM :-----\b\b\b\b\b");
					pwm_value	=  BSP_get_num(BASE10,5);
					api_pwm_O2(pwm_value);
					break;
			//venturi pwm
				case 'd':
					pwm_value	=  io_status_st_glb.pwm_VenturiFlow;
					pwm_value	+= 100;
					api_pwm_VenturiFlow(pwm_value);
					break;
				case 'c':
					pwm_value	=  io_status_st_glb.pwm_VenturiFlow;
					if(pwm_value>100){	pwm_value	-= 100;	}
					else{				pwm_value	=  0;	}
					api_pwm_VenturiFlow(pwm_value);
					break;
				case 'D':
					printf("\r\nEnter VenturiFlow PWM :-----\b\b\b\b\b");
					pwm_value	=  BSP_get_num(BASE10,5);
					api_pwm_VenturiFlow(pwm_value);
					break;
			//safety
				case 'v':
					api_io_safety_cutout(SAFTY_CUTOUT_EN);
					printf("\r\nCutout Enabled");
					break;
				case 'V':
					api_io_safety_cutout(SAFTY_CUTOUT_DIS);
					printf("\r\nCutout Disabled - Normal use");
					break;
				case 'b':
					printf("\r\nReset the watchdog_buz/safty cutout");
					api_watchdog_reset();
					break;

			//power en
				case 'n':
					api_io_power_5V_sensor(0);
					printf("\r\nSensor 5V power off");
					break;
				case 'N':
					api_io_power_5V_sensor(1);
					printf("\r\nSensor 5V power on");
					break;

			//calibration
				case '0':
					printf("\r\nWhich device is being calibrated?");
					printf("\r\n 0  - CAL_FLOW_O2_a");
					printf("\r\n 1  - CAL_FLOW_O2_b");
					printf("\r\n 2  - CAL_FLOW_O2_c");
					printf("\r\n 3  - CAL_FLOW_O2_d");
					printf("\r\n 4  - CAL_FLOW_O2_e");
					printf("\r\n 5  - CAL_FLOW_AIR_a");
					printf("\r\n 6  - CAL_FLOW_AIR_b");
					printf("\r\n 7  - CAL_FLOW_AIR_c");
					printf("\r\n 8  - CAL_FLOW_AIR_d");
					printf("\r\n 9  - CAL_FLOW_AIR_e");
					printf("\r\n 10 - CAL_SENSOR_O2");
					printf("\r\n 11 - CAL_SENSOR_PP");
					printf("\r\n:-\b");
					device				=  BSP_get_num(BASE10,2);
					api_calibrate_cmd((cal_devices_enum)device,CAL_INIT);

					printf("\r\nGo to low  point (then press 1)\r\n");
					break;
				case '1':
					api_calibrate_cmd((cal_devices_enum)device,CAL_POINT_LOW);
					printf("\r\nADC captured - Enter the low  calibration value :-----\b\b\b\b\b");
					cal_pt_low			=  BSP_get_num(BASE10,5);
					printf("\r\nGo to high point (then press 2)\r\n");
					break;
				case '2':
					api_calibrate_cmd((cal_devices_enum)device,CAL_POINT_HIGH);
					printf("\r\nADC captured - Enter the high calibration value :-----\b\b\b\b\b");
					cal_pt_high			=  BSP_get_num(BASE10,5);
					printf("\r\nPress 3	to calculate\r\n");
					break;
				case '3':
					api_calibrate_set_points(		(cal_devices_enum)device,cal_pt_low,cal_pt_high);
					result	=  api_calibrate_cmd(	(cal_devices_enum)device,CAL_CALCULATE);
					printf("\r\nCal finished -");
					if(result == 0){	printf("PASS");	}
					else{				printf("FAIL");	}
					//no break - go to print
				case '4':
					api_calibrate_cmd((cal_devices_enum)device,CAL_PRINT);
					break;
				case 'f':
					api_io_fan(2);
					
				

				default:
					printf("\r\n Invalid Command");
					break;
			}


		}
		
		if(sys_tic_rd() > sys_tic){
			sys_tic	=  sys_tic_rd() + 100;
//			if(++test_cnt >= 5){
//				test_cnt	=  0;
//				pwm_value	=  io_status_st_glb.pwm_Air;
//				if(test_mode == 1){
//					if(pwm_value < 1000){		pwm_value	=  1000;	}
//					else if(pwm_value <= 5750){	pwm_value	+= 250;	}
//					else if(pwm_value <= 9500){	pwm_value	+= 500;	}
//					else{
//						pwm_value		=  1000;
//						test_mode		=  2;
//						test_restart	=  0;
//					}
//				}
//				if(test_mode == 2){
//					test_restart	+= 1;
//					test_restart2	+= 1;
//					if(test_restart2	<  16){
//						if(test_restart >= (360-17)){	test_mode	=  1;	}		//every 1/2hour restart the test
//																					//17 is the number of 5second data samples taken
//					}
//					else{
//						if(test_restart >= (720-17)){	test_mode	=  1;	}		//every 1/2hour restart the test
//																					//17 is the number of 5second data samples taken
//					}
//				}
//			api_pwm_Air(pwm_value);
//			//	api_pwm_O2(pwm_value);
//			}
			if(test_mode){
				switch(test_cnt){
					case 0:				// 20
						if(io_status_st_glb.Temperature	>  20.0){
							test_restart				=  0;
							test_cnt					++;
						}
						break;
					case 4:				// 25
						if(io_status_st_glb.Temperature	>  25.0){
							test_restart				=  0;
							test_cnt					++;
						}
						break;
					case 8:				// 30
						if(io_status_st_glb.Temperature	>  30.0){
							test_restart				=  0;
							test_cnt					++;
						}
						break;
					case 12:			// 35
						if(io_status_st_glb.Temperature	>  35.0){
							test_restart				=  0;
							test_cnt					++;
						}
						break;
					case 16:			// 40
						if(io_status_st_glb.Temperature	>  40.0){
							test_restart				=  0;
							test_cnt					++;
						}
						break;
					case 1:		case 5:		case 9:		case 13:	case 17:
						switch(test_restart){
							case 0:			// 0
								app_pneumatic_set_flow(0, 0);
								break;
							case 200:		// 10
								app_pneumatic_set_flow(10, 0);
								break;
							case 400:		// 30
								app_pneumatic_set_flow(30, 0);
								break;
							case 600:		// 80
								app_pneumatic_set_flow(80, 0);
								break;
							case 800:		// 120
								app_pneumatic_set_flow(120, 0);
								break;
							case 1000:		// 0
								app_pneumatic_set_flow(0, 0);
								test_cnt				++;
						}
						test_restart					++;
						break;
					case 2:		case 6:		case 10:	case 14:	case 18:
						test_restart					=  0;
						test_cnt						++;
						break;
					case 3:		case 7:		case 11:	case 15:	case 19:
						switch(test_restart){
							case 0:			// 0
								app_pneumatic_set_flow(0, 100);
								break;
							case 200:		// 10
								app_pneumatic_set_flow(10, 100);
								break;
							case 400:		// 30
								app_pneumatic_set_flow(30, 100);
								break;
							case 600:		// 80
								app_pneumatic_set_flow(80, 100);
								break;
							case 800:		// 120
								app_pneumatic_set_flow(120, 100);
								break;
							case 1000:		// 0
								app_pneumatic_set_flow(0, 0);
								test_cnt				++;
						}
						test_restart					++;
						break;
					default:
						test_mode	=  0;
				}
			}
			api_print_status(print_mode);
		}
		
		app_sys_watchdog_reload();
	}
	return;
}



/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
