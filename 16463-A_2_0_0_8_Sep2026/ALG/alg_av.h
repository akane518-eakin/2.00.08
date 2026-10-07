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
 * Filename    :  alg_av.h
 * Date Created:  Tue 14 Nov 2017 11:09:14 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _ALG_AV_H_
#define _ALG_AV_H_

#include <stdint.h>

#define	NO_OF_AV_F		10

#define	ALG_AV_F_LEN	20
#define	ALG_AV_LONG_LEN	300


typedef enum{				//update 	alg_av_f_define()
	MAX_250_AV		=  0,
	PARACUBE_AV		=  1,
	PP_AV_1			=  2,
	PP_AV_2			=  3,
	PP_DBDT_AV		=  4,
	PP_BR_AV		=  5,
	FLOW_AIR_AV		=  6,
	FLOW_O2_AV		=  7,
	Temp_AV			=  8,   //flow testing 07/01/20
}av_f_enum;


typedef enum{
	FLOW_AIR_50_AV,
	FLOW_O2_50_AV,
	FLOW_AIR_150_AV,
	FLOW_O2_150_AV,
	FLOW_AIR_300_AV,
	FLOW_O2_300_AV,
	EXTRA_AV,
	NO_OF_AV_LONG
}av_long_enum;


typedef struct{
	float		his[ALG_AV_F_LEN];
	float		tot;
	uint16_t	pos;
	uint16_t	len;
	uint16_t	len_max;
	float		Av;
}ALG_av_f_st;


typedef struct{
	float		his[ALG_AV_LONG_LEN];
	float		tot;
	uint16_t	pos;
	uint16_t	len;
	uint16_t	len_max;
	float		Av;
}ALG_av_long_st;


float alg_av_f(av_f_enum ch, float input);
float alg_av_long(av_long_enum ch, float input);
float alg_av_f_oldest(av_f_enum ch);
void alg_av_f_init(av_f_enum ch);
void alg_av_long_init(av_long_enum ch);
void alg_av_f_define(void);


#endif	// __ALG_AV_H


/*
*********************************************************************************************************
*											End of alg_av.h
*********************************************************************************************************
*/
