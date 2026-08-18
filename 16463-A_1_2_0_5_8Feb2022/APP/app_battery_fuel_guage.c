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
 * Filename    :  app_battery_fuel_guage.c
 * Date Created:  Tue 26 Sep 2017 03:50:28 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>

#include	"app_battery_fuel_guage.h"
#include	"app_system.h"

#include	"api_STM32_touchscreen.h"
#include 	"api_leds.h"
#include	"hal_ltc2943.h"		//fuel gauge
#include	"csp_ltc2943.h"		//fuel gauge
#include	"csp_S25FL0xx.h"
#include	"pcb_pins.h"

battery_charger_t	battery_charger	={
	.init			=  0,
	.mode			=  BAT_UNKNOWN,
	.battery_status	=  0,
};
battery_charger_saved_t		battery_saved;


/*************************************************************************************************
* Function Name : 	app_battery_charger_manager
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
void app_battery_charger_manager(void)
{
	uint8_t						i2c_status;
	static uint32_t				next_update_time	=  0;
	static battery_mode_e		last_mode			=  BAT_UNKNOWN;
	static uint8_t				startup_delay		=  0;
	static uint8_t				last_ACP			= 0xff;
	static uint8_t				last_ACP_cnt		= 0;
	uint8_t						adc_delay			= 0;

	if(sys_tic_rd()									>  next_update_time){
		if(++startup_delay							<  12){
			next_update_time						=  sys_tic_rd() + 500;		//every second
		}
		else{
			startup_delay							=  12;
			next_update_time						=  sys_tic_rd() + 5000;		//every second
		}
		
		app_battery_charger_status();
		
	//init
		if(battery_charger.init						!= BATTERY_CHARGER_INIT){
			app_battery_charger_endis(0);
			for(adc_delay=0;adc_delay<4;adc_delay++){
				Delay(50);
				app_sys_watchdog_reload();
			}
			battery_charger.battery_adc_v			=  api_Vbat_rd();
			
			if(battery_charger.battery_adc_v		<  VBAT_FAULT){
				battery_charger.battery_status		=  2;	//fault
				printf("\r\nADC - No battery Detected %1.1fV", battery_charger.battery_adc_v);
			}
			else{
				if(battery_charger.battery_adc_v	<  VBAT_CUT){
					battery_charger.battery_status	=  1;	//fault
					printf("\r\nADC - Dead battery Detected %1.1fV", battery_charger.battery_adc_v);
				}
				else{
//					battery_charger.battery_status	=  0;	//no fault ... but do not clear previous fault
					printf("\r\nADC - Battery Detected %1.1fV", battery_charger.battery_adc_v);
				}
				i2c_status							=  app_battery_fuel_gauge_init();
				if(i2c_status						!= 0){
					printf("\r\nI2C err -battery-init");
				}
				else{
					battery_charger.init			=  BATTERY_CHARGER_INIT;
				}
				
				csp_mem_rd(&battery_saved.init, MEM_ADD_BATT_MANAGEMENT, sizeof(battery_saved) );
				
				// debug code	force battery to a certain percentage
//				battery_saved.charge				=  hal_ltc2943_formula_mAh_to_reg(BAT_CAPACITY *0.005);
//				battery_saved.full_flag				=  0;
				// end of debug
				if(battery_saved.full_flag			>= 2){
					battery_saved.full_flag			=  1;	//this means we will always attempt to set battery full point after a power up of the system
				}
				if(battery_saved.init				== BATTERY_SAVED_VALID){
					i2c_status						=  csp_ltc2943_RdReg_2b(ACC_CHARGE_MSB,&FuelGaugeReg.charge.a);
					
					if(FuelGaugeReg.charge.a		<  LTC_FAULT){
						printf("\r\nBat status restored but not updated");
					}
					else if(battery_saved.charge	>  LTC_CAPACITY){
						printf("\r\nBat status restored - max");
						hal_ltc2943_set_charge(1.0);
					}
					else if(battery_saved.charge	<  LTC_OFFSET){
						printf("\r\nBat status restored - min");
						hal_ltc2943_charge_write(LTC_OFFSET);
					}
					else{
						printf("\r\nBat status restored as saved");
						hal_ltc2943_charge_write(battery_saved.charge);
					}
				}
				else{
					printf("\r\nBat status not restored !");
				}
			}
			
			if(battery_charger.battery_status		>  0){
				hal_ltc2943_set_charge(0.0);
				printf("\r\nBat fault - reset charge to 0");
				if(battery_charger.init				== BATTERY_CHARGER_INIT){
					app_battery_fuel_save();
				}
			}
		}
	//regular read
		else{
			if(battery_charger.battery_status		>  0){
				app_battery_charger_endis(0);
				for(adc_delay=0;adc_delay<4;adc_delay++){
					Delay(50);
					app_sys_watchdog_reload();
				}
				battery_charger.battery_adc_v		=  api_Vbat_rd();
				
				if(battery_charger.battery_adc_v	<  VBAT_CUT){
					printf("\r\nADC - Dead battery Detected %1.1fV", battery_charger.battery_adc_v);
					hal_ltc2943_set_charge(0.0);
					printf("\r\nBat fault - reset charge to 0");
				}
			}
			
			i2c_status								=  0;
			i2c_status								+= hal_ltc2943_charge_read();
			i2c_status								+= hal_ltc2943_voltage_read();
			i2c_status								+= hal_ltc2943_current_read();
			i2c_status								+= hal_ltc2943_temp_read();
			if(i2c_status							!= 0){
				printf("\r\nI2C err -battery-read");
			}
			
			battery_charger.percentage				=  FuelGauge.percentage;		// Indicates the battery Charge percentage
			battery_charger.sec_remaining			=  FuelGauge.sec_remaining;
			
			hal_ltc2943_time_remaining();							//this should only be called here
																	//so av current is sampled at a constant rate
																	
			if(battery_charger.status.bits.ACP){		api_LED(LED_PWR	,LED_ON,	0xFF);	}
			else{										api_LED(LED_PWR	,LED_OFF,	0x00);	}
				
			if(battery_charger.status.bits.ACP){		//AC on
				if(battery_charger.status.bits.CHRG){	api_LED(LED_BATG,LED_FLASH	,0xFF);	api_LED(LED_BATR,LED_OFF	,0x00);	}	//device on		(AC) charging
				else{									api_LED(LED_BATG,LED_ON		,0xFF);	api_LED(LED_BATR,LED_OFF	,0x00);	}	//device on		(AC) charged
			}
			else{										//Bat mode
				if(battery_charger.percentage >= 21.0){	api_LED(LED_BATG,LED_ON		,0x80);	api_LED(LED_BATR,LED_OFF	,0x00);	}	//device on 	(bat) 	Bat >  20%
				else{									api_LED(LED_BATG,LED_OFF	,0x00);	api_LED(LED_BATR,LED_ON		,0x80);	}	//device on 	(bat) 	Bat <= 20%
			}
			
			if(battery_charger.status.bits.ACP){
				if(battery_charger.status.bits.ACP	!=  last_ACP){
					//this is a delay to allow CHRG bit to be correct after charger chip is powered on by external PSU
					//ACP reacts quickly, but CHRG takes a few seconds
					last_ACP						=  battery_charger.status.bits.ACP;
					last_ACP_cnt					=  0;
					battery_charger.mode			=  BAT_CHARGING;
					
					startup_delay					=  0;		//update faster
				}
				
				if(++last_ACP_cnt					>  3){
					last_ACP_cnt					=  3;
					
					if(battery_charger.status.bits.CHRG){
						battery_charger.mode		=  BAT_CHARGING;
					}
					else{
						//fully charged flag is set
						if(battery_charger.mode		!= BAT_CHARGED){
							battery_charger.mode	=  BAT_CHARGED;
							if(battery_saved.full_flag		<  2){
								printf("\r\nFuel Gauge - Set Max Charge");
								hal_ltc2943_set_charge(1.0);
								i2c_status					=  app_battery_fuel_save();
								if(i2c_status){
									battery_saved.full_flag	+=  1;
								}
							}
						}
					}
				}
			}
			else{
				last_ACP_cnt							=  0;
				battery_charger.mode					=  BAT_DISCHARGING;
				
//				if(battery_charger.percentage			>  10.0){
//					if(	(FuelGauge.act_voltage			<  VBAT_MIN)	||
//						(battery_charger.battery_adc_v	<  VBAT_MIN)	){
//						printf("\r\nFuel Gauge - set 10%%, battery voltage %.2f", FuelGauge.act_voltage);
//						hal_ltc2943_set_charge(0.099);		// Set slightly below 10% to ensure no repeat entry
//					}
//				}
			}
		}
		
		if(battery_charger.status.bits.ACP){	app_battery_charger_endis(1);	}
		else{									app_battery_charger_endis(0);	}

		//printf of status
		if(last_mode							!= battery_charger.mode){
			last_mode							=  battery_charger.mode;
			switch(battery_charger.mode){
				case BAT_UNKNOWN:		printf("\r\nBAT ??");			break;
				case BAT_CHARGING:		printf("\r\nBAT charging");		break;
				case BAT_CHARGED:		printf("\r\nBAT charged");		break;
				case BAT_DISCHARGING:	printf("\r\nBAT discharging");	break;
				default:				printf("\r\nBAT --");
			}
		}
	}
	
	return;
}


/*************************************************************************************************
* Function Name : 	app_battery_charger_endis
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
void app_battery_charger_endis(uint8_t en_dis)
{
	if(en_dis > 1){	en_dis	=  1;}
	battery_charger.status.bits.SHDN		=  en_dis;

	PinSet(LTC4009_SHDN,battery_charger.status.bits.SHDN);

	return;
}


/*************************************************************************************************
* Function Name : 	app_battery_charger_status
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
void app_battery_charger_status(void)
{
	if(PinRead(LTC4009_ICL) == 0){	battery_charger.status.bits.ICL		=  1;	}		//input current is limited
	else{							battery_charger.status.bits.ICL		=  0;	}		//

	if(PinRead(LTC4009_ACP) == 0){	battery_charger.status.bits.ACP		=  1;	}		//external PSU is available
	else{							battery_charger.status.bits.ACP		=  0;	}

	if(PinRead(LTC4009_CHRG) == 0){	battery_charger.status.bits.CHRG	=  1;	}		//battery is charging
	else{							battery_charger.status.bits.CHRG	=  0;	}		//battery is charged

	return;
}


/*************************************************************************************************
* Function Name : 	app_battery_fuel_gauge_init
* Description   : 	This Function initialises the fuel guage
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t app_battery_fuel_gauge_init(void)
{
	uint8_t		i2c_status;
	
	FuelGauge.alarm.charge_h		=  BAT_CAPACITY	* 0.9;			//Charge threshold high
	FuelGauge.alarm.charge_l		=  BAT_CAPACITY * 0.1;			//Charge threshold low
	FuelGauge.alarm.voltage_h		=  13;							//Voltage threshold high
	FuelGauge.alarm.voltage_l		=  10.5;						//Voltage threshold low
	FuelGauge.alarm.current_h		=  3;							//Current threshold high
	FuelGauge.alarm.current_l		=  0;							//Current threshold low
	FuelGauge.alarm.temperature_h	=  50;
	FuelGauge.alarm.temperature_l	=  -20;
	
	i2c_status	=  hal_ltc2943_init();
	
	return(i2c_status);
}


/*************************************************************************************************
* Function Name : 	app_battery_fuel_shutdown
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t app_battery_fuel_save(void)
{
	// Local Variables
	uint8_t		i2c_status;
	
	// Code
	i2c_status							=  hal_ltc2943_charge_read();
	if(i2c_status						== 0){
		if(FuelGaugeReg.charge.a		>  LTC_CAPACITY){
			battery_saved.charge		=  LTC_CAPACITY;
		}
		else if(FuelGaugeReg.charge.a	<  LTC_OFFSET){
			battery_saved.charge		=  LTC_OFFSET;
		}
		else{
			battery_saved.charge		=  FuelGaugeReg.charge.a;		// Raw register data
		}
		
		battery_saved.init				=  BATTERY_SAVED_VALID;
		
		csp_sys_mem_wr(&battery_saved.init, MEM_ADD_BATT_MANAGEMENT, sizeof(battery_saved) );
	}
	
	return(i2c_status);
}


/*************************************************************************************************
* Function Name :	app_battery_new_battery
* Description   : 	This Function is called if a new battery is detected/ recorded
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/11/17	W. Paul			Created
*************************************************************************************************/
uint8_t app_battery_new_battery(void)
{
	uint8_t		i2c_status		=  0;
	
	battery_saved.full_flag		=  0;								// This flag will go hi when the battery has been fully charged
	i2c_status					+= hal_ltc2943_set_charge(0.25);	// This is a guess at the new battery level
	
	i2c_status					+= app_battery_fuel_save();
	
	return(i2c_status);
}


/*
*********************************************************************************************************
*											End of app_battery_fuel_guage.c
*********************************************************************************************************
*/
