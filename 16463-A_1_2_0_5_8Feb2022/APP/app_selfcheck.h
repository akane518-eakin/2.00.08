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
 * Filename    :  app_selfcheck.h
 * Date Created:  Mon 23 Oct 2017 03:34:54 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _APP_SELFCHECK_H
#define _APP_SELFCHECK_H

#include "api_calibrate.h"
#include "api_audio.h"

#define	FAULT_DET_CNT				4		//number of 0.5seconds to detect a fault
#define	SELFTEST_STARTUP_DELAY		12		//number of checks before selfcheck at startup is ready

#define	TEST_UNSET			0x00
#define	TEST_PASS			0x01
#define	TEST_NOTICE			0x02
#define	TEST_WARNING		0x03
#define	TEST_FAIL			0x04
#define	TEST_FAIL_CRITICAL	0x05

#define	TEST_SETTLE_QUIET	0x26	//no alarms for a period after setings are changed to allow system to settle
#define	TEST_USER_QUIET		0x40	//alarm is set to quiet by the user

#define	TEST_RES_MASK		(uint16_t)(0x001F)
#define	TEST_FLAG_MASK		(uint16_t)(0x007F)
#define	TEST_QUIET_MASK		(uint16_t)(0x00E0)





typedef enum{
	STATUS		= 0,
	STATUS_LIVE,
}STATUS_SEL_enum;

typedef enum{
//Startup only					//		displayed on startup		startup test
//								//									app_selfcheck_startup_status();
	TEST_MEM_RDWR	=  0,		//0		yes							yes
	TEST_RTC,					//1		yes							yes
	TEST_CAL_FLOW_O2_a,			//2		yes							yes
	TEST_CAL_FLOW_O2_b,			//3		yes							yes
	TEST_CAL_FLOW_O2_c,			//4		yes							yes
	TEST_CAL_FLOW_O2_d,			//5		yes							yes
	TEST_CAL_FLOW_AIR_a,		//6		yes							yes
	TEST_CAL_FLOW_AIR_b,		//7		yes							yes
	TEST_CAL_FLOW_AIR_c,		//8		yes							yes
	TEST_CAL_FLOW_AIR_d,		//9		yes							yes
	TEST_CAL_SENSOR_O2,			//10	yes							yes
	TEST_CAL_SENSOR_PP,			//11	yes							yes

//Always check
	TEST_SWGEN,					//12	yes							not used
	TEST_BATTERY_FITTED,		//13	yes							yes
	TEST_5V,					//14	yes							yes
	TEST_24V,					//15	yes							not used
	TEST_SUPPLY_AIR,			//16	yes							either or
	TEST_SUPPLY_O2,				//17	yes							either or
	TEST_AC_SUPPLY,				//18	yes							No
	TEST_O2_SENSOR,				//19	yes							yes
	TEST_SENSOR_PP,				//20	yes							yes
	TEST_HELD_TOUCH,			//21								No
	TEST_HELD_KEY,				//22	yes							yes
	TEST_BATTERY_CHARGE,		//23	yes							yes
	TEST_O2_STARTUP_CAL,		//24

//during treatment
	TEST_PP_MIN,				//25
	TEST_PP_MAX,				//26
	TEST_APNOEA,				//27
	TEST_FMAX,					//28
	TEST_PLIMIT,				//29

	TEST_FIO2_HIGH,				//30
	TEST_FIO2_LOW,				//31
	TEST_32,					//32 not used
	TEST_33,					//33 not used
	
	FAN_DEFECT,					//34
	
	TEST_35,					//35 not used
	
	TEST_SENSOR_AIR,			//36	yes							yes
	TEST_SENSOR_O2,				//37	yes							yes
	
	TEST_CAL_FLOW_O2_e,			//38	yes							yes
	TEST_CAL_FLOW_AIR_e,		//39	yes							yes
	
	TEST_LAST,					//40
	TEST_WAIT,					//41
	

//bottom
	NoOfSelfTests,		//must be at the bottom
}SELFTEST_enum;



#define	TEST_STARTUP_START	TEST_MEM_RDWR
#define	TEST_ALWAYS_START	TEST_SWGEN
#define	TEST_ALWAYS_END		TEST_LAST
#define	TEST_STARTUP_END	TEST_BATTERY_CHARGE


typedef struct{
	union{
		uint32_t		all;
		struct{
			uint32_t		status_live	: 3;		//changes test to test
			uint32_t		status		: 3;		//can change up, but user must clear

			uint32_t		ignore		: 1;		//don't want to see this alarm again	eg running from battery power
			uint32_t		settle_quiet	: 1;		//alarm has been silenced
			uint32_t		user_quiet	: 1;		//alarm has been silenced
			uint32_t		single_alert	: 1;
			uint32_t		need_ack	: 1;

			uint32_t		userSilenceCnt	: 5;
			uint32_t		DetectCnt	: 4;
			uint32_t                alarm_priority  : 2;            // 0=low, 1=medium, 2=high
                        uint32_t                unused          : 10;           
		}bits;
	}flags;
}tst_status_t;

extern tst_status_t		SelfCheckRes[NoOfSelfTests];



/*********************************************************************************************************
 ********************************************************************************************************/
void		app_selfcheck_manager(void);

void		app_selfcheck_mode(uint8_t mode);
uint8_t		app_selfcheck_progress(void);
void		app_selfcheck_print_result(void);
void		app_selfcheck_print_status(void);

uint8_t		app_selfcheck_result_str(STATUS_SEL_enum status_select,uint8_t tst_no, char** lead_str,char** res_str,uint8_t* status);
uint8_t		app_selfcheck_next_fault(void);
uint8_t		app_selfcheck_cfp_status(void);
uint8_t		app_selfcheck_startup_status(void);
uint8_t		app_selfcheck_cfp_cnt(void);

void		app_selfcheck_SettleQuietAlarmStart(void);
void		app_selfcheck_UserQuietAlarmStart(void);
void		app_selfcheck_AckClearStatus_all(void);
void		app_selfcheck_AckClearStatus_1(SELFTEST_enum test, uint8_t status);
void		app_selfcheck_AutoClear(uint8_t value);
void		app_selfcheck_AudioMute(uint8_t value);
void		app_selfcheck_SetIgnore(uint8_t test_no, uint8_t val);

void		app_selfcheck_start_therapy_in_Bat_mode(void);

void		app_selfcheck_1ms_irq(void);

uint8_t		app_selfcheck_SysAlarm_Rd(void);
void		app_selfcheck_UserQuietAlarmSet(uint16_t val);
uint32_t	app_selfcheck_SysAlarm_Quiet_Tmr(void);

void		app_selfcheck_clearRuntimeAlarms(void);

uint8_t		app_selfcheck_test_menu(void);

ALARM_PRIORITY_e app_selfcheck_SysAlarm_Priority(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
