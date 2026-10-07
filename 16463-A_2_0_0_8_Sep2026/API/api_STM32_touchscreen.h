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
 * Filename    :  api_STM32_touchscreen.h
 * Date Created:  Mon 04 Sep 2017 11:35:22 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_STM32_TOUCHSCREEN_H
#define _API_STM32_TOUCHSCREEN_H

#include "stdint.h"

#define		X_MAX				800
#define		Y_MAX				480

#define		ADC_YU		13
#define		ADC_YD		11
#define		ADC_XL		10
#define		ADC_XR		12
#define		ADC_VBAT	14

#define		PIN_XL		GPIOC, GPIO_Pin_0
#define		PIN_YD		GPIOC, GPIO_Pin_1
#define		PIN_XR		GPIOC, GPIO_Pin_2
#define		PIN_YU		GPIOC, GPIO_Pin_3

//#define		PIN_VBAT	GPIOC, GPIO_Pin_4

#define		ADC_RESOLUTION		4096		//12bit ADC
#define		ADC_VRef			3.3
#define		Rxplate				1000			//thisgives the decimal point accuracy

#define		TouchPressureThreshold		60000	//this is the thresshold where touch is detected.
												//low value for centre screen
												//but need a higher value for edge of screen

#define		TOUCH_INIT			0x56





//	V= ADC/ADC_Res * Vref
//	V=  Rl/(Rl+Rh)*VBat
//	VBat	=  ADC	* [(Rl+Rh)/Rl * Vref / ADC_Res ]
//	VBat	=  ADC	* [ VBAT_m ]
#define		VBAT_Rlow			100000
#define		VBAT_Rhigh			1000000
#define		VBAT_pin			(ADC_VRef/ADC_RESOLUTION)
#define		VBAT_m				( (VBAT_Rlow + VBAT_Rhigh)/VBAT_Rlow)


// **********************************************************************************************************
// **********************************************************************************************************

typedef enum{
	TOUCH_INIT_X	= 0,
	TOUCH_INIT_Y	= 1,
	TOUCH_INIT_P1	= 2,
	TOUCH_INIT_P2	= 3,
	TOUCH_CAL		= 4,
}api_touch_enum;

typedef struct{
	int16_t 	x;		// signed value by calibration
	int16_t 	y;		// rotation or shift can return little neg value
}tPoint;

typedef struct{
	uint8_t				init;
    double				C[7];
}touch_cal_t;


typedef struct{
	uint8_t				init;
	api_touch_enum		state;
	//raw
    uint16_t   			XRaw;
    uint16_t    		YRaw;
    uint16_t    		Z1Raw;
    uint16_t    		Z2Raw;

    touch_cal_t			cal;
	//processed
	int16_t   			X;
    int16_t 	   		Y;
    uint16_t	    	RTouch;

}api_touch_t;


typedef struct{
	uint16_t    		Raw;
	float				pinVoltage;
	float				Value;
}api_Vbat_t;



// **********************************************************************************************************
// **********************************************************************************************************


// **********************************************************************************************************
// **********************************************************************************************************


void	api_touch_handler_IRQ(void);

void	api_touch_raw_rd(int16_t *rawX,int16_t *rawY);
uint8_t api_touch_CoOrds_rd(int16_t *X,int16_t *Y,uint16_t *TouchPressure);

void	api_touch_CalculateCalibrationConstants(tPoint Dp[], tPoint Tp[]);

uint8_t	api_touch_config_rd(void);
void	api_touch_config_wr(void);
void 	api_touch_config_default(void);

void 	api_touch_debug(uint8_t mode);
void 	api_touch_print(void);

float 	api_Vbat_rd(void);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
