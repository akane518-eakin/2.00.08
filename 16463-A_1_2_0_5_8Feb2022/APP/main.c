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
 * Filename    :  main.c
 * Date Created:  Tue 04 Apr 2017 08:53:40 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/

#include "stdio.h"
#include "stdint.h"
#include "string.h"

#include "app_debug.h"
#include "app_UI.h"
#include "app_touchscreen.h"
#include "app_button.h"
#include "app_pneumatic_ctrl.h"
#include "app_battery_fuel_guage.h"
#include "app_patient_pressure.h"
#include "app_selfcheck.h"
#include "app_system.h"

#include "api_bootloader_comms.h"
#include "api_bootloader.h"
#include "api_audio.h"
#include "api_reset.h"
#include "api_EmbededCodeVars.h"
#include "api_LEDs.h"

#include "csp_STM32_fsmc.h"
#include "csp_STM32_iwdg.h"
#include "csp_LCD_SSD1963.h"
#include "csp_paracube_O2.h"
#include "hal_lcd_text.h"
#include "Colours.h"
#include "fonts.h"

#include "csp_S25FL0xx.h"

#include "pcb_startup.h"

#include "stm32f10x_rcc.h"


//**********************************************************************************************************
//	GLOBAL VARIABLES
//**********************************************************************************************************
//The following info is in the *.bin file
//	Address 0x130 			( 4bytes)	- file size in bytes
//	Address 0x134			(16bytes)	- Firmware Version  type sw_version_t
//	Address 0x144 			(12bytes)	- Magic code to verify that the code is applicable for this device   - MARTURIONLtd
//	Address 'file_size'-8	( 4bytes)	- End of file marker	- 0x03,0x02,0x01,0xEE
//	Address 'file_size'-4	( 2bytes)	- File Checksum
//
//	NB checksum will not be added automatically to *.bin file on compile - but is programmed (post linker pre download)

__root const uint16_t 		checksum_flash 				@ "ielftool_checksum";
__root const uint8_t 		checksum_end[4]				@ "checksum_end_mark"		=  CODE_END_STR;
__root const uint32_t 		flash_size	 				@ "file_size"				=  (uint32_t)(&checksum_end[3])-0x08005000+5;
__root const uint8_t 		CodeVerify[12] 				@ "CodeVerification"		=  CODE_VER_STR;
__root const sw_version_t	firmware_version_st_glb 	@ "FirmwareVer"				=  {	2,			// 01	 Firmware Revision 01.02.03
																							0,			// 02
																							0,			// 03
																							7,			// 04
																							20,			// Date	 Firmware Date
																							8,			// Month
																							26,			// Year
																							__TIME__,	// time of compiler output
																							__DATE__	// date of compiler output
																						};

/**********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES
**********************************************************************************************************/
void		firmware_output(void);


/**********************************************************************************************************
**********************************************************************************************************/


/*************************************************************************************************
* Function Name : 	main
* Description   :
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/04/17	W. Paul			Created
*************************************************************************************************/
void main(void)
{
	uint8_t	debug_mode		=  0;	//0=bootloader,	1= debug menu
	uint8_t touch_status;
	uint8_t button_status;
	uint8_t loop1;
	uint8_t	main_loop_tog	=  0;
	uint8_t	boot_1st_time;

	pcb_startup(STARTUP_BASIC);
	boot_1st_time	=  api_bootloader_new_firmware_handler();

	firmware_output();

	//**************************************************************************************
	//**************************************************************************************
	// System looks off
	// battery charging is in progress
	// waiting for user to press on button
	//**************************************************************************************

	if(PinRead(LTC4009_ACP) == 0){	loop1	=  1;	}	//external PSU is available		therefore wait on user power on device
	else{							loop1	=  0;	}	//user is starting in bat mode so no loop
	if(boot_1st_time){				loop1	=  0;	}	//if 1st time after new code.. then auto on!

	if(	loop1){
		csp_mem_deconfig();
		
		printf("\r\nIn Standby mode");
		//wait here until power button is released (to be sure we dont restart after a powerdown
		button_status	=  app_button_handler();
		if(button_status){
			printf("\r\n");
			printf("\r\nPlease release the power button!");
		}
		do{
			button_status	=  app_button_handler();
		}while(button_status > 0);
	}

	while(loop1){
		app_battery_charger_manager();
		
		csp_mem_deconfig();				// In case FLASH is configured by charger
		
		api_LED_manager();
		button_status	=  app_button_handler();
		if(button_status == BUT_PWR_HOLD){ 	loop1	=  0; 	}

		if(PinRead(LTC4009_ACP) == 1){	//ext PSU removed
			printf("\r\nExt PSU plugged out");
			system_shutdown(DONT_RESET_SYSTEM);
		}

		app_sys_watchdog_reload();
	}

	//**************************************************************************************
	//**************************************************************************************
	// System is now fully on
	//**************************************************************************************
	printf("\r\nFD140i On");

	pcb_startup(STARTUP_FULLY);

	while(1){
		main_loop_tog	^= 0x01;
		PinSet(TP2,main_loop_tog);
		
                api_audio_manager();
                
		api_bootloader_serial_port_handler(&debug_mode);
		app_debug_handler(&debug_mode);
		
		app_selfcheck_manager();
		
		app_battery_charger_manager();
		
		api_LED_manager();
		
		app_PP_manager();
		csp_paracube_handler();
		app_pneumatic_manager();
		
		touch_status	=  app_touchscreen_handler();
		button_status	=  app_button_handler();
		app_UI(&touch_status,&button_status);
		
		
		app_sys_watchdog_reload();
	}

}


/**********************************************************************************************************
 * Function Name : firmware_output
 * Description   : This function is used to output the firmware Revison and Date.
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    23/07/2010      Stephen Serplus     Original Created
 * 1.0.1			 9/ 2/2011		William Paul		create structure
 *********************************************************************************************************/
void firmware_output(void)
{
	printf("\r\n\n ");
	printf(" Armstrong FD140i ");
	printf("\r\n%d.%d.%d.%d"		,firmware_version_st_glb.a
									,firmware_version_st_glb.b
									,firmware_version_st_glb.c
									,firmware_version_st_glb.d		);
	printf("    %02d/%02d/%02d"		,firmware_version_st_glb.day
									,firmware_version_st_glb.month
									,firmware_version_st_glb.year	);
	printf("\r\nBuild date: %s"		,firmware_version_st_glb.date_p	);
	printf("\r\nBuild time: %s"		,firmware_version_st_glb.time_p	);


	RCC_ClocksTypeDef	See_RCC_Clocks;
	RCC_GetClocksFreq(&See_RCC_Clocks);
	printf("\r\nSysCLK %d",	See_RCC_Clocks.SYSCLK_Frequency);
//	printf("\r\nHCLK   %d",	See_RCC_Clocks.HCLK_Frequency);
//	printf("\r\nPCLK1  %d",	See_RCC_Clocks.PCLK1_Frequency);
//	printf("\r\nPCLK2  %d",	See_RCC_Clocks.PCLK2_Frequency);
//	printf("\r\nADCCLK %d",	See_RCC_Clocks.ADCCLK_Frequency);

	printf("\r\n");



	return;
}





/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
