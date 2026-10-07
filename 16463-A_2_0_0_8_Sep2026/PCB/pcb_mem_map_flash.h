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
 * Filename    :  pcb_mem_map_flash.h
 * Date Created:  Thu 31 Mar 2016 08:41:43 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _PCB_MEM_MAP_FLASH_H
#define _PCB_MEM_MAP_FLASH_H


/**********************************************************************************************************
 *                                           DEFINES
 **********************************************************************************************************/

//STM32
/**********************************************************************************************************/
#define		MEM_ADD_PWM_CALIBATION				0x08004800

/**********************************************************************************************************/


//S25FL0XX
/**********************************************************************************************************/
//mem map
	//each page of data is 0x
#define		BOOTLOAD_SETTINGS_FLASH_ADDR		0x000000		//	0x000000->0x000fff		//1st 4kB sector
#define		MEM_ADD_TOUCH_CONFIG				0x001000		//	0x001000->0x001fff		//2nd 4kB sector
#define		MEM_ADD_CALIBATION					0x002000		//	0x002000->0x002fff		//3nd 4kB sector
#define		MEM_ADD_UI_VARS						0x003000		//	0x003000->0x003fff		//4rd 4kB sector
#define		MEM_ADD_BATT_MANAGEMENT				0x004000		//	0x004000->0x004fff		//5nd 4kB sector
#define		MEM_ADD_UNIT_SERIAL_NO				0x005000		//	0x005000->0x005fff		//6th 4kB sector
#define		MEM_ADD_UNIT_MODEL_NO				0x005020
#define		MEM_ADD_TS24ADC_CONFIG				0x006000		//	0x006000->0x006fff		//7th 4kB sector
#define		MEM_ADD_PNEUMATICS_VARS				0x007000		//	0x007000->0x007fff		//8th 4kB sector

#define		MEM_ADD_SELFCHECK_RD_WR				0x010000		//	0x010000->0x010FFF		//16th 4kB sector

//32 x 4kB sectors  0-0x020000 in 0x001000 sectors

#define		FLASH_ADDR_NEW_FIRMWARE				0x020000		//	0x020000->0x11FFFF		//1st 64KB sector
#define		FLASH_ADDR_OLD_COPY_FIRMWARE		0x120000		//	0x120000->0x21FFFF
#define		FLASH_ADDR_FACTORY_FIRMWARE			0x220000		//	0x220000->0x31FFFF

#define		FLASH_ADD_AUDIO_FILE_A				0x400000 		//	0x400000->0x415cdd
//1st mem chip	end								0x7fffff
//2nd mem chip	start							0x800000


//2nd mem chip	end								0xffffff

/**********************************************************************************************************/

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file

