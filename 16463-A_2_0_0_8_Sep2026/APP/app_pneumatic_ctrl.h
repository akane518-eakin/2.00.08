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
 * Filename    :  app_pneumatic_ctrl.h
 * Date Created:  Tue 12 Sep 2017 07:54:51 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _APP_PNEUMATIC_CTRL_H
#define _APP_PNEUMATIC_CTRL_H

#include <stdint.h>
#include "app_patient_pressure.h"

#define	PNEUMATICS_VALID			0x50
#define Nebuliser_min_flow			10	//l/min

#define	PID_LO_THRESH				10.0
#define	PID_VLO_THRESH				2.0
#define	CONC_VLO_THRESH				30.0
#define	CONC_MID					60.0

#define	APP_PNEUMATICS_FREQ			150

#define	O2_CAL_IDLE					0
#define	O2_CAL_START				1
#define	O2_CAL_ERR_NO_GAS			10
#define	O2_CAL_ERR_NO_GAS_ATSTART	11
#define	O2_CAL_ERR_NO_SENSOR		12
#define	O2_CAL_FINISHED				20

typedef enum{
	O2_SENSOR_MAX250	=  0,
	O2_SENSOR_PARACUBE,
}APP_PNEUMATIC_O2_SENSOR_enum;

typedef enum{
	PNEUMATIC_CTRL_PID_IDLE,

	PNEUMATIC_CTRL_PID_MATH,
	PNEUMATIC_CTRL_PID_VENTURI,

	PNEUMATIC_CTRL_TEST_O2,
	PNEUMATIC_CTRL_TEST_AIR,

	PNEUMATIC_CTRL_RAW_O2,
	PNEUMATIC_CTRL_RAW_AIR,

	PNEUMATIC_CTRL_PID_SHUTDOWN,
}APP_PNEUMATIC_CTRL_PID_enum;


typedef enum{
	PMODE_CPAP			=  0,
	PMODE_CPAP_PAED		=  1,
	PMODE_CPAP_HELMET	=  2,
	PMODE_BUBBLE_PAP	=  3,
	PMODE_HFOT			=  4,
	PMODE_POINT			=  5,

	PMODE_COUNT,				//always have this at the bottom
}app_Pmode_enum;


typedef enum{
	AT_NORMAL_FLOW,
	READY_TO_RETURN_TO_NORMAL,
	AT_2_LMIN,
}AT_2_LMIN_enum;


typedef struct{
	uint8_t		O2_conc;
	uint8_t		flow_rate;
	pp_limit_t	Patient_Pressure_min;
	pp_limit_t	Patient_Pressure_max;
	int16_t		Apnoea_alarm_wait;
	int16_t		Respiration_rate_max;
}user_settings_t;

typedef struct{
	uint8_t			flow_min;
	uint8_t			flow_max;

	uint8_t			O2_min;
	uint8_t			O2_max;

	uint8_t			pressure_measured;
	uint8_t			breath_freq_measured;

	uint8_t			nebuliser_avail;

	uint8_t			pressure_alarm_avail;
	int8_t			pressure_alarm_min;
	int8_t			pressure_alarm_max;

	uint8_t			apnoea_alarm_avail;
	int8_t			apnoea_alarm_min;
	int8_t			apnoea_alarm_max;
}app_mode_limits_t;


typedef struct{
	uint8_t					valid;
	uint8_t					Cal_status;

	app_Pmode_enum			mode;					//user selects mode of operation

	app_mode_limits_t		limits;					//operating mode limits
	uint8_t					pid_print_en;
	uint8_t					O2_print_en;
	uint8_t					startup_o2_cal_status;

	uint8_t					nebuliser_state;	//wtp

	uint8_t					O2_conc_target;		//wtp
	uint8_t					flow_rate_target;	//wtp
	AT_2_LMIN_enum			plimit_at_2_lmin;
	AT_2_LMIN_enum			pmax_at_2_lmin;

	uint16_t				air_ctrl_timer[3];
	uint16_t				O2_ctrl_timer[3];

	float					O2_conc_actual;			//O2 concentration input to control
	float					flow_rate_actual;
	float					flow_rate_nebuliser;

	float					flow_rate_O2_math;		//calculated flow rates
	float					flow_rate_air_math;

	uint32_t				flow_rate_O2_raw;
	uint32_t				flow_rate_air_raw;

	float					pid_AIR_output;			//pid output
	float					pid_O2_output;			//pid vars
	float					pid_venturi_O2_output;	//pid vars
	float					pid_conc_output;		//pid vars

	//control vars
	uint8_t							ADC_rd_mode;	//0-10 switch state for reading all ADC @100Hz Output at 10Hz
	uint8_t							ADC_rd_mode_park;
	uint32_t						ADC_rd_cnt;

	APP_PNEUMATIC_CTRL_PID_enum		PID_mode;		//selects PID control mode
	uint8_t							pid_conc_en;

	uint8_t							O2_conc_min;	//normally 21%, but possible to have lower if air supply is stripped of O2
	uint8_t							O2_conc_max;	//normally 100%, but possible to have lower if O2 supply is not pure
	APP_PNEUMATIC_O2_SENSOR_enum	O2_sensor_sel;	//which O2 sensor are we using

	uint32_t						ctrl_tic;
	uint32_t						ctrl_tic_last;
	uint32_t						cal_timer;
	
	uint32_t						pid_timer;

	uint32_t						ADC_reset_cnt;
}app_Pneumatics_t;

typedef struct{
	uint8_t					valid;
	float					air_m;
	float					air_c;
	float					O2_m;
	float					O2_c;
}app_Pneumatics_caltemp_t;


/*********************************************************************************************************
 *		Global Variables
 ********************************************************************************************************/
extern	app_Pneumatics_t			app_Pneumatics;
extern	app_Pneumatics_caltemp_t	app_Pneumatics_caltemp;
extern	float						paracube_val;


/*********************************************************************************************************
 *		Global Functions
 ********************************************************************************************************/
void 	app_pneumatic_manager(void);

void							app_pneumatic_pid_mode_set(APP_PNEUMATIC_CTRL_PID_enum mode);
APP_PNEUMATIC_CTRL_PID_enum 	app_pneumatic_pid_mode_read(void);

uint8_t	app_pneumatic_set_mode(app_Pmode_enum mode, uint8_t startup_init);

void	app_pneumatics_parameters_rd(void);
void 	app_pneumatics_parameters_wr(void);
void 	app_pneumatics_O2_sensor_print(void);
void 	app_pneumatics_O2_min_print(void);
void 	app_pneumatics_O2_max_print(void);

void 	app_pneumatic_set_flow(uint16_t flow_rate, uint16_t O2_conc);
void 	app_pneumatic_cal_flows(float conc_target);
void	app_pneumatic_set_nebuliser(uint8_t neb_on_off);
void 	app_pneumatics_Neb_flow_min_print(void);
void 	app_pneumatics_nebuliser_corrector(void);

void 	app_pneumatics_ctrl_irq(void);


void 	app_pneumatics_IRQ_en_dis(uint8_t en_dis);

void 	app_pneumatic_print_O2(void);
void	app_pneumatic_print_pid(void);

void 	app_pneumatics_pid_debug(uint8_t en);
void 	app_pneumatics_o2_debug(uint8_t en);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
