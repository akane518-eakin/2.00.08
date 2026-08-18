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
 * Filename    :  alg_NTC_thermistor.c
 * Date Created:  Wed 31 Jul 2019 12:09:11 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <math.h>


/*************************************************************************************************
* Function Name : 	alg_NTC_R2C
* Description   : 	This Function converts resistance to temperature for a NTC thermistor
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		31/07/19	W. Paul			Created
*************************************************************************************************/
float alg_NTC_R2C(float Beta, float R25, float RT)
{
	float temperatureC;
	float	temp;

	temp 			=  log(RT/R25);
	temp			/= Beta;
	temp			+= (1/298.15);
	temperatureC	=  1/temp;
	temperatureC	-= 273.15;
	
	if(isnormal(temperatureC)){
		return(temperatureC);
	}
	else{
		return(25.0);
	}
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
