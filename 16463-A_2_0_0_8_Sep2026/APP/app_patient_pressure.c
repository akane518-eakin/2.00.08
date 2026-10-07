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
 * Filename    :  app_patient_pressure.c
 * Date Created:  Tue 10 Oct 2017 11:08:29 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "app_patient_pressure.h"
#include "app_pneumatic_ctrl.h"
#include "alg_av.h"
#include "csp_STM32_delay.h"

#include "alg_butterworth.h"
#include "api_IO.h"


PP_data_st		PP_data;
PP_settings_st	PP_settings;


/*************************************************************************************************
* Function Name : 	app_PP_manager
* Description   : 	This Function is the central control of the patient pressure detect
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/09/17	W. Paul			Created
*************************************************************************************************/
void app_PP_manager(void)
{
	if(PP_settings.init != PP_Settings_Init){
		PP_settings.init	=  PP_Settings_Init;

	//	app_PP_set_alarm_patient_pressure(	EN, 0,
	//										EN, PP_MAX_Pressure);
	//	app_PP_set_alarm_apnoea(0);				//Off
	//	app_PP_set_alarm_resperation_rate(0);	//Off

		memset(PP_data.his,0,sizeof(PP_data.his));
		PP_settings.debug		=  0;
		PP_data.ptr_in			=  0;
		PP_data.ptr_avPP		=  PP_HIS_LEN - PP_AV_LEN;
		PP_data.ptr_print		=  0;

		PP_data.BreathRate.settle			=  50*6;	//6second settle period
		PP_data.BreathRate.len				=  0;
		PP_data.BreathRate.last_len			=  6000;

		PP_data.BreathRate.edge_a			=  0;
   		PP_data.BreathRate.edge_b			=  0;
        PP_data.BreathRate.edge_b1			=  0;
        PP_data.BreathRate.breath_min_len	=  10;
		PP_data.BreathRate.value			=  0;
		PP_data.BreathRate.value_av			=  0;
		PP_data.BreathRate.detect_flag		=  BREATH_IN;

		PP_data.BreathPmax_cnt				=  0;
		PP_data.BreathPmin_cnt				=  0;

		PP_data.PatientPressureTot_cnt		=  0;
		PP_data.PatientPressureTot			=  0.0;


		app_PP_stop_treatment();
	}

	if(PP_settings.debug){
		if(PP_data.ptr_print != PP_data.ptr_in){
//			printf("\r\n%d %.4f %.4f %.1f %.1f %d %d %d %.1f"
//				,PP_data.ptr_print
//				,PP_data.his[PP_data.ptr_print]
//				,PP_data.BreathRate.edge_b
//				,PP_data.BreathRate.value
//				,PP_data.BreathRate.value_av
//                ,PP_data.BreathRate.breath_min_len
//                ,PP_data.BreathRate.len
//                ,PP_data.BreathRate.detect_flag
//				,PP_data.BreathRate.PSigSz
//			);
			printf("\r\n%03d %.3f %04d %d %03.3f %03.1f %03.1f %03d %03d %03d %d %d"
				,PP_data.ptr_print
				,PP_data.his[PP_data.ptr_print]
				,PP_data.BreathRate.len
                ,PP_data.BreathRate.detect_flag
				,PP_data.BreathRate.PSigSz
				,PP_data.BreathRate.value
				,PP_data.BreathRate.value_av
				,PP_data.Apnoea.event_quiet_timer/1000
				,PP_settings.Apnoea_alarm_wait
				,PP_data.BreathRate.settle
				,PP_data.Apnoea.event_settle_status
				,PP_data.Apnoea.alarm_flag

			);
			if(++PP_data.ptr_print >= PP_HIS_LEN){
				PP_data.ptr_print	=  0;
			}
		}
	}
	return;
}


/*************************************************************************************************
* Function Name : 	app_PP_data_input
* Description   : 	This Function is the input data point to this module of work
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		10/10/17	W. Paul			Created
*************************************************************************************************/
void app_PP_data_input(float pressure)
{
	//50 Hz data capture
	float			av10;
	float			trigger;
	float			PPav;
	uint8_t			new_breath_event	= 0;

	//don't detect breathing rate / Apnoea if
	//	in these modes (Bubble pap, HFOT, Point		OR
	// 	no air & no O2 supply						OR
	//  not in a therapy mode
	if(	( (api_io_rd_AirSupply(0) == 0 ) && (api_io_rd_O2Supply(0) == 0) ) ||
		(app_Pneumatics.mode == PMODE_HFOT) ||
		(app_Pneumatics.mode == PMODE_POINT) ||
		(app_pneumatic_pid_mode_read() != PNEUMATIC_CTRL_PID_MATH) )
	{
		pressure = 0.0;
	}

	
	//clip data
	if(pressure >  40){pressure =   40;	}
	if(pressure < -40){pressure =  -40;	}

	//smooth input raw data
	av10	=  alg_av_f(PP_AV_1,pressure);	//av10
	PP_data.PatientPressure_Raw_x10	=  (int16_t)(av10 *10);

	//clip data
	if(av10 > PP_MAX_Pressure){av10 =  PP_MAX_Pressure;	}
	if(av10 < PP_MIN_Pressure){av10 =  PP_MIN_Pressure;	}

	//store averaged data
	PP_data.his[PP_data.ptr_in]	=  av10;
	if(++PP_data.ptr_in 	>= PP_HIS_LEN){	PP_data.ptr_in	 =  0;	}

	//find average patient pressure
	PP_data.PatientPressureTot	-= PP_data.his[PP_data.ptr_avPP];
	PP_data.PatientPressureTot	+= av10;
	if(++PP_data.PatientPressureTot_cnt >= PP_AV_LEN){		PP_data.PatientPressureTot_cnt	=  PP_AV_LEN;	}
	PPav	=  PP_data.PatientPressureTot;
	PPav	/= PP_data.PatientPressureTot_cnt;
	PPav	*= 10;
	PP_data.PatientPressureAv_x10	=  (int16_t)(PPav);
	if(++PP_data.ptr_avPP	>= PP_HIS_LEN){	PP_data.ptr_avPP =  0;	}

	//find max and min pressures last wave
	if(app_Pneumatics.mode	== PMODE_BUBBLE_PAP){
		av10				=  0.0;
	}
	if(av10 > PP_data.BreathRate.Pmax){	PP_data.BreathRate.Pmax	=  av10;	 }
	if(av10 < PP_data.BreathRate.Pmin){	PP_data.BreathRate.Pmin	=  av10;	}
	if(PP_data.BreathRate.Pmax > PP_data.BreathRate.Pmin){
		PP_data.BreathRate.Pmax	-= 0.0001;
		PP_data.BreathRate.Pmin	+= 0.0001;
	}
	PP_data.BreathRate.PSigSz	=  PP_data.BreathRate.Pmax;
	PP_data.BreathRate.PSigSz	-= PP_data.BreathRate.Pmin;

	trigger	=  av10;
	trigger	+= 30;		//so we never have values < 1
	trigger	*= 10;		//make value larger

	trigger	*= trigger;	//power 2 to make zero crossing more extreem
	//bandpass the data to remove noise and DC offset
	PP_data.BreathRate.edge_a	= alg_butterworth_bp(trigger);
	//av this data by 10
	PP_data.BreathRate.edge_b	= alg_av_f(PP_DBDT_AV,PP_data.BreathRate.edge_a);

	if(PP_data.BreathRate.settle){
		PP_data.BreathRate.settle -= 1;

		//reset all breathing detect parmas here
		PP_data.BreathRate.value		=  0;
		new_breath_event 				=  2;	//this sorts the av value
		PP_data.BreathRate.len			=  6000;
		PP_data.BreathRate.last_len		=  6000;
		PP_data.BreathRate.Pmax			=  av10;
		PP_data.BreathRate.Pmin			=  av10;
		PP_data.BreathRate.PSigSz		=  0;
	}
	else{
		//find crossing points and
		//cal breathing rate
		if(PP_data.BreathRate.len <= (120*50)){	//120seconds
			PP_data.BreathRate.len++;	//50hz	increment
		}
		if(	(PP_data.BreathRate.edge_b	< 0.0)&&
			(PP_data.BreathRate.edge_b1 > 0.0)&&
			(PP_data.BreathRate.PSigSz 	> 1.0)&&
		//	(PP_data.BreathRate.len 	>  PP_data.BreathRate.breath_min_len)	)	//ignore any edges within 0.2seconds	(max breathing rate should be 240/min or 4Hz or 0.25sec between edges)
			(PP_data.BreathRate.len 	>  MAX_RR_PERIOD)	)	//ignore any edges within 0.3seconds	(max breathing rate should be 200/min or 3.33Hz or 0.3sec between edges)
		{
	//		PP_data.BreathRate.breath_min_len	=  (uint32_t)(PP_data.BreathRate.len * 0.7);
	//		if(PP_data.BreathRate.breath_min_len < 10){	PP_data.BreathRate.breath_min_len	=  10;	}	//10- 0.2s	Breath rate 300 /min
	//		if(PP_data.BreathRate.breath_min_len > 50){	PP_data.BreathRate.breath_min_len	=  50;	}	//50- 1s	Breath rate  60 /min

			if(PP_data.BreathRate.last_len != 6000){
				PP_data.BreathRate.value		=  60*50;					//	samples / minute
				PP_data.BreathRate.value		/= PP_data.BreathRate.len;	//  calculates Breathing rate/min

				PP_data.BreathRate.detect_flag	=  BREATH_IN;
			}
			new_breath_event				=  1;
			PP_data.BreathRate.last_len		=  PP_data.BreathRate.len;
			PP_data.BreathRate.len			=  0;


			PP_data.BreathRate.Pmax			=  av10;
			PP_data.BreathRate.Pmin			=  av10;
		}
		else if(	(PP_data.BreathRate.edge_b	> 0.0)&&
					(PP_data.BreathRate.edge_b1 < 0.0)	)
		{
			PP_data.BreathRate.detect_flag	=  BREATH_OUT;
		}
		else if(PP_data.BreathRate.len >= (60*50) ){	//60seconds
			PP_data.BreathRate.value		=  0;

			new_breath_event 				=  2;
		}
		else if(PP_data.BreathRate.len 	 > (10*50) )//10seconds		ie less than 6Breaths/min
		{
			PP_data.BreathRate.value		=  60*50;					//	samples / minute
			PP_data.BreathRate.value		/= PP_data.BreathRate.len;	//  calculates Breathing rate/min

	//		new_breath_event 				=  2;
			if(PP_data.BreathRate.value < PP_data.BreathRate.value_av){	//stops an anomoly in the data
				new_breath_event			=  1;
			}

		}
		else if(PP_data.BreathRate.len > PP_data.BreathRate.last_len ){	//breathing rate is less than before so start to calculate
			PP_data.BreathRate.value		=  60*50;					//	samples / minute
			PP_data.BreathRate.value		/= PP_data.BreathRate.len;	//  calculates Breathing rate/min

			if(PP_data.BreathRate.value < PP_data.BreathRate.value_av){	//stops an anomoly in the data
				new_breath_event			=  1;
			}
		}
	}

	if(PP_data.BreathRate.value > MAX_RR){
		PP_data.BreathRate.value		= MAX_RR;
	}
	if(new_breath_event == 1){
		PP_data.BreathRate.value_av		=  alg_av_f(PP_BR_AV,PP_data.BreathRate.value);		//av3
	}
	if(new_breath_event == 2){
		//reset av, ready for new genuine breathing rate
		PP_data.BreathRate.value_av		=  PP_data.BreathRate.value;
		alg_av_f_init(PP_BR_AV);
	}


	PP_data.BreathRate.dis_val	=  (uint8_t)PP_data.BreathRate.value_av;
	if(PP_data.BreathRate.dis_val > MAX_RR){
		PP_data.BreathRate.dis_val	= MAX_RR;
	}

	PP_data.BreathRate.len_s	=  PP_data.BreathRate.len / 50;

	if(( PP_data.Apnoea.event_quiet_timer == 1)&&(PP_data.Apnoea.event_settle_status	==1)){
		PP_data.Apnoea.event_quiet_timer	= 0;
		PP_data.Apnoea.event_settle_status	= 2;
	}
	
	//Apnoea Alarm
	if(PP_data.BreathRate.len > (50*12) ){	//12 second window of no breaths -  activate alarm
		PP_data.Apnoea.event_alarm_no_breathing_detected	=  1;
		if(PP_data.Apnoea.event_settle_status == 2){
			PP_data.Apnoea.alarm_flag	=  1;	
		}
	}
	else{
		PP_data.Apnoea.event_alarm_no_breathing_detected	=  0;
		PP_data.Apnoea.alarm_flag	=  0;	
	}

	PP_data.BreathRate.edge_b1		=  PP_data.BreathRate.edge_b;

	return;
}

/*************************************************************************************************
* Function Name : 	app_PP_start_treatment
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/12/18	W. Paul			Created
*************************************************************************************************/
void app_PP_start_treatment(void)
{
	PP_data.Apnoea.event_alarm_no_breathing_detected	=  0;
	PP_data.Apnoea.event_settle_status					=  0;
	PP_data.Apnoea.event_quiet_timer					=  0;
	PP_data.Apnoea.alarm_flag							=  0;
	PP_data.BreathRate.settle			=  50*5;	//5second settle period (so there are no early peaks detected as system settles)

	return;
}

/*************************************************************************************************
* Function Name : 	app_PP_stop_treatment
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/12/18	W. Paul			Created
*************************************************************************************************/
void app_PP_stop_treatment(void)
{
	PP_data.Apnoea.event_quiet_timer					=  0;
	PP_data.Apnoea.event_alarm_no_breathing_detected	=  0;
	PP_data.Apnoea.event_settle_status					=  0;
	PP_data.Apnoea.alarm_flag							=  0;
	return;
}


/*************************************************************************************************
* Function Name : 	app_PP_data_init
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/10/17	W. Paul			Created
*************************************************************************************************/
void app_PP_data_init(uint8_t mode,	uint8_t	pix_range, float graph_max, float graph_min)
{
	//mode  blank all previous data
	//		recalculate all previous data
	PP_data.graph_min	=  graph_min;
	PP_data.graph_max	=  graph_max;
	PP_data.pix_range	=  pix_range;

	PP_data.scale	=  PP_data.pix_range;
	PP_data.scale	/= ( PP_data.graph_max - PP_data.graph_min);

	return;
}


/*************************************************************************************************
* Function Name : 	app_PP_graph_start_refresh
* Description   : 	This Function captures the current data in pointer.   If there is new data to be
*					displayed a 1 is returned else 0
* Arguments     : 	void
* Returns       : 	uint8_t		new data avail = 1
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/10/17	W. Paul			Created
*************************************************************************************************/
uint8_t app_PP_graph_start_refresh(void)
{
	if(PP_data.graph_ptr_out !=  PP_data.ptr_in){
		PP_data.graph_ptr_out	=  PP_data.ptr_in;
		return(1);
	}
	return(0);
}

/*************************************************************************************************
* Function Name : 	app_PP_data_graph_get
* Description   : 	This Function returnes where the last y point for this x was located and where the next y goes
* Arguments     : 	uint16_t	x			x location on graph
*					uint8_t		*last_pix	last y location
*					uint8_t		*new_pix	new y location
* Returns       : 	uint8_t		pix location has changed if 1,   0 no change
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/10/17	W. Paul			Created
*************************************************************************************************/
#define		GRAPH_AV_HIS_LEN	4
uint8_t app_PP_data_graph_get(uint16_t x,uint16_t *last_pix,uint16_t *new_pix)
{
	static float 		av_his[GRAPH_AV_HIS_LEN];
	static uint8_t		av_pos;
	static float		av_tot;
	static uint8_t		av_cnt;
	float				val;
	uint16_t			loc;


	//reset roling average
	if(x == 0){
		memset(av_his,0,sizeof(av_his));
		av_pos	=  0;
		av_tot	=  0;
		av_cnt	=  0;
	}


	*last_pix	=  PP_data.on_LCD_pix[x];

	//find next data location
	loc			=  x;
	loc			+= PP_data.graph_ptr_out;
	if(loc >= PP_HIS_LEN){				loc	-= PP_HIS_LEN;				}

	//roling average
	av_tot	-= av_his[av_pos];
	av_his[av_pos]	=  PP_data.his[loc];
	av_tot	+= av_his[av_pos];
	if(++av_pos >= GRAPH_AV_HIS_LEN){	av_pos	=  0;					}
	if(++av_cnt >= GRAPH_AV_HIS_LEN){	av_cnt	=  GRAPH_AV_HIS_LEN;	}
	val			=  av_tot / av_cnt;


	*new_pix	=  app_PP_data_graph_scale(val);

	//save point for erase
	PP_data.on_LCD_pix[x]	=  *new_pix;

	//change in pix location
	if(*last_pix == *new_pix){	return(0);	}
	else{						return(1);	}
}


/*************************************************************************************************
* Function Name : 	app_PP_data_graph_scale
* Description   : 	This Function scales the raw data for the graph
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		20/12/17	W. Paul			Created
*************************************************************************************************/
uint16_t app_PP_data_graph_scale(float val)
{
	uint16_t		output;
	//check limits
	if(val < PP_data.graph_min){	val = PP_data.graph_min;	}
	if(val > PP_data.graph_max){	val = PP_data.graph_max;	}

	output	=  (uint16_t)((val-PP_data.graph_min)*PP_data.scale);
	if(output > PP_data.pix_range){	output	= PP_data.pix_range;	}

	return(output);
}

/*************************************************************************************************
* Function Name : 	app_PP_data_max_min
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/10/17	W. Paul			Created
*************************************************************************************************/
void app_PP_data_max_min(void)
{
	uint16_t			x;
	float				max;
	float				min;

	//find max and min
	max	= PP_MIN_Pressure;
	min	= PP_MAX_Pressure;
	for(x=0;x<PP_HIS_LEN;x++){
		if(PP_data.his[x] > max){	max = PP_data.his[x];	}
		if(PP_data.his[x] < min){	min = PP_data.his[x];	}
	}

	app_PP_data_init(1,PP_data.pix_range, max, min);


	return;
}


/*************************************************************************************************
* Function Name :	app_PP_set_alarm_patient_pressure
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/10/17	W. Paul			Created
*************************************************************************************************/
void app_PP_set_alarm_patient_pressure( EN_DIS_t Pmin_en_dis, int16_t Pmin_value, EN_DIS_t Pmax_en_dis, int16_t Pmax_value)
{
//	if(Pmin == -5){	Pmin	= -10;	}
//	if(Pmax == -5){	Pmax	= 25;	}

	PP_settings.Patient_Pressure_min.en_dis		=  Pmin_en_dis;
	PP_settings.Patient_Pressure_min.value		=  Pmin_value;
	PP_settings.Patient_Pressure_max.en_dis		=  Pmax_en_dis;
	PP_settings.Patient_Pressure_max.value		=  Pmax_value;
	return;
}

/*************************************************************************************************
* Function Name : 	app_PP_set_alarm_apnoea
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/10/17	W. Paul			Created
*************************************************************************************************/
void app_PP_set_alarm_apnoea( int16_t val)
{
	PP_settings.Apnoea_alarm_wait	=  val;

	return;
}

/*************************************************************************************************
* Function Name : 	app_PP_set_alarm_resperation_rate
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/10/17	W. Paul			Created
*************************************************************************************************/
void app_PP_set_alarm_resperation_rate(	int16_t val)
{
	PP_settings.Respiration_rate_max	=  val;

	return;
}

/*************************************************************************************************
* Function Name : 	app_PP_settleAlarm_end
* Description   : 	This Function lets this block know the settle alarm has expired
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/10/17	W. Paul			Created
*************************************************************************************************/
void app_PP_settleAlarm_end(void)
{
	if(PP_data.Apnoea.event_settle_status == 0){
		PP_data.Apnoea.event_settle_status	=  1;
		PP_data.Apnoea.event_quiet_timer	=  PP_settings.Apnoea_alarm_wait * 1000;
		if(PP_data.Apnoea.event_alarm_no_breathing_detected){
			PP_data.Apnoea.event_settle_status	=  2;	//as alarm is being flagged.. end of settle period
			PP_data.Apnoea.alarm_flag			=  1;
		}
	}
	return;
}

/*************************************************************************************************
* Function Name : 	app_PP_settleAlarm_start
* Description   : 	This Function lets this block know the settle alarm has expired
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/10/17	W. Paul			Created
*************************************************************************************************/
void app_PP_settleAlarm_start(void)
{
	PP_data.Apnoea.event_settle_status					=  0;
	PP_data.Apnoea.event_quiet_timer					=  0;
	PP_data.Apnoea.alarm_flag							=  0;
	
	return;
}

/*************************************************************************************************
* Function Name : 	app_PP_1ms_IRQ
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		17/12/18	W. Paul			Created
*************************************************************************************************/
void app_PP_1ms_IRQ(void)
{
	if( PP_data.Apnoea.event_quiet_timer > 1){
		PP_data.Apnoea.event_quiet_timer	-= 1;
	}

	return;
}

/*************************************************************************************************
* Function Name : 	app_PP_debug
* Description   : 	This Function enables/disables the debug output
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/11/17	W. Paul			Created
*************************************************************************************************/
void app_PP_debug(uint8_t en)
{
	switch(en){
		default:
		case 0:		PP_settings.debug	=  0x00;	break;
		case 1:		PP_settings.debug	=  0x01;	break;
		case 2:		PP_settings.debug	^= 0x01;	break;
	}
	printf("\r\nPatient Pressure_print %d",PP_settings.debug);

	return;
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
