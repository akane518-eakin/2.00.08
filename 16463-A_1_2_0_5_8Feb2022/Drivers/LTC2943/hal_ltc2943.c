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
 * Filename    :  hal_ltc2943.c
 * Date Created:  Tue 08 Sept 2017 12:00:00 PM
 * Programmer  :  Aaron Lynn / Alex Lynn
 * Description :  This module is drivers for the LTC2943 chip
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "hal_ltc2943.h"
#include "csp_ltc2943.h"
#include "pcb_pins.h"

//#include "i2croutines.h"
#include "csp_STM32_uart.h"

FuelGauge_st			FuelGauge;
FuelGaugeReg_st			FuelGaugeReg;


/*************************************************************************************************
* Function Name : 	hal_ltc2943_shutdown
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t hal_ltc2943_shutdown(void)
{
	uint8_t		i2c_status;
	i2c_status	=  csp_ltc2943_WrReg_1b(CONTROL_REG,(ADC_MODE_AUTO|PRESCALE_REG|ALCC_DISABLED|SHUTDOWN_ACTIVE));

	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_init
* Description   : This function is used to through put flags
* Arguments     : F1
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		        10/05/2016	Aaron Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_init(void)
{
	uint8_t		i2c_status	=  0;
/*
	FuelGauge.alarm.charge_h		=  BAT_CAPACITY	* 0.9;			//Charge threshold high
	FuelGauge.alarm.charge_l		=  BAT_CAPACITY * 0.1;			//Charge threshold low
	FuelGauge.alarm.voltage_h		=  13;							//Voltage threshold high
	FuelGauge.alarm.voltage_l		=  10.5;						//Voltage threshold low
	FuelGauge.alarm.current_h		=  3;							//Current threshold high
	FuelGauge.alarm.current_l		=  0;							//Current threshold low
	FuelGauge.alarm.temperature_h	=  50;
	FuelGauge.alarm.temperature_l	=  -20;
*/
	FuelGaugeReg.charge_thres_h.a		=  hal_ltc2943_formula_mAh_to_reg(FuelGauge.alarm.charge_h);
	FuelGaugeReg.charge_thres_l.a		=  hal_ltc2943_formula_mAh_to_reg(FuelGauge.alarm.charge_l);
	FuelGaugeReg.voltage_thres_h.a		=  hal_ltc2943_formula_volt_to_reg(FuelGauge.alarm.voltage_h);
	FuelGaugeReg.voltage_thres_l.a		=  hal_ltc2943_formula_volt_to_reg(FuelGauge.alarm.voltage_l);
	FuelGaugeReg.current_thres_h.a		=  hal_ltc2943_formula_cur_to_reg(FuelGauge.alarm.current_h);
	FuelGaugeReg.current_thres_l.a		=  hal_ltc2943_formula_cur_to_reg(FuelGauge.alarm.current_l);
	FuelGaugeReg.temperature_thres_h	=  hal_ltc2943_formula_temp_to_reg(FuelGauge.alarm.temperature_h);
	FuelGaugeReg.temperature_thres_l	=  hal_ltc2943_formula_temp_to_reg(FuelGauge.alarm.temperature_l);

	i2c_status	+= hal_ltc2943_control_write(ADC_MODE_AUTO|PRESCALE_REG|ALCC_DISABLED|SHUTDOWN_DEACTIVE);

	i2c_status	+= hal_ltc2943_charge_threshold_write();
	i2c_status	+= hal_ltc2943_voltage_threshold_write();
	i2c_status	+= hal_ltc2943_current_threshold_write();
	i2c_status	+= hal_ltc2943_temp_threshold_write();

	i2c_status	+= hal_ltc2943_status_read();

	return(i2c_status);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_set_charge
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t hal_ltc2943_set_charge(float fraction)
{
	// Local Variables
	uint16_t	reg;
	uint8_t		i2c_status;
	
	// Code
	if(fraction	>  1.0){	fraction	=  1.0;		}
	if(fraction	<  0.0){	fraction	=  0.0;		}
	
	reg			=  hal_ltc2943_formula_mAh_to_reg(BAT_CAPACITY * fraction);
	i2c_status	=  hal_ltc2943_charge_write(reg);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_status_read
* Description   : This function is used to read the status byte of the battery
* Arguments     : Status_alert_union *Status_union
* Returns       : uint8_t value
* Notes         :
*
* Version	Date d/m/y		Programmer		Reason for Change
* 1.0.0		07/09/2017		Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_status_read(void)
{
	uint8_t		i2c_status;
	
	i2c_status	=  csp_ltc2943_RdReg_1b(STATUS_REG,&FuelGaugeReg.status.a); //read register and put it into 'status' which will be split into individual bits by the union
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_control_read
* Description   : This function is used to read the control
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		    11/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_control_read(void)
{
	uint8_t		i2c_status;
	
	i2c_status	=  csp_ltc2943_RdReg_1b(CONTROL_REG,&FuelGaugeReg.control);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_control_write
* Description   : This function is used to write to the control register location
* Arguments     : arguments
* Returns       : status
* Notes         : returns 0 if the data is invalid (bit 1 and 2 must never be BINARY 11) - see register B on data sheet for futher info
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		    11/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_control_write(uint8_t data)
{
	uint8_t		i2c_status;
	
	if(data & 0x06){ // bit 1 and 2 must never be BIN11, AND this with 0x06 will check if it is there or not
		printf("\r\nFuel gauge - Config error");
		data &= 0xF9;	//set ALCC to disabled
	}
	i2c_status	=  csp_ltc2943_WrReg_1b(CONTROL_REG, data);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_charge_read
* Description   : This function is used to read the accumulated charge bytes
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		    08/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_charge_read(void)
{
	// Local Variables
	uint8_t		i2c_status;
	float		per;
	
	// Code
	i2c_status				=  csp_ltc2943_RdReg_2b(ACC_CHARGE_MSB,&FuelGaugeReg.charge.a);
	FuelGauge.act_mAh		=  hal_ltc2943_formula_reg_to_mAh(FuelGaugeReg.charge.a);
	
	if(FuelGauge.act_mAh	<  0.0){			// Should never happen
		FuelGauge.act_mAh	=  0;
		hal_ltc2943_set_charge(0.0);
	}
	
	if(FuelGauge.act_mAh	>  BAT_CAPACITY){
		FuelGauge.act_mAh	=  BAT_CAPACITY;
		hal_ltc2943_set_charge(1.0);
	}
	
	per						=  FuelGauge.act_mAh;
	per						/= BAT_CAPACITY;
	per						*= 100.0;
	
	FuelGauge.percentage	=  per;
	if(FuelGauge.percentage	>  100.0){			// Should never happen
		FuelGauge.percentage=  100.0;
	}
	
	return(i2c_status);
}

/**********************************************************************************************************
* Function Name : hal_ltc2943_charge_write
* Description   : This function is used to write to ACC Charge register
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		    11/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_charge_write(uint16_t data)
{
	// Local Variables
	uint8_t		i2c_status	=  0;
	uint8_t		temp;
	
	// Code
	i2c_status	+= csp_ltc2943_RdReg_1b(CONTROL_REG,&temp);	// read control reg
	temp		|= 0x01;									// write control reg, 1 bit (to allow for write to happen - see data sheet for more info)
	i2c_status	+= csp_ltc2943_WrReg_1b(CONTROL_REG, temp);
	
	i2c_status	+= csp_ltc2943_WrReg_2b(ACC_CHARGE_MSB, data);
	
	temp		&= 0xFE;				//get rid of the 1, put the control register back in to regular use (i.e. not write mode)
	i2c_status	+= csp_ltc2943_WrReg_1b(CONTROL_REG, temp);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_charge_threshold_read
* Description   : This function is used to read the charge threshold high bytes
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version	Date d/m/y	Programmer		Reason for Change
* 1.0.0		08/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_charge_threshold_read(void)
{
	uint8_t		i2c_status	=  0;
	
	i2c_status	+= csp_ltc2943_RdReg_2b(CHARGE_THRESH_L_MSB,&FuelGaugeReg.charge_thres_l.a);
	i2c_status	+= csp_ltc2943_RdReg_2b(CHARGE_THRESH_H_MSB,&FuelGaugeReg.charge_thres_h.a);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_charge_threshold_write
* Description   : This function is used to write to Charge threshold high register
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		    11/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_charge_threshold_write(void)
{
	uint8_t		i2c_status	=  0;
	
	i2c_status	+= csp_ltc2943_WrReg_2b(CHARGE_THRESH_L_MSB,	FuelGaugeReg.charge_thres_l.a);
	i2c_status	+= csp_ltc2943_WrReg_2b(CHARGE_THRESH_H_MSB, 	FuelGaugeReg.charge_thres_h.a);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_voltage_read
* Description   : This function is used to through put flags
* Arguments     :
* Returns       : status
* Notes         : returns
*
* Version	Date d/m/y	Programmer	Reason for Change
* 1.0.0		10/05/2016	Aaron Lynn  Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_voltage_read(void)
{
	uint8_t		i2c_status;
	
	i2c_status	=  csp_ltc2943_RdReg_2b(VOLTAGE_MSB,&FuelGaugeReg.voltage.a);  //20 cycle
	FuelGauge.act_voltage	=  hal_ltc2943_formula_reg_to_volt(FuelGaugeReg.voltage.a);
	
	return(i2c_status);
}

/**********************************************************************************************************
* Function Name : hal_ltc2943_voltage_threshold_read
* Description   : This function is used to read the voltage threshold high bytes
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version	Date d/m/y	Programmer		Reason for Change
* 1.0.0		08/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_voltage_threshold_read(void)
{
	uint8_t		i2c_status	=  0;
	
	i2c_status	+= csp_ltc2943_RdReg_2b(VOLTAGE_THRESH_L_MSB,&FuelGaugeReg.voltage_thres_l.a);
	i2c_status	+= csp_ltc2943_RdReg_2b(VOLTAGE_THRESH_H_MSB,&FuelGaugeReg.voltage_thres_h.a);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_voltage_threshold_write
* Description   : This function is used to write to voltage threshold high register
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		    11/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_voltage_threshold_write(void)
{
	uint8_t		i2c_status	=  0;
	
	i2c_status	+=  csp_ltc2943_WrReg_2b(VOLTAGE_THRESH_L_MSB, FuelGaugeReg.voltage_thres_l.a);
	i2c_status	+=  csp_ltc2943_WrReg_2b(VOLTAGE_THRESH_H_MSB, FuelGaugeReg.voltage_thres_h.a);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_current_read
* Description   : This function is used to through put flags
* Arguments     : F1
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		        10/05/2016	Aaron Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_current_read(void)
{
	static uint8_t	cur_polarity_last	=  0xff;
	uint8_t		cur_polarity;
	uint8_t		i2c_status;
	
	i2c_status	=  csp_ltc2943_RdReg_2b(CURRENT_MSB,&FuelGaugeReg.current.a);
	FuelGauge.act_current	=  hal_ltc2943_formula_reg_to_cur(FuelGaugeReg.current.a);
	
	if(FuelGauge.act_current > 0.0){	cur_polarity	=  1;	}
	else{								cur_polarity	=  0;	}
		
	if(	cur_polarity != cur_polarity_last){
		cur_polarity_last	=  cur_polarity;
		
		for(FuelGauge.av_current.pos=0;FuelGauge.av_current.pos<LTC2943_HIS;FuelGauge.av_current.pos++){
			FuelGauge.av_current.his[FuelGauge.av_current.pos]	=  0;	
		}
		FuelGauge.av_current.pos	=  0;
		FuelGauge.av_current.cnt	=  0;
		FuelGauge.av_current.tot	=  0;
	}
	
	FuelGauge.av_current.tot	-= FuelGauge.av_current.his[FuelGauge.av_current.pos];
	FuelGauge.av_current.his[FuelGauge.av_current.pos]	=  FuelGauge.act_current;
	FuelGauge.av_current.tot	+= FuelGauge.av_current.his[FuelGauge.av_current.pos];
	
	if(++FuelGauge.av_current.pos >= LTC2943_HIS){	FuelGauge.av_current.pos	=  0;		}
	if(++FuelGauge.av_current.cnt >= LTC2943_HIS){	FuelGauge.av_current.cnt	=  LTC2943_HIS;	}
		
	FuelGauge.av_current.av		=  FuelGauge.av_current.tot;
	FuelGauge.av_current.av		/= FuelGauge.av_current.cnt;
	
	if(FuelGauge.act_current < 0){	FuelGauge.charging = 0;	}
	else{							FuelGauge.charging = 1;	}// Battery is Charging.
		
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_current_t_high_read
* Description   : This function is used to read the current threshold high bytes
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version	Date d/m/y	Programmer		Reason for Change
* 1.0.0		08/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_current_threshold_read(void)
{
	uint8_t		i2c_status	=  0;
	
	i2c_status	+= csp_ltc2943_RdReg_2b(CURRENT_THRESH_L_MSB,&FuelGaugeReg.current_thres_l.a);
	i2c_status	+= csp_ltc2943_RdReg_2b(CURRENT_THRESH_H_MSB,&FuelGaugeReg.current_thres_h.a);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_current_threshold_write
* Description   : This function is used to write to current threshold high register
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		    11/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_current_threshold_write(void)
{
	uint8_t		i2c_status	=  0;
	
	i2c_status	+= csp_ltc2943_WrReg_2b(CURRENT_THRESH_L_MSB,FuelGaugeReg.current_thres_l.a);
	i2c_status	+= csp_ltc2943_WrReg_2b(CURRENT_THRESH_H_MSB,FuelGaugeReg.current_thres_h.a);
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_temp_read
* Description   : This function is used to through put flags
* Arguments     : F1
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y		Programmer		Reason for Change
* 1.0.0		    10/05/2016		Aaron Lynn      Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_temp_read(void)
{
	uint8_t		i2c_status;
	
	i2c_status	=  csp_ltc2943_RdReg_2b(TEMPERATURE_MSB,&FuelGaugeReg.temperature.a);
	FuelGauge.act_temperature	=  hal_ltc2943_formula_reg_to_temp(FuelGaugeReg.temperature.a);
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_temp_threshold_read
* Description   : This function is used to read the current threshold low bytes
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version	Date d/m/y	Programmer		Reason for Change
* 1.0.0		11/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_temp_threshold_read(void)
{
	uint8_t		i2c_status	=  0;
	
	i2c_status	+= csp_ltc2943_RdReg_1b(TEMPERATURE_THRESH_H,&FuelGaugeReg.temperature_thres_h);
	i2c_status	+= csp_ltc2943_RdReg_1b(TEMPERATURE_THRESH_L,&FuelGaugeReg.temperature_thres_l);
	
	return(i2c_status);
}


/**********************************************************************************************************
* Function Name : hal_ltc2943_temp_threshold_write
* Description   : This function is used to write to temperature threshold (high and low) register
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		    11/09/2017	Alex Lynn      	Original Created
**********************************************************************************************************/
uint8_t hal_ltc2943_temp_threshold_write(void)
{
	uint8_t		i2c_status	=  0;
	
	i2c_status 	+= csp_ltc2943_WrReg_1b(TEMPERATURE_THRESH_H,	FuelGaugeReg.temperature_thres_h);
	i2c_status 	+= csp_ltc2943_WrReg_1b(TEMPERATURE_THRESH_L,	FuelGaugeReg.temperature_thres_l);
	
	return(i2c_status);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_formula_reg_to_temp
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
float hal_ltc2943_formula_reg_to_temp(uint16_t reg)
{
	float	temp;
	
	temp	=  reg;
	temp	/= 65535;
	temp	*= 510;
	temp	-= 273;
	
//	printf("\r\nreg->temp %4x %f",reg,temp);
	
	return(temp);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_formula_temp_to_reg
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t hal_ltc2943_formula_temp_to_reg(float temp)
{
	float	x;
	union{
		uint8_t		b[2];
		uint16_t	a;
	}reg;
	
	x	=  temp;
	x	+= 273;
	x	/= 510;
	x	*= 65535;
	reg.a	=  (uint16_t)x;
	
//	printf("\r\ntemp->reg %f (%4x)%2x",temp, reg.a,reg.b[1]);
	
	return(reg.b[1]);	//only the high byte
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_formula_reg_to_cur
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
float hal_ltc2943_formula_reg_to_cur(uint16_t reg)
{
	float	cur;
	
	cur		=  reg;
	cur		-= 0x7fff;
	cur		/= 0x7fff;
	cur		*= 60;
	cur		/= R_SENSE;
	
//	printf("\r\nreg->cur %4x %f",reg,cur);
	
	return(cur);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_formula_cur_to_reg
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
uint16_t hal_ltc2943_formula_cur_to_reg(float cur)
{
	float		x;
	uint16_t	reg;
	
	x	=  cur;
	x	*= R_SENSE;
	x	/= 60;
	x	*= 0x7fff;
	x	+= 0x7fff;
	reg	=  (uint16_t)x;
	
//	printf("\r\ncur->reg %f %4x",cur, reg);
	
	return(reg);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_formula_reg_to_volt
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
float hal_ltc2943_formula_reg_to_volt(uint16_t reg)
{
	float	volt;
	
	volt	=  reg;
	volt	/= 0xffff;
	volt	*= 23.6;
	
//	printf("\r\nreg->volt %4x %f",reg,volt);
	
	return(volt);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_formula_volt_to_reg
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
uint16_t hal_ltc2943_formula_volt_to_reg(float volt)
{
	float		x;
	uint16_t	reg;
	
	x	=  volt;
	x	/= 23.6;
	x	*= 0xffff;
	reg	=  (uint16_t)x;
	
//	printf("\r\nvolt->reg %f %4x",volt, reg);
	
	return(reg);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_formula_reg_to_mAh
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
float hal_ltc2943_formula_reg_to_mAh(uint16_t reg)
{
	float	mAh;
	
	if(reg	>  LTC_OFFSET){
		reg	-= LTC_OFFSET;
	}
	else{
		reg	=  0;
	}
	
	mAh		=  (float)reg;			//assumes that 0= empty
	mAh		*= 0.34;
	mAh		*= 50.0;
	mAh		/= R_SENSE;
	mAh		*= PRESCALE_VALUE;
	mAh		/= 4096.0;
	
//	printf("\r\nreg->mAh %4x %f",reg,mAh);
	
	return(mAh);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_formula_mAh_to_reg
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
uint16_t hal_ltc2943_formula_mAh_to_reg(float mAh)
{
	float		x;
	uint16_t	reg;
	
	x	=  mAh;
	x	*= 4096;
	x	/= PRESCALE_VALUE;
	x	*= R_SENSE;
	x	/= 50;
	x	/= 0.34;
	reg	=  (uint16_t)x;
	reg	+= LTC_OFFSET;
	
//	printf("\r\nmAh->reg %f %4x",mAh, reg);
	
	return(reg);
}


/*************************************************************************************************
* Function Name : 	hal_ltc2943_time_remaining
* Description   : 	This Function calculates number of seconds until fully charged or battery empty
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		02/10/17	W. Paul			Created
*************************************************************************************************/
void hal_ltc2943_time_remaining(void)
{
	float			rem;
	
	if(FuelGauge.act_current < 0){
		//discharging battery
		//cal time left in battery
		if(FuelGauge.av_current.av >= 0){	//HAVE TO WAIT ON AV RESULT CHANGING
			rem		=  60*60;
		}
		else{
			rem		=  FuelGauge.act_mAh;
			rem		/= 1000;
			rem		/= (FuelGauge.av_current.av * -1);
			rem		*= 3600;
		}
	}
	else{
		//charging battery
		//cal time to full battery
		if(FuelGauge.av_current.av <= 0){	//HAVE TO WAIT ON AV RESULT CHANGING
			rem		=  60*60;
		}
		else{
			rem		=  BAT_CAPACITY;
			rem		-= FuelGauge.act_mAh;
			rem		/= 1000;
			rem		/= FuelGauge.av_current.av;
			rem		*= 3600;
		}
		
		rem		=  60*60;
	}
	
//	printf("\r\n%f %d",rem,settle_cnt);
	
	FuelGauge.sec_remaining		=  (uint32_t)rem;
	
	return;
}


/*
*********************************************************************************************************
*											End of hal_ltc2943.c
*********************************************************************************************************
*/
