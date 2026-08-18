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
 *  Copyright 2019, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  alg_temp_comp.c
 * Date Created:  Fri 13 Dec 2019 12:09:11 PM
 * Programmer  :  Steven Surgenor
 * Description :  This module is used for adding a correction to the diffferential pressure to incorporate tempertaure compensation
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <math.h>

#include "api_calibrate.h"


/*************************************************************************************************
* Function Name : 	alg_temp_comp
* Description   : 	This Function is used for temp comp
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/12/19	S.Surgenor			Created
*************************************************************************************************/
float alg_temp_comp(float diff_pressure, float temperature_f)
{
	float	D0;
	float	D1;
	float	D2;
	float	K;
	float	S;
	float	D3;
		
	D1	=  diff_pressure;
	D0	=  calibration[CAL_FLOW_AIR_a].adc[0];
	D2	=  1-(D1/D0);
	K	=  -0.00657518867697464 * temperature_f + 0.273025537363409;
	S	=  1-(D2*K);
	D3	=  D1 * S;
	
	return (D3);
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
