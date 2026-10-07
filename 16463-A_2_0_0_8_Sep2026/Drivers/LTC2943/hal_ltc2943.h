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
 *  Copyright 2015, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  hal_ltc2943.h
 * Date Created:  Tue 10 Nov 2015 10:37:53 AM
 * Programmer  :  William Paul / Aaron Lynn / Alex Lynn
 * Description :  This module is the h file for the drivers written in the c file
 *
 **********************************************************************************************************/
#ifndef _HAL_LTC2943_H
#define _HAL_LTC2943_H

#include "csp_ltc2943.h"

/*
rsense = 50mV/Imax		=  50mV/5A	=	0.01	Ohms


M	> 4096.Qbat/(65535*0.034)*Rsense/50mR
M	> 4096*2600/65535/0.034*0.01/0.05
M	> 95.6
M	= 256

qLSB	=  0.34 * 0.05/rsense * M/4096
qLSB	=  0.34 * 0.05/0.01*256/4096
qLSB	=  0.10625 mAh

*/

#define		R_SENSE					10		// mOHMS
#define		BAT_CAPACITY			2400	// mAh		actually 2600mAh but set low to allow some space (now battery will not switch off due to low voltage)
#define		PRESCALE_VALUE			256
#define		PRESCALE_REG			PRESCALE_256
#define		LTC_CAPACITY			23600	// ( (BAT_CAPACITY * (4096/PRESCALE_VALUE) * (R_SENSE / 50) ) / 0.34) + 1000 = 23588 ... add very small margin
#define		LTC_OFFSET				1000
#define		LTC_FAULT				25000

#define		VBAT_MIN				10.5
#define		VBAT_CUT				10.0
#define		VBAT_FAULT				8.4




/*********************************************************************************************************
 *		Union & Structures
 ********************************************************************************************************/
typedef union{
	uint8_t		a;
	struct{
		uint8_t 	Reserved_byte				:1; 	//no current use
		uint8_t 	Current_alert				:1; 	//default 0
		uint8_t 	Acc_charge_over_under		:1; 	//default 0
		uint8_t 	Temperature_alert			:1;		//default 0
		uint8_t 	Charge_high_alert			:1; 	//default 0
		uint8_t 	Charge_low_alert			:1; 	//default 0
		uint8_t 	Voltage_alert				:1; 	//default 0
		uint8_t 	UnderVoltageLockOut_alert	:1; 	//default 1
	}bits;
}StatusAlert_u;

typedef union{
	uint8_t		b[2];
	uint16_t	a;
	int16_t		ua;
}U2B;




typedef struct{
	StatusAlert_u	status;
	uint8_t 		control;

	U2B		charge;						// charge
	U2B		charge_thres_h;			//Charge threshold high
	U2B		charge_thres_l;			//Charge threshold low

	U2B		voltage;					//Voltage
	U2B		voltage_thres_h;			//Voltage threshold high
	U2B		voltage_thres_l;				//Voltage threshold low

	U2B		current;					//Current
	U2B		current_thres_h;			//Current threshold high
	U2B		current_thres_l;			//Current threshold low

	U2B		temperature;				//Temperature
	uint8_t	temperature_thres_h;
	uint8_t	temperature_thres_l;

}FuelGaugeReg_st;

#define		LTC2943_HIS		12
typedef struct{
	float		his[LTC2943_HIS];
	float		tot;
	uint8_t		pos;
	uint8_t		cnt;
	float		av;
}AV_t;


typedef struct{
	struct{
		float	charge_h;			//Charge threshold high
		float	charge_l;			//Charge threshold low
		float	voltage_h;			//Voltage threshold high
		float	voltage_l;			//Voltage threshold low
		float	current_h;			//Current threshold high
		float	current_l;			//Current threshold low
		float	temperature_h;
		float	temperature_l;
	}alarm;

	float		act_mAh;
	float		act_voltage;
	float		act_current;
	float		act_temperature;

	AV_t		av_current;

	uint8_t		charging;		// Indication if the battery is charging or not
	float		percentage;		// Indicates the battery Charge percentage
	uint32_t	sec_remaining;
}FuelGauge_st;



//FuelGaugeReg.batt
extern FuelGaugeReg_st		FuelGaugeReg;
extern FuelGauge_st			FuelGauge;


/*********************************************************************************************************
 *		Functions
 ********************************************************************************************************/
uint8_t 	hal_ltc2943_shutdown(void);

uint8_t		hal_ltc2943_init(void);
uint8_t		hal_ltc2943_set_charge(float fraction);


uint8_t 	hal_ltc2943_status_read(void);

uint8_t 	hal_ltc2943_control_read(void);
uint8_t		hal_ltc2943_control_write(uint8_t data);

uint8_t		hal_ltc2943_charge_read(void);
uint8_t		hal_ltc2943_charge_write(uint16_t data);
uint8_t		hal_ltc2943_charge_threshold_read(void);
uint8_t		hal_ltc2943_charge_threshold_write(void);

uint8_t		hal_ltc2943_voltage_read(void);
uint8_t		hal_ltc2943_voltage_threshold_read(void);
uint8_t		hal_ltc2943_voltage_threshold_write(void);

uint8_t		hal_ltc2943_current_read(void);
uint8_t		hal_ltc2943_current_threshold_read(void);
uint8_t		hal_ltc2943_current_threshold_write(void);

uint8_t		hal_ltc2943_temp_read(void);
uint8_t		hal_ltc2943_temp_threshold_read(void);
uint8_t		hal_ltc2943_temp_threshold_write(void);

float		hal_ltc2943_formula_reg_to_temp(uint16_t reg);
uint8_t 	hal_ltc2943_formula_temp_to_reg(float temp);
float		hal_ltc2943_formula_reg_to_cur(uint16_t reg);
uint16_t	hal_ltc2943_formula_cur_to_reg(float cur);
float 		hal_ltc2943_formula_reg_to_volt(uint16_t reg);
uint16_t 	hal_ltc2943_formula_volt_to_reg(float volt);
float 		hal_ltc2943_formula_reg_to_mAh(uint16_t reg);
uint16_t 	hal_ltc2943_formula_mAh_to_reg(float mAh);

void 		hal_ltc2943_time_remaining(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
