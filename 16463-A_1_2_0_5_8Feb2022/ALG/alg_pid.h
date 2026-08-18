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
 * Filename    :  alg_pidn.h
 * Date Created:  Fri 14 Jul 2017 03:07:31 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _ALG_PIDn_H_
#define _ALG_PIDn_H_


//different uses of the pid output
#define	NO_OF_PID_CHANNELS		4

#define	PID_O2					0
#define	PID_AIR					1
#define	PID_VENTURI_FLOW		2
#define	PID_CONC				3

//differemnt paramters applied to the PID formula
typedef enum{
	PID_NO_CTRL,
	PID_FLOW_CTRL_HI,
	PID_FLOW_CTRL_HI_SLOW0,
	PID_FLOW_CTRL_HI_SLOW1,
	PID_FLOW_CTRL_HI_SLOW2,
	PID_FLOW_CTRL_LO,
	PID_FLOW_CTRL_LO_SLOW0,
	PID_FLOW_CTRL_LO_SLOW1,
	PID_FLOW_CTRL_LO_SLOW2,
	PID_O2_CTRL,
	PID_RAW_CTRL,
	PID_CONC_CTRL_HI,
	PID_CONC_CTRL_FAST_LO,
	PID_CONC_CTRL_SLOW_LO,
	PID_CONC_CTRL_FAST_VLO,
	PID_CONC_CTRL_SLOW_VLO,
	PID_CONC_CTRL_FAST_DIR,
	PID_CONC_CTRL_SLOW_DIR,
}pid_parms_enum;


#define	MAX_D_HIS	20
#define	PID_AV_LEN	6

typedef struct{
	pid_parms_enum	current;
	pid_parms_enum	desired;

	float		GainP;
	float		GainI;
	float		GainD;

	float		iMax;
	float		iMin;

	uint8_t		d_delay;

}PID_parms_st;


typedef struct{
	float		error;

	float		pTerm;
	float		iTerm;
	float		dTerm;
	float		pidTerm;
	uint8_t		output_null_cnt;

	float		itot;
	float		d_his[MAX_D_HIS];
	uint8_t		d_his_pt;
}PID_vars_st;

typedef struct{
	float		his[PID_AV_LEN];
	float		tot;
	uint8_t		pos;
	float		Av;
	uint8_t		d_his_cnt;
}PID_av_st;


extern PID_vars_st		pid_vars[];


double 	alg_PID(uint8_t ch,float input,float target);
void 	alg_PID_config(uint8_t ch);
void 	alg_PID_vars_reset(uint8_t ch);

void 	alg_PID_config_set(uint8_t ch,pid_parms_enum	desired);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
