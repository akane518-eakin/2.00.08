/*
*********************************************************************************************************
*											Marturion Ltd
*
*											Knockmore Hill Business Park
*											9 Ferguson Drive
*											Lisburn
*											Co. Antrim
*											Northern Ireland
*											BT28 2EX
*
*
*					Copyright 2020, Marturion Ltd, Lisburn, Co. Antrim, Northern Ireland
*											All Rights Reserved
*
*
*											alg_filt.c
*
* Filename    :  alg_filt.c
* Programmer  :  Tim Barr
* Description :  This module is used for digital filtering
* Compiler    :  GNU GCC
* Target      :  STM32
* Version     :  Version 3.5.0
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*											INCLUDE FILES
*********************************************************************************************************
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "alg_filt.h"


/*
*********************************************************************************************************
*											DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*											VARIABLES
*********************************************************************************************************
*/
alg_filt_var_t	alg_filt_var[ALG_FILT_CH_CNT];

float alg_filt_lpf_ord2_0_02_a[3]	=  {
	0.00362453228686862190,
	0.00724906457373724390,
	0.00362453228686862190
};

float alg_filt_lpf_ord2_0_02_b[3]	=  {
	1.00000000000000000000,
	-1.82269492519630800000,
	0.83718165125602251000
};

float alg_filt_lpf_ord4_0_01_a[5]	=  {
	0.00000091154710727084,
	0.00000364618842908337,
	0.00000546928264362506,
	0.00000364618842908337,
	0.00000091154710727084
};

float alg_filt_lpf_ord4_0_01_b[5]	=  {
	1.00000000000000000000,
	-3.83582554064734760000,
	5.52081913662222770000,
	-3.53353521946301410000,
	0.84855599926647673000
};

float alg_filt_lpf_ord1_0_005_a[2]	=  {
	0.01546629140379030000,
	0.01546629140379030000
};

float alg_filt_lpf_ord1_0_005_b[2]	=  {
	1.00000000000000000000,
	-0.96906741719379330000
};

float alg_filt_lpf_ord1_0_08_a[2]	=  {
	0.20430083808195365000,
	0.20430083808195365000
};

float alg_filt_lpf_ord1_0_08_b[2]	=  {
	1.00000000000000000000,
	-0.59139835139947106000
};


/*
*********************************************************************************************************
*											FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void	alg_filt_define(void);


/*
*********************************************************************************************************
* Function Name : alg_filt_f
* Description   : This function is used for processing digital filters
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			18/01/2020			Tim Barr		Original Created
*********************************************************************************************************
*/
float alg_filt_f(alg_filt_enum ch, float input)
{
	// Local Variables
	static	uint8_t	init_filt	=  0;
	uint8_t			i;
	float			temp_f[2];
	
	// Code
	if(init_filt				== 0){
		alg_filt_define();
		init_filt				=  1;
	}
	
	// Shift the old samples
	for(i=alg_filt_var[ch].cnt;i>0;i--){
		alg_filt_var[ch].x[i]	=  alg_filt_var[ch].x[i-1];
		alg_filt_var[ch].y[i]	=  alg_filt_var[ch].y[i-1];
	}
	
	// Calculate the new output
	alg_filt_var[ch].x[0]		=  input;
	alg_filt_var[ch].y[0]		=  alg_filt_var[FLOW_AIR_FILT].a_coef[0];
	alg_filt_var[ch].y[0]		*= alg_filt_var[ch].x[0];
	for(i=1;i<=alg_filt_var[ch].cnt;i++){
		temp_f[0]				=  alg_filt_var[ch].x[i];
		temp_f[0]				*= alg_filt_var[FLOW_AIR_FILT].a_coef[i];
		temp_f[1]				=  alg_filt_var[ch].y[i];
		temp_f[1]				*= alg_filt_var[FLOW_AIR_FILT].b_coef[i];
		alg_filt_var[ch].y[0]	+= temp_f[0];
		alg_filt_var[ch].y[0]	-= temp_f[1];
	}
	
	return alg_filt_var[ch].y[0];
}


/*
*********************************************************************************************************
* Function Name : alg_filt_define
* Description   : This function is used for defining filter variables before use
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			18/01/2020			Tim Barr		Original Created
*********************************************************************************************************
*/
void alg_filt_define(void)
{
	// Local Variables
	
	// Code
	memset(alg_filt_var, 0, (sizeof(alg_filt_var_t) * ALG_FILT_CH_CNT));
	
	alg_filt_var[FLOW_AIR_FILT].cnt		=  1;
	alg_filt_var[FLOW_AIR_FILT].a_coef	=  alg_filt_lpf_ord1_0_005_a;
	alg_filt_var[FLOW_AIR_FILT].b_coef	=  alg_filt_lpf_ord1_0_005_b;
	
	alg_filt_var[FLOW_O2_FILT].cnt		=  1;
	alg_filt_var[FLOW_O2_FILT].a_coef	=  alg_filt_lpf_ord1_0_005_a;
	alg_filt_var[FLOW_O2_FILT].b_coef	=  alg_filt_lpf_ord1_0_005_a;
	
	alg_filt_var[TEMP_FILT].cnt			=  1;
	alg_filt_var[TEMP_FILT].a_coef		=  alg_filt_lpf_ord1_0_005_a;
	alg_filt_var[TEMP_FILT].b_coef		=  alg_filt_lpf_ord1_0_005_a;
	
	return;
}


/*
*********************************************************************************************************
*						End of alg_filt.c
*********************************************************************************************************
*/
