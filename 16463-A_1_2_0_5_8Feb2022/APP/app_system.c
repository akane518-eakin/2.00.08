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
 * Filename    :  app_system.c
 * Date Created:  Wed 04 Oct 2017 08:20:45 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>

#include "app_system.h"

#include "app_battery_fuel_guage.h"
#include "app_UI.h"
#include "app_pneumatic_ctrl.h"

#include "api_io.h"
#include "api_LEDs.h"
#include "api_reset.h"
#include "pcb_pins.h"

#include "csp_STM32_iwdg.h"	//internal
#include "api_watchdog.h"	//external



#include "csp_LCD_SSD1963.h"


/*************************************************************************************************
* Function Name :	system_shutdown
* Description   : 	This Function switches off the system
*					NOTE  - No return from this function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
void system_shutdown(RESET_SYS_t reset_system)
{
	//Disable all valves
	api_pwm_Air(0);
	api_pwm_O2(0);
	api_pwm_VenturiFlow(0);

	api_valve_nebuliser(VALVE_NEBLISER_OFF);
	api_valve_O2_calibrate(VALVE_O2_CAL_OFF);
	api_valve_Venturi_En(VALVE_Venturi_DIS);

	api_io_safety_cutout(SAFTY_CUTOUT_DIS);

	//LCD_screen off
	LCD_DisplayOff();

	//read and save battery settings
	app_battery_fuel_save();

//	app_UI_parameters_wr();
//	app_pneumatics_parameters_wr();

	api_watchdog_init(0);	//Disable	//0 = watchdog disabled
	api_watchdog_reset();

	api_LED(LED_PWR	,LED_OFF	,0x00);
	api_LED(LED_BATG,LED_OFF	,0x00);
	api_LED(LED_BATR,LED_OFF	,0x00);
	api_LED(LED_ALM	,LED_OFF	,0x00);
	api_LED_manager();


	printf("\r\nPower_Off");
	api_io_power_sys(0);
	if(reset_system == RESET_SYSTEM){
		api_reset_now();
	}
	while(1);

//	return;

}


/*************************************************************************************************
 * Function Name :	app_sys_watchdog_reload
 * Description   :
 * Arguments     : 	None Listed
 * Returns       : 	None Listed
 * Notes         : 	None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	23/12/16	W. Paul		Created
 *
 *************************************************************************************************/
void app_sys_watchdog_reload(void)
{
//	watchdog_reload();			//internal watchdog
//	api_watchdog_reload();		//external device

	return;
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
