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
 * Filename    :  api_calibrate.h
 * Date Created:  Tue 12 Sep 2017 02:32:39 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_CALIBRATE_H
#define _API_CALIBRATE_H

#include <stdint.h>

#define		NO_DEVICES_TO_CALIBRATE		12

typedef enum
{
	CAL_FLOW_O2_a	=  0x00,
	CAL_FLOW_O2_b	=  0x01,
	CAL_FLOW_O2_c	=  0x02,
	CAL_FLOW_O2_d	=  0x03,
	CAL_FLOW_O2_e	=  0x04,
	CAL_FLOW_AIR_a	=  0x05,
	CAL_FLOW_AIR_b	=  0x06,
	CAL_FLOW_AIR_c	=  0x07,
	CAL_FLOW_AIR_d	=  0x08,
	CAL_FLOW_AIR_e	=  0x09,
	CAL_SENSOR_O2	=  0x0A,
	CAL_SENSOR_PP	=  0x0B,
}cal_devices_enum;


/**********************************************************************************************************
 **********************************************************************************************************/
#define		CAL_VALID			0xAA
#define		CAL_DEFAULT			0xA9

typedef enum
{
	CAL_INIT		=  0x00,
	CAL_POINT_LOW	=  0x01,
	CAL_POINT_HIGH	=  0x02,
	CAL_CALCULATE	=  0x03,
	CAL_FROM_MEM	=  0x04,
	CAL_PRINT		=  0x05,
}calib_enum;

typedef enum
{
	CALIB_SUCCESS	=	0xBB,
	CALIB_FAILED	= 	0xFF,
}calib_status_enum;

typedef struct
{
	uint8_t				valid;
	uint8_t				name[16];

	float				adc[2];
	float				calpt[2];

	float				m;
	float				c;

	float				m_max;
	float				m_min;

	calib_status_enum	calib_status; 		// 0xBB = calibration success, 0xFF or else calibration failed.
}API_CALIBRATION_t;

typedef struct{
	uint8_t				valid;
	calib_status_enum	calib_status; 		// 0xBB = calibration success, 0xFF or else calibration failed.
	uint16_t			pwm_air;
	uint16_t			pwm_O2;
}API_PWM_CALIBRATION_t;

/*********************************************************************************************************
 *		Global Variables
 ********************************************************************************************************/
extern	API_CALIBRATION_t		calibration[NO_DEVICES_TO_CALIBRATE];
extern	API_PWM_CALIBRATION_t	pwm_calibration;
extern	float					cal_temperature_air;
extern	float					cal_temperature_O2;


/*********************************************************************************************************
 *		Global Functions
 ********************************************************************************************************/
uint8_t	api_calibrate_cmd(cal_devices_enum device,calib_enum action);
void	api_calibrate_config(cal_devices_enum device,float m_max,float m_min);
void	api_calibrate_set_points(cal_devices_enum device,float lo,float hi);

void	api_calibrate_pwm_rd(void);
void	api_calibrate_pwm_wr(void);


#endif	// _API_CALIBRATE_H


/*
*********************************************************************************************************
*											End of api_calibrate.h
*********************************************************************************************************
*/
