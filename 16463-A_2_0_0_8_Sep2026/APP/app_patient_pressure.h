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
 * Filename    :  app_patient_pressure.h
 * Date Created:  Tue 10 Oct 2017 11:08:55 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _APP_PATIENT_PRESSURE_H
#define _APP_PATIENT_PRESSURE_H

#include <stdint.h>

#define	PP_HIS_LEN				355		//no of points on graph
#define	PP_AV_LEN				350		//no of points that PP av is calculated		 must be <= PP_HIS_LEN
										//350 = 7seconds

#define	MAX_RR					80		//199
#define	MAX_RR_PERIOD			15		//15	50/15	= 3.33Hz	= 200 Breaths/min	
										//20	50/20	= 2.5 Hz	= 150 Breaths/min	
										//25	50/25	= 2.0 Hz	= 120 Breaths/min	
										//30	50/30	= 1.66Hz	= 100 Breaths/min	
										//35	50/35	= 1.43Hz	=  86 Breaths/min	

#define	PP_MAX_Pressure			25
#define	PP_MIN_Pressure			-10

#define	BREATH_IN		0
#define	BREATH_OUT		1

/*********************************************************************************************************
 ********************************************************************************************************/
#define	PP_Settings_Init		0x15

typedef struct{
	uint16_t	settle;
	
	uint32_t	len;		//samples 50/second
	uint32_t	last_len;	//samples 50/second
	uint32_t	len_s;
	float		edge_a;
	float		edge_b;
	float		edge_b1;
	uint32_t	breath_min_len;
	float		value;
	float		value_av;
	uint8_t		dis_val;
	uint8_t		detect_flag;

    float		Pmax;
	float		Pmin;
    float		PSigSz;
}Breathing_t;


typedef struct{
	uint8_t		event_alarm_no_breathing_detected;	//if no breath detected after 12 seconds set flag
	uint8_t		event_settle_status;				//when therapy start set to 0..
													//when standard settle period is over set to 1
													//when extended setle period is over set to 2
	uint32_t	event_quiet_timer;		
	uint8_t		alarm_flag;

}Apnoea_t;

typedef struct{
	float		his[PP_HIS_LEN];
	uint16_t	ptr_in;
	uint16_t	ptr_avPP;
	uint16_t	ptr_print;

	uint8_t		graph_pix[PP_HIS_LEN];
	uint8_t		on_LCD_pix[PP_HIS_LEN];
	float		graph_min;
	float		graph_max;
	uint16_t	pix_range;
	float		scale;
	uint16_t	graph_ptr_out;

	Breathing_t	BreathRate;
	uint16_t	BreathPmax_cnt;
	uint16_t	BreathPmin_cnt;

	Apnoea_t	Apnoea;

	float		PatientPressureTot;
	uint16_t	PatientPressureTot_cnt;
	int16_t		PatientPressureAv_x10;
	int16_t		PatientPressure_Raw_x10;

}PP_data_st;

typedef enum{
	DIS		= 0,
	EN		= 1,
}EN_DIS_t;

typedef struct{
	EN_DIS_t	en_dis;
	int16_t		value;
}pp_limit_t;

typedef struct{
	uint8_t		init;
	uint8_t		debug;
	pp_limit_t	Patient_Pressure_min;
	pp_limit_t	Patient_Pressure_max;
	int16_t		Apnoea_alarm_wait;
	int16_t		Respiration_rate_max;

}PP_settings_st;



extern 	PP_data_st		PP_data;
extern	PP_settings_st	PP_settings;

/*********************************************************************************************************
 *		Global Functions
 ********************************************************************************************************/
void 		app_PP_manager(void);

void		app_PP_data_input(float pressure);
void		app_PP_data_init(uint8_t mode,	uint8_t	pix_range, float graph_max, float graph_min);
uint8_t		app_PP_graph_start_refresh(void);
uint8_t		app_PP_data_graph_get(uint16_t x,uint16_t *last_pix,uint16_t *new_pix);
uint16_t	app_PP_data_graph_scale(float val);
void 		app_PP_data_max_min(void);

void 		app_PP_start_treatment(void);
void 		app_PP_stop_treatment(void);

void		app_PP_set_alarm_patient_pressure(EN_DIS_t Pmin_en_dis, int16_t Pmin_value, EN_DIS_t Pmax_en_dis, int16_t Pmax_value);
void		app_PP_set_alarm_apnoea(int16_t val);
void		app_PP_set_alarm_resperation_rate(int16_t val);

void 		app_PP_settleAlarm_start(void);
void 		app_PP_settleAlarm_end(void);

void 		app_PP_1ms_IRQ(void);

void 		app_PP_debug(uint8_t en);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
