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
 * Filename    :  api_EmbededCodeVars.h
 * Date Created:  Wed 18 May 2016 12:42:38 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_EMBEDED_CODE_VARS_H
#define _API_EMBEDED_CODE_VARS_H

#include "glb_typedefs.h"

#define FAST_CRC16 1
//#define SLOW_CRC16

typedef struct{
	union{
		uint8_t		b[4];
		uint32_t	a;
	}filesize;
	sw_version_t	codeVer;
	uint8_t			text[13];
}FLASH_FIXED_t;

extern  const unsigned short ielftool_checksum;
extern  const uint16_t 		checksum_flash;
extern  const uint8_t 		checksum_end[4];
extern  const uint32_t 		flash_size;
extern  const uint8_t 		CodeVerify[12];
extern  const sw_version_t	firmware_version_st_glb;

uint8_t api_EmbededCodeChecksumTest(uint8_t print);


#ifdef FAST_CRC16
uint16_t	fast_crc16(uint16_t sum, uint8_t *p, uint32_t len);
#endif

#ifdef SLOW_CRC16
uint16_t	slow_crc16(uint16_t sum, uint8_t *p, uint32_t len);
#endif

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
