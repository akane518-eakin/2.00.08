/*
*********************************************************************************************************
*                                           Marturion Ltd
*
*                                           Knockmore Hill Business Park
*                                           9 Ferguson Drive
*                                           Lisburn
*                                           Co. Antrim
*                                           Northern Ireland
*                                           BT28 2EX
*
*
*                  Copyright 2010, Marturion Ltd, Lisburn, Co. Antrim, Northern Ireland
*                                          All Rights Reserved
*
*
*                                          api_IO.h
*
* Filename    :  api_IO.h
* Programmer  :  William Paul
* Description :  This module is used for
* Compiler    :  GNU GCC
* Target      :  STM32103
* Version     :  Version 1.0.0
*********************************************************************************************************
*/
#ifndef _API_IO_H_
#define _API_IO_H_

#include "stm32f10x.h"

/**********************************************************************************************************
 *                                           INCLUDE FILES
 **********************************************************************************************************/
#define		PWM_AIR_MAX					10000		//pwm
#define		PWM_AIR_MIN					    1		//pwm
#define		PWM_AIR_CLOSED				    0		//pwm

#define		PWM_O2_MAX					10000		//pwm
#define		PWM_O2_MIN					    1		//pwm
#define		PWM_O2_CLOSED				    0		//pwm

#define		PWM_VENTURI_MAX				10000		//pwm
#define		PWM_VENTURI_MIN_USABLE		 3000		//pwm
#define		PWM_VENTURI_MIN_UP		 	  100		//pwm
#define		PWM_VENTURI_MIN				    0		//pwm

#define		PWM_FAN_HI					60			//pwm
#define		PWM_FAN_MAX					100			//pwm

#define		FLOW_O2						0
#define		FLOW_AIR					1

//used to detect if the ADC is measuring outside of normal range
//these values will need updated
#define		ADC_O2_RAW_GRACE				0x0001FFFF
#define		ADC_AIR_RAW_GRACE				0x0001FFFF
#define		ADC_MAX250_RAW_GRACE			0x0001FFFF
#define		ADC_PP_RAW_GRACE				10.0



/**********************************************************************************************************
 *
 **********************************************************************************************************/
typedef enum{
	TERMINAL_DISABLED		=  0,
	TERMINAL_LINE			=  1,
	TERMINAL_FULL_SCREEN	=  2,
	TERMINAL_FAST			=  3,
}TERMINAL_PRINT_enum;

typedef enum{
	VALVE_NEBLISER_OFF		=  0,
	VALVE_NEBLISER_ON		=  1,
	VALVE_NEBLISER_TOG		=  2,
}Valve_Nebuliser_enum;

typedef enum{
	VALVE_O2_CAL_OFF		=  0,
	VALVE_O2_CAL_ON			=  1,
	VALVE_O2_CAL_TOG		=  2,
}Valve_O2_Cal_enum;

typedef enum{
	VALVE_Venturi_DIS		=  0,
	VALVE_Venturi_EN		=  1,
	VALVE_Venturi_TOG		=  2,
}Valve_Venturi_En_enum;

typedef enum{
	SAFTY_CUTOUT_DIS		=  0,
	SAFTY_CUTOUT_EN			=  1,
	SAFTY_CUTOUT_TOG		=  2,
}Safty_Cutout_enum;

typedef struct{
	//power &ctrl signals
	Safty_Cutout_enum		safety_cutout;
	uint8_t					sensor_5V;

	//desired
	uint16_t				pwm_des_Air;			//pwm value for timer8_ch1
	uint16_t				pwm_des_O2;				//pwm value for timer8_ch2
	uint16_t				pwm_des_VenturiFlow;	//pwm value for timer8_ch3
	//actual - after update IRQ period
	uint16_t				pwm_Air;				//pwm value for timer8_ch1
	uint16_t				pwm_O2;					//pwm value for timer8_ch2
	uint16_t				pwm_VenturiFlow;		//pwm value for timer8_ch3

	uint8_t					fan;


	Valve_Nebuliser_enum	valve_nebuliser;		//io valve
	Valve_O2_Cal_enum		valve_O2_calibrate;		//io valve
	Valve_Venturi_En_enum	valve_venturi_en;		//io valve

	uint8_t					io_PressureAir;			//io pressure input
	uint8_t					io_PressureO2;			//io pressure input

	uint32_t				ADC_Pressure_O2_raw;
	uint32_t				ADC_Pressure_Air_raw;
	uint32_t				ADC_Pressure_O2;
	uint32_t				ADC_Pressure_Air;
	float					ADC_Pressure_Air_compensated;
	uint32_t				ADC_O2_Sensor;
	uint32_t				ADC_patientP;
	uint32_t				ADC_Thermistor;
	uint32_t				ADC_5V;
	uint32_t				ADC_Themistor1;
	uint32_t				ADC_Themistor;

	float					Flow_O2;
	float					Flow_Air;
	
	float					Flow_O2_1;
	float					Flow_Air_1;
	
	float					Flow_O2_Av;
	float					Flow_Air_Av;
	
	float					Flow_O2_Of;
	float					Flow_Air_Of;
	uint16_t				Flow_Of_timer;

	uint8_t					Paracube_O2_fault;
	float					Max250_O2_value;
	float					Paracube_O2_value;
	float					Test_5V;
	float					ThermistorRes;
	float					Temperature;

	float					patientPTemp;
	float					patientP;
}IO_STATUS_t;




/**********************************************************************************************************
 *
 **********************************************************************************************************/
extern IO_STATUS_t				io_status_st_glb;


/**********************************************************************************************************
 *
 **********************************************************************************************************/
void 		api_pwm_Air(uint16_t value);
void 		api_pwm_O2(uint16_t value);
void 		api_pwm_VenturiFlow(uint16_t value);

void 		api_pwm_Air_IRQ(uint16_t value);
void 		api_pwm_O2_IRQ(uint16_t value);
void 		api_pwm_VenturiFlow_IRQ(uint16_t value);

void 		api_valve_nebuliser(Valve_Nebuliser_enum on_off );
void 		api_valve_O2_calibrate(Valve_O2_Cal_enum on_off );
void 		api_valve_Venturi_En(Valve_Venturi_En_enum en_dis );

uint8_t 	api_io_rd_AirSupply(uint8_t renew);
uint8_t 	api_io_rd_O2Supply(uint8_t renew);

uint8_t 	api_io_fan(uint8_t fanspeed );

void 		api_io_safety_cutout(Safty_Cutout_enum en_dis);
void 		api_io_power_sys(uint8_t en_dis);
void 		api_io_power_5V_sensor(uint8_t en_dis);

void		api_print_status(TERMINAL_PRINT_enum  print_mode);

void		api_IO_test(void);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
