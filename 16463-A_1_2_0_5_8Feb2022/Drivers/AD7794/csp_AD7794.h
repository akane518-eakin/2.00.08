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
*                  	Copyright 2012,Marturion Ltd, Lisburn, Co. Antrim, Northern Ireland
*                                          All Rights Reserved
*
*                                          csp_AD7794.h
*
* Filename    :  csp_AD7794.h
* Programmer  :  Pauric Lynch
* Description :
* Compiler    :  IAR
* Target      :  STM32F107
* Version     :  Version 1.1.0
**********************************************************************************************************
*/
#ifndef _CSP_AD7794_H
#define _CSP_AD7794_H

/*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************/
#include "stdint.h"

#define	AD7794_CH0_MAX250_O2_Sensor		0
#define	AD7794_CH1_O2_FLOW				1
#define	AD7794_CH2_AIR_FLOW				2
#define	AD7794_CH3_PATIENT_PRESSURE		3
#define	AD7794_CH4_5V					4
#define	AD7794_CH5_TEMPERATURE			5


#define AD7794_VREF_VALUE			4.096
/*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************/

/****** MODE REGISTER ******/
#define REG_RD						0x40
#define REG_WR						0x00
#define REG_CREAD					0x04

#define REG_STATUS					0x00
#define REG_MODE					0x08
#define REG_CONFIG					0x10
#define REG_DATA					0x18
#define REG_ID						0x20
#define REG_IO						0x28
#define REG_OFFSET					0x30
#define REG_FULL_SCALE				0X38

/****** MODE REGISTER ******/
#define REG_MODE_CONTINUOUS			0x0000
#define REG_MODE_SINGLE				0x2000
#define REG_MODE_IDLE				0x4000
#define REG_MODE_POWER_DOWN			0x6000
#define REG_MODE_INT_ZERO_CALB		0x8000
#define REG_MODE_INT_FULL_CALB		0xA000
#define REG_MODE_SYS_ZERO_CALB		0xC000
#define REG_MODE_SYS_FULL_CALB		0xE000

#define	REG_MODE_R0					0x0000
#define REG_MODE_R500          		0x0001
#define REG_MODE_R250          		0x0002
#define REG_MODE_R125          		0x0003
#define REG_MODE_R62_5         		0x0004
#define REG_MODE_R50           		0x0005
#define REG_MODE_R39_2         		0x0006
#define REG_MODE_R33           		0x0007
#define REG_MODE_R19_6         		0x0008
#define REG_MODE_R16_7_50Hz    		0x0009
#define REG_MODE_R16_7         		0x000A
#define REG_MODE_R12_5         		0x000B
#define REG_MODE_R10           		0x000C
#define REG_MODE_R8_33         		0x000D
#define REG_MODE_R6_25         		0x000E
#define REG_MODE_R4_17         		0x000F

/****** CONFIGURATION REGISTER ******/
#define REG_CONF_UNIPOLAR			0x1000
#define REG_CONF_BIPOLAR			0x0000

#define	REG_CONF_GAIN1         		0x0000
#define REG_CONF_GAIN2         		0x0100
#define REG_CONF_GAIN4         		0x0200
#define REG_CONF_GAIN8         		0x0300
#define REG_CONF_GAIN16        		0x0400
#define REG_CONF_GAIN32        		0x0500
#define REG_CONF_GAIN64        		0x0600
#define REG_CONF_GAIN128       		0x0700

#define REG_CONF_REF_INT    		0x0080
#define REG_CONF_REF_EXT_2  		0x0040
#define REG_CONF_REF_EXT_1  		0x0000

#define REG_CONF_REF_DET    		0x0020

#define REG_CONF_BUF        		0x0010

#define REG_CONF_CH1 				0x0000
#define REG_CONF_CH2 				0x0001
#define REG_CONF_CH3 				0x0002
#define REG_CONF_CH4 				0x0003
#define REG_CONF_CH5 				0x0004
#define REG_CONF_CH6 				0x0005
#define REG_CONF_CHT 				0x0006
#define REG_CONF_CHV 				0x0007








/*********************************************************************************************************
*                                           VARIABLES
*********************************************************************************************************/
typedef union{
	uint8_t		b[4];
	uint32_t	a;
}U4Bytes_t;

/*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************/
void 		AD7794_spi_fun_sel(uint8_t (*spi_funct_ptr)(uint8_t),void (*chip_select_ptr)(uint8_t));

void		ad7794_reg_wr8(	uint8_t chip,uint8_t reg, uint8_t  data);
void 		ad7794_reg_wr16(uint8_t chip,uint8_t reg, uint16_t data);
void 		ad7794_reg_wr24(uint8_t chip,uint8_t reg, uint32_t data);
uint8_t		ad7794_reg_rd8(	uint8_t chip,uint8_t reg);
uint16_t	ad7794_reg_rd16(uint8_t chip,uint8_t reg);
uint32_t	ad7794_reg_rd24(uint8_t chip,uint8_t reg);

void		ad7794_Reset(			uint8_t chip);
void 		ad7794_reg_print(uint8_t chip);

void		ad7794_convert_single_start(uint8_t chip,uint8_t ch);
uint8_t		ad7794_convert_status(void);
uint32_t	ad7794_convert_value(uint8_t chip);
void 		ad7794_convert_continuous_start(uint8_t chip,uint8_t ch);
uint32_t 	ad7794_convert_continuous_value(uint8_t chip,uint8_t no_of_samples,uint8_t *sam_no,uint32_t *run_total);
void 		ad7794_convert_continuous_stop(uint8_t chip);

uint32_t	ad7794_convert_single_wait(uint8_t chip,uint8_t ch);
uint32_t 	ad7794_convert_continuous_wait(uint8_t chip,uint8_t ch,uint8_t no_of_samples);

uint8_t 	ad7794_err_check(uint8_t chip);

#endif
/*********************************************************************************************************
*********************************************************************************************************/