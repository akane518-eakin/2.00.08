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
 * Filename    :  api_bootloader.c
 * Date Created:  Fri 05 May 2017 10:34:56 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "glb_typedefs.h"

#include	"app_system.h"

#include "api_bootloader.h"

#include "api_EmbededCodeVars.h"

#include "csp_S25FL0xx.h"
#include "csp_STM32_uart.h"
#include "csp_STM32_uart2.h"
#include "csp_STM32_uart1.h"
//#include "csp_STM32_iwdg.h"
#include "csp_STM32_delay.h"

/**********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES
**********************************************************************************************************/


/**********************************************************************************************************
*                                           LOCAL VARIABLES
**********************************************************************************************************/
BootSetting_t	BootSettings;

uint32_t		rx_packet_next_address_required;
uint32_t		flash_mem_base_address;
uint16_t		rxFileChecksum 		=  0;
uint32_t		bytes_received		=  0;



/*************************************************************************************************
* Function Name : 	api_bootloader_new_firmware_handler
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	07/01/16		W. Paul			Created
*************************************************************************************************/
uint8_t api_bootloader_new_firmware_handler(void)
{
	// Local Variables
	uint8_t		checksum_pass;


	api_bootloader_settings_load();

	if(BootSettings.CodeFirstTimeRun){
		printf("\r\n1st Time Code Run\r\n");

		checksum_pass	=  api_EmbededCodeChecksumTest(1);			//0=no print info
		if(checksum_pass){	printf("\r\nFail Checksum Test");		}
		checksum_pass	+= strncmp((char const*)CodeVerify,CODE_VER_STR,12);			//0 = same
		if(checksum_pass){	printf("\r\nFail CodeVerify String");	}
		checksum_pass	+= strncmp((char const*)checksum_end,CODE_END_STR,4);
		if(checksum_pass){	printf("\r\nFail CodeEnd String");		}

		if(checksum_pass != 0){	//fail
			if(BootSettings.LastBootSource == BOOTLOAD_NEWCODE){
				printf("\r\nVerify Fail - Boot Old code");
				Delay(100);
				api_bootloader_FlashNewCode(BOOTLOAD_OLDCODE);

			}
			else{
				printf("\r\nVerify Fail - Boot Factory code");
				Delay(100);
				api_bootloader_FlashNewCode(BOOTLOAD_SAFEFACTORYCODE);
			}
		}
		else{		//pass
			api_bootloader_TestingNewCodePass();
			printf("\r\nVerify Pass");
			return(1);
		}
		return(0);
	}
	return(0);
}




/*************************************************************************************************
* Function Name : 	api_bootloader_packet_InOut
* Description   : 	This Function
* Arguments     : 	bootloader_packet_t 	*packetIn			input
*					uint8_t 				noofbytes
*					uint32_t 				*packetAddress		output
* Returns       : 	uint8_t	PassFail		0 = No error
*									1 = Incorrect packet
*									2 = packet cs error		send *packetAddress
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	23/02/16		W. Paul			Created
*************************************************************************************************/
uint8_t api_bootloader_packet_InOut(uint32_t pck_address,uint8_t *pck_data,uint16_t noofbytes,uint32_t *nextAddress)
{
    uint32_t    no_of_bytes;
	if(rx_packet_next_address_required != pck_address){
		*nextAddress	=  rx_packet_next_address_required;

		printf("\r\nbootloader err - %x %x",rx_packet_next_address_required,pck_address);
		return(1);
	}

	printf("\rBootloader A %x ",rx_packet_next_address_required);
	//write data to mem
	csp_mem_wr(pck_data, flash_mem_base_address + rx_packet_next_address_required, noofbytes);

    no_of_bytes     =   BootSettings.NewCode.firmwareSize;
    no_of_bytes     -=  bytes_received;
	if( no_of_bytes <= 240){
		//this is the last packet
		//we don't wnat to crc check the last 4 bytes
		rxFileChecksum	=  fast_crc16(rxFileChecksum, pck_data, no_of_bytes-4);
	}
	else{
		rxFileChecksum	=  fast_crc16(rxFileChecksum, pck_data, noofbytes);
	}

	rx_packet_next_address_required	+= noofbytes;
	*nextAddress		=  rx_packet_next_address_required;
	bytes_received		+= noofbytes;

	return(0);
}


/**********************************************************************************************************
* Function Name : api_bootloader_prepare_ext_flash
* Description   : This function erases the memory for a new file to be saved.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	07/01/16	    	Pauric Lynch        Original Created
**********************************************************************************************************/
void api_bootloader_prepare_ext_flash(uint32_t flash_start_add)
{
/* Local Variables */

/* Code */
	flash_mem_base_address	=  flash_start_add;

	csp_erase_64kb_sectors(flash_mem_base_address,16);		// max size of firmware will be 1MB ie 64*16
	rx_packet_next_address_required	=  0;
	rxFileChecksum		=  0;
	bytes_received		=  0;

	return;
}



/*************************************************************************************************
* Function Name : 	api_bootloader_FlashNewCode
* Description   : 	This Function sets the bootloader to upload new code and then performs a softreset to start the process
* Arguments     : 	uint8_t codeLoc
*					BOOTLOAD_NOT_AVAILABLE
*					BOOTLOAD_NEWCODE
*					BOOTLOAD_OLDCODE
*					BOOTLOAD_SAFEFACTORYCODE
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	02/03/16		W. Paul			Created
*************************************************************************************************/
void api_bootloader_FlashNewCode(uint8_t codeLoc)
{
	/* If the firmware downloaded ok, get ready to save to System Flash */

	BootSettings.BootFromStatus		=  codeLoc;
	api_bootloader_settings_save();

	printf("\r\nReset");
	Delay(250);

	NVIC_SystemReset();		/* Do a software Reset to enter the bootlaoder code */
	Delay(250);
	while(1){};

//	return;
}



/*************************************************************************************************
* Function Name : 	api_bootloader_TestingNewCodePass
* Description   : 	This Function sets the bootloader to upload new code and then performs a softreset to start the process
* Arguments     :
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	02/03/16		W. Paul			Created
*************************************************************************************************/
void api_bootloader_TestingNewCodePass(void)
{
	/* If the firmware downloaded ok, get ready to save to System Flash */

	BootSettings.CodeFirstTimeRun	=  0;
	BootSettings.BootFromStatus		=  BOOTLOAD_NOT_AVAILABLE;
	BootSettings.TestNewCodeStatus	=  0;
	api_bootloader_settings_save();

	return;
}



/*************************************************************************************************
* Function Name : 	api_bootloaderUpdateSettings
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	uint8_t		res= 0	no err
*								res= 1	CODE_VER_STR incorrect
*								res= 2	Checksum incorrect
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		02/03/16	W. Paul			Created
*************************************************************************************************/
uint8_t api_bootloaderUpdateSettings(void)
{
	FLASH_FIXED_t		data;
	union{
		uint8_t		b[2];
		uint16_t	a;
	}File_cs;
	uint8_t			result	=  0;


	csp_mem_rd(&data.filesize.b[0], FLASH_ADDR_NEW_FIRMWARE+0x130, sizeof(data));


	data.text[12]	= 0x00;
	if(strcmp((char*)data.text,CODE_VER_STR)==0){	BootSettings.NewCode.valid	=  1;	}
	else{											result	=  1;						}

	if(result	== 0){
		csp_mem_rd(&File_cs.b[0], FLASH_ADDR_NEW_FIRMWARE + data.filesize.a - 4 , sizeof(File_cs));

		if(File_cs.a	== rxFileChecksum){	BootSettings.NewCode.valid	=  1;	}
		else{								result	=  2;						}
	}

	if(result	== 0){

		// If the firmware downloaded OK
		BootSettings.NewCode.valid			=  1;
		BootSettings.NewCode.firmwareSize	=  data.filesize.a;
		BootSettings.NewCode.fileChecksum 	=  rxFileChecksum;
		BootSettings.NewCode.BootAddress 	=  flash_mem_base_address;
        BootSettings.NewCode.version.a      =  data.codeVer.a;
        BootSettings.NewCode.version.b      =  data.codeVer.b;
        BootSettings.NewCode.version.c      =  data.codeVer.c;
        BootSettings.NewCode.version.d      =  data.codeVer.d;
        BootSettings.NewCode.version.day    =  data.codeVer.day;
        BootSettings.NewCode.version.month  =  data.codeVer.month;
        BootSettings.NewCode.version.year   =  data.codeVer.year;
		printf("\r\nUpdate Bootloader params");
	}
	else{
		// If the firmware downloaded NOT ok
		BootSettings.NewCode.valid			=  0;
		BootSettings.NewCode.firmwareSize	=  0;
		BootSettings.NewCode.fileChecksum 	=  0;
		BootSettings.NewCode.BootAddress 	=  flash_mem_base_address;
        BootSettings.NewCode.version.a      =  0;
        BootSettings.NewCode.version.b      =  0;
        BootSettings.NewCode.version.c      =  0;
        BootSettings.NewCode.version.d      =  0;
        BootSettings.NewCode.version.day    =  0;
        BootSettings.NewCode.version.month  =  0;
        BootSettings.NewCode.version.year   =  0;
	}
	api_bootloader_settings_save();

	return(result);
}

/*************************************************************************************************
* Function Name : 	api_bootloader_settings_load
* Description   : 	This Function reads settings from external flash
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	This data is shared between this firmware and the bootloader firmware
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/05/17	W. Paul			Created
*************************************************************************************************/
void api_bootloader_settings_load(void)
{
/* Local Variables */

/* Code */
	csp_mem_rd((uint8_t*)&BootSettings, BOOTLOAD_SETTINGS_FLASH_ADDR, sizeof(BootSettings));
	if(BootSettings.checkByte != BOOTLOAD_CHECK_BYTE)
	{
		printf("\r\nBootSettings.checkByte invalid (main)");
		memset(&BootSettings, 0, sizeof(BootSettings));
		api_bootloader_settings_save();
	}

	return;
}

/*************************************************************************************************
* Function Name : 	api_bootloader_settings_save
* Description   : 	This Function saves the settings to the external flash
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/05/17	W. Paul			Created
*************************************************************************************************/
void api_bootloader_settings_save(void)
{
	BootSettings.checkByte = BOOTLOAD_CHECK_BYTE;
	csp_sys_mem_wr((uint8_t*)&BootSettings, BOOTLOAD_SETTINGS_FLASH_ADDR, sizeof(BootSettings));
	return;
}


/*************************************************************************************************
* Function Name : 	api_bootloader_settings_print
* Description   : 	This Function prints the settings
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/05/17	W. Paul			Created
*************************************************************************************************/
void api_bootloader_settings_print(void)
{
	uint8_t	init_byte	=  BOOTLOAD_CHECK_BYTE;

	api_bootloader_settings_load();

	printf("\r\n");
	printf("\r\nInitialised (%2x)  - %2x",init_byte
										,BootSettings.checkByte
										);
	printf("\r\nFlag 1st time     - %d"	,BootSettings.CodeFirstTimeRun);		//set to 1 in bootloader code after bootloader has been activated
	printf("\r\nBoot from source  - %d"	,BootSettings.BootFromStatus);			//0= don't bootload  1-3 selects which code to bootload
	printf("\r\nTesting status    - %d"	,BootSettings.TestNewCodeStatus);		//0= testing complete  1= needs tested (set 1 after bootloader evoked
	printf("\r\nLast boot source  - %d"	,BootSettings.LastBootSource);
	printf("\r\n   (Source  0-NotAvail 1-new 2-old 3-factory)");

	printf("\r\n Code        V Sz       FlashAdd Checksum Version");
	printf("\r\n NewCode     %d 0x6%x 0x6%x 0x%4x %d.%d.%d.%d %02d/%02d/%02d"
														,BootSettings.NewCode.valid
														,BootSettings.NewCode.firmwareSize
														,BootSettings.NewCode.BootAddress
														,BootSettings.NewCode.fileChecksum
														,BootSettings.NewCode.version.a
													    ,BootSettings.NewCode.version.b
													    ,BootSettings.NewCode.version.c
													    ,BootSettings.NewCode.version.d
													    ,BootSettings.NewCode.version.day
													    ,BootSettings.NewCode.version.month
													    ,BootSettings.NewCode.version.year
														);

	printf("\r\n LastCode    %d 0x%6x 0x%6x 0x%4x %d.%d.%d.%d %02d/%02d/%02d"
														,BootSettings.LastCode.valid
														,BootSettings.LastCode.firmwareSize
														,BootSettings.LastCode.BootAddress
														,BootSettings.LastCode.fileChecksum
														,BootSettings.LastCode.version.a
													    ,BootSettings.LastCode.version.b
													    ,BootSettings.LastCode.version.c
													    ,BootSettings.LastCode.version.d
													    ,BootSettings.LastCode.version.day
													    ,BootSettings.LastCode.version.month
													    ,BootSettings.LastCode.version.year
														);

	printf("\r\n FactoryCode %d 0x%6x 0x%6x 0x%4x %d.%d.%d.%d %02d/%02d/%02d"
														,BootSettings.SafeFactoryCode.valid
														,BootSettings.SafeFactoryCode.firmwareSize
														,BootSettings.SafeFactoryCode.BootAddress
														,BootSettings.SafeFactoryCode.fileChecksum
														,BootSettings.SafeFactoryCode.version.a
													    ,BootSettings.SafeFactoryCode.version.b
													    ,BootSettings.SafeFactoryCode.version.c
													    ,BootSettings.SafeFactoryCode.version.d
													    ,BootSettings.SafeFactoryCode.version.day
													    ,BootSettings.SafeFactoryCode.version.month
													    ,BootSettings.SafeFactoryCode.version.year
														);
	printf("\r\nEnd");

	return;
}

/*************************************************************************************************
* Function Name : 	api_bootloader_create_safecode
* Description   : 	This Function copies the current code in the uprocessor to external flash
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	17/03/16		W. Paul			Created
*************************************************************************************************/
void api_bootloader_create_safecode(uint32_t flash_start_add)
{
	uint32_t		i;
	uint32_t		j;
	uint8_t			*flash_add;
//	uint16_t		checksum;
	uint8_t 		flash_mem_cpy[256];
	uint32_t		buf_sz;
	uint32_t		FileSize	=  flash_size;


	buf_sz			=  sizeof(flash_mem_cpy);
//	checksum		=  0;
	flash_add		=  (uint8_t*)FLASH_ADDRESS_CODE_START;


	// Erase the Memory Before Saving the Firmware
	api_bootloader_prepare_ext_flash(flash_start_add);

	//copy internal code flash to external flash memory
	if(flash_start_add == FLASH_ADDR_OLD_COPY_FIRMWARE){
		printf("\r\nCreating Safecopy of Current Code in Flash");
	}
	if(flash_start_add == FLASH_ADDR_FACTORY_FIRMWARE){
		printf("\r\nCreating FactoryCopy of Current Code in Flash");
	}
	printf("\r\n");

	for(i=0; i<FileSize; i += buf_sz){
		for(j=0; j<buf_sz;j++){
			flash_mem_cpy[j]	=  *flash_add;
			flash_add++;
		}

		csp_mem_wr(flash_mem_cpy, flash_start_add + i, buf_sz);

		printf("\r%2.0f%% ", (100.0*(float)i)/(float)FileSize);


		app_sys_watchdog_reload();

	}

	//Update header file parameters
	if(flash_start_add == FLASH_ADDR_OLD_COPY_FIRMWARE){
		BootSettings.LastCode.valid 				=  1;
		BootSettings.LastCode.firmwareSize 			=  FileSize;
		BootSettings.LastCode.fileChecksum 			=  ielftool_checksum;
		BootSettings.LastCode.BootAddress 			=  flash_start_add;
        BootSettings.LastCode.version.a             =  firmware_version_st_glb.a;
        BootSettings.LastCode.version.b             =  firmware_version_st_glb.b;
        BootSettings.LastCode.version.c             =  firmware_version_st_glb.c;
        BootSettings.LastCode.version.d             =  firmware_version_st_glb.d;
        BootSettings.LastCode.version.day           =  firmware_version_st_glb.day;
        BootSettings.LastCode.version.month         =  firmware_version_st_glb.month;
        BootSettings.LastCode.version.year          =  firmware_version_st_glb.year;
	}
	if(flash_start_add == FLASH_ADDR_FACTORY_FIRMWARE){
		BootSettings.SafeFactoryCode.valid 				=  1;
		BootSettings.SafeFactoryCode.firmwareSize 		=  FileSize;
		BootSettings.SafeFactoryCode.fileChecksum 		=  ielftool_checksum;
		BootSettings.SafeFactoryCode.BootAddress 		=  flash_start_add;
        BootSettings.SafeFactoryCode.version.a          =  firmware_version_st_glb.a;
        BootSettings.SafeFactoryCode.version.b          =  firmware_version_st_glb.b;
        BootSettings.SafeFactoryCode.version.c          =  firmware_version_st_glb.c;
        BootSettings.SafeFactoryCode.version.d          =  firmware_version_st_glb.d;
        BootSettings.SafeFactoryCode.version.day        =  firmware_version_st_glb.day;
        BootSettings.SafeFactoryCode.version.month      =  firmware_version_st_glb.month;
        BootSettings.SafeFactoryCode.version.year       =  firmware_version_st_glb.year;
	}
	api_bootloader_settings_save();

	printf("\r\ndone");

	return;
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
