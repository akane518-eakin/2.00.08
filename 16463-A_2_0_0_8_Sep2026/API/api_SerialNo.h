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
 *  Copyright 2016, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  api_SerialNo.h
 * Date Created:  Tue 19 Jul 2016 08:30:06 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_SERIALNO_H
#define _API_SERIALNO_H


#define		SN_POPULATED		0x12ED
#define		SN_LEN				16

#define		MN_POPULATED		0x1780
#define		MN_LEN				16

__packed typedef struct{
	uint16_t	tamper;
	uint8_t 	Serial_No[SN_LEN];
}SerialNo_t;

__packed typedef struct{
	uint16_t	tamper;
	uint8_t 	Model_No[MN_LEN];
}ModelNo_t;


uint8_t api_SerialNo_read(uint8_t *str);
void 	api_SerialNo_write(uint8_t *str,uint8_t set_valid);
void 	api_SerialNo_set(void);
void 	api_SerialNo_print(void);

uint8_t api_ModelNo_read(uint8_t *str);
void 	api_ModelNo_write(uint8_t *str,uint8_t set_valid);
void 	api_ModelNo_print(void);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file

