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
 * Filename    :  app_selfcheck.c
 * Date Created:  Mon 23 Oct 2017 03:31:52 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "app_selfcheck.h"

#include "app_pneumatic_ctrl.h"
#include "app_patient_pressure.h"
#include "app_touchscreen.h"
#include "app_button.h"
#include "app_UI.h"
#include "app_battery_fuel_guage.h"
#include "api_IO.h"
#include "api_LEDs.h"
#include "api_audio.h"
#include "api_RTC.h"
#include "Language.h"

#include "api_calibrate.h"
#include "csp_S25FL0xx.h"
#include "csp_LCD_SSD1963.h"

#include "hal_STM32_uart.h"
#include "csp_STM32_uart.h"
#include "csp_paracube_O2.h"

//#include "hal_lcd.h"						//LCD
//#include "hal_lcd_text.h"					//LCD
//#include "csp_LCD_SSD1963.h"
//#include "fonts.h"
//#include "bitmaps.h"
//#include "Colours.h"

tst_status_t			SelfCheckRes[NoOfSelfTests];
SELFTEST_enum			selftest_num;

uint8_t					selfcheck_init			=  0;
uint8_t					selftest_mode			=  0;
uint32_t				selftest_loop_cnt		=  0;
uint8_t					selfcheck_AutoClear		=  0;
uint8_t					selfcheck_audio_mute	=  0;

uint8_t					SelfCheckResByte;

uint8_t					ignore_cnt;
uint8_t					pass_cnt;
uint8_t					notice_cnt;
uint8_t					warning_cnt;
uint8_t					fault_cnt;
uint8_t					critical_cnt;

uint8_t					quiet_settle_cnt;
uint8_t					quiet_user_cnt;
uint8_t					quiet_single_alert_cnt;

uint8_t					alarm_S0_cnt;
uint8_t					alarm_S1_cnt;

uint8_t					system_alarm_flag	=  0;
uint8_t					last_system_alarm_flag	=  0xff;

ALARM_PRIORITY_e                        system_alarm_priority      = ALARM_PRIORITY_LOW;
ALARM_PRIORITY_e                        last_system_alarm_priority = ALARM_PRIORITY_LOW;

uint32_t				UserQuietAlarmTmr		=  0;
uint32_t				FIO2HighAlarmTmr		=  0;
uint32_t				FIO2LowAlarmTmr			=  0;
uint32_t				SettleQuietAlarmTmr		=  0;
uint32_t				SettleQuietAlarmFiO2Tmr	=  0;
uint32_t				SelfCheck_loop_time		=  0;
uint16_t				swgen_state				=  TEST_PASS;

uint8_t		mem_data_A[0x20];
uint8_t		mem_data_B[0x20];

/*************************************************************************************************
* Function Name : 	app_selfcheck_manager
* Description   : 	This Function performs the selfchecks
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/1/18		W. Paul			Created
* 0.2.0         11/06/26        A.Kane                  Added Priority Tracking
*************************************************************************************************/
void app_selfcheck_manager(void)
{
	uint8_t							i;
	uint16_t						res	=  0;
	char* 							lead_str;
	char* 							res_str;
	uint8_t							status;
	APP_PNEUMATIC_CTRL_PID_enum		mode;
	uint32_t						adc_max;
	uint32_t						adc_min;
    extern	float		tacho;
	float fan_HI = 98*1.4; //these thresholds may need changed?
	float fan_LOW = 98*0.4; // ^^ see above 

	if(selfcheck_init == 0){
		selfcheck_init	=  1;
		memset(&SelfCheckRes,0x00,sizeof(SelfCheckRes));
		selftest_num	=  (SELFTEST_enum)(0);
                
                // Setting Fault Priorities
                SelfCheckRes[TEST_MEM_RDWR         ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_O2_a    ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_O2_b    ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_O2_c    ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_O2_d    ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_O2_e    ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_AIR_a   ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_AIR_b   ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_AIR_c   ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_AIR_d   ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_FLOW_AIR_e   ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_SENSOR_O2    ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_CAL_SENSOR_PP    ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_5V               ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_SUPPLY_AIR       ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_SUPPLY_O2        ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_SENSOR_AIR       ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_SENSOR_O2        ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_PP_MIN           ].flags.bits.alarm_priority = ALARM_PRIORITY_MEDIUM;
                SelfCheckRes[TEST_PP_MAX           ].flags.bits.alarm_priority = ALARM_PRIORITY_MEDIUM;
                SelfCheckRes[TEST_APNOEA           ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_PLIMIT           ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[FAN_DEFECT            ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_O2_SENSOR        ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_FIO2_HIGH        ].flags.bits.alarm_priority = ALARM_PRIORITY_MEDIUM;
                SelfCheckRes[TEST_FIO2_LOW         ].flags.bits.alarm_priority = ALARM_PRIORITY_MEDIUM;
                SelfCheckRes[TEST_FMAX             ].flags.bits.alarm_priority = ALARM_PRIORITY_MEDIUM;
                SelfCheckRes[TEST_O2_STARTUP_CAL   ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_RTC              ].flags.bits.alarm_priority = ALARM_PRIORITY_LOW;
                SelfCheckRes[TEST_AC_SUPPLY        ].flags.bits.alarm_priority = ALARM_PRIORITY_LOW;
                SelfCheckRes[TEST_BATTERY_FITTED   ].flags.bits.alarm_priority = ALARM_PRIORITY_MEDIUM;
                SelfCheckRes[TEST_BATTERY_CHARGE   ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_HELD_TOUCH       ].flags.bits.alarm_priority = ALARM_PRIORITY_MEDIUM;
                SelfCheckRes[TEST_HELD_KEY         ].flags.bits.alarm_priority = ALARM_PRIORITY_LOW;
                SelfCheckRes[TEST_SENSOR_PP        ].flags.bits.alarm_priority = ALARM_PRIORITY_HIGH;
                SelfCheckRes[TEST_SWGEN            ].flags.bits.alarm_priority = ALARM_PRIORITY_LOW;
                	}
       

	switch(selftest_num){
	// *********************************************************************************************************
	//only at startup
		case TEST_MEM_RDWR:
			res		=  TEST_PASS;
			if(SelfCheckRes[TEST_MEM_RDWR].flags.bits.status			!= TEST_PASS){
				csp_erase_4kb_sectors(MEM_ADD_SELFCHECK_RD_WR);
				for(i=0;i< sizeof(mem_data_A);i++){	mem_data_A[i]		=  i*7;	}
				csp_mem_wr(mem_data_A, MEM_ADD_SELFCHECK_RD_WR, sizeof(mem_data_A));
				csp_mem_rd(mem_data_B, MEM_ADD_SELFCHECK_RD_WR, sizeof(mem_data_B));
				if( memcmp(mem_data_A,mem_data_B,sizeof(mem_data_A))	== 0){	res		=  TEST_PASS;			}
				else{															res		=  TEST_FAIL_CRITICAL;	}
			}
			break;
		case TEST_RTC:
			if( time(0) >= TIME_MIN ){	res		=  TEST_PASS;		}
			else{						res		=  TEST_WARNING;	}
			break;
		case TEST_CAL_FLOW_O2_a:
		case TEST_CAL_FLOW_O2_b:
		case TEST_CAL_FLOW_O2_c:
		case TEST_CAL_FLOW_O2_d:
			if( (calibration[selftest_num-(TEST_CAL_FLOW_O2_a-(SELFTEST_enum)CAL_FLOW_O2_a)].valid			== CAL_VALID)		&&
				(calibration[selftest_num-(TEST_CAL_FLOW_O2_a-(SELFTEST_enum)CAL_FLOW_O2_a)].calib_status	== CALIB_SUCCESS)	){	res	=  TEST_PASS;			}
			else{																													res	=  TEST_FAIL_CRITICAL;	}
			if(	(cal_temperature_O2		== 0.0)	||	(isnan(cal_temperature_O2))		){												res	=  TEST_FAIL_CRITICAL;	}
			break;
		case TEST_CAL_FLOW_AIR_a:
		case TEST_CAL_FLOW_AIR_b:
		case TEST_CAL_FLOW_AIR_c:
		case TEST_CAL_FLOW_AIR_d:
			if( (calibration[selftest_num-(TEST_CAL_FLOW_AIR_a-(SELFTEST_enum)CAL_FLOW_AIR_a)].valid		== CAL_VALID)		&&
				(calibration[selftest_num-(TEST_CAL_FLOW_AIR_a-(SELFTEST_enum)CAL_FLOW_AIR_a)].calib_status	== CALIB_SUCCESS)	){	res	=  TEST_PASS;			}
			else{																													res	=  TEST_FAIL_CRITICAL;	}
			if(	(cal_temperature_air	== 0.0)	||	(isnan(cal_temperature_air))	){												res	=  TEST_FAIL_CRITICAL;	}
			break;
		case TEST_CAL_SENSOR_O2:
			if(app_Pneumatics.O2_sensor_sel				== O2_SENSOR_MAX250){
				if( (calibration[CAL_SENSOR_O2].valid														== CAL_VALID)		&&
					(calibration[CAL_SENSOR_O2].calib_status												== CALIB_SUCCESS)	){	res	=  TEST_PASS;			}
				else{																												res	=  TEST_FAIL_CRITICAL;	}
			}
			else{
				res																														=  TEST_PASS;
			}
			break;
		case TEST_CAL_SENSOR_PP:
			if( (calibration[CAL_SENSOR_PP].valid															== CAL_VALID)		&&
				(calibration[CAL_SENSOR_PP].calib_status													== CALIB_SUCCESS)	){	res	=  TEST_PASS;			}
			else{																													res	=  TEST_FAIL_CRITICAL;	}
			break;
		case TEST_CAL_FLOW_O2_e:
			if(	(calibration[CAL_FLOW_O2_e].valid															== CAL_VALID)		&&
				(calibration[CAL_FLOW_O2_e].calib_status													== CALIB_SUCCESS)	){	res	=  TEST_PASS;			}
			else{																													res	=  TEST_FAIL_CRITICAL;	}
			if(	(cal_temperature_O2		== 0.0)	||	(isnan(cal_temperature_O2))		){												res	=  TEST_FAIL_CRITICAL;	}
			break;
		case TEST_CAL_FLOW_AIR_e:
			if(	(calibration[CAL_FLOW_AIR_e].valid															== CAL_VALID)		&&
				(calibration[CAL_FLOW_AIR_e].calib_status													== CALIB_SUCCESS)	){	res	=  TEST_PASS;			}
			else{																													res	=  TEST_FAIL_CRITICAL;	}
			if(	(cal_temperature_air	== 0.0)	||	(isnan(cal_temperature_air))	){												res	=  TEST_FAIL_CRITICAL;	}
			break;
	// *********************************************************************************************************
	//check always
		case TEST_SWGEN:
			res		=  swgen_state;
			break;
			
		case TEST_BATTERY_FITTED:
			if(battery_charger.battery_status	== 0){				res	=  TEST_PASS;	}
			else{													res	=  TEST_FAIL;	}
			break;
			
		case TEST_5V:
			if(		(io_status_st_glb.Test_5V	<  4.00)	||
					(io_status_st_glb.Test_5V	>  6.00)	){		res	=  TEST_FAIL_CRITICAL;	}
			else if((io_status_st_glb.Test_5V	<  4.75)	||
					(io_status_st_glb.Test_5V	>  5.25)	){		res	=  TEST_FAIL;	}
			else{													res	=  TEST_PASS;	}
			break;
			
		case TEST_24V:
			res		=  TEST_PASS;
			break;
			
		case TEST_SUPPLY_AIR:
			mode														=  app_pneumatic_pid_mode_read();
			if(mode														== PNEUMATIC_CTRL_PID_MATH){
				if(SelfCheckRes[selftest_num].flags.bits.single_alert	!= 0){		// Depends on code below - just entered therapy
					SelfCheckRes[selftest_num].flags.bits.user_quiet	=  0;
				}
				SelfCheckRes[selftest_num].flags.bits.single_alert		=  0;
			}
			else{
				SelfCheckRes[selftest_num].flags.bits.single_alert		=  1;
			}
			
			if(api_io_rd_AirSupply(0)){
				res														=  TEST_PASS;
				SelfCheckRes[selftest_num].flags.bits.userSilenceCnt	= 0;
			}
			else{
				res														=  TEST_FAIL_CRITICAL;
				if(SelfCheckRes[selftest_num].flags.bits.status_live	<  TEST_FAIL_CRITICAL){		// If new critical fault ...
					if(SettleQuietAlarmTmr								>  0){						// ... and in settling period ...
						SettleQuietAlarmTmr								=  1;						// ... end settling period
					}
					SelfCheckRes[selftest_num].flags.bits.status_live	=  TEST_FAIL_CRITICAL;
					SelfCheckRes[selftest_num].flags.bits.DetectCnt		=  FAULT_DET_CNT;			// no delay on this error .. so jump past detect count
				}
			}
			break;
			
		case TEST_SUPPLY_O2:
			mode														=  app_pneumatic_pid_mode_read();
			if(mode														== PNEUMATIC_CTRL_PID_MATH){
				if(SelfCheckRes[selftest_num].flags.bits.single_alert	!= 0){		// Depends on code below - just entered therapy
					SelfCheckRes[selftest_num].flags.bits.user_quiet	=  0;
				}
				SelfCheckRes[selftest_num].flags.bits.single_alert		=  0;
			}
			else{
				SelfCheckRes[selftest_num].flags.bits.single_alert		=  1;
			}

			if(api_io_rd_O2Supply(0)){
				res														=  TEST_PASS;
				SelfCheckRes[selftest_num].flags.bits.userSilenceCnt	=  0;
			}
			else{
				res														=  TEST_FAIL_CRITICAL;
				if(SelfCheckRes[selftest_num].flags.bits.status_live	<  TEST_FAIL_CRITICAL){		// If new critical fault ...
					if(SettleQuietAlarmTmr								>  0){						// ... and in settling period ...
						SettleQuietAlarmTmr								=  1;						// ... end settling period
					}
					SelfCheckRes[selftest_num].flags.bits.status_live	=  TEST_FAIL_CRITICAL;
					SelfCheckRes[selftest_num].flags.bits.DetectCnt		=  FAULT_DET_CNT;			// no delay on this error .. so jump past detect count
				}
			}
			break;
			
		case TEST_AC_SUPPLY:
			//may not be updated in battery manager so update status here
			app_battery_charger_status();
			if(battery_charger.status.bits.ACP							== 1){
				res														=  TEST_PASS;
				SelfCheckRes[selftest_num].flags.bits.userSilenceCnt	=  0;		//restart count
				SelfCheckRes[selftest_num].flags.bits.status_live		=  res;
				SelfCheckRes[selftest_num].flags.bits.status			=  res;
				SelfCheckRes[selftest_num].flags.bits.need_ack			=  0;
				SelfCheckRes[selftest_num].flags.bits.single_alert		=  0;
				SelfCheckRes[selftest_num].flags.bits.settle_quiet		=  0;		//can't be delayed for settling
				SelfCheckRes[selftest_num].flags.bits.user_quiet		=  0;		//can't be silenced by user
			}
			else{
				if(app_pneumatic_pid_mode_read()						== PNEUMATIC_CTRL_PID_MATH){
					res													=  TEST_WARNING;
					SelfCheckRes[selftest_num].flags.bits.single_alert	=  1;		// single alert
				}
				else{
					res													=  TEST_NOTICE;
					SelfCheckRes[selftest_num].flags.bits.single_alert	=  1;		//single alert
				}
			}
			break;
			
		case TEST_O2_SENSOR:
			res															=  TEST_PASS;
			if(app_Pneumatics.O2_sensor_sel								== O2_SENSOR_PARACUBE){
				if(io_status_st_glb.Paracube_O2_fault					!= 0){
					res													=  TEST_WARNING;
					if(SelfCheckRes[TEST_O2_SENSOR].flags.bits.ignore	== 0){			// if not in caliration
						csp_paracube_command(PARACUBE_RESTORE_CAL, 0);					// attempt fix
					}
				}
				
				if(csp_paracube_timeout_read(0)							== 1){			// Timeout elapsed, not cleared
					res													=  TEST_FAIL;	// No response from sensor
				}
			}
			if(app_Pneumatics.O2_conc_actual							>  110.0){
				res														=  TEST_FAIL;	//calibration is wrong
			}
			break;
			
		case TEST_SENSOR_PP:
			res															=  TEST_PASS;
			
			if(	(UI.select_mode							== PMODE_HFOT)	||
				(UI.select_mode							== PMODE_POINT)	){
				// do nothing
			}
			else if(calibration[CAL_SENSOR_PP].valid	== CAL_VALID){
				adc_max	=  (uint32_t)( (float)((PP_MAX_Pressure + ADC_PP_RAW_GRACE) - calibration[CAL_SENSOR_PP].c)/calibration[CAL_SENSOR_PP].m);
				adc_min	=  (uint32_t)( (float)((PP_MIN_Pressure - ADC_PP_RAW_GRACE) - calibration[CAL_SENSOR_PP].c)/calibration[CAL_SENSOR_PP].m);
				if(		io_status_st_glb.ADC_patientP	>  adc_max	){
					res	=  TEST_FAIL;
					printf("\r\nPatient Pressure sensor: adc high");
				}
				else if(io_status_st_glb.ADC_patientP	<  adc_min	){
					res	=  TEST_FAIL;
					printf("\r\nPatient Pressure sensor: adc low");
				}
			}
			break;
		case TEST_HELD_TOUCH:
			if(app_touch_hold_err()){	res	=  TEST_WARNING;	}
			else{						res	=  TEST_PASS;		}
			break;
		case TEST_HELD_KEY:
			if(app_button_hold_err()){	res	=  TEST_WARNING;	}
			else{						res	=  TEST_PASS;		}
			break;
		case TEST_BATTERY_CHARGE:
			SelfCheckRes[selftest_num].flags.bits.single_alert				=  1;
			if(battery_charger.mode											!= BAT_UNKNOWN){
				if(PinRead(LTC4009_ACP)										== 0){			//charging
					res	=  TEST_PASS;
					SelfCheckRes[selftest_num].flags.bits.userSilenceCnt	=  0;
					SelfCheckRes[selftest_num].flags.bits.status_live		=  res;
					SelfCheckRes[selftest_num].flags.bits.status			=  res;
					SelfCheckRes[selftest_num].flags.bits.settle_quiet		=  0;
					SelfCheckRes[selftest_num].flags.bits.user_quiet		=  0;
					SelfCheckRes[selftest_num].flags.bits.need_ack			=  0;
					if(SelfCheckRes[TEST_BATTERY_CHARGE].flags.bits.ignore){
						app_selfcheck_SetIgnore(TEST_BATTERY_CHARGE,0);
					}
				}
				else{
					if(			(battery_charger.sec_remaining	<  120)					||
								(battery_charger.percentage		<  0.5)					){
						res										=  TEST_FAIL_CRITICAL;
					}
					else if(	(battery_charger.sec_remaining	<  15*60)				||
								(battery_charger.percentage		<  21.0)				){
						res										=  TEST_FAIL;
					}
					else{
						res										=  TEST_PASS;
					}
					
					//alarm has been reported and will be silenced
					if(	(res	>  SelfCheckRes[selftest_num].flags.bits.status)		||
						(res	== TEST_FAIL_CRITICAL)									){
						SelfCheckRes[selftest_num].flags.bits.userSilenceCnt	=  0;		//restart count
						SelfCheckRes[selftest_num].flags.bits.single_alert		=  0;
						SelfCheckRes[selftest_num].flags.bits.settle_quiet		=  0;		//can't be delayed for settling
						SelfCheckRes[selftest_num].flags.bits.user_quiet		=  0;		//can't be silenced by user
						app_selfcheck_SetIgnore(TEST_BATTERY_CHARGE, 0);
					}
				}
			}
			else{
				res				=  TEST_NOTICE;
			}
			
			break;
			
		case TEST_O2_STARTUP_CAL:
			SelfCheckRes[selftest_num].flags.bits.single_alert	=  1;
			switch(app_Pneumatics.startup_o2_cal_status){
				case 0:		res	=  TEST_FAIL;			break;
				case 1:		res	=  TEST_PASS;			break;
				case 2:		res	=  TEST_NOTICE;			break;
				default:	res	=  TEST_PASS;
			}
			break;
	// *********************************************************************************************************
	//during treatment
		case TEST_PP_MIN:
			if(	(app_pneumatic_pid_mode_read()	!= PNEUMATIC_CTRL_PID_MATH)	||
				(UI.select_mode					== PMODE_HFOT)				||
				(UI.select_mode					== PMODE_POINT)				){
				res								=  TEST_PASS;
			}
			else{
				if(PP_settings.Patient_Pressure_min.en_dis	== DIS){											res	=  TEST_PASS;	}
				else if(PP_data.PatientPressureAv_x10		<  PP_settings.Patient_Pressure_min.value * 10){	res	=  TEST_FAIL;	}
				else{																							res	=  TEST_PASS;	}
			}
			break;
		case TEST_PP_MAX:
			if(	(app_pneumatic_pid_mode_read()	!= PNEUMATIC_CTRL_PID_MATH)	||
				(UI.select_mode					== PMODE_HFOT)				||
				(UI.select_mode					== PMODE_POINT)				){
				res								=  TEST_PASS;
			}
			else{
				if(PP_settings.Patient_Pressure_max.en_dis	== DIS){											res	=  TEST_PASS;	}
				else if(PP_data.PatientPressureAv_x10		>  PP_settings.Patient_Pressure_max.value * 10){	res	=  TEST_FAIL;	}
				else{																							res	=  TEST_PASS;	}
			}
			if(res	== TEST_FAIL){
				//reduce flow to 1L/min
				if(app_Pneumatics.flow_rate_target			>  2){
					app_Pneumatics.flow_rate_target			=  2;
					app_pneumatic_cal_flows((float)app_Pneumatics.O2_conc_target);
					app_Pneumatics.pmax_at_2_lmin			=  AT_2_LMIN;
					
					SelfCheckRes[selftest_num].flags.bits.status_live	=  TEST_FAIL;
					SelfCheckRes[selftest_num].flags.bits.DetectCnt		=  FAULT_DET_CNT;	//no delay on this error .. so jump past detect count
					SelfCheckRes[selftest_num].flags.bits.settle_quiet	=  0;				//can't be delayed for settling
					SelfCheckRes[selftest_num].flags.bits.user_quiet	=  0;				//can't be silenced by user
				}
			}
			else{
				if(app_Pneumatics.pmax_at_2_lmin			== AT_2_LMIN){
					app_Pneumatics.pmax_at_2_lmin			=  READY_TO_RETURN_TO_NORMAL;
					
					if(app_Pneumatics.plimit_at_2_lmin		!= AT_2_LMIN){					//check that other flags are ready to return to normal
						app_Pneumatics.plimit_at_2_lmin		=  AT_NORMAL_FLOW;
						app_Pneumatics.pmax_at_2_lmin		=  AT_NORMAL_FLOW;
						
						if(	(UI.flow_override				== OVERRIDE_NONE)	||
							(UI.flow_override				== OVERRIDE_AVAIL)	){
							app_Pneumatics.flow_rate_target	=  UI.flowRate;
						}
						else{
							app_Pneumatics.flow_rate_target	=  UI.flowRate_override;
						}
						app_pneumatic_cal_flows((float)app_Pneumatics.O2_conc_target);
					}
				}
			}
			break;
		case TEST_APNOEA:
			if(	(app_pneumatic_pid_mode_read()	!= PNEUMATIC_CTRL_PID_MATH)	||
				(UI.select_mode					== PMODE_BUBBLE_PAP)		||
				(UI.select_mode					== PMODE_HFOT)				||
				(UI.select_mode					== PMODE_POINT)				){
				res								=  TEST_PASS;
			}
			else{
				if(PP_data.Apnoea.alarm_flag){
					res							=  TEST_FAIL;
					if(SelfCheckRes[selftest_num].flags.bits.status_live	!= TEST_FAIL){
						SelfCheckRes[selftest_num].flags.bits.status_live	=  TEST_FAIL;
						SelfCheckRes[selftest_num].flags.bits.DetectCnt		=  FAULT_DET_CNT;	//no delay on this error .. so jump past detect count
					}
				}
				else{
					res							=  TEST_PASS;
				}
			}
			break;
		case TEST_FMAX:
			if(	(app_pneumatic_pid_mode_read()	!= PNEUMATIC_CTRL_PID_MATH)	||
				(UI.select_mode					== PMODE_HFOT)				||
				(UI.select_mode					== PMODE_POINT)				){
				res								=  TEST_PASS;
			}
			else{
				if(	(PP_data.BreathRate.value			>  PP_settings.Respiration_rate_max)	&&
					(PP_settings.Respiration_rate_max	!= 0)									){
					res									=  TEST_WARNING;
				}
				else{
					res									=  TEST_PASS;
				}
			}
			break;
		case TEST_PLIMIT:
			if(	(app_pneumatic_pid_mode_read()	!= PNEUMATIC_CTRL_PID_MATH)	||
				(UI.select_mode					== PMODE_HFOT)				||
				(UI.select_mode					== PMODE_POINT)				){
				res								=  TEST_PASS;
			}
			else if(	(UI.select_mode			== PMODE_CPAP)				||
						(UI.select_mode			== PMODE_CPAP_PAED)			||
						(UI.select_mode			== PMODE_CPAP_HELMET)		){
				if(PP_data.PatientPressure_Raw_x10	>  ((25)*10)){
					if(UserQuietAlarmTmr			>  1){
						UserQuietAlarmTmr			=  1;
					}
					res								=  TEST_FAIL;
				}
				else{
					res								=  TEST_PASS;
				}
			}
			else if(UI.select_mode				== PMODE_BUBBLE_PAP){
				if(PP_data.PatientPressure_Raw_x10	>  ((15)*10)){
					if(UserQuietAlarmTmr			>  1){
						UserQuietAlarmTmr			=  1;
					}
					res								=  TEST_FAIL;
				}
				else{
					res								=  TEST_PASS;
				}
			}
			
			if(res								== TEST_FAIL){
				//reduce flow to 2L/min
				if(app_Pneumatics.flow_rate_target	>  2){
					app_Pneumatics.flow_rate_target	=  2;
					app_pneumatic_cal_flows((float)app_Pneumatics.O2_conc_target);
					app_Pneumatics.plimit_at_2_lmin	=  AT_2_LMIN;
					SelfCheckRes[selftest_num].flags.bits.status_live	=  TEST_FAIL;
					SelfCheckRes[selftest_num].flags.bits.DetectCnt		=  FAULT_DET_CNT;	//no delay on this error .. so jump past detect count
					SelfCheckRes[selftest_num].flags.bits.settle_quiet	=  0;				//can't be delayed for settling
					SelfCheckRes[selftest_num].flags.bits.user_quiet	=  0;				//can't be silenced by user
				}
			}
			else{
				if(app_Pneumatics.plimit_at_2_lmin	== AT_2_LMIN){
					app_Pneumatics.plimit_at_2_lmin	=  READY_TO_RETURN_TO_NORMAL;
					if(app_Pneumatics.pmax_at_2_lmin	!= AT_2_LMIN){						//check that other flags are ready to return to normal
						app_Pneumatics.plimit_at_2_lmin	=  AT_NORMAL_FLOW;
						app_Pneumatics.pmax_at_2_lmin	=  AT_NORMAL_FLOW;
						
						if(	(UI.flow_override				== OVERRIDE_NONE)	||
							(UI.flow_override				== OVERRIDE_AVAIL)	){
							app_Pneumatics.flow_rate_target	=  UI.flowRate;
						}
						else{
							app_Pneumatics.flow_rate_target	=  UI.flowRate_override;
						}
						app_pneumatic_cal_flows((float)app_Pneumatics.O2_conc_target);
					}
				}
			}
			break;
			
		case TEST_FIO2_HIGH:
			res											=  TEST_PASS;
			if(app_pneumatic_pid_mode_read()			!= PNEUMATIC_CTRL_PID_IDLE){
				if(app_Pneumatics.O2_conc_actual		>  app_Pneumatics.O2_conc_target + 5.0){
					if(FIO2HighAlarmTmr					== 0){
						FIO2HighAlarmTmr				=  18000;		// 18 seconds
					}
					else if(FIO2HighAlarmTmr			== 1){
						res								=  TEST_WARNING;
					}
				}
				else if(FIO2HighAlarmTmr				>  0){
					FIO2HighAlarmTmr					=  0;
				}
			}
			else if(FIO2HighAlarmTmr					>  0){
				FIO2HighAlarmTmr						=  0;
			}
			break;
			
		case TEST_FIO2_LOW:
			res											=  TEST_PASS;
			if(app_pneumatic_pid_mode_read()			!= PNEUMATIC_CTRL_PID_IDLE){
				if(app_Pneumatics.O2_conc_actual		<  18){
					res									=  TEST_WARNING;
				}
				else if(app_Pneumatics.O2_conc_actual	<  app_Pneumatics.O2_conc_target - 5.0){
					if(FIO2LowAlarmTmr					== 0){
						FIO2LowAlarmTmr					=  18000;		// 18 seconds
					}
					else if(FIO2LowAlarmTmr				== 1){
						res								=  TEST_WARNING;
					}
				}
				else if(FIO2LowAlarmTmr					>  0){
					FIO2LowAlarmTmr						=  0;
				}
			}
			else if(FIO2LowAlarmTmr						>  0){
				FIO2LowAlarmTmr							=  0;
			}
			break;
			
		case TEST_32:
			res		=  TEST_PASS;
			break;
			
		case TEST_33:
			res		=  TEST_PASS;
			break;
			
		case FAN_DEFECT:
			res		=  TEST_PASS;
			
			if(io_status_st_glb.fan	>= PWM_FAN_HI){	// Fan On
				if(tacho			<= fan_LOW){
					res				=  TEST_FAIL;
					printf("\r\n Fan speed lower than it should be");
				}
			}
			else{
				if(tacho			>= fan_HI){		// Fan On
					res				=  TEST_FAIL; 
					printf("\r\n Fan speed higher than it should be");
				}
			}
			
			break;
		
		case TEST_35:
			res		=  TEST_PASS;
			break;
			
		case TEST_SENSOR_AIR:
			res		=  TEST_PASS;
			if(UI.demo_mode							== 0){
				//AIR
				if(api_io_rd_AirSupply(0)			== 0){			//no air supply
					if(io_status_st_glb.Flow_Air	>  20.0){
						res							=  TEST_FAIL;	//	but flow detected
						printf("\r\nAir flow sensor: Flow detected when no air supply");
					}
				}
				else{												//air supply
					if(	(io_status_st_glb.pwm_Air	>  pwm_calibration.pwm_air + 1500)	&&	//	valve open
						(io_status_st_glb.Flow_Air	<  1.0)								){	//	but no flow
						res							=  TEST_FAIL;
						printf("\r\nAir flow sensor: Flow NOT detected when air supply avail and valve enabled");
					}
					if(	(io_status_st_glb.pwm_Air	== PWM_AIR_MIN)	&&						//	valve closed
						(io_status_st_glb.Flow_Air	>  20.0)		){						//	but flow
						res							=  TEST_FAIL;
						printf("\r\nAir flow sensor: Flow detected when air supply avail and valve disabled");
					}
				}
			}
			break;
				//O2
		case TEST_SENSOR_O2:
			res										=  TEST_PASS;
			if(UI.demo_mode							== 0){
				if(api_io_rd_O2Supply(0)			== 0){					//no O2 supply
					if(io_status_st_glb.Flow_O2		>  20.0){				//	but flow detected
						res							=  TEST_FAIL;
						printf("\r\nO2 flow sensor: Flow detected when no O2 supply");
					}
				}
				else{														//O2 supply
					if(	(io_status_st_glb.pwm_O2	>  pwm_calibration.pwm_O2 + 1500)	&&	//	valve open
						(io_status_st_glb.Flow_O2	<  1.0)								){	//	but no flow
						res							=  TEST_FAIL;
						printf("\r\nO2 flow sensor: Flow NOT detected when O2 supply avail and valve enabled");
					}
					if(	(io_status_st_glb.pwm_O2	== PWM_O2_MIN)	&&						//	valve closed
						(io_status_st_glb.Flow_O2	>  20.0)		){						//	but flow
						res							=  TEST_FAIL;
						printf("\r\nO2 flow sensor: Flow detected when O2 supply avail and valve disabled");
					}
				}
			}
			break;

	// *********************************************************************************************************
	//end of tests
		case TEST_LAST:
			ignore_cnt				=  0;
			pass_cnt				=  0;
			notice_cnt				=  0;
			warning_cnt				=  0;
			fault_cnt				=  0;
			critical_cnt			=  0;
			quiet_settle_cnt		=  0;
			quiet_user_cnt			=  0;
			quiet_single_alert_cnt	=  0;

			alarm_S0_cnt			=  0;
			alarm_S1_cnt			=  0;
                        system_alarm_priority   = ALARM_PRIORITY_LOW;   // reset before loop
			for(i=0;i<NoOfSelfTests-2;i++){	//don't include TEST_LAST or TEST_WAIT
                                if( (SelfCheckRes[i].flags.bits.status      >  TEST_PASS) &&
                                  (SelfCheckRes[i].flags.bits.ignore       == 0)       &&
                                  (SelfCheckRes[i].flags.bits.user_quiet   == 0)        ){
                                if(SelfCheckRes[i].flags.bits.alarm_priority > system_alarm_priority){
                                    system_alarm_priority = (ALARM_PRIORITY_e)SelfCheckRes[i].flags.bits.alarm_priority;
                                  }
                                }
				if(			SelfCheckRes[i].flags.bits.ignore	== 1){						ignore_cnt				+= 1;	}
				else{
					if(		SelfCheckRes[i].flags.bits.status		== TEST_PASS){			pass_cnt				+= 1;	}
					else if(SelfCheckRes[i].flags.bits.status		== TEST_NOTICE){		notice_cnt				+= 1;	}
					else if(SelfCheckRes[i].flags.bits.status		== TEST_WARNING){		warning_cnt				+= 1;	}
					else if(SelfCheckRes[i].flags.bits.status		== TEST_FAIL){			fault_cnt				+= 1;	}
					else if(SelfCheckRes[i].flags.bits.status		== TEST_FAIL_CRITICAL){	critical_cnt			+= 1;	}
				}
				if(			SelfCheckRes[i].flags.bits.settle_quiet	== 1){					quiet_settle_cnt		+= 1;	}
				if(			SelfCheckRes[i].flags.bits.user_quiet	== 1){					quiet_user_cnt			+= 1;	}
				if(			SelfCheckRes[i].flags.bits.single_alert	== 1){					quiet_single_alert_cnt	+= 1;	}


				//these are the conditions that say there is no fault
				if(	(SelfCheckRes[i].flags.bits.status				== TEST_PASS)	||
					(SelfCheckRes[i].flags.bits.ignore)								||
					(SelfCheckRes[i].flags.bits.settle_quiet		== 1)			||
					(	(SelfCheckRes[i].flags.bits.single_alert	== 1)		&&
						(SelfCheckRes[i].flags.bits.userSilenceCnt	>= 1)		)	){
					alarm_S0_cnt									+= 1;			//if all alarms meet this condition then no alarm
				}
				//these are the conditions that say there is no alarm... but ack is needed
				if(	(SelfCheckRes[i].flags.bits.status				== TEST_PASS)	||
					(SelfCheckRes[i].flags.bits.status_live			== TEST_PASS)	||
//					(SelfCheckRes[i].flags.bits.status_live			<= SelfCheckRes[i].flags.bits.status)	||
					(SelfCheckRes[i].flags.bits.ignore)								||
					(	(SelfCheckRes[i].flags.bits.single_alert	== 1)	&&
						(SelfCheckRes[i].flags.bits.userSilenceCnt	>= 1)	)		||
						(SelfCheckRes[i].flags.bits.settle_quiet	== 1)			||
					(SelfCheckRes[i].flags.bits.user_quiet			== 1)			){
					alarm_S1_cnt									+= 1;
				}
			}
			
			SelfCheckResByte		=  notice_cnt + warning_cnt + fault_cnt + critical_cnt;
			if(	(SelfCheckResByte	== 0)	&&
				(UserQuietAlarmTmr	>  0)	){
				//if all alarms are fixed... then stop quiet alarm timer
				UserQuietAlarmTmr	=  1;
			}
			
		//Settle Quiet Alarms
			if(SettleQuietAlarmTmr	== 1){
				printf("\r\nQuiet Alarms - Settle - Disabled");
				
				for(i=TEST_SUPPLY_AIR;i<=TEST_SUPPLY_O2;i++){
					SelfCheckRes[i].flags.bits.settle_quiet			=  0;
					SelfCheckRes[i].flags.bits.DetectCnt			=  0;
					SelfCheckRes[i].flags.bits.need_ack				=  0;
					SelfCheckRes[i].flags.bits.status				=  SelfCheckRes[i].flags.bits.status_live;
				}//
				for(i=TEST_PP_MIN;i<=TEST_PLIMIT;i++){
					SelfCheckRes[i].flags.bits.settle_quiet			=  0;
					SelfCheckRes[i].flags.bits.DetectCnt			=  0;
					SelfCheckRes[i].flags.bits.need_ack				=  0;
					SelfCheckRes[i].flags.bits.status				=  SelfCheckRes[i].flags.bits.status_live;
				}//
				SelfCheckRes[FAN_DEFECT].flags.bits.settle_quiet	=  0;
				SelfCheckRes[FAN_DEFECT].flags.bits.DetectCnt		=  0;
				SelfCheckRes[FAN_DEFECT].flags.bits.need_ack		=  0;
				SelfCheckRes[FAN_DEFECT].flags.bits.status			=  SelfCheckRes[FAN_DEFECT].flags.bits.status_live;
				
				last_system_alarm_flag								=  0xff;
				SettleQuietAlarmTmr									=  0;
				app_PP_settleAlarm_end();
				
				api_audio_alarm_reset();		// clear audio stop flag
			}
			if(SettleQuietAlarmFiO2Tmr	== 1){
				printf("\r\nQuiet Alarms (FiO2) - Settle - Disabled");
				
				for(i=TEST_FIO2_HIGH;i<=TEST_FIO2_LOW;i++){
					SelfCheckRes[i].flags.bits.settle_quiet	=  0;
					SelfCheckRes[i].flags.bits.DetectCnt	=  0;
					SelfCheckRes[i].flags.bits.need_ack		=  0;
					SelfCheckRes[i].flags.bits.status		=  SelfCheckRes[i].flags.bits.status_live;
				}//
				SettleQuietAlarmFiO2Tmr						=  0;
			}
		//User Quiet Alarms
			if(UserQuietAlarmTmr							== 1){
				printf("\r\nQuiet Alarms - User - Disabled");
				for(i=TEST_ALWAYS_START;i<TEST_ALWAYS_END;i++){
					SelfCheckRes[i].flags.bits.user_quiet	=  0;
					SelfCheckRes[i].flags.bits.DetectCnt	=  FAULT_DET_CNT;
//					SelfCheckRes[i].flags.bits.status = SelfCheckRes[i].flags.bits.status_live;		//wtp 21/5/19
				}
				UserQuietAlarmTmr							=  0;
			}
			
			if(alarm_S0_cnt									>= (NoOfSelfTests - 2)	){
				system_alarm_flag							=  0;
				if(UserQuietAlarmTmr){
					UserQuietAlarmTmr						=  3;
				}
			}
			else if(alarm_S1_cnt							>= (NoOfSelfTests-2)	){
				system_alarm_flag							=  1;
			}
			
			//Startup selftest flag
			if(selftest_loop_cnt							<  0xfffffffe){
				selftest_loop_cnt++;
			}
			break;
			
		case TEST_WAIT:
			break;
			
		default:
			selftest_num	=  (SELFTEST_enum)(0);
	}
	
	if(selftest_mode				== 1){		//no led or alarm during the startup self test
		if((last_system_alarm_flag     != system_alarm_flag) ||
   (last_system_alarm_priority != system_alarm_priority)){

    last_system_alarm_flag      = system_alarm_flag;      // existing
    last_system_alarm_priority  = system_alarm_priority;  // ADD HERE

    switch(system_alarm_flag){
        case 0:
            api_LED(LED_ALM, LED_OFF, 0x00);
            api_audio_play_file_stop();
            api_audio_alarm_reset(); 
            printf("\r\n[No Faults]");
            break;
        case 1:
            api_LED(LED_ALM, LED_RAMP, 0xFF);
            api_audio_play_file_stop();
            api_audio_alarm_reset(); 
            printf("\r\n[Faults quieted]");
            break;
        case 2:
            switch(system_alarm_priority){
            case ALARM_PRIORITY_HIGH:
                api_LED_flash_rate(LED_ALM, 250);
                api_LED(LED_ALM, LED_FLASH, 0xFF);
                break;
            case ALARM_PRIORITY_MEDIUM:
                api_LED_flash_rate(LED_ALM, 833);
                api_LED(LED_ALM, LED_FLASH, 0xFF);
                break;
            case ALARM_PRIORITY_LOW:
                default:
                api_LED(LED_ALM, LED_ON, 0xFF);
                break;
            }
            mode = app_pneumatic_pid_mode_read();
            if( (selfcheck_audio_mute    == 0)                          &&
                ( (mode                  != PNEUMATIC_CTRL_PID_MATH)    ||
                  (SettleQuietAlarmTmr   == 0)                      )   &&
                (UI.demo_mode            == 0)                          &&
                (UI.screen               >= UI_MODE_SETUP)              ){
                api_audio_play_file_stop();
                api_audio_alarm_reset();
                if(api_audio_alarm(system_alarm_priority, 0)){
                    last_system_alarm_flag = 0xFF;
                }
            }
            else{
                last_system_alarm_flag = 0xFF;
            }
            printf("\r\n[Faults]");
            break;
    }
}
	}
	
	if(SelfCheckRes[selftest_num].flags.bits.status_live					!= res){
		SelfCheckRes[selftest_num].flags.bits.DetectCnt						=  0;
	}
	SelfCheckRes[selftest_num].flags.bits.status_live						=  res;
	
	if(SelfCheckRes[selftest_num].flags.bits.status							== TEST_PASS){
		SelfCheckRes[selftest_num].flags.bits.userSilenceCnt				=  0;
	}
	
	if(selftest_num															<  TEST_LAST){
		//&&(selftest_loop_cnt >= SELFTEST_STARTUP_DELAY)){
		if(	(SelfCheckRes[selftest_num].flags.bits.status					!= SelfCheckRes[selftest_num].flags.bits.status_live)	||
			(SelfCheckRes[selftest_num].flags.bits.DetectCnt				<= FAULT_DET_CNT)										){
			if(++SelfCheckRes[selftest_num].flags.bits.DetectCnt			>  FAULT_DET_CNT){
				SelfCheckRes[selftest_num].flags.bits.DetectCnt				=  FAULT_DET_CNT + 1;
				
				if(SelfCheckRes[selftest_num].flags.bits.status_live		>  SelfCheckRes[selftest_num].flags.bits.status){		//new error
					if(SelfCheckRes[selftest_num].flags.bits.status_live	>  TEST_PASS){
						SelfCheckRes[selftest_num].flags.bits.need_ack		=  1;													//will need acknowledged at some point
					}
					SelfCheckRes[selftest_num].flags.bits.status			=  SelfCheckRes[selftest_num].flags.bits.status_live;	//save the error to 'status'
				}
				if(selfcheck_AutoClear										== 1){													//if 'autoclearing' of errors is enabled
					SelfCheckRes[selftest_num].flags.bits.need_ack			=  0;													//no ack of error is needed
					SelfCheckRes[selftest_num].flags.bits.status			=  SelfCheckRes[selftest_num].flags.bits.status_live;	//allow downgrading of the alrm as it happens
				}
				
				app_selfcheck_result_str(STATUS,selftest_num, &lead_str, &res_str, &status);
				if(SelfCheckRes[selftest_num].flags.bits.ignore){
				}
				else if(SelfCheckRes[selftest_num].flags.bits.settle_quiet){
				}
				else if(SelfCheckRes[selftest_num].flags.bits.user_quiet){
				}
				else if(	(SelfCheckRes[selftest_num].flags.bits.single_alert		== 1)	&&
							(SelfCheckRes[selftest_num].flags.bits.userSilenceCnt	>= 1)	){
					SelfCheckRes[selftest_num].flags.bits.status					=  SelfCheckRes[selftest_num].flags.bits.status_live;
				}
				else{
					if(	(SelfCheckRes[selftest_num].flags.bits.status				== TEST_PASS)	||
						(SelfCheckRes[selftest_num].flags.bits.status_live			== TEST_PASS)	){
					}
					else{
						printf("\r\nSystem Alarm - %s\t\t%s", res_str, lead_str);
						system_alarm_flag											=  2;
						app_UI_screen_lock_timer_reset(1);							//unlock screen
						SelfCheck_loop_time											=  5;
						
						if(selftest_num												== TEST_BATTERY_CHARGE){
							if(SelfCheckRes[TEST_BATTERY_CHARGE].flags.bits.status	== TEST_FAIL_CRITICAL){
								if(UI.screen										!= UI_AUTOOFF_WAIT){
									UI.screen										=  UI_AUTOOFF_120;
									app_UI_stop_therapy(0);			// Do not remove flag of therapy stopped, for UI_AUTOOFF_120 update
								}
							}
						}
					}
				}
			}
		}
	}
	
	if(selftest_num					== TEST_WAIT){
		if(SelfCheck_loop_time		== 0){
			SelfCheck_loop_time		=  500;		//all tests taken every 0.5seconds
			if(selftest_mode		== 0){	selftest_num	=  TEST_STARTUP_START;	}
			else{							selftest_num	=  TEST_ALWAYS_START;	}		//go to start of 'always check' section
			
			if(LCD_Check_Reset()	>  0){
				UI.screen			&= 0xFE;	//rebuild screen
			}
		}
	}
	else{
		selftest_num++;
	}
	
	return;
}


/*************************************************************************************************
* Function Name : 	app_selfcheck_print_result
* Description   : 	This Function prints the result from the selfchecks
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		10/4/18		W. Paul			Created
*************************************************************************************************/
void app_selfcheck_print_result(void)
{
	uint8_t		tst_no;
	char*		lead_str;
	char*		res_str;
	uint8_t		status;
	printf("\r\nSelfcheck");
	
	for(tst_no=0;tst_no<NoOfSelfTests-2;tst_no++){
		app_selfcheck_result_str(STATUS,tst_no, &lead_str, &res_str, &status);
		
		//force debug language to english
		res_str		=  (char*)EnglishPhrase[lang_array_test[status]];
		lead_str	=  (char*)EnglishPhrase[lang_array_selftest[tst_no]];
		
		printf("\r\n %s %s"		,res_str	,lead_str	);
	}
	
	printf("\r\n > %d",SelfCheckResByte);
	printf("\r\n^");
	return;
}


/*************************************************************************************************
* Function Name : 	app_selfcheck_print_result
* Description   : 	This Function prints the result from the selfchecks
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		29/6/18		W. Paul			Created
*************************************************************************************************/
void app_selfcheck_print_status(void)
{
	uint8_t		tst_no;
	char*		lead_str;

	printf("\r\nSelfcheck");
	printf("\r\n\\");
	printf("\r\n L Status Live");
	printf("\r\n S Status");
	printf("\r\n I Ignore");
	printf("\r\n S Settle quiet");
	printf("\r\n U user quiet");
	printf("\r\n 1 single alert");
	printf("\r\n U user Silence count");
	printf("\r\n D Detect count");
	printf("\r\n/");
	printf("\r\n    Type  qt   cnt");
	printf("\r\n No L S ISU1 U Det dis ack name");

	for(tst_no=0;tst_no < NoOfSelfTests-2;tst_no++){
		//force debug language to english
		lead_str	=  (char*)EnglishPhrase[lang_array_selftest[tst_no] ];
		
		printf("\r\n %2d %d %d %d%d%d%d %d  %d   %d   %s"	,tst_no
															,SelfCheckRes[tst_no].flags.bits.status_live
															,SelfCheckRes[tst_no].flags.bits.status
															,SelfCheckRes[tst_no].flags.bits.ignore
															,SelfCheckRes[tst_no].flags.bits.settle_quiet
															,SelfCheckRes[tst_no].flags.bits.user_quiet
															,SelfCheckRes[tst_no].flags.bits.single_alert
															,SelfCheckRes[tst_no].flags.bits.userSilenceCnt
															,SelfCheckRes[tst_no].flags.bits.DetectCnt
															,SelfCheckRes[tst_no].flags.bits.need_ack
															,lead_str
															);
	}
	printf("\r\n      %d %d %d %d"							,ignore_cnt
															,quiet_settle_cnt
															,quiet_user_cnt
															,quiet_single_alert_cnt
															);
	printf("\r\n  Alarm 0 (S0) %d",							alarm_S0_cnt);
	printf("\r\n  Alarm 1 (S1) %d",							alarm_S1_cnt);
	printf("\r\n  AutoClear    %d",							selfcheck_AutoClear);

	printf("\r\nUser   Quiet %d",							UserQuietAlarmTmr);
	printf("\r\nSettle Quiet %d",							SettleQuietAlarmTmr);
	printf("\r\nSettle FiO2  %d",							SettleQuietAlarmTmr);
	printf("\r\nSystem Alarm %d",							system_alarm_flag);
	printf("\r\n^");
	
	return;
}


/*************************************************************************************************
* Function Name : 	app_selfcheck_result_str
* Description   : 	This Function prints the result from the selfchecks
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		10/4/18		W. Paul			Created
*************************************************************************************************/
uint8_t app_selfcheck_result_str(STATUS_SEL_enum status_select,uint8_t tst_no, char** lead_str,char** res_str,uint8_t* status)
{
	if(tst_no < NoOfSelfTests-2){
		if(status_select == STATUS){	*status	=  SelfCheckRes[tst_no].flags.bits.status;		}
		else{							*status	=  SelfCheckRes[tst_no].flags.bits.status_live;	}
		*res_str		=  LanguageStr(lang_array_test[*status] );
		*lead_str		=  LanguageStr(lang_array_selftest[tst_no] );

		return(1);
	}
	*status			=  0;
	*res_str		=  LanguageStr(LangStr_Unset );
	*lead_str		=  LanguageStr(LangStr_Unset );
	return(0);
}


/*************************************************************************************************
* Function Name : 	app_selfcheck_mode
* Description   : 	This Function starts the selfcheck sequence from the start
* Arguments     : 	uint8_t mode	= 0	startup seftest mode (runs tests at top of table too)
*									= 1 normal runtime selftesting (runs the middle of the table)
*														(bottom of the table only runs when in therapy mode
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/01/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_mode(uint8_t mode)
{
	if((selftest_mode != 0)&&(mode == 0)){
		selfcheck_init		=  0;
		selftest_num		=  (SELFTEST_enum)0;
		selftest_loop_cnt	=  0;
	}

	selftest_mode		=  mode;
	return;
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_progress
* Description   : 	This Function shows progress throght the process
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/01/18	W. Paul			Created
*************************************************************************************************/
uint8_t app_selfcheck_progress(void)
{
	if(selftest_loop_cnt >= SELFTEST_STARTUP_DELAY){	return(1);	}	//10 = 5seconds
	else{												return(0);	}
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_next_fault
* Description   : 	This Function shows result score
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		2/05/18		W. Paul			Created
*************************************************************************************************/
uint8_t app_selfcheck_next_fault(void)
{
	static uint8_t		last_err_reported;
	uint8_t				err_no;
	uint8_t				i;

	for(i=0,err_no=last_err_reported;i<NoOfSelfTests-2;i++){
		if(++err_no >= (NoOfSelfTests-2)){	err_no	=  0;	}

		if(
		//	(SelfCheckRes[err_no].flags.bits.ignore				== 0)			&&
			(SelfCheckRes[err_no].flags.bits.settle_quiet		== 0)			&&
		//	(SelfCheckRes[err_no].flags.bits.user_quiet			== 0)			&&
		//	(SelfCheckRes[err_no].flags.bits.status				>  TEST_PASS)
			(
		//	 	(SelfCheckRes[err_no].flags.bits.status_live	>  TEST_PASS)	||
				(SelfCheckRes[err_no].flags.bits.status			>  TEST_PASS)	||		//needs to be this otherwise we see green alarms(where alarm has been set off but status has not been updated)
				(SelfCheckRes[err_no].flags.bits.need_ack		== 1)
			)
		)
		{
			i	= 0xfe;		//leave loop with err_no
		}
	}

	if(	i == NoOfSelfTests-2 ){
		last_err_reported	=  0;
		return(0xff);
	}
	last_err_reported	=  err_no;
	return(last_err_reported);
}


/*************************************************************************************************
* Function Name : 	app_selfcheck_cfp_status	(Critical Fail Pass) status
* Description   : 	This Function returns 	0 if no errors,
*											1 if any notice
*											2 if any warnings
*											3 if any faults and
*											4 if any critical faults
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		2/05/18		W. Paul			Created
*************************************************************************************************/
uint8_t app_selfcheck_cfp_status(void)
{
	if(critical_cnt){	return(4);	}
	if(fault_cnt){		return(3);	}
	if(warning_cnt){	return(2);	}
	if(notice_cnt){		return(1);	}
	if(pass_cnt){		return(0);	}

	return(0);
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_startup_status
* Description   : 	This Function returns 	0 if no errors,
*											1 if cant pass startup
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		2/05/18		W. Paul			Created
*************************************************************************************************/
uint8_t app_selfcheck_startup_status(void)
{
	uint8_t		err_no;
	
	if(SelfCheckRes[TEST_MEM_RDWR].flags.bits.status		>  TEST_PASS){		return(1);	}
	
	if(SelfCheckRes[TEST_RTC].flags.bits.status				>  TEST_WARNING){	return(1);	}
	
	for(err_no=TEST_CAL_FLOW_O2_a;err_no<=TEST_5V;err_no++){
		if(SelfCheckRes[err_no].flags.bits.status			>  TEST_PASS){		return(1);	}
	}
	
	if(SelfCheckRes[TEST_CAL_FLOW_O2_e].flags.bits.status	>  TEST_PASS){		return(1);	}
	
	if(SelfCheckRes[TEST_CAL_FLOW_AIR_e].flags.bits.status	>  TEST_PASS){		return(1);	}
	
	if(	(SelfCheckRes[TEST_SUPPLY_AIR].flags.bits.status	>  TEST_PASS)	&&
		(SelfCheckRes[TEST_SUPPLY_O2].flags.bits.status		>  TEST_PASS)	){	return(1);	}
	
	if(SelfCheckRes[TEST_O2_SENSOR].flags.bits.status		>  TEST_PASS){		return(1);	}
	
	if(	(SelfCheckRes[TEST_SENSOR_PP].flags.bits.status		>  TEST_PASS)	&&
		(SelfCheckRes[TEST_SENSOR_AIR].flags.bits.status	>  TEST_PASS)	&&
		(SelfCheckRes[TEST_SENSOR_O2].flags.bits.status		>  TEST_PASS)	){	return(1);	}
	
	if(SelfCheckRes[TEST_HELD_KEY].flags.bits.status		>  TEST_PASS){		return(1);	}
	
	if(SelfCheckRes[TEST_BATTERY_CHARGE].flags.bits.status	>  TEST_FAIL){		return(1);	}
	
	return(0);
}


/*************************************************************************************************
* Function Name : 	app_selfcheck_cfp_cnt	(Critical Fail Pass) count
* Description   : 	This Function returns 	0 if no errors,
*											1 if any notice
*											2 if any warnings
*											3 if any faults and
*											4 if any critical faults
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		2/05/18		W. Paul			Created
*************************************************************************************************/
uint8_t app_selfcheck_cfp_cnt(void)
{
	return(critical_cnt + fault_cnt + warning_cnt + notice_cnt);
}


/*************************************************************************************************
* Function Name :app_selfcheck_SettleQuietAlarmStart
* Description   : 	This Function starts a quiet alarm timer
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/06/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_SettleQuietAlarmStart(void)
{
	uint8_t		i;

	SettleQuietAlarmTmr							=  60000;	// 60seconds
	SettleQuietAlarmFiO2Tmr						=  180000;	// 180seconds
	printf("\r\nQuiet Alarms - Settle - Enabled");

	app_PP_settleAlarm_start();
	
	for(i=TEST_PP_MIN;i<=TEST_FIO2_LOW;i++){
		SelfCheckRes[i].flags.bits.settle_quiet			=  1;
	}
	SelfCheckRes[FAN_DEFECT].flags.bits.settle_quiet	=  1;

	return;
}

/*************************************************************************************************
* Function Name : app_selfcheck_start_therapy_in_Bat_mode
* Description   : 	This Function starts a quiet alarm timer
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		17/12/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_start_therapy_in_Bat_mode(void)
{

	return;
}

/*************************************************************************************************
* Function Name : app_selfcheck_UserQuietAlarmStart
* Description   : 	This Function starts a quiet alarm timer
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/06/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_UserQuietAlarmStart(void)
{
	uint8_t		i;
	uint8_t		pass_count	=  0;


	printf("\r\nQuiet Alarms - User - Enabled");
	for(i=TEST_ALWAYS_START;i<TEST_ALWAYS_END;i++){
		if(SelfCheckRes[i].flags.bits.status > TEST_PASS){			//any alarm that is not 'pass' is silenced for this duration
			if(++SelfCheckRes[i].flags.bits.userSilenceCnt <= 10){
				if(SelfCheckRes[i].flags.bits.ignore == 0){
					SelfCheckRes[i].flags.bits.user_quiet	=  1;
					if(UI.pneumatics_mode == 1){	UserQuietAlarmTmr		=  120000;	}	//2mins
					else{							UserQuietAlarmTmr		=  0;		}
				}
			}
			else{
				SelfCheckRes[i].flags.bits.userSilenceCnt = 10;
			}

		}
		else{
			pass_count++;	//count the not fails(not all tests may be set to pass)
		}
	}

	return;
}

/*************************************************************************************************
* Function Name : app_selfcheck_AckClearStatus_all
* Description   : 	This Function is an acknowledge of status of alarm being down graded
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/06/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_AckClearStatus_all(void)
{
	uint8_t			i;
	uint8_t			status;
	char* 			lead_str;
	char* 			res_str;


	printf("\r\nAckClearStatus");
	for(i=TEST_ALWAYS_START;i<TEST_ALWAYS_END;i++){
		if(	SelfCheckRes[i].flags.bits.status > SelfCheckRes[i].flags.bits.status_live){			//any alarm that is now lower than status
			app_selfcheck_result_str(STATUS,i, &lead_str, &res_str, &status);
			printf("\r\nAckClear - %s"		,res_str	);

			SelfCheckRes[i].flags.bits.status = SelfCheckRes[i].flags.bits.status_live;
			if(SelfCheckRes[i].flags.bits.status == TEST_PASS){
				SelfCheckRes[i].flags.bits.settle_quiet	=  0;
				SelfCheckRes[i].flags.bits.user_quiet	=  0;
				SelfCheckRes[i].flags.bits.need_ack		=  0;
			//	SelfCheckRes[i].flags.bits.single_alert	=  0;
			}

			app_selfcheck_result_str(STATUS,i, &lead_str, &res_str, &status);
			printf("->%s\t\t%s"		,res_str	,lead_str	);
		}
	}

	SelfCheck_loop_time	=  5;

	//no faults so silence alarm
//	if((pass_cnt + quiet_user_cnt + quiet_settle_cnt) >= (NoOfSelfTests-2) ){
//		system_alarm_flag	=  0;
//		api_LED(LED_ALM		,LED_OFF		,0x00);
//		api_audio_play_file_stop();
//		printf(" - [No Faults]");
//	}
	return;
}


/*************************************************************************************************
* Function Name : app_selfcheck_AckClearStatus_1
* Description   : 	This Function is an acknowledge of status of alarm being down graded
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/06/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_AckClearStatus_1(SELFTEST_enum test, uint8_t status)
{
	uint8_t			stat;
	char* 			lead_str;
	char* 			res_str;

	printf("\r\nAckClearStatus_1 set ");
	app_selfcheck_result_str(STATUS,test, &lead_str, &res_str, &stat);
	printf("\r\nAckClear - %s"		,res_str	);

	SelfCheckRes[test].flags.bits.status		=  status;
	SelfCheckRes[test].flags.bits.status_live	=  status;
	if(SelfCheckRes[test].flags.bits.status == TEST_PASS){
		SelfCheckRes[test].flags.bits.settle_quiet	=  0;
		SelfCheckRes[test].flags.bits.user_quiet	=  0;
		SelfCheckRes[test].flags.bits.need_ack		=  0;
	//	SelfCheckRes[test].flags.bits.single_alert	=  0;
	}

	SelfCheck_loop_time	=  5;

	app_selfcheck_result_str(STATUS,test, &lead_str, &res_str, &stat);
	printf("->%s\t\t%s"		,res_str	,lead_str	);


	return;
}
/*************************************************************************************************
* Function Name :	app_selfcheck_AutoClear
* Description   : 	This Function is used to set the flag selfcheck_AutoClear
*					when 1 and ack is needed to clear alarms (user must ack old alarm before it is cleared)
*					when 0 no ack is needed (when fault is fixed the alarm is cleared
* Arguments     : 	uint8_t value
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/08/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_AutoClear(uint8_t value)
{
	selfcheck_AutoClear	=  value;
	printf("\r\nselfcheck_AutoClear = %d",selfcheck_AutoClear);
	return;
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_AudioMute
* Description   : 	This Function shuts down the audio, so no audio alarm is atarted when an alarm is detected
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/08/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_AudioMute(uint8_t value)
{
	selfcheck_audio_mute	=  value;
	printf("\r\nselfcheck_audio_mute = %d",selfcheck_audio_mute);
	return;
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_SetIgnore
* Description   : 	This Function sets the ignore flag
* Arguments     : 	uint8_t test_no
*					uint8_t val
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/08/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_SetIgnore(uint8_t test_no, uint8_t val)
{
	printf("\r\nIgnoreFlag #%d to %d",test_no,val);
	SelfCheckRes[test_no].flags.bits.ignore = val;
	return;
}

/*************************************************************************************************
* Function Name : app_selfcheck_UserQuietAlarmRead
* Description   : 	This Function returns the number of seconds before the alarm is set off again
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/06/18	W. Paul			Created
*************************************************************************************************/
uint16_t app_selfcheck_UserQuietAlarmRead(void)
{
	return(UserQuietAlarmTmr/1000);
}


/*
*********************************************************************************************************
* Function Name : app_selfcheck_UserQuietAlarmSet
* Description   : This function is used for setting UserQuietAlarmTmr outside app_selfcheck.c
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			17/12/2021			Tim Barr		Original Created
*********************************************************************************************************
*/
void app_selfcheck_UserQuietAlarmSet(uint16_t val)
{
	// Local Variables
	
	// Code
	UserQuietAlarmTmr	=  val;
	
	return;
}


/*************************************************************************************************
* Function Name :app_selfcheck_1ms_irq
* Description   : 	This Function is called at 1ms tic
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/06/18	W. Paul			Created
*************************************************************************************************/
void app_selfcheck_1ms_irq(void)
{
	if(SelfCheck_loop_time		> 0){	SelfCheck_loop_time--;		}
	if(SettleQuietAlarmTmr		> 1){	SettleQuietAlarmTmr--;		}
	if(SettleQuietAlarmFiO2Tmr	> 1){	SettleQuietAlarmFiO2Tmr--;	}
	if(UserQuietAlarmTmr		> 1){	UserQuietAlarmTmr--;		}
	if(FIO2HighAlarmTmr			> 1){	FIO2HighAlarmTmr--;			}
	if(FIO2LowAlarmTmr			> 1){	FIO2LowAlarmTmr--;			}
	return;
}


/*************************************************************************************************
* Function Name : 	app_selfcheck_SysAlarm_Rd
* Description   : 	This Function reads the syatus of the system alarm flag
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/06/18	W. Paul			Created
*************************************************************************************************/
uint8_t app_selfcheck_SysAlarm_Rd(void)
{

	return(system_alarm_flag);
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_SysAlarm_Quiet_Tmr
* Description   : 	This Function reads the syatus of the system alarm flag
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		28/06/18	W. Paul			Created
*************************************************************************************************/
uint32_t app_selfcheck_SysAlarm_Quiet_Tmr(void)
{
	if(UserQuietAlarmTmr >1){			return( UserQuietAlarmTmr);		}

	return(0);
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_clearRuntimeAlarms
* Description   : 	This Function clears the alarms thast can be set during treatment.
*					Function Used when treatment is stopped
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		03/08/18	W. Paul			Created
* 0.2.0         11/06/26        A/Kane                  Updated so alarm_priority is not cleared
*************************************************************************************************/

void app_selfcheck_clearRuntimeAlarms(void)
{
    SELFTEST_enum   i;
    for(i = TEST_PP_MIN; i <= TEST_FIO2_LOW; i++){
        SelfCheckRes[i].flags.bits.status_live  = TEST_PASS;
        SelfCheckRes[i].flags.bits.status       = TEST_PASS;
        SelfCheckRes[i].flags.bits.ignore       = 0;
        SelfCheckRes[i].flags.bits.settle_quiet = 0;
        SelfCheckRes[i].flags.bits.user_quiet   = 0;
        SelfCheckRes[i].flags.bits.single_alert = 0;
        SelfCheckRes[i].flags.bits.need_ack     = 0;
        SelfCheckRes[i].flags.bits.userSilenceCnt = 0;
        SelfCheckRes[i].flags.bits.DetectCnt    = 0;

    }
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_test_menu
* Description   : 	This Function provides test menus for the selfcheck module
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/08/18	W. Paul			Created
*************************************************************************************************/
uint8_t app_selfcheck_test_menu(void)
{
	uint8_t					rec_status_u8;
	uint8_t					rec_char_u8;

	uint8_t 				i;
	uint8_t					fault_no;
	uint8_t					last_fault_no;
	char* 					lead_str;
	char* 					res_str;
	uint8_t 				status;



	rec_char_u8 = debug_getchar(10,&rec_status_u8);
	if(rec_status_u8){
		switch(rec_char_u8){
			case ' ':
				printf("\r\n -------------------------------");
				printf("\r\n app_selfcheck menu");
				printf("\r\nEsc Exit Menu");
				printf("\r\na   AckclearStatus");
				printf("\r\nu   UserQuietAlarmStart");
				printf("\r\ns   SettleQuietAlarmStart");
				printf("\r\np   print");
				printf("\r\n0-5 swgen state");
				printf("\r\nt   reduce settle timer to 2s");
				printf("\r\ny   reduce user timer to 2s");
				printf("\r\nf   fault list");

				break;
			case 0x1b:	//ESC
				return(0);	//leave this menu

			case 'a':	printf("\r\nAckClearStatus");				app_selfcheck_AckClearStatus_all();			break;
			case 'u':	printf("\r\nUserQuietAlarmStart");			app_selfcheck_UserQuietAlarmStart();		break;
			case 's':	printf("\r\nSettleQuietAlarmStart");		app_selfcheck_SettleQuietAlarmStart();		break;
			case 'p':												app_selfcheck_print_status();				break;

			case '0':	printf("\r\nswgen state = unset");			swgen_state	=  TEST_UNSET;  				break;
			case '1':	printf("\r\nswgen state = pass");			swgen_state	=  TEST_PASS; 	 				break;
			case '2':	printf("\r\nswgen state = notice");			swgen_state	=  TEST_NOTICE;  				break;
			case '3':	printf("\r\nswgen state = warning");		swgen_state	=  TEST_WARNING;  				break;
			case '4':	printf("\r\nswgen state = fail");			swgen_state	=  TEST_FAIL;  					break;
			case '5':	printf("\r\nswgen state = fail critical");	swgen_state	=  TEST_FAIL_CRITICAL;			break;

			case 't':
				if(SettleQuietAlarmTmr	> 2000){
					SettleQuietAlarmTmr	= 2000;
					printf("\r\n SettleQuiet Alarm reduced to 2s");
				}
				break;
			case 'y':
				if(UserQuietAlarmTmr 	> 2000){
					UserQuietAlarmTmr	= 2000;
					printf("\r\n UserQuiet Alarm reduced to 2s");
				}
				break;
			case 'f':
				printf("\r\nList Cur Faults");
				last_fault_no	=  app_selfcheck_next_fault();
				for(i=TEST_ALWAYS_START;i<TEST_ALWAYS_END;i++){
					fault_no	=  app_selfcheck_next_fault();
					if( fault_no != 0xff){
						app_selfcheck_result_str(STATUS,fault_no,&lead_str,&res_str,&status);
						printf("\r\n #%d %s %s %d",fault_no,lead_str,res_str,status);
						if(last_fault_no == fault_no){
							i	=  0xfe;
						}
					}
					else{
						i	=  0xfe;
					}
				}
				printf("\r\nEnd");
				break;

		}
	}
	return(1);
}

/*************************************************************************************************
* Function Name : 	app_selfcheck_test_menu
* Description   : 	This Function provides test menus for the selfcheck module
* Arguments     : 	void
* Returns       : 	system_alarm_priority
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/06/26	A. Kane			Created
*************************************************************************************************/

ALARM_PRIORITY_e app_selfcheck_SysAlarm_Priority(void)
{
    return system_alarm_priority;
}
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
