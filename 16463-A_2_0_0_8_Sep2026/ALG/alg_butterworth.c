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
 *  Copyright 2018, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  alg_butterworth.c
 * Date Created:  Tue 27 Mar 2018 05:24:50 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>

#include "alg_butterworth.h"


/*************************************************************************************************
* Function Name : 	alg_butterworth_bp
	http://www-users.cs.york.ac.uk/~fisher/mkfilter
* Description   : 	This Function is
	filtertype	=	Butterworth
	passtype	=	Highpass
	ripple		=
	order		=	2
	samplerate	=	50
	corner1		=	0.03	slow breathing rate of 0.03Hz or   2/min
	corner2		=	4		fast breathing rate of 4   Hz or 240/min
	adzero		=
	logmin		=
* Arguments     : 	float input
* Returns       : 	float
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		27/03/18	W. Paul			Created
*************************************************************************************************/
//float alg_butterworth_bp(float input)
//{
//	static float	xv[5]   = {0.0, 0.0, 0.0, 0.0, 0.0};
//	static float	yv[5]   = {0.0, 0.0, 0.0, 0.0, 0.0};
//
//	xv[0]	=  xv[1];
//	xv[1]	=  xv[2];
//	xv[2]	=  xv[3];
//	xv[3]	=  xv[4];
//	xv[4]	=  input / 21.37941809;
//
//	yv[0]	=  yv[1];
//	yv[1]	=  yv[2];
//	yv[2]	=  yv[3];
//	yv[3]	=  yv[4];
//
//	yv[4]	=  xv[0];
//	yv[4]	+= ( -2.0 * xv[2]);
//	yv[4]	+= xv[4];
//	yv[4]	+= ( -0.4944188561 * yv[0]);
//	yv[4]	+= (  2.2988854595 * yv[1]);
//	yv[4]	+= ( -4.1135386970 * yv[2]);
//	yv[4]	+= (  3.3090694664 * yv[3]);
//
//	return(yv[4]);
//}


/*************************************************************************************************
* Function Name : 	alg_butterworth_bp
	http://www-users.cs.york.ac.uk/~fisher/mkfilter
* Description   : 	This Function is
	filtertype	=	Butterworth
	passtype	=	Highpass
	ripple		=
	order		=	2
	samplerate	=	50
	corner1		=	0.03	slow breathing rate of 0.03Hz or   2/min
	adzero		=
	logmin		=
* Arguments     : 	float input
* Returns       : 	float
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/04/19	W. Paul			Created
*************************************************************************************************/
float alg_butterworth_bp(float input)
{
	static float	xv[3]   = {0.0, 0.0, 0.0};
	static float	yv[3]   = {0.0, 0.0, 0.0};

	xv[0]	=  xv[1];
	xv[1]	=  xv[2];
	xv[2]	=  input / 1.002669286;

	yv[0]	=  yv[1];
	yv[1]	=  yv[2];

	yv[2]	=  xv[0];
	yv[2]	+= ( -2.0 * xv[1] );
	yv[2]	+= xv[2];
	yv[2]	+= ( -0.9946827275 * yv[0]);
	yv[2]	+= (  1.9946685531 * yv[1]);

	return(yv[2]);
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
