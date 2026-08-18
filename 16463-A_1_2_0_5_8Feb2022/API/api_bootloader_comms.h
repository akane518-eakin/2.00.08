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
 * Filename    :  api_bootloader_comms.h
 * Date Created:  Mon 11 Jan 2016 10:40:51 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_BOOTLOADER_COMMS_H
#define _API_BOOTLOADER_COMMS_H

/*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************/
#include "stdint.h"
#include "stdbool.h"

#include "api_bootloader.h"


/*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************/
#define	LOAD_FIRMWARE_ECHO					0xFE10
#define LOAD_FIRMWARE_START					0xFE11
#define LOAD_FIRMWARE_CREATE_CPY			0xFE12
#define LOAD_FIRMWARE_CREATE_FACTORY		0xFE13
#define LOAD_FIRMWARE_DATA					0xFE14
#define LOAD_FIRMWARE_BOOT_TO_NEW			0xFE15
#define LOAD_FIRMWARE_BOOT_TO_LAST			0xFE16
#define LOAD_FIRMWARE_BOOT_TO_FACTORY		0xFE17
#define LOAD_FIRMWARE_INFO_REQ				0xFE18
#define LOAD_FIRMWARE_CUR_VERSION_REQ      	0xFE19

#define	LOAD_FIRMWARE_READY					0xFE20
#define	LOAD_FIRMWARE_REQ_NEXT				0xFE21
#define LOAD_FIRMWARE_INFO					0xFE22
#define	LOAD_FIRMWARE_RESULT				0xFE23
#define LOAD_FIRMWARE_CUR_VERSION         	0xFE24

typedef union{
	uint16_t	a;
	uint8_t		b[2];
}U2B_t;

typedef union{
	uint32_t	a;
	uint8_t		b[4];
}U4B_t;

//***************************************************************************
//Header and footer
__packed typedef struct{
	uint8_t		stx;		//start of packet	0x02
	U2B_t		cmd;
	U2B_t		d_len;
	uint8_t		h_cs;
	uint8_t		eoh;		//end of header		0x1f
}header_t;

__packed typedef struct{
	U2B_t		cs;			//checksum
	uint8_t		eop;		//end of packet		0x03
}footer_t;

//***************************************************************************
__packed typedef struct{
	U4B_t		start_add;
	uint8_t		data[256];
}cmd0001_t;

__packed typedef struct{
	U4B_t		next_add;
}cmd0002_t;

__packed typedef struct{
	U4B_t		file_size;
	uint8_t		firmware_version[16];
}cmd0003_t;

__packed typedef struct{
	uint8_t		result;
}cmd0004_t;
__packed typedef struct{
	uint8_t		a;
    uint8_t		b;
    uint8_t		c;
    uint16_t	d;
    uint8_t		date;
    uint8_t		month;
    uint8_t		year;
}cmd0005_t;
//***************************************************************************
__packed typedef union{
	char	raw[280];			// storage location for incoming and outgoing packets
	__packed struct{
		header_t			header;
		union{
			char			raw[270];		//MAXPacketLength

			cmd0001_t		cmd0001;
			cmd0002_t		cmd0002;
			cmd0003_t		cmd0003;
			cmd0004_t		cmd0004;
            cmd0005_t       cmd0005;
			BootSetting_t	BootSetting;
		}data;
	}format;
}packet_t;

/*********************************************************************************************************
*                                           VARIABLES
*********************************************************************************************************/


/*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************/
void 	api_bootloader_fun_sel(		uint8_t (*getchar_ptr)(	uint16_t wait_time_u16,	uint8_t *rec_status_u8),
									void 	(*putchar_ptr)(	uint8_t tx_char_u8)
                	       	 );

void	api_bootloader_serial_port_handler(uint8_t *debug_mode);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
