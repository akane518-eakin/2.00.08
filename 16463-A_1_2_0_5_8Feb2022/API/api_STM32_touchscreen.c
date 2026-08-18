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
 * Filename    :  api_STM32_touchscreen.c
 * Date Created:  Mon 04 Sep 2017 11:35:49 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include "api_STM32_touchscreen.h"

#include "csp_STM32_ADC1.h"
#include "csp_STM32_ADC2.h"
#include "csp_S25FL0xx.h"

#include "pcb_pins.h"

uint8_t				touch_debug	=  0;

/*********************************************************************************************************
 *		Notes
 ********************************************************************************************************/
//
//	Ref  AN2376_001-15228_0D_V.pdf
//	this data sheet describes the calibration principles
//
//	LCD pix width		XW
//	LCD pix height		YH
//
//	best calibration points
//	dp[0]	=  (XW*0.1 , YH*0.1)
//	dp[1]	=  (XW*0.5 , YH*0.9)
//	dp[2]	=  (XW*0.9 , YH*0.5)

//call	api_touch_handler_IRQ from a 100Hz timer
//

/*********************************************************************************************************
 *		Local Variables
 ********************************************************************************************************/
api_touch_t 	touch;
api_Vbat_t		Vbat;

/*********************************************************************************************************
 ********************************************************************************************************/


/*************************************************************************************************
* Function Name : 	api_touch_handler_IRQ
* Description   : 	This Function is called from a low speed timer interrupt  (50-250Hz).
*					the function will set IO pins and read the ADC.
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/09/17	W. Paul			Created
*************************************************************************************************/
void api_touch_handler_IRQ(void)
{
	double	d1;
	double	dtotal;

	if(touch.init != TOUCH_INIT){
		touch.init	=  TOUCH_INIT;
		touch.state	=  TOUCH_INIT_X;
		csp_ADC1_Setup(ADC_VBAT);
	}


	switch(touch.state){
		case TOUCH_INIT_X:
			//read	VBAT
			Vbat.Raw			=  csp_ADC1_Read();
			Vbat.pinVoltage		=  Vbat.Raw;
			Vbat.pinVoltage		*= VBAT_pin;
			Vbat.Value			=  Vbat.pinVoltage;
			Vbat.Value			*= VBAT_m;

			//init X
			io_pins_config1(PIN_YU,GPIO_Mode_AIN);	        // YU I
			io_pins_config1(PIN_YD,GPIO_Mode_IN_FLOATING);	// YD I
			io_pins_config1(PIN_XL,GPIO_Mode_Out_PP);		// XL LOW
			io_pins_config1(PIN_XR,GPIO_Mode_Out_PP);		// XR HI
			PinSet(PIN_XL,0);
			PinSet(PIN_XR,1);
			csp_ADC1_Setup(ADC_YU);
			break;
		case TOUCH_INIT_Y:
			//read	X
			touch.XRaw	=  csp_ADC1_Read();

			//init Y
			io_pins_config1(PIN_XL,GPIO_Mode_AIN);	        // XL I
			io_pins_config1(PIN_XR,GPIO_Mode_IN_FLOATING);	// XR I
			io_pins_config1(PIN_YD,GPIO_Mode_Out_PP);		// YD LOW
			io_pins_config1(PIN_YU,GPIO_Mode_Out_PP);		// YU HI
			PinSet(PIN_YD,0);
			PinSet(PIN_YU,1);
			csp_ADC1_Setup(ADC_XL);
			break;
		case TOUCH_INIT_P1:
			//read	Y
			touch.YRaw	=  csp_ADC1_Read();

			//init P1
			io_pins_config1(PIN_XL,GPIO_Mode_AIN);	        // XL I
			io_pins_config1(PIN_YD,GPIO_Mode_IN_FLOATING);	// YD I
			io_pins_config1(PIN_YU,GPIO_Mode_Out_PP);		// YU HI
			io_pins_config1(PIN_XR,GPIO_Mode_Out_PP);		// XR LOW
			PinSet(PIN_XR,0);
			PinSet(PIN_YU,1);								//
			csp_ADC1_Setup(ADC_XL);
			break;
		case TOUCH_INIT_P2:
			//read P1
			touch.Z1Raw	=  csp_ADC1_Read();

			//init P2
			io_pins_config1(PIN_XL,GPIO_Mode_IN_FLOATING);	// XL I
			io_pins_config1(PIN_YD,GPIO_Mode_AIN);	        // YD I
			io_pins_config1(PIN_YU,GPIO_Mode_Out_PP);		// YU HI
			io_pins_config1(PIN_XR,GPIO_Mode_Out_PP);		// XR LOW
			PinSet(PIN_XR,0);
			PinSet(PIN_YU,1);								//
			csp_ADC1_Setup(ADC_YD);
			break;
		//bonus ADC read
		case TOUCH_CAL:
			//read P2
			touch.Z2Raw	=  csp_ADC1_Read();

			//x axis
			dtotal	=  touch.XRaw;
			dtotal	*= touch.cal.C[1];
			d1		=  touch.YRaw;
			d1		*= touch.cal.C[2];
			dtotal	+= d1;
			dtotal	+= touch.cal.C[3];
			dtotal	/= touch.cal.C[0];
			touch.X		=  (int16_t)dtotal;

			//y axis
			dtotal	=  touch.XRaw;
			dtotal	*= touch.cal.C[4];
			d1		=  touch.YRaw;
			d1		*= touch.cal.C[5];
			dtotal	+= d1;
			dtotal	+= touch.cal.C[6];
			dtotal	/= touch.cal.C[0];
			touch.Y		=  (int16_t)dtotal;

			//touch resistance
			dtotal	=  touch.Z2Raw;
			dtotal	/= touch.Z1Raw;
			dtotal	-= 1;
			dtotal	*= touch.XRaw;
			dtotal	/= ADC_RESOLUTION;
			dtotal	*= Rxplate;
			if(		dtotal > 65000){	dtotal	=  65000;	}
			else if(dtotal < 0){		dtotal	=  0;		}

			touch.RTouch	=  (uint16_t)dtotal;



			//init Vbat
			csp_ADC1_Setup(ADC_VBAT);
			break;
		default:
			touch.state	=  TOUCH_INIT_X;
	}

	//increment state
	if(++touch.state > TOUCH_CAL){
		touch.state = TOUCH_INIT_X;
	}
	return;
}

/*************************************************************************************************
* Function Name : 	api_touch_raw_rd
* Description   : 	This Function
* Arguments     : 	uint16_t *rawX
*					uint16_t *rawY
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/09/17	W. Paul			Created
*************************************************************************************************/
void api_touch_raw_rd(int16_t *rawX,int16_t *rawY)
{
	*rawX	=  (int16_t)touch.XRaw;
	*rawY	=  (int16_t)touch.YRaw;

	return;
}


/*************************************************************************************************
* Function Name : 	api_touch_CoOrds_rd
* Description   : 	This Function
* Arguments     : 	uint16_t	*X
*					uint16_t	*Y
*					uint16_t	*TouchPressure
* Returns       : 	uint8_t		valid
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_touch_CoOrds_rd(int16_t *X,int16_t *Y,uint16_t *TouchPressure)
{
	*X				=  touch.X;
	*Y				=  touch.Y;
	*TouchPressure	=  touch.RTouch;

	if(*TouchPressure > TouchPressureThreshold)	return(0);
	if(*X < 0)		return(0);
	if(*Y < 0)		return(0);
	if(*X > X_MAX)	return(0);
	if(*Y > Y_MAX)	return(0);

	return(1);
}

/*************************************************************************************************
* Function Name : 	api_touch_CalculateCalibrationConstants
* Description   : 	This Function calculates the 7 constants needed to calibrate the touch pannel
* Arguments     : 	tPoint Dp[]Armstriong		display point (pixel)	array of 3 points(x,y)
*					tPoint Tp[]		Touch point (adc)		array of 3 points(x,y)
* Returns       : 	void
* Notes         : 	AN2376
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/09/17	W. Paul			Created
*************************************************************************************************/
void api_touch_CalculateCalibrationConstants(tPoint Dp[], tPoint Tp[])
{
	touch.cal.C[0]	= (double)( Tp[0].x - Tp[2].x )*( Tp[1].y - Tp[2].y )
					- (double)( Tp[1].x - Tp[2].x )*( Tp[0].y - Tp[2].y );

	touch.cal.C[1]	= (double)( Dp[0].x - Dp[2].x )*( Tp[1].y - Tp[2].y )
					- (double)( Dp[1].x - Dp[2].x )*( Tp[0].y - Tp[2].y );

	touch.cal.C[2]	= (double)( Tp[0].x - Tp[2].x )*( Dp[1].x - Dp[2].x )
					- (double)( Dp[0].x - Dp[2].x )*( Tp[1].x - Tp[2].x );

	touch.cal.C[3]	= (double)Tp[0].y * ( (double)(Tp[2].x * Dp[1].x) - (double)(Tp[1].x * Dp[2].x) )
					+ (double)Tp[1].y * ( (double)(Tp[0].x * Dp[2].x) - (double)(Tp[2].x * Dp[0].x) )
					+ (double)Tp[2].y * ( (double)(Tp[1].x * Dp[0].x) - (double)(Tp[0].x * Dp[1].x) );

	touch.cal.C[4] 	= (double)( Dp[0].y - Dp[2].y )*( Tp[1].y - Tp[2].y )
					- (double)( Dp[1].y - Dp[2].y )*( Tp[0].y - Tp[2].y );

	touch.cal.C[5] 	= (double)( Tp[0].x - Tp[2].x )*( Dp[1].y - Dp[2].y )
					- (double)( Dp[0].y - Dp[2].y )*( Tp[1].x - Tp[2].x );

	touch.cal.C[6]	= (double)Tp[0].y * ( (double)(Tp[2].x * Dp[1].y) - (double)(Tp[1].x *Dp[2].y ) )
					+ (double)Tp[1].y * ( (double)(Tp[0].x * Dp[2].y) - (double)(Tp[2].x *Dp[0].y ) )
					+ (double)Tp[2].y * ( (double)(Tp[1].x * Dp[0].y) - (double)(Tp[0].x *Dp[1].y ) );

	touch.cal.init	=  TOUCH_INIT;

	api_touch_config_wr();

	return;
}

/*************************************************************************************************
* Function Name : 	api_touch_config_rd
* Description   : 	This Function reads the config from flash
*					if no config avail use defaults
* Arguments     : 	void
* Returns       : 	uint8_t		0	no issue
*								1	defaults used
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_touch_config_rd(void)
{
	csp_mem_rd((uint8_t*)&touch.cal.init, MEM_ADD_TOUCH_CONFIG, sizeof(touch_cal_t) );

	if(touch.cal.init != TOUCH_INIT){
		api_touch_config_default();
		return(1);
	}
	else{
		return(0);
	}
}

/*************************************************************************************************
* Function Name : 	api_touch_config_wr
* Description   : 	This Function saves the config data to external flash
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/09/17	W. Paul			Created
*************************************************************************************************/
void api_touch_config_wr(void)
{
	csp_sys_mem_wr((uint8_t*)&touch.cal, MEM_ADD_TOUCH_CONFIG, sizeof(touch_cal_t) );
	return;
}


/*************************************************************************************************
* Function Name : 	api_touch_config_default
* Description   : 	This Function sets the touch calibration defaults
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/09/17	W. Paul			Created
*************************************************************************************************/
void api_touch_config_default(void)
{
	touch.cal.init	=  0;

	touch.cal.C[0]	=  1;
	touch.cal.C[1]	=  0.1;
	touch.cal.C[2]	=  0.0;
	touch.cal.C[3]	=  0.0;
	touch.cal.C[4]	=  0.1;
	touch.cal.C[5]	=  0.0;
	touch.cal.C[6]	=  0.0;

	return;
}

/*************************************************************************************************
* Function Name : 	api_touch_print
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		02/10/17	W. Paul			Created
*************************************************************************************************/
void api_touch_print(void)
{
	if(touch_debug){
		printf("\rTouch %d %d (%d)(%d %d)(%d %d)"	,touch.X
			   										,touch.Y
													,touch.RTouch
													,touch.XRaw
													,touch.YRaw
													,touch.Z1Raw
													,touch.Z2Raw		);
	}

	return;
}

/*************************************************************************************************
* Function Name : 	api_touch_debug
* Description   : 	This Function togles the touchscreen debug output
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/11/18	W. Paul			Created
*************************************************************************************************/
void api_touch_debug(uint8_t mode)
{
	if(mode	< 2){	touch_debug	=  mode;	}
	else{			touch_debug	^= 0x01;	}
	return;
}

/*************************************************************************************************
* Function Name : 	api_Vbat_rd
* Description   : 	This Function returns the Vbat as read in the interrupt timed adc read
* Arguments     : 	void
* Returns       : 	float Vbat voltage
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/09/17	W. Paul			Created
*************************************************************************************************/
float api_Vbat_rd(void)
{
//    Vbat.Raw			=  csp_ADC2_SetupWaitRead(ADC_VBAT);
//	Vbat.pinVoltage		=  Vbat.Raw;
//	Vbat.pinVoltage		*= VBAT_pin;
//	Vbat.Value			=  Vbat.pinVoltage;
//	Vbat.Value			*= VBAT_m;


    return(Vbat.Value);
}



/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
