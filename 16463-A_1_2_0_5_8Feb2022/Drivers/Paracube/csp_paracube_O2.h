/*
*********************************************************************************************************
*											Marturion Ltd
*
*											Knockmore Hill Business Park
*											9 Ferguson Drive
*											Lisburn
*											Co. Antrim
*											Northern Ireland
*											BT28 2EX
*
*
*					Copyright 2021, Marturion Ltd, Lisburn, Co. Antrim, Northern Ireland
*											All Rights Reserved
*
*
*											csp_paracube_O2.h
*
* Filename    :  csp_paracube_O2.h
* Programmer  :  Tim Barr
* Description :  This module is used for 
* Compiler    :  GNU GCC
* Target      :  STM32
* Version     :  Version 3.5.0
*********************************************************************************************************
*/
#ifndef __csp_paracube_O2_H
#define __csp_paracube_O2_H


/*
*********************************************************************************************************
*											INCLUDE FILES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*											DEFINES
*********************************************************************************************************
*/
#define		CSP_PARACUBE_SET_TIME		3000

/*
*********************************************************************************************************
*											TYPEDEFS
*********************************************************************************************************
*/
typedef enum{
	PARACUBE_ID,
	PARACUBE_VER,
	PARACUBE_PRES_COMP_RD,
	PARACUBE_PRES_COMP_SET,
	PARACUBE_2_POINT_CAL_LOW,
	PARACUBE_2_POINT_CAL_HIGH,
	PARACUBE_1_POINT_CAL,
	PARACUBE_IDENTITY,
	PARACUBE_RESTORE_CAL,
	PARACUBE_CRC,
}PARACUBE_CMDS_enum;

typedef struct{
	uint8_t		init;
	uint8_t		rx_position;
	uint8_t		cmd_out;
	float		rx_value;
	union{
		uint8_t		byte;
		struct{
			uint8_t	B		:1;
			uint8_t	C		:1;
			uint8_t	E		:1;
			uint8_t	S		:1;
			uint8_t	X		:1;
			uint8_t unused	:3;
		}bits;
	}err_flags;
}PARACUBE_RX_st;


/*
*********************************************************************************************************
*											GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*											FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void		csp_paracube_fun_sel (	uint8_t(*getchar_ptr)(uint16_t,uint8_t*)	,	void(*putchar_ptr)(uint8_t)	);

void		csp_paracube_handler(void);
uint8_t		csp_paracube_debug(uint8_t en_dis);

void		csp_paracube_command(PARACUBE_CMDS_enum cmd, float value);

void		csp_paracube_timer(void);
uint32_t	csp_paracube_timeout_read(uint8_t reset);


#endif	// __CSP_PARACUBE_O2_H


/*
*********************************************************************************************************
*											End of csp_paracube_O2.h
*********************************************************************************************************
*/
