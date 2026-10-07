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
 * Filename    :  alg_av.c
 * Date Created:  Tue 14 Nov 2017 11:00:54 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h> //temp comp
#include "alg_av.h"


ALG_av_f_st		av_f[NO_OF_AV_F];
ALG_av_long_st	av_long[NO_OF_AV_LONG];
uint8_t			init_filters	=  0;


/*************************************************************************************************
* Function Name : 	alg_av_f
* Description   : 	This Function averages the data
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/17	W. Paul			Created
*************************************************************************************************/
float alg_av_f(av_f_enum ch, float input)
{
	uint16_t		pos;
	uint16_t		len_max;

	if(init_filters			== 0){
		alg_av_f_define();
		init_filters		=  1;
	}
	
	pos						=  av_f[ch].pos;
	len_max					=  av_f[ch].len_max;

	av_f[ch].tot			-= 	av_f[ch].his[pos];
	av_f[ch].his[pos]		=   input;
	av_f[ch].tot			+= 	av_f[ch].his[pos];

	if(++av_f[ch].pos		>= len_max){av_f[ch].pos	=  0;				}
	if(++av_f[ch].len		>= len_max){av_f[ch].len	=  len_max;			}

	av_f[ch].Av				=  av_f[ch].tot;
	if(av_f[ch].Av			!= 0){	av_f[ch].Av			/= av_f[ch].len;	}
	else{							av_f[ch].Av			=  0;				}
	
	return(av_f[ch].Av);
}


/*************************************************************************************************
* Function Name : 	alg_av_long
* Description   : 	This Function averages the data
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/07/20	T. Barr			Created
*************************************************************************************************/
float alg_av_long(av_long_enum ch, float input)
{
	// Local Variables
	uint16_t		pos;
	uint16_t		len_max;
	
	// Code
	if(isnormal(input)		== 0){
		return(av_long[ch].Av);
	}
	
	if(init_filters			== 0){
		alg_av_f_define();
		init_filters		=  1;
	}
	
	pos						=  av_long[ch].pos;
	len_max					=  av_long[ch].len_max;
	
	av_long[ch].tot			-= 	av_long[ch].his[pos];
	av_long[ch].his[pos]	=   input;
	av_long[ch].tot			+= 	av_long[ch].his[pos];

	if(++av_long[ch].pos	>= len_max){av_long[ch].pos	=  0;				}
	if(++av_long[ch].len	>= len_max){av_long[ch].len	=  len_max;			}

	av_long[ch].Av			=  av_long[ch].tot;
	if(av_long[ch].Av		!= 0){	av_long[ch].Av		/= av_long[ch].len;	}
	else{							av_long[ch].Av		=  0;				}
	
	return(av_long[ch].Av);
}


/*************************************************************************************************
* Function Name : 	alg_av_f_oldest
* Description   : 	This Function averages the data
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/17	W. Paul			Created
*************************************************************************************************/
float alg_av_f_oldest(av_f_enum ch)
{
	uint8_t		pos;
	pos		=  av_f[ch].pos;

	return(av_f[ch].his[pos]);
}


/*************************************************************************************************
* Function Name : 	alg_av_f_init
* Description   : 	This Function initialises the dat in the av to 0
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/17	W. Paul			Created
*************************************************************************************************/
void alg_av_f_init(av_f_enum ch)
{
	memset(av_f[ch].his,0x00,sizeof(av_f[ch].his) );
	av_f[ch].tot		=  0;
	av_f[ch].len		=  0;
	av_f[ch].Av			=  0;
}


/*************************************************************************************************
* Function Name : 	alg_av_long_init
* Description   : 	This Function initialises the dat in the av to 0
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/07/20	T. Barr			Created
*************************************************************************************************/
void alg_av_long_init(av_long_enum ch)
{
	memset(av_long[ch].his,0x00,sizeof(av_long[ch].his) );
	av_long[ch].tot		=  0;
	av_long[ch].len		=  0;
	av_long[ch].Av		=  0;
}


/*************************************************************************************************
* Function Name : 	alg_av_f_define
* Description   : 	This Function defines the av lens etc
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/11/17	W. Paul			Created
*************************************************************************************************/
void alg_av_f_define(void)
{
//0
	av_f[MAX_250_AV].len_max				=  3;	//3 point rolling av
	alg_av_f_init(MAX_250_AV);

//1
	av_f[PARACUBE_AV].len_max				=  3;	//3 point rolling av
	alg_av_f_init(PARACUBE_AV);

//2
	av_f[PP_AV_1].len_max					=  10;	//10 point rolling av
	alg_av_f_init(PP_AV_1);

//3
	av_f[PP_AV_2].len_max					=  10;	//10 point rolling av
	alg_av_f_init(PP_AV_2);

//4
	av_f[PP_DBDT_AV].len_max				=  10;	//10 point rolling av
	alg_av_f_init(PP_DBDT_AV);

//5
	av_f[PP_BR_AV].len_max					=  3;	//3 point rolling av
	alg_av_f_init(PP_BR_AV);

//6
	av_f[FLOW_AIR_AV].len_max				=  20;	//20 point rolling av
	alg_av_f_init(FLOW_AIR_AV);

//7
	av_f[FLOW_O2_AV].len_max				=  20;	//20 point rolling av
	alg_av_f_init(FLOW_O2_AV);

//8	
	av_f[Temp_AV].len_max					=  20;	//20 point rolling av
	alg_av_f_init(Temp_AV);

//Air 50
	av_long[FLOW_AIR_50_AV].len_max			=  50;
	alg_av_long_init(FLOW_AIR_50_AV);

//O2 50
	av_long[FLOW_O2_50_AV].len_max			=  50;
	alg_av_long_init(FLOW_O2_50_AV);

//Air 150
	av_long[FLOW_AIR_150_AV].len_max		=  150;
	alg_av_long_init(FLOW_AIR_150_AV);

//O2 150
	av_long[FLOW_O2_150_AV].len_max			=  150;
	alg_av_long_init(FLOW_O2_150_AV);

//Air 300
	av_long[FLOW_AIR_300_AV].len_max		=  300;
	alg_av_long_init(FLOW_AIR_300_AV);

//O2 300
	av_long[FLOW_O2_300_AV].len_max			=  300;
	alg_av_long_init(FLOW_O2_300_AV);

	return;
}


/*
*********************************************************************************************************
*						End of alg_av.c
*********************************************************************************************************
*/
