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
 * Filename    :  language.h
 * Date Created:  Thu 14 Dec 2017 04:37:14 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include <stdint.h>

/*********************************************************************************************************
 *		Language files included
 ********************************************************************************************************/
extern const char* EnglishPhrase[];
extern const char* FrenchPhrase[];
extern const char* GermanPhrase[];
extern const char* SpanishPhrase[];
extern const char* DutchPhrase[];
extern const char* ItalianPhrase[];
extern const char* ArabicPhrase[];
extern const char* FinnishPhrase[];
extern const char* NorwegianPhrase[];
extern const char* PortuguesePhrase[];
extern const char* GreekPhrase[];
extern const char* IndonesianPhrase[];
extern const char* LatvianPhrase[];
extern const char* PolishPhrase[];
extern const char* RomanianPhrase[];
extern const char* SwedishPhrase[];
extern const char* TurkishPhrase[];
extern const char* VietnamesePhrase[];



typedef enum{
	Lang_English	= 0,
	Lang_French		= 1,
	Lang_German		= 2,
	Lang_Spanish	= 3,
	Lang_Dutch		= 4,
	Lang_Italian	= 5,
        Lang_Arabic	= 6,
	Lang_Finnish		= 7,
	Lang_Norwegian		= 8,
	Lang_Portuguese	= 9,
	Lang_Greek		= 10,
	Lang_Indonesian	= 11,
        Lang_Latvian	= 12,
	Lang_Polish		= 13,
	Lang_Romanian	= 14,
        Lang_Swedish	= 15,
	Lang_Turkish		= 16,
	Lang_Vietnamese	= 17,
	Lang_Max			//do not remove
}LANG_SELECT_enum;


typedef enum{
	LangStr_English		=  0,
	LangStr_French,
	LangStr_German,
	LangStr_Spanish,
	LangStr_Dutch,
	LangStr_Italian,
        LangStr_Arabic,
	LangStr_Finnish,
	LangStr_Norwegian,
	LangStr_Portuguese,
	LangStr_Greek,
	LangStr_Indonesian,
        LangStr_Latvian,
	LangStr_Polish,
	LangStr_Romanian,
        LangStr_Swedish,
	LangStr_Turkish,
	LangStr_Vietnamese,


	LangStr_Jan,
	LangStr_Feb,
	LangStr_Mar,
	LangStr_Apr,
	LangStr_May,
	LangStr_June,
	LangStr_July,
	LangStr_Aug,
	LangStr_Sept,
	LangStr_Oct,
	LangStr_Nov,
	LangStr_Dec,

	LangStr_Sun,
	LangStr_Mon,
	LangStr_Tue,
	LangStr_Wed,
	LangStr_Thur,
	LangStr_Fri,
	LangStr_Sat,

	LangStr_Unset,
	LangStr_Pass,
	LangStr_Notice,
	LangStr_Warning,
	LangStr_Fault,
	LangStr_Critical_Fault,

	LangStr_Mem_Rd_Wr,
	LangStr_RTC,
	LangStr_Calibration_O2_Flow_a,
	LangStr_Calibration_O2_Flow_b,
	LangStr_Calibration_O2_Flow_c,
	LangStr_Calibration_O2_Flow_d,
	LangStr_Calibration_Air_Flow_a,
	LangStr_Calibration_Air_Flow_b,
	LangStr_Calibration_Air_Flow_c,
	LangStr_Calibration_Air_Flow_d,
	LangStr_Calibration_O2_Sensor,
	LangStr_Calibration_PP_Sensor,
	LangStr_SwGenErr,
	LangStr_Battery,
	LangStr_5V,
	LangStr_24V,
	LangStr_Supply_Air,
	LangStr_Supply_O2,
	LangStr_AC_Supply,
	LangStr_O2_SENSOR,
	LangStr_SENSOR_PP,
	LangStr_HELD_TOUCH,
	LangStr_HELD_BUTTON,
	LangStr_BATTERY_CHARGE,
	LangStr_O2_STARTUP_CAL,
	LangStr_P_MIN,
	LangStr_P_MAX,
	LangStr_APNOEA,
	LangStr_FMAX,
	LangStr_PLIMIT,
	LangStr_FIO2_HIGH,
	LangStr_FIO2_LOW,
	LangStr_TEST_32,
	LangStr_TEST_33,	
	LangStr_FAN_DEFECT,
	LangStr_TEST_35,
	LangStr_SENSOR_AIR,
	LangStr_SENSOR_O2,
	LangStr_Calibration_O2_Flow_e,
	LangStr_Calibration_Air_Flow_e,

	LangStr_Serial,
	LangStr_Ver,
	LangStr_Result,
  	LangStr_Charging,

	LangStr_Air,
	LangStr_Flow,
	LangStr_Oxygen,
	LangStr_O2Conc,
	LangStr_250Max,
	LangStr_CalmFlow,
	LangStr_dAir,
	LangStr_dO2,
	LangStr_dTot,
	LangStr_Patient,
	LangStr_Pressure,
	LangStr_FSetting,
	LangStr_pOxygen,
	LangStr_CPAPPres,
	LangStr_Pmin,
	LangStr_Pmax,
	LangStr_F_max,
	LangStr_Override,

	LangStr_CalEn,
	LangStr_CutOut,
	LangStr_Timer,
	LangStr_Fan,
	LangStr_Audio,
	LangStr_VolP,
	LangStr_VolN,
	LangStr_Scale,
	LangStr_CAll,
	LangStr_CVent,
	LangStr_AFlow,
	LangStr_OFlow,
	LangStr_CalSens,

	LangStr_CmH20,
	LangStr_RR,
	LangStr_Lmin,
	LangStr_Hr,
	LangStr_Min,
	LangStr_perMin,
	LangStr_Day,
	LangStr_Days,

	LangStr_CalO2,
	LangStr_Query,
	LangStr_CalTime,
	LangStr_TCal,
	LangStr_Point,
	LangStr_CComplete,
	LangStr_UOff,

	LangStr_Yes,
	LangStr_No,
	LangStr_Wait,
	LangStr_Remaining,
	LangStr_ConAct,
	LangStr_Menu,
	LangStr_Ok,
	LangStr_Bat,
	LangStr_Charged,
	LangStr_Vol,
	LangStr_calmOxygen,
	LangStr_Off,
	LangStr_AlarmSet,
	LangStr_Disabled,
	LangStr_Ack,
	LangStr_Alarm,
	LangStr_Demo,

	LangStr_Shutdown,
	LangStr_Unknown,
	LangStr_Silence,
	LangStr_Unlock,
	LangStr_Stop,
	LangStr_FlowOver,
	LangStr_FlowOverExitAdapt,  //override exit and dapt
	LangStr_FlowOverExit,
	LangStr_FlowOverAdjust,
	LangStr_NoChange,
	LangStr_FlowSetting,
	LangStr_AlarmChange,
	LangStr_NebuliserChange,
	LangStr_MainSupply,
	LangStr_BatteryOnly,
	LangStr_Cal02Now,
	LangStr_ConNebChange,
	LangStr_FlowInc,
	LangStr_CalFail,
	LangStr_NoAir,
	LangStr_NoO2,
	LangStr_FlowQuery,
	LangStr_TwoGas,
	LangStr_BatLevCrit,
	LangStr_TherHasStopped,
	LangStr_SwitchOffIn,
	LangStr_NoGAS_Supply,

	LangStr_StartCpap,
	LangStr_StartPaed,
	LangStr_StartHelmet,
	LangStr_StartBubble,
	LangStr_StartHFOT,
	LangStr_StartPoint,


	LangStr_Max			//do not remove
}LANG_STR_enum;



/*********************************************************************************************************
 *		Global Variable extern
 ********************************************************************************************************/
extern LANG_STR_enum		Lang_array_Lang[];
extern LANG_STR_enum		Lang_array_Month3[];
extern LANG_STR_enum		Lang_array_Day3[];
extern LANG_STR_enum		lang_array_test[];
extern LANG_STR_enum		lang_array_selftest[];
extern LANG_STR_enum		lang_array_slider[];
extern LANG_STR_enum		lang_array_buttons[];
extern LANG_STR_enum		lang_array_units[];
extern LANG_STR_enum		lang_array_cal[];
extern LANG_STR_enum		lang_array_common[];
extern LANG_STR_enum		lang_array_popups[];
extern LANG_STR_enum		lang_array_therapyPop[];

/*********************************************************************************************************
 *		Functions
 ********************************************************************************************************/

void 	    Language_set(	LANG_SELECT_enum 	Lang);
char*       LanguageStr(	LANG_STR_enum 		LangStr);

void 	    Lanugage_test(	void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
