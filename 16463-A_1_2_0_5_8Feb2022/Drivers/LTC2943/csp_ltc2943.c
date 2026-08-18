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
 * Date Created:  Tue 10 Nov 2015 10:37:46 AM
 * Programmer  :  Aaron Lynn
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "csp_ltc2943.h"
#include "csp_STM32_I2Cx.h"

uint16_t    		battery_tmr = 0;
I2C_TypeDef* 		FUEL_GAUGE_I2Cx = I2C2;
uint8_t				i2c_debug	=  0;


/**********************************************************************************************************
* Function Name : csp_ltc2943_RdReg_1b
* Description   : This function is used to through put flags
* Arguments     : F1
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		        10/05/2016	Aaron Lynn      	Original Created
**********************************************************************************************************/
uint8_t csp_ltc2943_RdReg_1b(uint8_t reg, uint8_t *data)
{
	uint8_t i2c_status;
	uint8_t buf[3];

	buf[0] = reg; // read the status register.

	i2c_status	=  (uint8_t)I2C_BufferWrRd(FUEL_GAUGE_I2Cx, buf, 1, 1, FUEL_GAUGE_ADD);
	*data		=  buf[0];

    if(i2c_debug && !i2c_status){
    	printf("\r\nData Reg Read1: %02X %02X (%d)",reg,buf[0],i2c_status);
    }
	return (!i2c_status);
}

/**********************************************************************************************************
* Function Name : csp_ltc2943_RdReg_2b
* Description   : This function is used to through put flags
* Arguments     : F1
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		        10/05/2016	Aaron Lynn      	Original Created
**********************************************************************************************************/
uint8_t csp_ltc2943_RdReg_2b(uint8_t reg, uint16_t *data)
{
	uint8_t i2c_status;
	uint8_t buf[4];
	union{
		uint8_t		b[2];
		uint16_t	a;
	}u2b;

	/*code*/
	buf[0] = reg;

	i2c_status	=  (uint8_t)I2C_BufferWrRd(FUEL_GAUGE_I2Cx, buf, 1, 2, FUEL_GAUGE_ADD);

	u2b.b[1]	=  buf[0];
	u2b.b[0]	=  buf[1];
	*data		=  u2b.a;
	if(i2c_debug && !i2c_status){
        printf("\r\nData Reg Read2: %02X %04X (%d)",reg,u2b.a,i2c_status);
    }
	return(!i2c_status);
}

/**********************************************************************************************************
* Function Name : csp_ltc2943_WrReg_1b
* Description   : This function is used to through put flags
* Arguments     : F1
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		        10/05/2016	Aaron Lynn      	Original Created
**********************************************************************************************************/
uint8_t csp_ltc2943_WrReg_1b(uint8_t reg, uint8_t data)
{
	uint8_t i2c_status;
	uint8_t buf[3];

	/*code*/
	buf[0] = reg;
	buf[1] = data;

	i2c_status	=  (uint8_t)I2C_BufferWr(FUEL_GAUGE_I2Cx, buf, 2, FUEL_GAUGE_ADD);
	if(i2c_debug && !i2c_status){
        printf("\r\nData Reg Write1: %02X %02X (%d)",buf[0],buf[1],i2c_status);
    }
    return(!i2c_status);
}

/**********************************************************************************************************
* Function Name : csp_ltc2943_WrReg_2b
* Description   : This function is used to through put flags
* Arguments     : F1
* Returns       : status
* Notes         : returns
*
* Version		Date d/m/y	Programmer		Reason for Change
* 1.0.0		        10/05/2016	Aaron Lynn      	Original Created
**********************************************************************************************************/
uint8_t csp_ltc2943_WrReg_2b(uint8_t reg, uint16_t data)
{
	uint8_t i2c_status = 0;
	uint8_t buf[3];
	union{
		uint8_t		b[2];
		uint16_t	a;
	}u2b;

	u2b.a	=  data;
	buf[0] = reg;
	buf[1] = u2b.b[1];
	buf[2] = u2b.b[0];

	i2c_status	=  (uint8_t)I2C_BufferWr(FUEL_GAUGE_I2Cx, buf, 3, FUEL_GAUGE_ADD);
	if(i2c_debug && !i2c_status){
        printf("\r\nData Reg Write2: %02X %02X (%d)",buf[0],buf[1],i2c_status);
    }
    return (!i2c_status);
}



/**********************************************************************************************************
* Function Name : csp_ltc2943_tmr
* Description   : This function is used to through put flags
* Arguments     : arguments
* Returns       : status
* Notes         : returns
*
* Version	Date d/m/y	Programmer		Reason for Change
* 1.0.0		10/05/2016	Aaron Lynn      Original Created
**********************************************************************************************************/
void csp_ltc2943_tmr(void){
	battery_tmr++;
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
