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
 * Filename    :  alg_pid.c
 * Date Created:  Fri 14 Jul 2017 03:04:46 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "alg_pid.h"

#include "app_pneumatic_ctrl.h"


PID_parms_st	pid_parms[NO_OF_PID_CHANNELS];		//all pid gains and parameters are in here
PID_vars_st		pid_vars[NO_OF_PID_CHANNELS];		//all local vars are storred here
PID_av_st		pid_av[NO_OF_PID_CHANNELS];			//this is the averager for the dterm




/*************************************************************************************************
* Function Name : 	alg_PID
* Description   : 	This Function is the pid algorithm
* Arguments     : 	uint8_t		ch
*					uint16_t	input
*					double		target
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/07/17	W. Paul			Created
*************************************************************************************************/
double alg_PID(uint8_t ch,float input,float target)
{
	// Local Variables
	
	// Code
	alg_PID_config(ch);

//find error
	pid_vars[ch].error	=  target;
	pid_vars[ch].error	-= input;

//get rolling av of input to smooth d term
	pid_av[ch].tot				-= 	pid_av[ch].his[pid_av[ch].pos];
	pid_av[ch].his[pid_av[ch].pos]	=   pid_vars[ch].error;
	pid_av[ch].tot				+= 	pid_av[ch].his[pid_av[ch].pos];
	if(++pid_av[ch].pos 		>= PID_AV_LEN){		pid_av[ch].pos			=  0;	}
	if(++pid_av[ch].d_his_cnt 	>= PID_AV_LEN){		pid_av[ch].d_his_cnt	=  PID_AV_LEN;	}
	if(pid_av[ch].tot){
		pid_av[ch].Av			=  pid_av[ch].tot;
		pid_av[ch].Av			/= pid_av[ch].d_his_cnt;
	}
	else{
		pid_av[ch].Av			=  0;
	}

//p term
	pid_vars[ch].pTerm 	=  pid_vars[ch].error;
	pid_vars[ch].pTerm 	*= pid_parms[ch].GainP;

//i term
	pid_vars[ch].itot 		+= pid_vars[ch].error;
	if(		pid_vars[ch].itot > pid_parms[ch].iMax)	pid_vars[ch].itot = pid_parms[ch].iMax;
	else if(pid_vars[ch].itot < pid_parms[ch].iMin)	pid_vars[ch].itot = pid_parms[ch].iMin;
	pid_vars[ch].iTerm		=  pid_vars[ch].itot;
	pid_vars[ch].iTerm		*= pid_parms[ch].GainI;

//d term
	pid_vars[ch].dTerm						=  pid_vars[ch].d_his[pid_vars[ch].d_his_pt];
	pid_vars[ch].d_his[pid_vars[ch].d_his_pt]	=  pid_av[ch].Av;
	pid_vars[ch].dTerm 						-= pid_vars[ch].d_his[pid_vars[ch].d_his_pt];
	pid_vars[ch].dTerm						*= pid_parms[ch].GainD;
	if(++pid_vars[ch].d_his_pt >= pid_parms[ch].d_delay){		pid_vars[ch].d_his_pt	=  0;	}
	if(pid_vars[ch].d_his_pt >= MAX_D_HIS){						pid_vars[ch].d_his_pt	=  0;	}

//pid output
	if(pid_vars[ch].output_null_cnt <= pid_vars[ch].d_his_pt){
		pid_vars[ch].output_null_cnt	+= 1;
	//	pid_vars[ch].pidTerm			=  0;
		return(pid_vars[ch].pidTerm);
	}

	pid_vars[ch].pidTerm		=  pid_vars[ch].pTerm;
	pid_vars[ch].pidTerm		+= pid_vars[ch].iTerm;
	pid_vars[ch].pidTerm		+= pid_vars[ch].dTerm;
	return(pid_vars[ch].pidTerm);
}


/*************************************************************************************************
* Function Name : 	alg_PID_vars_reset
* Description   : 	This Function resets the vars when pid parameters are changed
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/07/17	W. Paul			Created
*************************************************************************************************/
void alg_PID_vars_reset(uint8_t ch)
{
	uint8_t	i;

	pid_vars[ch].itot		=  0;
	for(i=0;i<MAX_D_HIS;i++){
		pid_vars[ch].d_his[i]	=  0;
	}
	pid_vars[ch].d_his_pt	=  0;
	pid_av[ch].d_his_cnt	=  0;

	pid_vars[ch].output_null_cnt	=  0;

	return;
}


/*************************************************************************************************
* Function Name : 	alg_PID_config
* Description   : 	This Function set the pid parameters for different use cases
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/07/17	W. Paul			Created
*************************************************************************************************/
void alg_PID_config(uint8_t ch)
{
	float	hold_Gi;

	if(pid_parms[ch].current != pid_parms[ch].desired){
		hold_Gi	=  pid_parms[ch].GainI;

		switch(pid_parms[ch].desired){
			default:
			case PID_NO_CTRL:	//no control
				pid_parms[ch].GainP			=  0;
				pid_parms[ch].GainI			=  0;
				pid_parms[ch].GainD			=  0;
				
				pid_parms[ch].iMax			=  0;
				pid_parms[ch].iMin			=  0;
				
				pid_parms[ch].d_delay		=  5;
				break;
			case PID_FLOW_CTRL_HI:
				pid_parms[ch].GainP			=  0.12;
				pid_parms[ch].GainI			=  0.0002;
				pid_parms[ch].GainD			=  0.00001;
				
				pid_parms[ch].iMax			=  0.05 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -0.05 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  20;
				break;
			case PID_FLOW_CTRL_HI_SLOW0:
				pid_parms[ch].GainP			=  0.08;
				pid_parms[ch].GainI			=  0.0;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  0.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_FLOW_CTRL_HI_SLOW1:
				pid_parms[ch].GainP			=  0.04;
				pid_parms[ch].GainI			=  0.0;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  0.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_FLOW_CTRL_HI_SLOW2:
				pid_parms[ch].GainP			=  0.02;
				pid_parms[ch].GainI			=  0.0;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  0.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_FLOW_CTRL_LO:
				pid_parms[ch].GainP			=  0.3;
				pid_parms[ch].GainI			=  0.0;
				pid_parms[ch].GainD			=  0.001;
				
				pid_parms[ch].iMax			=  0.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_FLOW_CTRL_LO_SLOW0:
				pid_parms[ch].GainP			=  0.15;
				pid_parms[ch].GainI			=  0.0;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  0.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_FLOW_CTRL_LO_SLOW1:
				pid_parms[ch].GainP			=  0.075;
				pid_parms[ch].GainI			=  0.0;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  0.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_FLOW_CTRL_LO_SLOW2:
				pid_parms[ch].GainP			=  0.0375;
				pid_parms[ch].GainI			=  0.0;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  0.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_O2_CTRL:							//output range 0-1000	100Hz		input range 0-100
				pid_parms[ch].GainP			=  0.0;
				pid_parms[ch].GainI			=  0.0;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  0.0;			// 100 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			=  0.0;			//-100 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  10;
				break;
			case PID_RAW_CTRL:							//output range 0-1000	100Hz		input range 65535
				pid_parms[ch].GainP			= -0.01;		//	-ve because rawvalue goes down as flow increases
				pid_parms[ch].GainI			= -0.0000005;	//	-ve because rawvalue goes down as flow increases
				pid_parms[ch].GainD			= -0.0;
				
				pid_parms[ch].iMax			= -0.05 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			=  0.05 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  10;
				break;
			case PID_CONC_CTRL_HI:						//output range 0-100	100Hz		//input range 0-100
				if(app_Pneumatics.O2_sensor_sel	== O2_SENSOR_PARACUBE){
					pid_parms[ch].GainP		=  0.05;
					pid_parms[ch].GainI		=  0.02;
				}
				else{
					pid_parms[ch].GainP		=  0.03;
					pid_parms[ch].GainI		=  0.01;
				}
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  10.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -10.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_CONC_CTRL_FAST_LO:					//output range 0-100	100Hz		//input range 0-100
				pid_parms[ch].GainP			=  0.5;
				pid_parms[ch].GainI			=  0.1;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  20.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -20.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_CONC_CTRL_SLOW_LO:					//output range 0-100	100Hz		//input range 0-100
				pid_parms[ch].GainP			=  0.5;
				pid_parms[ch].GainI			=  0.02;
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  20.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -20.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_CONC_CTRL_FAST_VLO:				//output range 0-100	100Hz		//input range 0-100
				pid_parms[ch].GainP			=  2.0;
				pid_parms[ch].GainI			=  0.05;
				
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  50.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -50.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_CONC_CTRL_SLOW_VLO:				//output range 0-100	100Hz		//input range 0-100
				pid_parms[ch].GainP			=  1.0;
				pid_parms[ch].GainI			=  0.02;
				
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMax			=  50.0 / pid_parms[ch].GainI;
				pid_parms[ch].iMin			= -50.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_CONC_CTRL_FAST_DIR:				//output range 0-100	100Hz		//input range 0-100
				if(app_Pneumatics.O2_sensor_sel	== O2_SENSOR_PARACUBE){
					pid_parms[ch].GainP		=  20.0;
					pid_parms[ch].GainI		=  0.05;
					pid_parms[ch].iMax		=  1000.0 / pid_parms[ch].GainI;
				}
				else{
					pid_parms[ch].GainP		=  20.0;
					pid_parms[ch].GainI		=  0.005;
					pid_parms[ch].iMax		=  2500.0 / pid_parms[ch].GainI;
				}
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMin			=  0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			case PID_CONC_CTRL_SLOW_DIR:				//output range 0-100	100Hz		//input range 0-100
				if(app_Pneumatics.O2_sensor_sel	== O2_SENSOR_PARACUBE){
					pid_parms[ch].GainP		=  10.0;
					pid_parms[ch].GainI		=  0.02;
					pid_parms[ch].iMax		=  1000.0 / pid_parms[ch].GainI;
				}
				else{
					pid_parms[ch].GainP		=  20.0;
					pid_parms[ch].GainI		=  0.002;
					pid_parms[ch].iMax		=  2500.0 / pid_parms[ch].GainI;
				}
				pid_parms[ch].GainD			=  0.0;
				
				pid_parms[ch].iMin			=  0.0 / pid_parms[ch].GainI;
				
				pid_parms[ch].d_delay		=  0;
				break;
			//add other cases here
		}
		
		//fix pid parameters
		if(pid_parms[ch].current			== PID_NO_CTRL){
			alg_PID_vars_reset(ch);
		}
		else{
			pid_vars[ch].itot				*= hold_Gi;	//old
			if((pid_vars[ch].itot != 0)	&&	(pid_parms[ch].GainI != 0)){
				pid_vars[ch].itot			/= pid_parms[ch].GainI;	//new
			}
			else{
				pid_vars[ch].itot			=  0;
			}
			pid_vars[ch].output_null_cnt	=  0;
		}
		pid_parms[ch].current				=  pid_parms[ch].desired;

	}
	return;
}


/*************************************************************************************************
* Function Name : 	alg_PID_config_set
* Description   : 	This Function set the pid parameters for different use cases
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/07/17	W. Paul			Created
*************************************************************************************************/
void alg_PID_config_set(uint8_t ch,pid_parms_enum	desired)
{
	pid_parms[ch].desired	=  desired;

	return;
}


/*
*********************************************************************************************************
*						End of alg_pid.c
*********************************************************************************************************
*/
