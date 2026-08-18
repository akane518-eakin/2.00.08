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
 * Filename    :  app_battery_fuel_guage.h
 * Date Created:  Tue 26 Sep 2017 03:50:53 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _APP_BATTERY_FUEL_GUAGE_H
#define _APP_BATTERY_FUEL_GUAGE_H

#include <stdint.h>
#include "hal_ltc2943.h"

#define	BATTERY_CHARGER_INIT	0x23
#define	BATTERY_SAVED_VALID		0x12

typedef enum{
	BAT_UNKNOWN		=  0,
	BAT_CHARGING	=  1,
	BAT_CHARGED		=  2,
	BAT_DISCHARGING	=  4,
}battery_mode_e;

typedef union{
	uint8_t		byte;
	struct{
		uint8_t		ICL		:1;		//input current is limited
		uint8_t		ACP		:1;		//external PSU is available
		uint8_t		CHRG	:1;		//battery chraging/charged
		uint8_t		SHDN	:1;		//chargher shutdown/active
		uint8_t		unused	:4;
	}bits;
}battery_charger_status_u;

typedef struct
{
	uint8_t						init;
	float						battery_adc_v;
	uint8_t						battery_status;		//0= no fault, 1=fault

	battery_charger_status_u	status;
	battery_mode_e				mode;
	uint8_t						flash;
	uint8_t						unplugged_icon;
	float						percentage;		// Indicates the battery Charge percentage
	uint32_t					sec_remaining;

}battery_charger_t;

typedef struct
{
	uint8_t						init;
	uint8_t						full_flag;
	uint16_t					charge;
}battery_charger_saved_t;

/*********************************************************************************************************
 *		Global Variables
 ********************************************************************************************************/
extern battery_charger_t		battery_charger;
extern battery_charger_saved_t	battery_saved;



/*********************************************************************************************************
 ********************************************************************************************************/

void 	app_battery_charger_manager(void);

void 	app_battery_charger_endis(uint8_t en_dis);
void 	app_battery_charger_status(void);

uint8_t	app_battery_fuel_gauge_init(void);
uint8_t	app_battery_fuel_save(void);

uint8_t	app_battery_new_battery(void);
#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
