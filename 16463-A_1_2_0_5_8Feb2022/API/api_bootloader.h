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
 *  Copyright 2017, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  api_bootloader.h
 * Date Created:  Fri 05 May 2017 10:39:55 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_BOOTLOADER_H
#define _API_BOOTLOADER_H

#include "stdint.h"
#include "stdbool.h"

#include "pcb_mem_map_flash.h"

/*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************/
/* Memory Addresses */

#define	FLASH_START				0x08000000
#define	BOOTLOADER_SZ			0x00005000
#define	DEVICE_FLASH_SZ			0x00080000		//512kB device


#define	FLASH_ADDRESS_CODE_START		(FLASH_START + BOOTLOADER_SZ)

#define	DEVICE_FLASH_SIZE			(DEVICE_FLASH_SZ - BOOTLOADER_SZ)
/*
//see  pcb_mem_map_flash.h

#define	FLASH_ADDR_NEW_FIRMWARE			0x020000		//1MB 0x020000->0x11FFFF
#define	FLASH_ADDR_OLD_COPY_FIRMWARE	0x120000		//1MB 0x120000->0x21FFFF
#define	FLASH_ADDR_FACTORY_FIRMWARE		0x220000		//1MB 0x220000->0x31FFFF

#define BOOTLOAD_SETTINGS_FLASH_ADDR		0x000000		//    0x000000->0x00003F
*/


#define	BOOTLOAD_NOT_AVAILABLE		0
#define	BOOTLOAD_NEWCODE			1
#define	BOOTLOAD_OLDCODE			2
#define	BOOTLOAD_SAFEFACTORYCODE	3

/* System Defines */
#define	BOOTLOAD_CHECK_BYTE			0x22	// This must be changed in Bootloader program too


/*********************************************************************************************************
*                                           VARIABLES
*********************************************************************************************************/
/* This Struct must be also updated in the Bootloader code for consistency
	otherwise it may not work properly */
__packed typedef struct{
    uint8_t     a;
    uint8_t     b;
    uint8_t     c;
    uint16_t    d;
    uint8_t     day;
    uint8_t     month;
    uint8_t     year;
}version_t;

typedef struct{
	uint8_t	valid;					//initialised structure
	uint8_t	b[3];					//unused
	uint32_t	firmwareSize;		//size of file
	uint32_t	BootAddress;		//where in flash the code is located
	uint16_t	fileChecksum;		//the file cs
	uint8_t	    c[2];	            //unused
    version_t   version;            //version			
}BootFirmware_t;


typedef struct{
	uint8_t			checkByte;				//check if this structure has been initialised
	uint8_t			CodeFirstTimeRun;		//set to 1 in bootloader code after bootloader has been activated
	uint8_t			BootFromStatus;			//0= don't bootload  1-3 selects which code to bootload
	uint8_t			TestNewCodeStatus;		//0= testing complete  1= needs tested (set 1 after bootloader evoked
	uint8_t			LastBootSource;
	uint8_t			a[11];		//unused

	BootFirmware_t		NewCode;
	BootFirmware_t		LastCode;
	BootFirmware_t		SafeFactoryCode;
}BootSetting_t;


extern	BootSetting_t 	BootSettings;

/*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************/
void	api_bootloader_prepare_ext_flash(	uint32_t flash_start_add);

uint8_t	api_bootloader_new_firmware_handler(void);
uint8_t api_bootloader_packet_InOut(uint32_t pck_address,uint8_t *pck_data,uint16_t noofbytes,uint32_t *nextAddress);

void 	api_load_system_settings(void);

void 	api_bootloader_FlashNewCode(uint8_t codeLoc);
void 	api_bootloader_TestingNewCodePass(void);

uint8_t	api_bootloaderUpdateSettings(void);			//boot_app
void 	api_bootloader_settings_load(void);
void 	api_bootloader_settings_save(void);
void 	api_bootloader_settings_print(void);

void 	api_bootloader_create_safecode(uint32_t flash_start_add);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
