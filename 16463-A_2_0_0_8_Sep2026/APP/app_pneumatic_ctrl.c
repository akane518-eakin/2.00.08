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
 * Filename    :  app_pneumatic_ctrl.c
 * Date Created:  Tue 12 Sep 2017 07:54:32 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <math.h>

#include "api_audio.h"

#include "app_pneumatic_ctrl.h"
#include "app_patient_pressure.h"
#include "app_selfcheck.h"

#include "alg_filt.h"
#include "alg_pid.h"
#include "alg_NTC_Thermistor.h"
#include "alg_av.h"
#include "api_IO.h"
#include "api_calibrate.h"
#include "csp_AD7794.h"
#include "csp_STM32_timer8.h"
#include "csp_STM32_delay.h"
#include "csp_S25FL0xx.h"
#include "csp_paracube_O2.h"
#include "pcb_spi1.h"
#include "pcb_spi2.h"
#include "pcb_pins.h"

app_Pneumatics_t			app_Pneumatics;
app_Pneumatics_caltemp_t	app_Pneumatics_caltemp;
float						paracube_val;
uint8_t						app_ctrl_use_direct;
uint8_t						app_ctrl_cnt;

uint8_t app_pneumatics_IRQ_status(void);


/*************************************************************************************************
* Function Name : 	app_pneumatic_manager
* Description   : 	This Function is the central control of the pneumatics
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatic_manager(void)
{
	static uint8_t		fan_status					=  2;
	static uint8_t		AirSupply_Status			=  0xff;
	static uint8_t		O2Supply_Status				=  0xff;
	uint8_t				AirSupply;
	uint8_t				O2Supply;

	if(app_Pneumatics.valid							!= PNEUMATICS_VALID){
		app_pneumatics_parameters_rd();

		app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);

		app_pneumatic_set_mode(PMODE_CPAP,			1);

		//retrieve calibration vars from mem
		api_calibrate_cmd(CAL_FLOW_O2_a,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_O2_b,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_O2_c,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_O2_d,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_O2_e,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_AIR_a,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_AIR_b,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_AIR_c,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_AIR_d,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_FLOW_AIR_e,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_SENSOR_O2,			CAL_FROM_MEM);
		api_calibrate_cmd(CAL_SENSOR_PP,			CAL_FROM_MEM);
		api_calibrate_pwm_rd();

		alg_PID_config_set(PID_O2,					PID_NO_CTRL);
		alg_PID_config_set(PID_AIR,					PID_NO_CTRL);
		alg_PID_config_set(PID_VENTURI_FLOW,		PID_NO_CTRL);
		alg_PID_config_set(PID_CONC,				PID_NO_CTRL);

		app_Pneumatics.startup_o2_cal_status		=  0xff;
		app_Pneumatics.Cal_status					=  O2_CAL_IDLE;
		app_Pneumatics.ADC_reset_cnt				=  0;
		
		io_status_st_glb.Flow_Air_Of				=  0.0;
		io_status_st_glb.Flow_O2_Of					=  0.0;
		io_status_st_glb.Flow_Of_timer				=  0;
	}

	AirSupply	=  api_io_rd_AirSupply(0);
	O2Supply	=  api_io_rd_O2Supply(0);


	//set .Cal_status to 1 to start the calprocess
	switch(app_Pneumatics.Cal_status){
		case O2_CAL_IDLE:
			//do nothing - normal mode
			break;
		case O2_CAL_START:	//init
			if((AirSupply == 0)||(O2Supply == 0)){
				app_Pneumatics.Cal_status	=  O2_CAL_ERR_NO_GAS_ATSTART;
			}
			else{
				app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);
				app_pneumatic_set_flow(0, 0);
				app_pneumatic_set_nebuliser(0);
			//	csp_paracube_command(PARACUBE_RESTORE_CAL, 0);
				api_calibrate_cmd(CAL_SENSOR_O2,CAL_INIT);
				app_Pneumatics.Cal_status	+= 1;
			}
			app_selfcheck_SetIgnore(TEST_O2_SENSOR,	1);
			app_selfcheck_SetIgnore(TEST_FIO2_HIGH,	1);
			app_selfcheck_SetIgnore(TEST_FIO2_LOW,	1);
			break;
		case 2:	//Start AIR FLOW
			if((AirSupply == 0)||(O2Supply == 0)){
				app_Pneumatics.Cal_status	=  O2_CAL_ERR_NO_GAS;
			}
			else{
				app_pneumatic_set_flow(50, 0);
				app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_TEST_AIR);		//set up air flow
				app_Pneumatics.cal_timer	=  30*APP_PNEUMATICS_FREQ;		//start timer
				app_Pneumatics.Cal_status	+= 1;
			}
			break;
		case 3: //WAIT, TAKE low cal point, start O2 flow
			if(AirSupply == 0){	app_Pneumatics.Cal_status	=  O2_CAL_ERR_NO_GAS;	}
			if(app_Pneumatics.cal_timer == 0){							//wait for timer
				csp_paracube_command(PARACUBE_2_POINT_CAL_LOW, app_Pneumatics.O2_conc_min);		//take cal reading
				api_calibrate_cmd(CAL_SENSOR_O2,CAL_POINT_LOW);
				app_Pneumatics.cal_timer	=  5*APP_PNEUMATICS_FREQ;	//start timer
				app_Pneumatics.Cal_status	+= 1;
			}
			break;
		case 4:
			if(app_Pneumatics.cal_timer == 0){							//wait for timer
				app_pneumatic_set_flow(50, 100);
				app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_TEST_O2);		//set up O2 flow
				app_Pneumatics.cal_timer	=  30*APP_PNEUMATICS_FREQ;	//start timer
				app_Pneumatics.Cal_status	+= 1;
			}
			break;
		case 5:	 //WAIT, TAKE high cal point, start flush with air flow
			if(O2Supply == 0){	app_Pneumatics.Cal_status	=  O2_CAL_ERR_NO_GAS;	}
			if(app_Pneumatics.cal_timer == 0){							//wait for timer
				csp_paracube_command(PARACUBE_2_POINT_CAL_HIGH, app_Pneumatics.O2_conc_max);		//take cal reading
				api_calibrate_cmd(CAL_SENSOR_O2,CAL_POINT_HIGH);
				api_calibrate_cmd(CAL_SENSOR_O2,CAL_CALCULATE);
				app_Pneumatics.cal_timer	=  5*APP_PNEUMATICS_FREQ;	//start timer
				app_Pneumatics.Cal_status	+= 1;
			}
			break;
		case 6:
			if(app_Pneumatics.cal_timer == 0){							//wait for timer
				app_pneumatic_set_flow(50, 0);
				app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_TEST_AIR);	//set up O2 flow
				app_Pneumatics.cal_timer	=  5*APP_PNEUMATICS_FREQ;	//start timer
				app_Pneumatics.Cal_status	+= 1;
			}
			break;
		case 7:	//WAIT, stop
			if(app_Pneumatics.cal_timer == 0){							//wait for timer
				app_pneumatic_set_flow(0, 0);
				app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);		//set up O2 flow
				app_Pneumatics.cal_timer	=  0;					//start timer
				app_Pneumatics.Cal_status	+= 1;
				app_Pneumatics.startup_o2_cal_status		=  1;
				app_selfcheck_AckClearStatus_1(TEST_O2_STARTUP_CAL, TEST_PASS);	//clear any err flags that might exist
			}
			break;
		case 8:
			app_Pneumatics.Cal_status	=  O2_CAL_FINISHED;	//end of process flag
			break;


		case O2_CAL_ERR_NO_GAS:	//failure mode
		case O2_CAL_ERR_NO_SENSOR:
			app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);		//set up O2 flow
			csp_paracube_command(PARACUBE_RESTORE_CAL, 0);
			api_calibrate_cmd(CAL_SENSOR_O2, CAL_FROM_MEM);
			app_Pneumatics.startup_o2_cal_status		=  0;	//process started and was not completed so fail
			printf("\r\nEnd Calibration process - Fail");
			break;
		case O2_CAL_ERR_NO_GAS_ATSTART:
			csp_paracube_command(PARACUBE_RESTORE_CAL, 0);
			app_Pneumatics.startup_o2_cal_status		=  0;	//user asked for process to start so should be a fail
			app_Pneumatics.Cal_status	=  O2_CAL_FINISHED;	//end of process flag
			printf("\r\nEnd Calibration process - Not started");
			break;

		case O2_CAL_FINISHED:
			break;

		default:
			app_Pneumatics.Cal_status	=  O2_CAL_IDLE;
	}
	
	if(		(app_Pneumatics.Cal_status		>= O2_CAL_START)		&&		// During calibration
			(app_Pneumatics.Cal_status		<  O2_CAL_ERR_NO_GAS)	&&		// During calibration
			(app_Pneumatics.O2_sensor_sel	== O2_SENSOR_PARACUBE)	&&		// Paracube selected
			(csp_paracube_timeout_read(0)	== 1)					){		// RX timout reached
		app_Pneumatics.Cal_status			=  O2_CAL_ERR_NO_SENSOR;
	}
	
	//*****************************************************
	//adjust PID if one gas supply is lost
	if((AirSupply_Status	!= AirSupply)||(O2Supply_Status	!= O2Supply)){
		AirSupply_Status	=  AirSupply;
		O2Supply_Status		=  O2Supply;
		printf("\r\n***************************************************");
		printf("\r\n* Supply lost A%d O%d!! Adjusting flow parameters!! *", AirSupply, O2Supply);
		printf("\r\n***************************************************");
	}

	//*****************************************************
	//Fan
	if(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_IDLE){
		api_valve_nebuliser( VALVE_NEBLISER_OFF);
		if(fan_status != 0){
			fan_status	=  0;
			api_io_fan(0);
		}
	}
	else{
		api_valve_nebuliser( (Valve_Nebuliser_enum)app_Pneumatics.nebuliser_state);
		if(fan_status != 1){
			fan_status	=  1;
			api_io_fan(1);
		}
	}
	//*****************************************************

	if(app_Pneumatics.ctrl_tic_last != app_Pneumatics.pid_timer){
		app_Pneumatics.ctrl_tic_last = app_Pneumatics.pid_timer;
		app_pneumatic_print_pid();

		if(app_Pneumatics.ctrl_tic_last %100 ==0){
			app_pneumatic_print_O2();
		}
	}
	return;
}

/**********************************************************************************************************
 * Function Name : app_pneumatics_parameters_rd
 * Description   : This function is used to read the setup parameters from mem
 * Arguments     : None
 * Returns       : void
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 16/11/17	William Paul	Original Created
 **********************************************************************************************************/
void app_pneumatics_parameters_rd(void)
{
	//read settings from memory
	csp_mem_rd((uint8_t*)&app_Pneumatics, MEM_ADD_PNEUMATICS_VARS, sizeof(app_Pneumatics_t));

	//does data need initialised
	if(app_Pneumatics.valid != PNEUMATICS_VALID){
		app_Pneumatics.valid	= PNEUMATICS_VALID;
		printf("\r\n!!Pneumatic Data struct initialised!!");

		//set all default values in struct here
		app_Pneumatics.O2_conc_min			=  21;
		app_Pneumatics.O2_conc_max			=  100;
		app_Pneumatics.O2_sensor_sel		=  O2_SENSOR_PARACUBE;
		app_Pneumatics.flow_rate_nebuliser	=  6.0;

		app_pneumatics_parameters_wr();
		app_pneumatics_O2_sensor_print();
	}

	//On start up we always want to ...
	app_Pneumatics.plimit_at_2_lmin			=  AT_NORMAL_FLOW;
	app_Pneumatics.pmax_at_2_lmin			=  AT_NORMAL_FLOW;
	app_Pneumatics.pid_print_en				=  0;
	app_Pneumatics.O2_print_en				=  0;

	app_Pneumatics.O2_conc_actual			=  0;			//O2 concentration input to control
	app_Pneumatics.flow_rate_actual			=  0;

	app_Pneumatics.pid_AIR_output			=  0;			//pid output
	app_Pneumatics.pid_O2_output			=  0;			//pid vars
	app_Pneumatics.pid_venturi_O2_output	=  0;	//pid vars

	ad7794_Reset(AD7794_CHIP_EN);
	app_pneumatics_IRQ_en_dis(1);
	app_Pneumatics.ADC_rd_cnt		=  0;

	app_Pneumatics.ctrl_tic			=  0;
	app_Pneumatics.ctrl_tic_last	=  0;

	csp_mem_rd((uint8_t*)&app_Pneumatics_caltemp, MEM_ADD_CALIBATION + 0x600, sizeof(app_Pneumatics_caltemp_t));
	if(app_Pneumatics_caltemp.valid		!= PNEUMATICS_VALID){
		app_Pneumatics_caltemp.valid	=  PNEUMATICS_VALID;
		app_Pneumatics_caltemp.air_m	=  -37.470;
		app_Pneumatics_caltemp.air_c	=  25518.0;
		app_Pneumatics_caltemp.O2_m		=  -42.486;
		app_Pneumatics_caltemp.O2_c		=  18092.0;
		csp_sys_mem_wr((uint8_t*)&app_Pneumatics_caltemp, MEM_ADD_CALIBATION + 0x600, sizeof(app_Pneumatics_caltemp_t));
	}

	return;
}


/**********************************************************************************************************
 * Function Name : app_pneumatics_parameters_wr
 * Description   : This function is used to save the setup parameters
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 16/11/17	William Paul	Original Created
 **********************************************************************************************************/
void app_pneumatics_parameters_wr(void)
{
	csp_sys_mem_wr((uint8_t*)&app_Pneumatics, MEM_ADD_PNEUMATICS_VARS, sizeof(app_Pneumatics_t));

	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatics_O2_sensor_print
* Description   : 	This Function prints the current O2 sensor in use
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/04/18	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_O2_sensor_print(void)
{
	printf("\r\nO2 sensor select  ");
	switch(app_Pneumatics.O2_sensor_sel){
		case O2_SENSOR_MAX250:		printf("MAX250");	break;
		case O2_SENSOR_PARACUBE:	printf("PARACUBE");	break;
		default:					printf("Not defined properly  !!Fault!!");	break;
	}
	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatics_O2_min_print
* Description   : 	This Function prints the current O2 sensor in use
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/04/18	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_O2_min_print(void)
{
	printf("\r\nO2 minimum conc   %d%%",app_Pneumatics.O2_conc_min);
	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatics_O2_max_print
* Description   : 	This Function prints the current O2 sensor in use
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/04/18	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_O2_max_print(void)
{
	printf("\r\nO2 maximum conc   %d%%",app_Pneumatics.O2_conc_max);
	return;
}


/*************************************************************************************************
* Function Name : 	app_pneumatic_pid_mode_set
* Description   : 	This Function sets the pid control
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatic_pid_mode_set(APP_PNEUMATIC_CTRL_PID_enum mode)
{
	APP_PNEUMATIC_CTRL_PID_enum		prev_mode	=  app_Pneumatics.PID_mode;

	switch(mode){
		case PNEUMATIC_CTRL_PID_MATH:
			if(app_Pneumatics.flow_rate_O2_math		<= PID_LO_THRESH){
				alg_PID_config_set(PID_O2			,PID_FLOW_CTRL_LO);
			}
			else{
				alg_PID_config_set(PID_O2			,PID_FLOW_CTRL_HI);
			}
			if(app_Pneumatics.flow_rate_air_math	<= PID_LO_THRESH){
				alg_PID_config_set(PID_AIR			,PID_FLOW_CTRL_LO);
			}
			else{
				alg_PID_config_set(PID_AIR			,PID_FLOW_CTRL_HI);
			}
			alg_PID_config_set(PID_VENTURI_FLOW		,PID_FLOW_CTRL_HI);	//not used yet
			
			if(app_ctrl_use_direct					== 1){
				alg_PID_config_set(PID_CONC			,PID_CONC_CTRL_FAST_DIR);
			}
			else if(app_Pneumatics.flow_rate_target	<= PID_VLO_THRESH){
				alg_PID_config_set(PID_CONC			,PID_CONC_CTRL_FAST_VLO);
			}
			else if(app_Pneumatics.flow_rate_target	<  PID_LO_THRESH){
				alg_PID_config_set(PID_CONC			,PID_CONC_CTRL_FAST_LO);
			}
			else{
				alg_PID_config_set(PID_CONC			,PID_CONC_CTRL_HI);
			}
			
			app_Pneumatics.pid_conc_en				=  0;
			app_ctrl_cnt							=  0;
			
			if(io_status_st_glb.Flow_Of_timer		>= 7000){
				io_status_st_glb.Flow_Air_Of		=  io_status_st_glb.Flow_Air_Av;
				if(io_status_st_glb.Flow_Air_Of		>= 5.0){
					io_status_st_glb.Flow_Air_Of	=  5.0;
				}
				if(io_status_st_glb.Flow_Air_Of		<= -5.0){
					io_status_st_glb.Flow_Air_Of	=  -5.0;
				}
				
				io_status_st_glb.Flow_O2_Of			=  io_status_st_glb.Flow_O2_Av;
				if(io_status_st_glb.Flow_O2_Of		>= 5.0){
					io_status_st_glb.Flow_O2_Of		=  5.0;
				}
				if(io_status_st_glb.Flow_O2_Of		<= -5.0){
					io_status_st_glb.Flow_O2_Of		=  -5.0;
				}
			}
			break;
		case PNEUMATIC_CTRL_PID_IDLE:
		case PNEUMATIC_CTRL_PID_VENTURI:
		case PNEUMATIC_CTRL_TEST_O2:
		case PNEUMATIC_CTRL_TEST_AIR:
		case PNEUMATIC_CTRL_PID_SHUTDOWN:
			alg_PID_config_set(PID_O2				,PID_FLOW_CTRL_LO);
			alg_PID_config_set(PID_AIR				,PID_FLOW_CTRL_LO);			//normal parameters AIR responds fast O2 responds slowly
			alg_PID_config_set(PID_VENTURI_FLOW		,PID_FLOW_CTRL_LO);			//not used yet
			alg_PID_config_set(PID_CONC				,PID_CONC_CTRL_FAST_LO);
			break;
		case PNEUMATIC_CTRL_RAW_O2:
		case PNEUMATIC_CTRL_RAW_AIR:
			alg_PID_config_set(PID_O2				,PID_RAW_CTRL);				//used for calibration
			alg_PID_config_set(PID_AIR				,PID_RAW_CTRL);				//used for calibration
			alg_PID_config_set(PID_VENTURI_FLOW		,PID_FLOW_CTRL_HI);			//not used yet
			alg_PID_config_set(PID_CONC				,PID_CONC_CTRL_HI);
			break;
	}

	if(mode == PNEUMATIC_CTRL_PID_MATH){
		if(prev_mode != PNEUMATIC_CTRL_PID_MATH){
			app_selfcheck_SettleQuietAlarmStart();
		}
	}
	if(mode == PNEUMATIC_CTRL_PID_SHUTDOWN){
		app_selfcheck_clearRuntimeAlarms();

		app_Pneumatics.pid_O2_output			=  0;
		app_Pneumatics.pid_AIR_output			=  0;
		app_Pneumatics.pid_venturi_O2_output	=  0;
		app_Pneumatics.pid_conc_output			=  0;

		alg_PID_vars_reset(PID_O2);
		alg_PID_vars_reset(PID_AIR);
		alg_PID_vars_reset(PID_VENTURI_FLOW);
		alg_PID_vars_reset(PID_CONC);
	}

	app_Pneumatics.PID_mode =  mode;
	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatic_pid_mode_read
* Description   : 	This Function reads the pid control
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/17	W. Paul			Created
*************************************************************************************************/
APP_PNEUMATIC_CTRL_PID_enum app_pneumatic_pid_mode_read(void)
{
	return(app_Pneumatics.PID_mode);
}


/*************************************************************************************************
* Function Name : 	app_pneumatic_set_mode
* Description   : 	This Function sets the defaults for a particular mode
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t app_pneumatic_set_mode(app_Pmode_enum mode, uint8_t startup_init)
{
	static 	user_settings_t		user_settings[PMODE_COUNT];

	if(startup_init == 1){
		user_settings[PMODE_CPAP].O2_conc								=  30;
		user_settings[PMODE_CPAP].flow_rate								=  60;
		user_settings[PMODE_CPAP].Patient_Pressure_min.en_dis			=  EN;
		user_settings[PMODE_CPAP].Patient_Pressure_min.value			=  2;
		user_settings[PMODE_CPAP].Patient_Pressure_max.en_dis			=  EN;
		user_settings[PMODE_CPAP].Patient_Pressure_max.value			=  12;
		user_settings[PMODE_CPAP].Apnoea_alarm_wait						=  20;
		user_settings[PMODE_CPAP].Respiration_rate_max					=  20;

		user_settings[PMODE_CPAP_PAED].O2_conc							=  30;
		user_settings[PMODE_CPAP_PAED].flow_rate						=  20;
		user_settings[PMODE_CPAP_PAED].Patient_Pressure_min.en_dis		=  EN;
		user_settings[PMODE_CPAP_PAED].Patient_Pressure_min.value		=  2;
		user_settings[PMODE_CPAP_PAED].Patient_Pressure_max.en_dis		=  EN;
		user_settings[PMODE_CPAP_PAED].Patient_Pressure_max.value		=  12;
		user_settings[PMODE_CPAP_PAED].Apnoea_alarm_wait				=  20;
		user_settings[PMODE_CPAP_PAED].Respiration_rate_max				=  20;

		user_settings[PMODE_CPAP_HELMET].O2_conc						=  30;
		user_settings[PMODE_CPAP_HELMET].flow_rate						=  60;
		user_settings[PMODE_CPAP_HELMET].Patient_Pressure_min.en_dis	=  EN;
		user_settings[PMODE_CPAP_HELMET].Patient_Pressure_min.value		=  2;
		user_settings[PMODE_CPAP_HELMET].Patient_Pressure_max.en_dis	=  EN;
		user_settings[PMODE_CPAP_HELMET].Patient_Pressure_max.value		=  12;
		user_settings[PMODE_CPAP_HELMET].Apnoea_alarm_wait				=  20;
		user_settings[PMODE_CPAP_HELMET].Respiration_rate_max			=  20;

		user_settings[PMODE_BUBBLE_PAP].O2_conc							=  30;
		user_settings[PMODE_BUBBLE_PAP].flow_rate						=  5;
		user_settings[PMODE_BUBBLE_PAP].Patient_Pressure_min.en_dis		=  EN;
		user_settings[PMODE_BUBBLE_PAP].Patient_Pressure_min.value		=  2;
		user_settings[PMODE_BUBBLE_PAP].Patient_Pressure_max.en_dis		=  EN;
		user_settings[PMODE_BUBBLE_PAP].Patient_Pressure_max.value		=  10;
		user_settings[PMODE_BUBBLE_PAP].Apnoea_alarm_wait				=  60;
		user_settings[PMODE_BUBBLE_PAP].Respiration_rate_max			=  20;

		user_settings[PMODE_HFOT].O2_conc								=  30;
		user_settings[PMODE_HFOT].flow_rate								=  20;
		user_settings[PMODE_HFOT].Patient_Pressure_min.en_dis			=  EN;
		user_settings[PMODE_HFOT].Patient_Pressure_min.value			=  2;
		user_settings[PMODE_HFOT].Patient_Pressure_max.en_dis			=  EN;
		user_settings[PMODE_HFOT].Patient_Pressure_max.value			=  10;
		user_settings[PMODE_HFOT].Apnoea_alarm_wait						=  20;
		user_settings[PMODE_HFOT].Respiration_rate_max					=  20;

		user_settings[PMODE_POINT].O2_conc								=  60;
		user_settings[PMODE_POINT].flow_rate							=  30;
		user_settings[PMODE_POINT].Patient_Pressure_min.en_dis			=  EN;
		user_settings[PMODE_POINT].Patient_Pressure_min.value			=  2;
		user_settings[PMODE_POINT].Patient_Pressure_max.en_dis			=  EN;
		user_settings[PMODE_POINT].Patient_Pressure_max.value			=  10;
		user_settings[PMODE_POINT].Apnoea_alarm_wait					=  20;
		user_settings[PMODE_POINT].Respiration_rate_max					=  20;
	}

//	if(startup_init == 0){	//normal operation
//		if(app_Pneumatics.mode == mode){	//no change in mode
//			return(0);
//		}
//
//		//save the last settings
//		user_settings[app_Pneumatics.mode].O2_conc							=  app_Pneumatics.O2_conc_target;
//		user_settings[app_Pneumatics.mode].flow_rate						=  app_Pneumatics.flow_rate_target;
//		user_settings[app_Pneumatics.mode].Patient_Pressure_min.en_dis		=  PP_settings.Patient_Pressure_min.en_dis;
//		user_settings[app_Pneumatics.mode].Patient_Pressure_min.value		=  PP_settings.Patient_Pressure_min.value;           // This section saves previous 
//		user_settings[app_Pneumatics.mode].Patient_Pressure_max.en_dis		=  PP_settings.Patient_Pressure_max.en_dis;          // settings - currently disabeld will return to
//		user_settings[app_Pneumatics.mode].Patient_Pressure_max.value		=  PP_settings.Patient_Pressure_max.value;           // default values.
//		user_settings[app_Pneumatics.mode].Apnoea_alarm_wait				=  PP_settings.Apnoea_alarm_wait;
//		user_settings[app_Pneumatics.mode].Respiration_rate_max				=  PP_settings.Respiration_rate_max;
//	}

	//now change to the new mode
	app_Pneumatics.mode		=  mode;
	switch(app_Pneumatics.mode){
		case PMODE_CPAP:
			app_Pneumatics.limits.flow_min						=  20;
			app_Pneumatics.limits.flow_max						=  140;
			app_Pneumatics.limits.O2_min						=  0;
			app_Pneumatics.limits.O2_max						=  100;
			app_Pneumatics.limits.pressure_measured				=  1;
			app_Pneumatics.limits.breath_freq_measured			=  1;
			app_Pneumatics.limits.nebuliser_avail				=  1;
			app_Pneumatics.limits.pressure_alarm_avail			=  1;
			app_Pneumatics.limits.pressure_alarm_min			=  2;
			app_Pneumatics.limits.pressure_alarm_max			=  25;
			app_Pneumatics.limits.apnoea_alarm_avail			=  1;
			app_Pneumatics.limits.apnoea_alarm_min				=  20;
			app_Pneumatics.limits.apnoea_alarm_max				=  60;
			break;
		case PMODE_CPAP_PAED:
			app_Pneumatics.limits.flow_min						=  10;
			app_Pneumatics.limits.flow_max						=  70;
			app_Pneumatics.limits.O2_min						=  0;
			app_Pneumatics.limits.O2_max						=  100;
			app_Pneumatics.limits.pressure_measured				=  1;
			app_Pneumatics.limits.breath_freq_measured			=  1;
			app_Pneumatics.limits.nebuliser_avail				=  1;
			app_Pneumatics.limits.pressure_alarm_avail			=  1;
			app_Pneumatics.limits.pressure_alarm_min			=  2;
			app_Pneumatics.limits.pressure_alarm_max			=  25;
			app_Pneumatics.limits.apnoea_alarm_avail			=  1;
			app_Pneumatics.limits.apnoea_alarm_min				=  20;
			app_Pneumatics.limits.apnoea_alarm_max				=  60;
			break;
		case PMODE_CPAP_HELMET:
			app_Pneumatics.limits.flow_min						=  40;
			app_Pneumatics.limits.flow_max						=  140;
			app_Pneumatics.limits.O2_min						=  0;
			app_Pneumatics.limits.O2_max						=  100;
			app_Pneumatics.limits.pressure_measured				=  1;
			app_Pneumatics.limits.breath_freq_measured			=  1;
			app_Pneumatics.limits.nebuliser_avail				=  0;
			app_Pneumatics.limits.pressure_alarm_avail			=  1;
			app_Pneumatics.limits.pressure_alarm_min			=  2;
			app_Pneumatics.limits.pressure_alarm_max			=  25;
			app_Pneumatics.limits.apnoea_alarm_avail			=  1;
			app_Pneumatics.limits.apnoea_alarm_min				=  20;
			app_Pneumatics.limits.apnoea_alarm_max				=  60;
			break;
		case PMODE_BUBBLE_PAP:
			app_Pneumatics.limits.flow_min						=  2;
			app_Pneumatics.limits.flow_max						=  20;
			app_Pneumatics.limits.O2_min						=  0;
			app_Pneumatics.limits.O2_max						=  80;
			app_Pneumatics.limits.pressure_measured				=  1;
			app_Pneumatics.limits.breath_freq_measured			=  0;
			app_Pneumatics.limits.nebuliser_avail				=  0;
			app_Pneumatics.limits.pressure_alarm_avail			=  1;
			app_Pneumatics.limits.pressure_alarm_min			=  2;
			app_Pneumatics.limits.pressure_alarm_max			=  15;
			app_Pneumatics.limits.apnoea_alarm_avail			=  0;
			app_Pneumatics.limits.apnoea_alarm_min				=  20;
			app_Pneumatics.limits.apnoea_alarm_max				=  60;
			break;
		case PMODE_HFOT:
			app_Pneumatics.limits.flow_min						=  2;
			app_Pneumatics.limits.flow_max						=  70;
			app_Pneumatics.limits.O2_min						=  0;
			app_Pneumatics.limits.O2_max						=  100;
			app_Pneumatics.limits.pressure_measured				=  0;
			app_Pneumatics.limits.breath_freq_measured			=  0;
			app_Pneumatics.limits.nebuliser_avail				=  1;
			app_Pneumatics.limits.pressure_alarm_avail			=  0;
			app_Pneumatics.limits.pressure_alarm_min			=  0;
			app_Pneumatics.limits.pressure_alarm_max			=  20;
			app_Pneumatics.limits.apnoea_alarm_avail			=  0;
			app_Pneumatics.limits.apnoea_alarm_min				=  20;
			app_Pneumatics.limits.apnoea_alarm_max				=  60;
			break;
		case PMODE_POINT:
			app_Pneumatics.limits.flow_min						=  10;
			app_Pneumatics.limits.flow_max						=  80;
			app_Pneumatics.limits.O2_min						=  0;
			app_Pneumatics.limits.O2_max						=  100;
			app_Pneumatics.limits.pressure_measured				=  0;
			app_Pneumatics.limits.breath_freq_measured			=  0;
			app_Pneumatics.limits.nebuliser_avail				=  1;
			app_Pneumatics.limits.pressure_alarm_avail			=  0;
			app_Pneumatics.limits.pressure_alarm_min			=  0;
			app_Pneumatics.limits.pressure_alarm_max			=  20;
			app_Pneumatics.limits.apnoea_alarm_avail			=  0;
			app_Pneumatics.limits.apnoea_alarm_min				=  20;
			app_Pneumatics.limits.apnoea_alarm_max				=  60;
			break;

		default:
			while(1);
	}

	app_pneumatic_set_flow(		 		 user_settings[app_Pneumatics.mode].flow_rate
										,user_settings[app_Pneumatics.mode].O2_conc							);
	app_pneumatic_set_nebuliser(		 0 	);		//DEFAULT OFF	//app_Pneumatics.limits.nebuliser_avail

	app_PP_set_alarm_patient_pressure(	 user_settings[app_Pneumatics.mode].Patient_Pressure_min.en_dis
										,user_settings[app_Pneumatics.mode].Patient_Pressure_min.value
										,user_settings[app_Pneumatics.mode].Patient_Pressure_max.en_dis
										,user_settings[app_Pneumatics.mode].Patient_Pressure_max.value		);
	app_PP_set_alarm_apnoea(			 user_settings[app_Pneumatics.mode].Apnoea_alarm_wait				);
	app_PP_set_alarm_resperation_rate(	 user_settings[app_Pneumatics.mode].Respiration_rate_max			);

	return(1);
}




/*************************************************************************************************
* Function Name : 	app_pneumatic_set_flow
* Description   : 	This Function sets the flow rate and O2 conc from the user to the control system
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatic_set_flow(uint16_t flow_rate, uint16_t O2_conc)
{
	app_Pneumatics.O2_conc_target				=  O2_conc;
	app_Pneumatics.flow_rate_target				=  flow_rate;
	
	if(app_Pneumatics.O2_sensor_sel				== O2_SENSOR_PARACUBE){
		if(	(app_Pneumatics.flow_rate_target	<= (PID_VLO_THRESH+1))	&&
			(app_Pneumatics.O2_conc_target		<  CONC_VLO_THRESH)		){
			app_ctrl_use_direct					=  1;
		}
		else{
			app_ctrl_use_direct					=  0;
		}
	}
	else{
		if(app_Pneumatics.flow_rate_target		<= PID_LO_THRESH){
			app_ctrl_use_direct					=  1;
		}
		else{
			app_ctrl_use_direct					=  0;
		}
	}
	
	app_Pneumatics.pid_conc_output				=  (float)app_Pneumatics.O2_conc_target;
	app_pneumatic_cal_flows(					(float)app_Pneumatics.O2_conc_target);

	if(app_pneumatic_pid_mode_read()			== PNEUMATIC_CTRL_PID_MATH){
		app_pneumatic_pid_mode_set(				PNEUMATIC_CTRL_PID_MATH);			// configure new PID parameters
		app_selfcheck_SettleQuietAlarmStart();
	}
	
	return;
}


/*************************************************************************************************
* Function Name : 	app_pneumatic_cal_flows
* Description   : 	This Function calculates the mathematics flow rates required
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/08/18	W. Paul			Created
*************************************************************************************************/
void app_pneumatic_cal_flows(float conc_target)
{
	float	ft1;
	float	ft2;
	uint8_t	AirSupply	=  api_io_rd_AirSupply(0);
	uint8_t	O2Supply	=  api_io_rd_O2Supply(0);


	if( (AirSupply == 1) && (O2Supply == 1) ){
		if(conc_target < app_Pneumatics.O2_conc_min){
			app_Pneumatics.flow_rate_air_math	=  app_Pneumatics.flow_rate_target;
			app_Pneumatics.flow_rate_O2_math	=  0;
		}
		else{
			app_Pneumatics.flow_rate_O2_math	=  app_Pneumatics.flow_rate_target;
			ft1		=  100 - app_Pneumatics.O2_conc_min;
			ft1		*= conc_target;
			ft2		=  100 - conc_target;
			ft2		*= app_Pneumatics.O2_conc_min;
			app_Pneumatics.flow_rate_O2_math	*= (ft1-ft2);

			ft1		=  100 - app_Pneumatics.O2_conc_min;
			ft1		*= app_Pneumatics.O2_conc_max;
			ft2		=  100 - app_Pneumatics.O2_conc_max;
			ft2		*= app_Pneumatics.O2_conc_min;
			app_Pneumatics.flow_rate_O2_math	/= (ft1-ft2);


			app_Pneumatics.flow_rate_air_math		=  app_Pneumatics.flow_rate_target * conc_target;
			app_Pneumatics.flow_rate_air_math		-= app_Pneumatics.flow_rate_O2_math * app_Pneumatics.O2_conc_max;
			if(app_Pneumatics.nebuliser_state){
				app_Pneumatics.flow_rate_air_math	-= app_Pneumatics.flow_rate_nebuliser * app_Pneumatics.O2_conc_min;
			}
			app_Pneumatics.flow_rate_air_math		/= app_Pneumatics.O2_conc_min;
		}
	}
	else if( (AirSupply == 0) && (O2Supply == 1) ){
		app_Pneumatics.flow_rate_O2_math	=  app_Pneumatics.flow_rate_target;
		app_Pneumatics.flow_rate_air_math	=  0;
	}
	else if( (AirSupply == 1) && (O2Supply == 0) ){
		app_Pneumatics.flow_rate_O2_math	=  0;
		app_Pneumatics.flow_rate_air_math	=  app_Pneumatics.flow_rate_target;
	}

	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatic_set_nebuliser
* Description   : 	This Function sets the nebuliser status, from the user to the control
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		19/10/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatic_set_nebuliser(uint8_t neb_on_off)
{
	app_Pneumatics.nebuliser_state	=  neb_on_off;

	app_pneumatic_cal_flows((float)app_Pneumatics.O2_conc_target);

	if(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_MATH){
		app_selfcheck_SettleQuietAlarmStart();
	}

	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatics_Neb_flow_min_print
* Description   : 	This Function prints the current O2 sensor in use
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/04/18	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_Neb_flow_min_print(void)
{
	printf("\r\nNeb flow rate     %1.2fL/min",app_Pneumatics.flow_rate_nebuliser);
	return;
}

/*************************************************************************************************
* Function Name	:	app_pneumatics_ctrl_irq
* Description	:	This Function writes to	the	pwm	controlled proportional	valves
* Arguments		:	void
* Returns		:	void
* Notes			:	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/07/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_ctrl_irq(void)
{
	float		temperature_f										=  0.0;
	float		flow_offset											=  0.0;
	float		compensate_f										=  0.0;
	float		conc_f;
	float		conc_margin_f;
	float		conc_pid_out;
	float		flow_low_thresh;
	float		conc_thresh											=  20.0;
	float		flow_av[3];
	
	// Code
	PinSet(TP5,1);
	
	//Timer se to 100Hz
//	AD7794_CH0_MAX250_O2_Sensor		//10Hz sample
//	AD7794_CH1_O2_FLOW				//10Hz sample
//	AD7794_CH2_AIR_FLOW				//10Hz sample
//	AD7794_CH3_PATIENT_PRESSURE		//50Hz sample
//	AD7794_CH4_5V					//10Hz sample
//	AD7794_CH5_TEMPERATURE			//10Hz sample
	
	if(app_Pneumatics.ADC_rd_mode									!= 0xff){
		switch(app_Pneumatics.ADC_rd_mode){	//each step is visited at 10Hz
			case 2:
			case 4:
			case 6:
			case 8:
			case 10:
				io_status_st_glb.ADC_patientP						=  ad7794_convert_value(AD7794_CHIP_EN);
				io_status_st_glb.patientP							=  io_status_st_glb.ADC_patientP;
				io_status_st_glb.patientP							*= calibration[CAL_SENSOR_PP].m;
				io_status_st_glb.patientP							+= calibration[CAL_SENSOR_PP].c;
				app_PP_data_input(io_status_st_glb.patientP);
				//no break here
			case 0:
				if(app_Pneumatics.ADC_rd_mode_park){
					app_Pneumatics.ADC_rd_mode						=  0xfe;
				}
				else{
					switch(app_Pneumatics.ADC_rd_mode){
						case 10:
						case 0:	ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH0_MAX250_O2_Sensor);	break;
						case 2:	ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH1_O2_FLOW);			break;
						case 4:	ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH2_AIR_FLOW);			break;
						case 6:	ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH4_5V);					break;
						case 8:	ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH5_TEMPERATURE);		break;
					}
				}
				break;
			case 1:
				io_status_st_glb.ADC_O2_Sensor						=  	ad7794_convert_value(AD7794_CHIP_EN);
				io_status_st_glb.Max250_O2_value					=  io_status_st_glb.ADC_O2_Sensor;
				io_status_st_glb.Max250_O2_value					*= calibration[CAL_SENSOR_O2].m;
				io_status_st_glb.Max250_O2_value					+= calibration[CAL_SENSOR_O2].c;

				if(io_status_st_glb.Max250_O2_value					<  0){		io_status_st_glb.Max250_O2_value	= 0;	}
				if(io_status_st_glb.Max250_O2_value					>  100){	io_status_st_glb.Max250_O2_value	= 100;	}

				if(app_Pneumatics.O2_sensor_sel						== O2_SENSOR_MAX250){
					app_Pneumatics.O2_conc_actual					=  alg_av_f(MAX_250_AV,io_status_st_glb.Max250_O2_value);
				}
				if(app_Pneumatics.ADC_rd_mode_park){
					app_Pneumatics.ADC_rd_mode						=  0xfe;
				}
				else{
					ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH3_PATIENT_PRESSURE);
				}
				break;
			case 3:
				io_status_st_glb.ADC_Pressure_O2_raw				=  ad7794_convert_value(AD7794_CHIP_EN);
				
				io_status_st_glb.ADC_Pressure_O2					=  (uint32_t)alg_av_f(FLOW_O2_AV, io_status_st_glb.ADC_Pressure_O2_raw);
				
				io_status_st_glb.Flow_O2_1							=  io_status_st_glb.ADC_Pressure_O2;
				
				flow_offset											=  io_status_st_glb.Temperature;
				if(isnormal(cal_temperature_O2)){
					flow_offset										-= cal_temperature_O2;
				}
				
				if(isnormal(app_Pneumatics.flow_rate_O2_math)){
					compensate_f									=  app_Pneumatics.flow_rate_O2_math;
				}
				if(isnormal(app_Pneumatics_caltemp.O2_m)){
					compensate_f									*= app_Pneumatics_caltemp.O2_m;
				}
				if(isnormal(app_Pneumatics_caltemp.O2_c)){
					compensate_f									+= app_Pneumatics_caltemp.O2_c;
				}
				
				flow_offset											*= compensate_f;
				io_status_st_glb.Flow_O2_1							+= flow_offset;
				
				if(io_status_st_glb.Flow_O2_1						>  calibration[CAL_FLOW_O2_a].adc[1]){		//> because -m gradient
					io_status_st_glb.Flow_O2_1						*= calibration[CAL_FLOW_O2_a].m;
					io_status_st_glb.Flow_O2_1						+= calibration[CAL_FLOW_O2_a].c;
				}
				else if(io_status_st_glb.Flow_O2_1					> calibration[CAL_FLOW_O2_b].adc[1]){		//> because -m gradient
					io_status_st_glb.Flow_O2_1						*= calibration[CAL_FLOW_O2_b].m;
					io_status_st_glb.Flow_O2_1						+= calibration[CAL_FLOW_O2_b].c;
				}
				else if(io_status_st_glb.Flow_O2_1					>  calibration[CAL_FLOW_O2_c].adc[1]){		//> because -m gradient
					io_status_st_glb.Flow_O2_1						*= calibration[CAL_FLOW_O2_c].m;
					io_status_st_glb.Flow_O2_1						+= calibration[CAL_FLOW_O2_c].c;
				}
				else if(io_status_st_glb.Flow_O2_1					>  calibration[CAL_FLOW_O2_d].adc[1]){		//> because -m gradient
					io_status_st_glb.Flow_O2_1						*= calibration[CAL_FLOW_O2_d].m;
					io_status_st_glb.Flow_O2_1						+= calibration[CAL_FLOW_O2_d].c;
				}
				else{
					io_status_st_glb.Flow_O2_1						*= calibration[CAL_FLOW_O2_e].m;
					io_status_st_glb.Flow_O2_1						+= calibration[CAL_FLOW_O2_e].c;
				}
				
				io_status_st_glb.Flow_O2							=  io_status_st_glb.Flow_O2_1;
				io_status_st_glb.Flow_O2							-= io_status_st_glb.Flow_O2_Of;
				if(io_status_st_glb.Flow_O2	<  0.10){				io_status_st_glb.Flow_O2	=  0.0;	}
//				if(io_status_st_glb.Flow_O2	>  140){				io_status_st_glb.Flow_O2	=  140;	}
				
				flow_av[0]											=  alg_av_long(FLOW_O2_50_AV, io_status_st_glb.Flow_O2_1);
				flow_av[1]											=  alg_av_long(FLOW_O2_150_AV, io_status_st_glb.Flow_O2_1);
				flow_av[2]											=  alg_av_long(FLOW_O2_300_AV, io_status_st_glb.Flow_O2_1);
				io_status_st_glb.Flow_O2_Av							=  flow_av[0];
				if(app_ctrl_cnt										== 1){
					if(	(pid_vars[PID_CONC].error					<   1.0)	&&
						(pid_vars[PID_CONC].error					>  -1.0)	){
						if(app_Pneumatics.O2_ctrl_timer[2]			<  300){
							app_Pneumatics.O2_ctrl_timer[2]			++;
						}
					}
					else{
						app_Pneumatics.O2_ctrl_timer[2]				=  0;
					}
					if(	(pid_vars[PID_CONC].error					<   2.0)	&&
						(pid_vars[PID_CONC].error					>  -2.0)	){
						if(app_Pneumatics.O2_ctrl_timer[1]			<  150){
							app_Pneumatics.O2_ctrl_timer[1]			++;
						}
					}
					else{
						app_Pneumatics.O2_ctrl_timer[1]				=  0;
					}
					if(	(pid_vars[PID_CONC].error					<   3.0)	&&
						(pid_vars[PID_CONC].error					>  -3.0)	){
						if(app_Pneumatics.O2_ctrl_timer[0]			<  50){
							app_Pneumatics.O2_ctrl_timer[0]			++;
						}
					}
					else{
						app_Pneumatics.O2_ctrl_timer[0]				=  0;
					}
					
					if(app_Pneumatics.O2_ctrl_timer[2]				== 300){
						io_status_st_glb.Flow_O2_1					=  flow_av[2];
						if(app_Pneumatics.flow_rate_O2_math			<= PID_LO_THRESH){
							alg_PID_config_set(PID_O2,				PID_FLOW_CTRL_LO_SLOW2);
						}
						else{
							alg_PID_config_set(PID_O2,				PID_FLOW_CTRL_HI_SLOW2);
						}
						
						if(app_Pneumatics.O2_sensor_sel				== O2_SENSOR_PARACUBE){
							if(	(io_status_st_glb.pwm_Air			>  pwm_calibration.pwm_air)	&&
								(io_status_st_glb.pwm_O2			>  pwm_calibration.pwm_O2)	){
								if(	(pid_vars[PID_CONC].pidTerm		>  10.0)	&&
									(io_status_st_glb.Flow_O2_Of	<  5.0)		){
									io_status_st_glb.Flow_O2_Of		+= 0.0001;
								}
								if(	(pid_vars[PID_CONC].pidTerm		<  -10.0)	&&
									(io_status_st_glb.Flow_O2_Of	>  -5.0)	){
									io_status_st_glb.Flow_O2_Of		-= 0.0001;
								}
							}
						}
					}
					else if(app_Pneumatics.O2_ctrl_timer[1]			== 150){
						io_status_st_glb.Flow_O2_1					=  flow_av[1];
						if(app_Pneumatics.flow_rate_O2_math			<= PID_LO_THRESH){
							alg_PID_config_set(PID_O2,				PID_FLOW_CTRL_LO_SLOW1);
						}
						else{
							alg_PID_config_set(PID_O2,				PID_FLOW_CTRL_HI_SLOW1);
						}
					}
					else if(app_Pneumatics.O2_ctrl_timer[0]			== 50){
						io_status_st_glb.Flow_O2_1					=  flow_av[0];
						if(app_Pneumatics.flow_rate_O2_math			<= PID_LO_THRESH){
							alg_PID_config_set(PID_O2,				PID_FLOW_CTRL_LO_SLOW0);
						}
						else{
							alg_PID_config_set(PID_O2,				PID_FLOW_CTRL_HI_SLOW0);
						}
					}
					else{
						if(app_Pneumatics.flow_rate_O2_math			<= PID_LO_THRESH){
							alg_PID_config_set(PID_O2,				PID_FLOW_CTRL_LO);
						}
						else{
							alg_PID_config_set(PID_O2,				PID_FLOW_CTRL_HI);
						}
					}
				}
				if(io_status_st_glb.Flow_O2_Of						>  5.0){
					io_status_st_glb.Flow_O2_Of						=  0.0;
				}
				if(io_status_st_glb.Flow_O2_Of						<  -5.0){
					io_status_st_glb.Flow_O2_Of						=  0.0;
				}
				io_status_st_glb.Flow_O2_1							-= io_status_st_glb.Flow_O2_Of;

				if(app_Pneumatics.ADC_rd_mode_park){
					app_Pneumatics.ADC_rd_mode						=  0xfe;
				}
				else{
					ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH3_PATIENT_PRESSURE);
				}
				break;
			case 5:
				io_status_st_glb.ADC_Pressure_Air_raw				=  ad7794_convert_value(AD7794_CHIP_EN);
				
				io_status_st_glb.ADC_Pressure_Air					=  (uint32_t)alg_av_f(FLOW_AIR_AV, io_status_st_glb.ADC_Pressure_Air_raw);
				
				io_status_st_glb.Flow_Air_1							=  io_status_st_glb.ADC_Pressure_Air;
				
				flow_offset											=  io_status_st_glb.Temperature;
				if(isnormal(cal_temperature_air)){
					flow_offset										-= cal_temperature_air;
				}
				
				if(isnormal(app_Pneumatics.flow_rate_air_math)){
					compensate_f									=  app_Pneumatics.flow_rate_air_math;
				}
				if(isnormal(app_Pneumatics_caltemp.air_m)){
					compensate_f									*= app_Pneumatics_caltemp.air_m;
				}
				if(isnormal(app_Pneumatics_caltemp.air_c)){
					compensate_f									+= app_Pneumatics_caltemp.air_c;
				}
				
				flow_offset											*= compensate_f;
				io_status_st_glb.Flow_Air_1							+= flow_offset;
				
				if(	io_status_st_glb.Flow_Air_1						>  calibration[CAL_FLOW_AIR_a].adc[1]){		//> because -m gradient
					io_status_st_glb.Flow_Air_1						*= calibration[CAL_FLOW_AIR_a].m;
					io_status_st_glb.Flow_Air_1						+= calibration[CAL_FLOW_AIR_a].c;
				}
				else if(io_status_st_glb.Flow_Air_1					>  calibration[CAL_FLOW_AIR_b].adc[1]){		//> because -m gradient
					io_status_st_glb.Flow_Air_1						*= calibration[CAL_FLOW_AIR_b].m;
					io_status_st_glb.Flow_Air_1						+= calibration[CAL_FLOW_AIR_b].c;
				}
				else if(io_status_st_glb.Flow_Air_1					>  calibration[CAL_FLOW_AIR_c].adc[1]){		//> because -m gradient
					io_status_st_glb.Flow_Air_1						*= calibration[CAL_FLOW_AIR_c].m;
					io_status_st_glb.Flow_Air_1						+= calibration[CAL_FLOW_AIR_c].c;
				}
				else if(io_status_st_glb.Flow_Air_1					>  calibration[CAL_FLOW_AIR_d].adc[1]){		//> because -m gradient
					io_status_st_glb.Flow_Air_1						*= calibration[CAL_FLOW_AIR_d].m;
					io_status_st_glb.Flow_Air_1						+= calibration[CAL_FLOW_AIR_d].c;
				}
				else{
					io_status_st_glb.Flow_Air_1						*= calibration[CAL_FLOW_AIR_e].m;
					io_status_st_glb.Flow_Air_1						+= calibration[CAL_FLOW_AIR_e].c;
				}
				
				io_status_st_glb.Flow_Air							=  io_status_st_glb.Flow_Air_1;
				io_status_st_glb.Flow_Air							-= io_status_st_glb.Flow_Air_Of;
				if(io_status_st_glb.Flow_Air < 0.10){				io_status_st_glb.Flow_Air	= 0.0;	}
//				if(io_status_st_glb.Flow_Air > 140){				io_status_st_glb.Flow_Air	= 140;	}
				
				flow_av[0]											=  alg_av_long(FLOW_AIR_50_AV, io_status_st_glb.Flow_Air_1);
				flow_av[1]											=  alg_av_long(FLOW_AIR_150_AV, io_status_st_glb.Flow_Air_1);
				flow_av[2]											=  alg_av_long(FLOW_AIR_300_AV, io_status_st_glb.Flow_Air_1);
				io_status_st_glb.Flow_Air_Av						=  flow_av[0];
				if(app_ctrl_cnt										== 1){
					if(		(pid_vars[PID_CONC].error				<   1.0)	&&
							(pid_vars[PID_CONC].error				>  -1.0)	){
						if(app_Pneumatics.air_ctrl_timer[2]			<  300){
							app_Pneumatics.air_ctrl_timer[2]		++;
						}
					}
					else{
						app_Pneumatics.air_ctrl_timer[2]			=  0;
					}
					if(	(pid_vars[PID_CONC].error					<   2.0)	&&
						(pid_vars[PID_CONC].error					>  -2.0)	){
						if(app_Pneumatics.air_ctrl_timer[1]			<  150){
							app_Pneumatics.air_ctrl_timer[1]		++;
						}
					}
					else{
						app_Pneumatics.air_ctrl_timer[1]			=  0;
					}
					if(	(pid_vars[PID_CONC].error					<   3.0)	&&
						(pid_vars[PID_CONC].error					>  -3.0)	){
						if(app_Pneumatics.air_ctrl_timer[0]			<  50){
							app_Pneumatics.air_ctrl_timer[0]		++;
						}
					}
					else{
						app_Pneumatics.air_ctrl_timer[0]			=  0;
					}
					
					if(app_Pneumatics.air_ctrl_timer[2]				== 300){
						io_status_st_glb.Flow_Air_1					=  flow_av[2];
						if(app_Pneumatics.flow_rate_air_math		<= PID_LO_THRESH){
							alg_PID_config_set(PID_AIR,				PID_FLOW_CTRL_LO_SLOW2);
						}
						else{
							alg_PID_config_set(PID_AIR,				PID_FLOW_CTRL_HI_SLOW2);
						}
						
						if(app_Pneumatics.O2_sensor_sel				== O2_SENSOR_PARACUBE){
							if(	(io_status_st_glb.pwm_Air			>  pwm_calibration.pwm_air)	&&
								(io_status_st_glb.pwm_O2			>  pwm_calibration.pwm_O2)	){
								if(	(pid_vars[PID_CONC].pidTerm		>  10.0)	&&
									(io_status_st_glb.Flow_Air_Of	<  5.0)		){
									io_status_st_glb.Flow_Air_Of	-= 0.0001;
								}
								if(	(pid_vars[PID_CONC].pidTerm		<  -10.0)	&&
									(io_status_st_glb.Flow_Air_Of	>  -5.0)	){
									io_status_st_glb.Flow_Air_Of	+= 0.0001;
								}
							}
						}
					}
					else if(app_Pneumatics.air_ctrl_timer[1]		== 150){
						io_status_st_glb.Flow_Air_1					=  flow_av[1];
						if(app_Pneumatics.flow_rate_air_math		<= PID_LO_THRESH){
							alg_PID_config_set(PID_AIR,				PID_FLOW_CTRL_LO_SLOW1);
						}
						else{
							alg_PID_config_set(PID_AIR,				PID_FLOW_CTRL_HI_SLOW1);
						}
					}
					else if(app_Pneumatics.air_ctrl_timer[0]		== 50){
						io_status_st_glb.Flow_Air_1					=  flow_av[0];
						if(app_Pneumatics.flow_rate_air_math		<= PID_LO_THRESH){
							alg_PID_config_set(PID_AIR,				PID_FLOW_CTRL_LO_SLOW0);
						}
						else{
							alg_PID_config_set(PID_AIR,				PID_FLOW_CTRL_HI_SLOW0);
						}
					}
					else{
						if(app_Pneumatics.flow_rate_air_math		<= PID_LO_THRESH){
							alg_PID_config_set(PID_AIR,				PID_FLOW_CTRL_LO);
						}
						else{
							alg_PID_config_set(PID_AIR,				PID_FLOW_CTRL_HI);
						}
					}
				}
				if(io_status_st_glb.Flow_Air_Of						>  5.0){
					io_status_st_glb.Flow_Air_Of					=  0.0;
				}
				if(io_status_st_glb.Flow_Air_Of						<  -5.0){
					io_status_st_glb.Flow_Air_Of					=  0.0;
				}
				io_status_st_glb.Flow_Air_1							-= io_status_st_glb.Flow_Air_Of;
				
				if(app_Pneumatics.ADC_rd_mode_park){
					app_Pneumatics.ADC_rd_mode						=  0xfe;
				}
				else{
					ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH3_PATIENT_PRESSURE);
				}
				break;
			case 7:
				io_status_st_glb.ADC_5V								=  ad7794_convert_value(AD7794_CHIP_EN);
				io_status_st_glb.Test_5V							=  io_status_st_glb.ADC_5V;
				io_status_st_glb.Test_5V							*= AD7794_VREF_VALUE;
				io_status_st_glb.Test_5V							/= 0xffffff;
				io_status_st_glb.Test_5V							*= (2000000 / 1000000);

				if(app_Pneumatics.ADC_rd_mode_park){
					app_Pneumatics.ADC_rd_mode						=  0xfe;
				}
				else{
					ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH3_PATIENT_PRESSURE);
				}

				if(io_status_st_glb.Test_5V							<  4.0){
					app_Pneumatics.ADC_rd_mode						=  0xf1;	//reset the ADC
				}
				break;
			case 9:
				io_status_st_glb.ADC_Themistor1						=  ad7794_convert_value(AD7794_CHIP_EN);
				io_status_st_glb.ADC_Themistor						=  (uint32_t)alg_av_f(Temp_AV,io_status_st_glb.ADC_Themistor1);
				io_status_st_glb.ThermistorRes						=  io_status_st_glb.ADC_Themistor;
				io_status_st_glb.ThermistorRes						*= 10000;		//Resistor pull up value
				io_status_st_glb.ThermistorRes						/= (0xffffff - io_status_st_glb.ADC_Themistor);
				temperature_f										=  alg_NTC_R2C(3380.0, 10000.0, io_status_st_glb.ThermistorRes);
				io_status_st_glb.Temperature						=  temperature_f;

				if(ad7794_err_check(AD7794_CHIP_EN)					== 0){
					app_Pneumatics.ADC_rd_mode						=  0xf1;	//reset the ADC
				}
				else if(app_Pneumatics.ADC_rd_mode_park){
					app_Pneumatics.ADC_rd_mode						=  0xfe;
				}
				else{
					ad7794_convert_single_start(AD7794_CHIP_EN, AD7794_CH3_PATIENT_PRESSURE);
				}
				break;
			case 11:
				if(app_Pneumatics.pid_timer							<  10){
					app_Pneumatics.pid_timer						++;
				}
				else if(app_Pneumatics.PID_mode						== PNEUMATIC_CTRL_PID_MATH){
					app_Pneumatics.pid_timer						=  0;
					if(	(api_audio_play_status()					== AUDIO_IDLE)	&&									// alarm not sounding
						(app_Pneumatics.flow_rate_actual			>= 1.0)			){
						conc_f										=  app_Pneumatics.O2_conc_actual;
						if(conc_f									>  (float)app_Pneumatics.O2_conc_max){	// - 3.0){
							conc_f									=  (float)app_Pneumatics.O2_conc_max;
						}
						if(conc_f									<  (float)app_Pneumatics.O2_conc_min){	// + 3.0){
							conc_f									=  (float)app_Pneumatics.O2_conc_min;
						}
						
						if(app_Pneumatics.O2_sensor_sel				== O2_SENSOR_PARACUBE){
							conc_margin_f							=  4.0;
						}
						else{
							conc_margin_f							=  3.0;
						}
						
						if(	(conc_f									<  (float)app_Pneumatics.O2_conc_target + conc_margin_f)	&&
							(conc_f									>  (float)app_Pneumatics.O2_conc_target - conc_margin_f)	){
							if(app_ctrl_use_direct					== 1){
								alg_PID_config_set(PID_CONC,		PID_CONC_CTRL_SLOW_DIR);
							}
							else if(app_Pneumatics.flow_rate_target	<= PID_VLO_THRESH){
								alg_PID_config_set(PID_CONC,		PID_CONC_CTRL_SLOW_VLO);
							}
							else if(app_Pneumatics.flow_rate_target	<  PID_LO_THRESH){
								alg_PID_config_set(PID_CONC,		PID_CONC_CTRL_SLOW_LO);
							}
						}
						else{
							if(app_ctrl_use_direct					== 1){
								alg_PID_config_set(PID_CONC,		PID_CONC_CTRL_FAST_DIR);
							}
							else if(app_Pneumatics.flow_rate_target	<= PID_VLO_THRESH){
								alg_PID_config_set(PID_CONC,		PID_CONC_CTRL_FAST_VLO);
							}
							else if(app_Pneumatics.flow_rate_target	<  PID_LO_THRESH){
								alg_PID_config_set(PID_CONC,		PID_CONC_CTRL_FAST_LO);
							}
						}
						
						if(	(app_ctrl_cnt								== 1)	&&
							(app_Pneumatics.pid_conc_en					== 1)	){
							conc_pid_out							=  alg_PID(	PID_CONC								// PID variables
																				,conc_f									// input
																				,(float)app_Pneumatics.O2_conc_target);	// target
							conc_pid_out							+= (float)app_Pneumatics.O2_conc_target;
							
							if(	(app_Pneumatics.flow_rate_O2_math	>= 0.0)	&&
								(app_Pneumatics.flow_rate_air_math	>= 0.0)	){
								app_Pneumatics.pid_conc_output		=  conc_pid_out;
								
								conc_thresh							=  20.0;
								if(app_Pneumatics.flow_rate_target	<= PID_VLO_THRESH){
									conc_thresh						=  50.0;
								}
								
								if(app_Pneumatics.pid_conc_output	>  (float)app_Pneumatics.O2_conc_target + conc_thresh){
									app_Pneumatics.pid_conc_output	=  (float)app_Pneumatics.O2_conc_target + conc_thresh;
								}
								if(app_Pneumatics.pid_conc_output	<  (float)app_Pneumatics.O2_conc_target - conc_thresh){
									app_Pneumatics.pid_conc_output	=  (float)app_Pneumatics.O2_conc_target - conc_thresh;
								}
								
								if(app_Pneumatics.pid_conc_output	>  (float)app_Pneumatics.O2_conc_max){
									app_Pneumatics.pid_conc_output	=  (float)app_Pneumatics.O2_conc_max;
								}
								if(app_Pneumatics.pid_conc_output	<  (float)app_Pneumatics.O2_conc_min){
									app_Pneumatics.pid_conc_output	=  (float)app_Pneumatics.O2_conc_min;
								}
								
								app_pneumatic_cal_flows(app_Pneumatics.pid_conc_output);
							}
							
							if(app_Pneumatics.flow_rate_O2_math		<= 0.0){
								conc_pid_out						-= (float)app_Pneumatics.O2_conc_min;
								conc_pid_out						/= 10.0;
								app_Pneumatics.flow_rate_O2_math	=  conc_pid_out;
							}
							if(app_Pneumatics.flow_rate_air_math	<= 0.0){
								conc_pid_out						-= (float)app_Pneumatics.O2_conc_max;
								conc_pid_out						/= -10.0;
								app_Pneumatics.flow_rate_air_math	=  conc_pid_out;
							}
						}
						else if(app_ctrl_cnt						>  1){			// delay 10s after flow control stabilises
							app_ctrl_cnt							--;
						}
//						else if(	((app_Pneumatics.flow_rate_O2_math * io_status_st_glb.Flow_O2_1)	>= 0.0)		&&	// target and measured are on the same side of 0.0
//									((app_Pneumatics.flow_rate_air_math * io_status_st_glb.Flow_Air_1)	>= 0.0)		){
						else if(app_Pneumatics.O2_sensor_sel		== O2_SENSOR_PARACUBE){
							if(	(app_Pneumatics.flow_rate_target	>  (PID_VLO_THRESH+1))	||
								(app_Pneumatics.O2_conc_target		>= CONC_VLO_THRESH)		){
								app_Pneumatics.pid_conc_en			=  1;
							}
							if(app_ctrl_cnt							== 0){
								app_ctrl_cnt						=  11;
								app_Pneumatics.air_ctrl_timer[0]	=  0;
								app_Pneumatics.air_ctrl_timer[1]	=  0;
								app_Pneumatics.air_ctrl_timer[2]	=  0;
								app_Pneumatics.O2_ctrl_timer[0]		=  0;
								app_Pneumatics.O2_ctrl_timer[1]		=  0;
								app_Pneumatics.O2_ctrl_timer[2]		=  0;
							}
						}
						else{
							if(app_Pneumatics.flow_rate_target		>  PID_LO_THRESH){
								app_Pneumatics.pid_conc_en			=  1;
							}
							if(app_ctrl_cnt							== 0){
								app_ctrl_cnt						=  41;
								app_Pneumatics.air_ctrl_timer[0]	=  0;
								app_Pneumatics.air_ctrl_timer[1]	=  0;
								app_Pneumatics.air_ctrl_timer[2]	=  0;
								app_Pneumatics.O2_ctrl_timer[0]		=  0;
								app_Pneumatics.O2_ctrl_timer[1]		=  0;
								app_Pneumatics.O2_ctrl_timer[2]		=  0;
							}
						}
					}
				}
				break;

			case 0xf1:	//reset the ADC
				ad7794_Reset(AD7794_CHIP_EN);
				break;
			case 0xf2:	//2nd reset and return to main loop
				ad7794_Reset(AD7794_CHIP_EN);
				app_Pneumatics.ADC_reset_cnt						+= 1;
				app_Pneumatics.ADC_rd_mode							=  0;	//back to normal mode
				break;

		}
		app_Pneumatics.ADC_rd_mode									+= 1;
		if(	(app_Pneumatics.ADC_rd_mode								>  15)		&&
			(app_Pneumatics.ADC_rd_mode								<= 0xf0)	){
			app_Pneumatics.ADC_rd_mode								=  1;
			app_Pneumatics.ADC_rd_cnt								++;
		}
	}
	
	
	app_Pneumatics.ctrl_tic	+=  1;
	switch(app_Pneumatics.PID_mode){
		case PNEUMATIC_CTRL_PID_IDLE:
			//do nothing
			break;
		case PNEUMATIC_CTRL_PID_MATH:
			if(api_audio_play_status()								!= AUDIO_IDLE){					// no control if alarm sounding
				break;
			}
			
			app_Pneumatics.flow_rate_actual							=  (uint8_t)io_status_st_glb.Flow_O2;
			app_Pneumatics.flow_rate_actual							+= (uint8_t)io_status_st_glb.Flow_Air;
			app_pneumatics_nebuliser_corrector();
			
			flow_low_thresh											=  -2.0;
			if(app_ctrl_cnt											== 0){
				flow_low_thresh										=  0.0;
			}
			
			if(	(app_ctrl_use_direct								== 1)			&&
				(app_Pneumatics.O2_conc_target						<= CONC_MID)	){
				conc_f												=  app_Pneumatics.O2_conc_actual;
				if(conc_f											>  (float)app_Pneumatics.O2_conc_max){
					conc_f											=  (float)app_Pneumatics.O2_conc_max;
				}
				if(conc_f											<  (float)app_Pneumatics.O2_conc_min){
					conc_f											=  (float)app_Pneumatics.O2_conc_min;
				}
				
				conc_pid_out										=  pwm_calibration.pwm_O2;
				conc_pid_out										+= alg_PID(	PID_CONC								//PID variables
																				,conc_f									//input
																				,(float)app_Pneumatics.O2_conc_target);	//target
				app_Pneumatics.pid_O2_output						=  conc_pid_out;
			}
			else if((io_status_st_glb.Flow_O2_1						<= flow_low_thresh)	&&
					(app_Pneumatics.flow_rate_O2_math				<= PID_LO_THRESH)	){
				app_Pneumatics.pid_O2_output						+= 1.0;
			}
			else{
				app_Pneumatics.pid_O2_output						+= alg_PID(	PID_O2									//PID variables
																				,io_status_st_glb.Flow_O2_1				//input
																				,app_Pneumatics.flow_rate_O2_math);		//target
			}
			
			if(	(app_ctrl_use_direct								== 1)			&&
				(app_Pneumatics.O2_conc_target						>  CONC_MID)	){
				conc_f												=  app_Pneumatics.O2_conc_actual;
				if(conc_f											>  (float)app_Pneumatics.O2_conc_max){
					conc_f											=  (float)app_Pneumatics.O2_conc_max;
				}
				if(conc_f											<  (float)app_Pneumatics.O2_conc_min){
					conc_f											=  (float)app_Pneumatics.O2_conc_min;
				}
				
				conc_pid_out										=  pwm_calibration.pwm_O2;
				// swap input and target to control on Air
				conc_pid_out										+= alg_PID(	PID_CONC								//PID variables
																				,(float)app_Pneumatics.O2_conc_target	//input
																				,conc_f);								//target
				app_Pneumatics.pid_AIR_output						=  conc_pid_out;
			}
			else if((io_status_st_glb.Flow_Air_1					<= flow_low_thresh)	&&
					(app_Pneumatics.flow_rate_air_math				<= PID_LO_THRESH)	){
				app_Pneumatics.pid_AIR_output						+= 1.0;
			}
			else{
				app_Pneumatics.pid_AIR_output						+= alg_PID(	PID_AIR									//PID variables
																				,io_status_st_glb.Flow_Air_1			//input
																				,app_Pneumatics.flow_rate_air_math);	//target
			}
			break;
			
		case PNEUMATIC_CTRL_PID_VENTURI:
			app_Pneumatics.flow_rate_actual							=  (uint8_t)io_status_st_glb.Flow_O2;
			app_pneumatics_nebuliser_corrector();
			app_Pneumatics.pid_O2_output							+= alg_PID(	PID_O2										//PID variables
																			,app_Pneumatics.flow_rate_actual				//input
																			,(float)app_Pneumatics.flow_rate_target);		//target
			break;
		case PNEUMATIC_CTRL_TEST_O2:
			app_Pneumatics.flow_rate_actual							=  (uint8_t)io_status_st_glb.Flow_O2;
			app_Pneumatics.flow_rate_actual							+= (uint8_t)io_status_st_glb.Flow_Air;
			app_pneumatics_nebuliser_corrector();
			
			app_Pneumatics.pid_O2_output							+= alg_PID(	PID_O2										//PID variables
																		,app_Pneumatics.flow_rate_actual					//input
																		,(float)app_Pneumatics.flow_rate_target);			//target
			
			app_Pneumatics.pid_AIR_output							=  0;
			break;
		case PNEUMATIC_CTRL_TEST_AIR:
			app_Pneumatics.flow_rate_actual							=  (uint8_t)io_status_st_glb.Flow_O2;
			app_Pneumatics.flow_rate_actual							+= io_status_st_glb.Flow_Air;
			app_pneumatics_nebuliser_corrector();
			
			app_Pneumatics.pid_AIR_output							+= alg_PID(	PID_AIR										//PID variables
																		,app_Pneumatics.flow_rate_actual					//input
																		,(float)app_Pneumatics.flow_rate_target);			//target
			
			app_Pneumatics.pid_O2_output							=  0;
			break;

		case PNEUMATIC_CTRL_RAW_O2:
			app_Pneumatics.pid_O2_output							+= alg_PID(	PID_O2										//PID variables
																		,(float)io_status_st_glb.ADC_Pressure_O2_raw/256	//input
																		,(float)app_Pneumatics.flow_rate_O2_raw/256);		//target
			app_Pneumatics.pid_AIR_output							=  0;
			break;
			
		case PNEUMATIC_CTRL_RAW_AIR:
			app_Pneumatics.pid_O2_output							=  0;
			app_Pneumatics.pid_AIR_output							+= alg_PID(	PID_AIR										//PID variables
																		,(float)io_status_st_glb.ADC_Pressure_Air_raw/256	//input
																		,(float)app_Pneumatics.flow_rate_air_raw/256);		//target
			break;

		case PNEUMATIC_CTRL_PID_SHUTDOWN:
			io_status_st_glb.pwm_des_Air							=  0;
			io_status_st_glb.pwm_des_O2								=  0;
			io_status_st_glb.pwm_des_VenturiFlow					=  0;

			app_Pneumatics.PID_mode									=  PNEUMATIC_CTRL_PID_IDLE;

			break;
	}

	if(app_Pneumatics.PID_mode != PNEUMATIC_CTRL_PID_IDLE){

		if(			app_Pneumatics.pid_O2_output 	> PWM_O2_MAX ){						app_Pneumatics.pid_O2_output			=  PWM_O2_MAX;				}
		else if( (	app_Pneumatics.pid_O2_output 	> PWM_O2_MIN ) &&
			(		app_Pneumatics.pid_O2_output 	< pwm_calibration.pwm_O2 )	){		app_Pneumatics.pid_O2_output			=  pwm_calibration.pwm_O2;	}
		else if(	app_Pneumatics.pid_O2_output 	<= pwm_calibration.pwm_O2 ){		app_Pneumatics.pid_O2_output			=  PWM_O2_MIN;				}
		io_status_st_glb.pwm_des_O2	=  (uint16_t)app_Pneumatics.pid_O2_output;

		if(			app_Pneumatics.pid_AIR_output	> PWM_AIR_MAX ){					app_Pneumatics.pid_AIR_output			=  PWM_AIR_MAX;				}
		else if( (	app_Pneumatics.pid_AIR_output	> PWM_AIR_MIN )	&&
			(		app_Pneumatics.pid_AIR_output	< pwm_calibration.pwm_air )	){		app_Pneumatics.pid_AIR_output			=  pwm_calibration.pwm_air;	}
		else if(	app_Pneumatics.pid_AIR_output	<= pwm_calibration.pwm_air ){		app_Pneumatics.pid_AIR_output			=  PWM_AIR_MIN;				}
		io_status_st_glb.pwm_des_Air	=  (uint16_t)app_Pneumatics.pid_AIR_output;

		if(			app_Pneumatics.pid_venturi_O2_output > PWM_VENTURI_MAX ){			app_Pneumatics.pid_venturi_O2_output	=  PWM_VENTURI_MAX;			}
		else if( (	app_Pneumatics.pid_venturi_O2_output > PWM_VENTURI_MIN ) &&
			(		app_Pneumatics.pid_venturi_O2_output < PWM_VENTURI_MIN_USABLE )	){	app_Pneumatics.pid_venturi_O2_output	=  PWM_VENTURI_MIN_USABLE;	}
		else if(	app_Pneumatics.pid_venturi_O2_output <= PWM_VENTURI_MIN_USABLE ){	app_Pneumatics.pid_venturi_O2_output	=  PWM_VENTURI_MIN;			}
		io_status_st_glb.pwm_des_VenturiFlow	=  (uint16_t)app_Pneumatics.pid_venturi_O2_output;

	}

	//if gas failure don't try to use their supply
	//because if supply is returned we would get a sudden inrush
	if(api_io_rd_AirSupply(0)	== 0){	io_status_st_glb.pwm_des_Air	=  0;	}
	if(api_io_rd_O2Supply(0)	== 0){	io_status_st_glb.pwm_des_O2		=  0;	}


	if(io_status_st_glb.pwm_Air	!= io_status_st_glb.pwm_des_Air){
		io_status_st_glb.pwm_Air	=  io_status_st_glb.pwm_des_Air;
		timer8_ch1_config(io_status_st_glb.pwm_Air);
	}
	if(io_status_st_glb.pwm_O2	!= io_status_st_glb.pwm_des_O2){
		io_status_st_glb.pwm_O2		=  io_status_st_glb.pwm_des_O2;
		timer8_ch2_config(io_status_st_glb.pwm_O2);
	}
	if(io_status_st_glb.pwm_VenturiFlow	!= io_status_st_glb.pwm_des_VenturiFlow){
		io_status_st_glb.pwm_VenturiFlow	=  io_status_st_glb.pwm_des_VenturiFlow;
		timer8_ch3_config(io_status_st_glb.pwm_VenturiFlow);
	}

	if(app_Pneumatics.cal_timer){	app_Pneumatics.cal_timer	-= 1;	}
	
	api_io_rd_AirSupply(1);
	api_io_rd_O2Supply(1);

	PinSet(TP5,0);
	return;
}


/*************************************************************************************************
* Function Name : 	app_pneumatics_nebuliser_corrector
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		07/06/18	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_nebuliser_corrector(void)
{
	float	New_O2_conc;
	float	neb_O2_conc;

	if(app_Pneumatics.nebuliser_state){
		New_O2_conc	=  app_Pneumatics.flow_rate_actual;
		New_O2_conc	*= app_Pneumatics.O2_conc_actual;

		neb_O2_conc	=  app_Pneumatics.O2_conc_min;
		neb_O2_conc	*= app_Pneumatics.flow_rate_nebuliser;
		New_O2_conc	+= neb_O2_conc;

		app_Pneumatics.flow_rate_actual	+= app_Pneumatics.flow_rate_nebuliser;
		New_O2_conc	/= app_Pneumatics.flow_rate_actual;


		app_Pneumatics.O2_conc_actual	=  New_O2_conc;
	}
	return;
}

/*************************************************************************************************
* Function Name :	app_pneumatics_IRQ_en_dis;
* Description   : 	This Function shuts down/enables the IRQ read sequence of the ADC
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/09/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_IRQ_en_dis(uint8_t en_dis)
{
	if(en_dis){
		app_Pneumatics.ADC_rd_mode		=  0;		//start the IRQ read of ADC
		app_Pneumatics.ADC_rd_mode_park	=  0;
	}
	else{
		app_Pneumatics.ADC_rd_mode_park	=  1;
	}
	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatics_IRQ_status
* Description   : 	This Function reads the status of the IRQ rd mode byte
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/11/17	W. Paul			Created
*************************************************************************************************/
uint8_t app_pneumatics_IRQ_status(void)
{
	return(app_Pneumatics.ADC_rd_mode);
}


/*************************************************************************************************
* Function Name : 	app_pneumatic_print_O2
* Description   : 	This Function prints the O2 sensor data
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		24/01/19	W. Paul			Created
*************************************************************************************************/
void app_pneumatic_print_O2(void)
{
	if(app_Pneumatics.O2_print_en){
//		printf("\r\nMode %d Tar %d Para %3.1f Max250 %3.1f PID %3.1f "
//												,app_Pneumatics.PID_mode
//												,app_Pneumatics.O2_conc_target
//												,io_status_st_glb.Paracube_O2_value
//												,io_status_st_glb.Max250_O2_value
//												,app_Pneumatics.pid_O2_output
//												);
		printf("\r\n%x %x %x %x %x %x"
												,io_status_st_glb.ADC_Pressure_Air_raw
												,io_status_st_glb.ADC_Pressure_O2_raw
												,io_status_st_glb.ADC_O2_Sensor
												,io_status_st_glb.ADC_patientP
												,io_status_st_glb.ADC_Thermistor
												,io_status_st_glb.ADC_Pressure_Air
											);

	}
	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatic_print_pid
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatic_print_pid(void)
{
	if(app_Pneumatics.pid_print_en){
		switch(app_Pneumatics.PID_mode){
			case PNEUMATIC_CTRL_PID_IDLE:
				printf("\r\n%d,%d,Idle,%3d,%3d,,,,,,%.2f,,%.2f"
					,sys_tic_rd()
					,app_Pneumatics.PID_mode
					,io_status_st_glb.pwm_Air
					,io_status_st_glb.pwm_O2
					,io_status_st_glb.Flow_Air_1
					,io_status_st_glb.Flow_O2_1
				);
				break;
			case PNEUMATIC_CTRL_PID_SHUTDOWN:
				printf("\r\n%d,%d,Shutdown,%3d,%3d,"
					,sys_tic_rd()
					,app_Pneumatics.PID_mode
					,io_status_st_glb.pwm_Air
					,io_status_st_glb.pwm_O2
				);
				break;
			case PNEUMATIC_CTRL_PID_MATH:	//calculates the % of air and O2 mathematically
				printf("\r\n%d,%d,Normal (O2 ctrl),%d,%.2f,%.2f,%.2f,%.2f,%.4f,%.4f,%.2f,%.2f,%.2f,%.2f,%d,%d,%.2f,%.2f,%.2f"
					,sys_tic_rd()
					,app_Pneumatics.PID_mode
					,app_Pneumatics.O2_conc_target			//target
					,app_Pneumatics.O2_conc_actual			//actual
					,paracube_val
					,app_Pneumatics.pid_conc_output			//PID output
					,pid_vars[PID_CONC].error				//error
					,pid_vars[PID_CONC].pTerm
					,pid_vars[PID_CONC].iTerm
					,app_Pneumatics.flow_rate_air_math
					,io_status_st_glb.Flow_Air_1
					,app_Pneumatics.flow_rate_O2_math
					,io_status_st_glb.Flow_O2_1
					,io_status_st_glb.pwm_Air
					,io_status_st_glb.pwm_O2
					,io_status_st_glb.Flow_Air_Of
					,io_status_st_glb.Flow_O2_Of
					,io_status_st_glb.Temperature
				);
				break;
			case PNEUMATIC_CTRL_PID_VENTURI:	//not used
				printf("\r\n%d,%d,Venturi,%d,%.2f,%d,%.2f,%.2f,%.2f %.2f,%.2f"
					,sys_tic_rd()
					,app_Pneumatics.PID_mode
						,app_Pneumatics.O2_conc_target
						,app_Pneumatics.O2_conc_actual
						,app_Pneumatics.flow_rate_target
						,app_Pneumatics.flow_rate_actual
						,app_Pneumatics.pid_venturi_O2_output
						,pid_vars[PID_VENTURI_FLOW].pTerm
						,pid_vars[PID_VENTURI_FLOW].iTerm
						,pid_vars[PID_VENTURI_FLOW].dTerm
				);
				break;
			case PNEUMATIC_CTRL_TEST_AIR:	//controls using set proportional valve %
				printf("\r\n%d,%d,Flow (Test Air),%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
					,sys_tic_rd()
					,app_Pneumatics.PID_mode
						,app_Pneumatics.flow_rate_target
						,app_Pneumatics.flow_rate_actual
						,pid_vars[PID_AIR].error					//error
						,app_Pneumatics.pid_AIR_output
						,pid_vars[PID_AIR].pTerm
						,pid_vars[PID_AIR].iTerm
						,pid_vars[PID_AIR].dTerm
				);
				break;
			case PNEUMATIC_CTRL_TEST_O2:	//controls using set proportional valve %
				printf("\r\n%d,%d,O2 (Test O2),%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
					,sys_tic_rd()
					,app_Pneumatics.PID_mode
						,app_Pneumatics.O2_conc_target
						,app_Pneumatics.O2_conc_actual
						,pid_vars[PID_O2].error					//error
						,app_Pneumatics.pid_O2_output
						,pid_vars[PID_O2].pTerm
						,pid_vars[PID_O2].iTerm
						,pid_vars[PID_O2].dTerm
				);
				break;
			case PNEUMATIC_CTRL_RAW_O2:		//controls using rawinput flow ADC values
				printf("\r\n%d,%d,O2_RAW,%d,%d,%.0f,%.2f,%.2f,%.2f,%.2f"
					,sys_tic_rd()
					,app_Pneumatics.PID_mode
						,app_Pneumatics.flow_rate_O2_raw		//target
						,io_status_st_glb.ADC_Pressure_O2		//actual
						,pid_vars[PID_O2].error					//error
						,app_Pneumatics.pid_O2_output			//PID output
						,pid_vars[PID_O2].pTerm
						,pid_vars[PID_O2].iTerm
						,pid_vars[PID_O2].dTerm
				);
				break;
			case PNEUMATIC_CTRL_RAW_AIR:	//controls using rawinput flow ADC values
				printf("\r\n%d,%d,AIR_RAW,%d,%d,%.0f,%.2f,%.2f,%.2f,%.2f"
					,sys_tic_rd()
					,app_Pneumatics.PID_mode
						,app_Pneumatics.flow_rate_air_raw		//target
						,io_status_st_glb.ADC_Pressure_Air		//actual
						,pid_vars[PID_AIR].error
						,app_Pneumatics.pid_AIR_output			//PID output
						,pid_vars[PID_AIR].pTerm
						,pid_vars[PID_AIR].iTerm
						,pid_vars[PID_AIR].dTerm
				);
				break;
		}
	}
	return;
}



/*************************************************************************************************
* Function Name : 	app_pneumatics_pid_debug
* Description   : 	This Function enables/disables the debug output
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/11/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_pid_debug(uint8_t en)
{
	switch(en){
		default:
		case 0:		app_Pneumatics.pid_print_en		=  0x00;	break;
		case 1:		app_Pneumatics.pid_print_en		=  0x01;	break;
		case 2:		app_Pneumatics.pid_print_en		^= 0x01;	break;
	}
	printf("\r\nPneumatics_pid_print %d",app_Pneumatics.pid_print_en);

	return;
}

/*************************************************************************************************
* Function Name : 	app_pneumatics_o2_debug
* Description   : 	This Function enables/disables the debug output
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/11/17	W. Paul			Created
*************************************************************************************************/
void app_pneumatics_o2_debug(uint8_t en)
{
	switch(en){
		default:
		case 0:		app_Pneumatics.O2_print_en		=  0x00;	break;
		case 1:		app_Pneumatics.O2_print_en		=  0x01;	break;
		case 2:		app_Pneumatics.O2_print_en		^= 0x01;	break;
	}
	printf("\r\nPneumatics_O2_print %d",app_Pneumatics.O2_print_en);

	return;
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
