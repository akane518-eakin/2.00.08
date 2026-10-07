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
 * Filename    :  csp_ltc2943.c
 * Date Created:  Mon 11 Sept 2017 16:06:00 PM
 * Programmer  :  Alex Lynn
 * Description :  This module is
 *
 **********************************************************************************************************/
#ifndef _CSP_LTC2943_H
#define _CSP_LTC2943_H

//#include "i2croutines.h"


#define FUEL_GAUGE_ADD         0x64<<1	//7bit address now 8bit address


/*********************************************************************************************************
 ********************************************************************************************************/
#define STATUS_REG              (uint8_t)0x00    //Read Only
#define CONTROL_REG             (uint8_t)0x01    //R/W
#define ACC_CHARGE_MSB          (uint8_t)0x02    //R/W
#define ACC_CHARGE_LSB          (uint8_t)0x03    //R/W

#define CHARGE_THRESH_H_MSB     (uint8_t)0x04    //R/W
#define CHARGE_THRESH_H_LSB     (uint8_t)0x05    //R/W
#define CHARGE_THRESH_L_MSB     (uint8_t)0x06    //R/W
#define CHARGE_THRESH_L_LSB     (uint8_t)0x07    //R/W

#define VOLTAGE_MSB             (uint8_t)0x08    //Read Only
#define VOLTAGE_LSB             (uint8_t)0x09    //Read Only
#define VOLTAGE_THRESH_H_MSB    (uint8_t)0x0A    //R/W
#define VOLTAGE_THRESH_H_LSB    (uint8_t)0x0B    //R/W
#define VOLTAGE_THRESH_L_MSB    (uint8_t)0x0C    //R/W
#define VOLTAGE_THRESH_L_LSB    (uint8_t)0x0D    //R/W

#define CURRENT_MSB             (uint8_t)0x0E    //Read Only
#define CURRENT_LSB             (uint8_t)0x0F    //Read Only
#define CURRENT_THRESH_H_MSB    (uint8_t)0x10    //R/W
#define CURRENT_THRESH_H_LSB    (uint8_t)0x11    //R/W
#define CURRENT_THRESH_L_MSB    (uint8_t)0x12    //R/W
#define CURRENT_THRESH_L_LSB    (uint8_t)0x13    //R/W

#define TEMPERATURE_MSB         (uint8_t)0x14    //Read Only
#define TEMPERATURE_LSB         (uint8_t)0x15    //Read Only
#define TEMPERATURE_THRESH_H    (uint8_t)0x16    //R/W
#define TEMPERATURE_THRESH_L    (uint8_t)0x17    //R/W

#define UVLA                    0       //Indicates recovery from undervoltage. If set to 1, a UVLO has occurred and the contents of the registers are uncertain
#define VOLTAGE_ALERT           1       //Indicates one of the voltage limits was exceeded
#define CHAL                    2       //Indicates that the ACR value exceeded the charge threshold low limit
#define CHAH                    3       //Indicates that the ACR value exceeded the charge threshold high limit
#define TEMP_ALERT              4       //Indicates one of the temperature limits was exceeded
#define ACO_U                   5       //Indicates that the value of the ACR hit either top or bottom
#define CUR_ALERT               6       //Indicates one of the current limits was exceeded


/*****************************Control Register B*******************************/

//ADC MODE
#define ADC_MODE_AUTO           0xC0
#define ADC_MODE_SCAN           0x80
#define ADC_MODE_MANUAL         0x40
#define ADC_MODE_SLEEP          0x00

//PRESCALER M
#define PRESCALE_1              0x00
#define PRESCALE_4              0x08
#define PRESCALE_16             0x10
#define PRESCALE_64             0x18
#define PRESCALE_256            0x20
#define PRESCALE_1024           0x28
#define PRESCALE_4096_1         0x30
#define PRESCALE_4096_2         0x38

//ALCC CONFIG
#define ALCC_ALERT              0x04
#define ALCC_CHARGE_COMPLE      0x02
#define ALCC_DISABLED           0x00

//Shutdown
#define SHUTDOWN_ACTIVE         0x01
#define SHUTDOWN_DEACTIVE       0x00

//extern	I2C_TypeDef* 	FUEL_GAUGE_I2Cx;
extern	uint16_t    	battery_tmr;
/*********************************************************************************************************
		Functions
 ********************************************************************************************************/

uint8_t		csp_ltc2943_RdReg_1b(uint8_t reg, uint8_t *data);
uint8_t 	csp_ltc2943_RdReg_2b(uint8_t reg, uint16_t *data);

uint8_t		csp_ltc2943_WrReg_1b(uint8_t reg, uint8_t data);
uint8_t		csp_ltc2943_WrReg_2b(uint8_t reg, uint16_t data);

void		csp_ltc2943_tmr(void);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
