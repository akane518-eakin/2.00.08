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
 * Filename    :  api_calibrate.c
 * Date Created:  Tue 12 Sep 2017 02:32:09 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "api_IO.h"

#include "api_calibrate.h"
#include "csp_STM32_delay.h"
#include "csp_S25FL0xx.h"



API_CALIBRATION_t		calibration[NO_DEVICES_TO_CALIBRATE];
API_PWM_CALIBRATION_t	pwm_calibration;
float					cal_temperature_air;
float					cal_temperature_O2;


/*************************************************************************************************
* Function Name : 	api_calibrate_cmd
* Description   : 	This Function commands the calibration process
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_calibrate_cmd(cal_devices_enum device,calib_enum action)
{
	uint8_t		result	=  0;
	float		ftemp;
	uint8_t		i;
	uint32_t	adc_sum_air;
	uint32_t	adc_sum_O2;
	uint32_t	adc_sum_conc;
	uint32_t	adc_sum_pres;

	switch(action){
		case CAL_INIT:
			//hard coded limits for the gradient of the 3 calibration curves
			api_calibrate_config(CAL_FLOW_O2_a,	 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_O2_b,	 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_O2_c,	 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_O2_d,	 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_O2_e,	 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_AIR_a, 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_AIR_b, 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_AIR_c, 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_AIR_d, 0.0,	-0.01);
			api_calibrate_config(CAL_FLOW_AIR_e, 0.0,	-0.01);
			api_calibrate_config(CAL_SENSOR_O2,	 0.03	, -0.03);
			api_calibrate_config(CAL_SENSOR_PP,	 0.005, 0.000001);	// ??

			calibration[device].m		=  1;
			calibration[device].c		=  0;
			break;
		case CAL_POINT_LOW:
		case CAL_POINT_HIGH:
			adc_sum_air		=  0;
			adc_sum_O2		=  0;
			adc_sum_conc	=  0;
			adc_sum_pres	=  0;
			for(i=0;i<20;i++){
				Delay(10);
				adc_sum_air		+= io_status_st_glb.ADC_Pressure_Air;
				adc_sum_O2		+= io_status_st_glb.ADC_Pressure_O2;
				adc_sum_conc	+= io_status_st_glb.ADC_O2_Sensor;
				adc_sum_pres	+= io_status_st_glb.ADC_patientP;
			}
			adc_sum_air		/=  20;
			adc_sum_O2		/=  20;
			adc_sum_conc	/=  20;
			adc_sum_pres	/=  20;
			switch(device){
				case CAL_FLOW_O2_a:		calibration[device].adc[action-1]	=  adc_sum_O2;		break;
				case CAL_FLOW_O2_b:		calibration[device].adc[action-1]	=  adc_sum_O2;		break;
				case CAL_FLOW_O2_c:		calibration[device].adc[action-1]	=  adc_sum_O2;		break;
				case CAL_FLOW_O2_d:		calibration[device].adc[action-1]	=  adc_sum_O2;		break;
				case CAL_FLOW_O2_e:		calibration[device].adc[action-1]	=  adc_sum_O2;		break;
				case CAL_FLOW_AIR_a:	calibration[device].adc[action-1]	=  adc_sum_air;		break;
				case CAL_FLOW_AIR_b:	calibration[device].adc[action-1]	=  adc_sum_air;		break;
				case CAL_FLOW_AIR_c:	calibration[device].adc[action-1]	=  adc_sum_air;		break;
				case CAL_FLOW_AIR_d:	calibration[device].adc[action-1]	=  adc_sum_air;		break;
				case CAL_FLOW_AIR_e:	calibration[device].adc[action-1]	=  adc_sum_air;		break;
				case CAL_SENSOR_O2:		calibration[device].adc[action-1]	=  adc_sum_conc;	break;
				case CAL_SENSOR_PP:		calibration[device].adc[action-1]	=  adc_sum_pres;	break;
			}
			break;
		case CAL_CALCULATE:
			ftemp							=  calibration[device].adc[1];
			ftemp							-= calibration[device].adc[0];
			calibration[device].m			=  calibration[device].calpt[1];
			calibration[device].m			-= calibration[device].calpt[0];
			calibration[device].m			/= ftemp;

			calibration[device].c			=  calibration[device].calpt[0];
			calibration[device].c			-= (calibration[device].m * calibration[device].adc[0] );

			calibration[device].valid			=  CAL_VALID;
			calibration[device].calib_status	=  CALIB_FAILED;		//will be updated after the read function below

			printf("\r\n Calibration Points = %3.0f %3.0f"	,calibration[device].calpt[0]
															,calibration[device].calpt[1]	);
			printf("\r\n Calibration ADC    = %8.1f %8.1f"	,calibration[device].adc[0]
															,calibration[device].adc[1]	);
			printf("\r\nCalibration m check  %.3f<%.7f<%.3f"	,calibration[device].m_min
																,calibration[device].m
																,calibration[device].m_max	);
			printf("\r\nCalibration c check  %.4f"				,calibration[device].c		);
			if(	(calibration[device].m <= calibration[device].m_max) &&
				(calibration[device].m >= calibration[device].m_min) 		)
			{
				csp_sys_mem_wr((uint8_t*)(&calibration[device].valid),	MEM_ADD_CALIBATION + (device*sizeof(API_CALIBRATION_t)), sizeof(API_CALIBRATION_t));
				printf("\r\nCalibration saved");
			}
			else{
				result	=  1;
				csp_mem_rd((uint8_t*)(&calibration[device].valid),	MEM_ADD_CALIBATION + (device*sizeof(API_CALIBRATION_t)), sizeof(API_CALIBRATION_t));
				printf("\r\nCalibration NOT saved!!!");
			}
		//	break;	//run straight to CAL_FROM_MEM
		case CAL_FROM_MEM:
			csp_mem_rd((uint8_t*)(&calibration[device].valid),	MEM_ADD_CALIBATION + (device*sizeof(API_CALIBRATION_t)), sizeof(API_CALIBRATION_t));

			calibration[device].calib_status	=  CALIB_FAILED;
			if(	(	isfinite(calibration[device].m) )	&&
				(	isfinite(calibration[device].c)	)		)
			{
				calibration[device].calib_status	=  CALIB_SUCCESS;
			}


			if(	(calibration[device].valid 			!= CAL_VALID)	||
				(calibration[device].calib_status	!= CALIB_SUCCESS)		)
			{
				printf("\r\n** Device %d Not Calibrated! Default Values! **",device);
				calibration[device].m			=  (4.096/0xFFFFFF);
				calibration[device].m			*= 10;
				calibration[device].c			=  0.0;
				calibration[device].valid		=  CAL_DEFAULT;

				sprintf((char*)calibration[CAL_FLOW_O2_a].name,		"CAL_FLOW_O2_a "	);
				sprintf((char*)calibration[CAL_FLOW_O2_b].name,		"CAL_FLOW_O2_b "	);
				sprintf((char*)calibration[CAL_FLOW_O2_c].name,		"CAL_FLOW_O2_c "	);
				sprintf((char*)calibration[CAL_FLOW_O2_d].name,		"CAL_FLOW_O2_d "	);
				sprintf((char*)calibration[CAL_FLOW_O2_e].name,		"CAL_FLOW_O2_e "	);
				sprintf((char*)calibration[CAL_FLOW_AIR_a].name,	"CAL_FLOW_AIR_a"	);
				sprintf((char*)calibration[CAL_FLOW_AIR_b].name,	"CAL_FLOW_AIR_b"	);
				sprintf((char*)calibration[CAL_FLOW_AIR_c].name,	"CAL_FLOW_AIR_c"	);
				sprintf((char*)calibration[CAL_FLOW_AIR_d].name,	"CAL_FLOW_AIR_d"	);
				sprintf((char*)calibration[CAL_FLOW_AIR_e].name,	"CAL_FLOW_AIR_e"	);
				sprintf((char*)calibration[CAL_SENSOR_O2].name,		"CAL_SENSOR_O2 "	);
				sprintf((char*)calibration[CAL_SENSOR_PP].name,		"CAL_SENSOR_PP "	);

				csp_sys_mem_wr((uint8_t*)(&calibration[device].valid),	MEM_ADD_CALIBATION + (device*sizeof(API_CALIBRATION_t)), sizeof(API_CALIBRATION_t));
			}
			
			csp_mem_rd((uint8_t*)(&cal_temperature_air),	MEM_ADD_CALIBATION + 0x400, sizeof(float));
			csp_mem_rd((uint8_t*)(&cal_temperature_O2),		MEM_ADD_CALIBATION + 0x404, sizeof(float));
			break;
		case CAL_PRINT:
			printf("\r\nCalibration Vars   %s"				,calibration[device].name);
		//	printf("\r\n Mem Valid %2x (%2x)"				,calibration[device].valid,CAL_VALID	);
			printf("\r\n Calibration Points = %f %f"		,calibration[device].calpt[0]
															,calibration[device].calpt[1]	);
			printf("\r\n Calibration ADC    = %8.1f %8.1f"	,calibration[device].adc[0]
															,calibration[device].adc[1]	);
			printf("\r\n Output = %f ADC + (%f)"			,calibration[device].m
															,calibration[device].c	);
		//	printf("\r\n Status %2x (%2x)"					,calibration[device].calib_status,CALIB_SUCCESS	);
			printf("\r\n");
			break;

	}

	return(result);
}

/*************************************************************************************************
* Function Name : 	api_calibrate_config
* Description   : 	This Function configures the calibration acceptace criteria
* Arguments     : 	cal_devices_enum 	device
*					float 				m_max
*					float 				m_min
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
void api_calibrate_config(cal_devices_enum device,float m_max,float m_min)
{
	calibration[device].m_max	= m_max;
	calibration[device].m_min	= m_min;

	return;
}

/*************************************************************************************************
* Function Name : 	api_calibrate_set_points
* Description   : 	This Function configures the calibrated hi and low points
* Arguments     : 	cal_devices_enum 	device
*					float 				m_max
*					float 				m_min
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
void api_calibrate_set_points(cal_devices_enum device,float lo,float hi)
{
	calibration[device].calpt[1]	=  hi;
	calibration[device].calpt[0]	=  lo;
	return;
}


/*
*********************************************************************************************************
* Function Name : api_calibrate_pwm_rd
* Description   : This function is used for retrieving minimum PWM values for air and O2
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			30/07/2020			Tim Barr		Original Created
*********************************************************************************************************
*/
void api_calibrate_pwm_rd(void)
{
	// Local Variables
	
	// Code
	memcpy((uint8_t*)&pwm_calibration, (uint32_t*)MEM_ADD_PWM_CALIBATION, sizeof(API_PWM_CALIBRATION_t));
	
	if(pwm_calibration.valid			!= CAL_VALID){
		pwm_calibration.valid			=  CAL_VALID;
		pwm_calibration.pwm_air			=  3000;
		pwm_calibration.pwm_O2			=  3000;
		pwm_calibration.calib_status	=  CALIB_FAILED;
		api_calibrate_pwm_wr();
	}
	
	return;
}


/*
*********************************************************************************************************
* Function Name : api_calibrate_pwm_wr
* Description   : This function is used for saving minimum PWM values for air and O2
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			30/07/2020			Tim Barr		Original Created
*********************************************************************************************************
*/
void api_calibrate_pwm_wr(void)
{
	// Local Variables
	uint8_t			i;
	uint16_t		*wr_prt		=  (uint16_t*)&pwm_calibration;
	
	// Code
	FLASH_Unlock();
	FLASH_ErasePage(MEM_ADD_PWM_CALIBATION);
	for(i=0;i<sizeof(API_PWM_CALIBRATION_t);i+=2){
		FLASH_ProgramHalfWord((MEM_ADD_PWM_CALIBATION + i), *wr_prt);
		wr_prt++;
	}
	FLASH_Lock();
	
	return;
}


/*
*********************************************************************************************************
*						End of api_calibrate.c
*********************************************************************************************************
*/
