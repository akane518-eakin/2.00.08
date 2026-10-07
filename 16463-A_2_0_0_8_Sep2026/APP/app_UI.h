/**********************************************************************************************************
 *  Marturion Ltd
 *
 *	Knockmore Hill Business Park
 *	9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2010, Marturion Ltd
 *  All Rights Reserved
 *
 *
 * Filename    :  app_UI.h
 * Programmer  :  William Paul
 * Description :
 *
 **********************************************************************************************************/
#ifndef _APP_UI_H
#define _APP_UI_H

#include <time.h>
#include "glb_typedefs.h"
#include "hal_lcd.h"
#include "app_pneumatic_ctrl.h"
#include "app_touchscreen.h"
#include "Language.h"

/*********************************************************************************************************
 *		Defines
 ********************************************************************************************************/
#define	UI_VOLUME_MAX				100
#define	UI_VOLUME_STEP				 10
#define	UI_VOLUME_MIN				 10

#define	UI_BRIGHTNESS_MAX			100
#define	UI_BRIGHTNESS_STEP			 10
#define	UI_BRIGHTNESS_MIN			 10

#define	RET_MAIN_SCR				20000		//20sec		if leave main screen we auto return after this duration
#define	POPUP_TIMEOUT				20000
#define	POPUP_SHUTDOWN_TIMEOUT		20000
#define POPUP_NOTIMEOUT				0xffffffff
#define	SCREEN_LOCK_TIME			(uint32_t)(30*1000)	//30seconds

#define uINIT_s16					-32768
//********************************************************************************************************
//BUTTON definitions


#define	BUTTON_TIME					BUTTON_0
#define	BUTTON_BATTERY				BUTTON_1
#define	BUTTON_ALARM				BUTTON_2
#define	BUTTON_SIDE1				BUTTON_3
#define	BUTTON_SIDE2				BUTTON_4
#define	BUTTON_SIDE3				BUTTON_5

#define	BUTTON_TIME_HOLD			BUTTON_0_HOLD
#define	BUTTON_BATTERY_HOLD			BUTTON_1_HOLD
#define	BUTTON_SIDE1_HOLD			BUTTON_3_HOLD
#define	BUTTON_SIDE2_HOLD			BUTTON_4_HOLD
#define	BUTTON_SIDE3_HOLD			BUTTON_5_HOLD

//********************************************************************************************************

												//RRRRRGGGGGGBBBBB
#define	FILTER_RGB	0xC618						//1100011000011000	//only keep top 2 sig bits of colour

#define	DOT_SIZE	3

//********************************************************************************************************
//sidebar build
#define	BUT_BLANK			0
#define	BUT_MODE			1
#define	BUT_NEXT			2
#define	BUT_BACK			3
#define	BUT_ALARMS			4
#define	BUT_START			5
#define	BUT_STOP			6
#define BUT_LOCKED			8
#define BUT_SETTINGS		9


#define	BUILD_ALL_NEW		1
#define	BUILD_NEW			2
#define	BUILD_REFRESH		3

typedef enum{
	POPUP_UNKNOWN,
	POPUP_SHUTDOWN,
	POPUP_SYSTEM_ALARM,
	POPUP_SCREEN_UNLOCK,
	POPUP_START_THERAPY,
	POPUP_STOP_THERAPY,
	POPUP_CONFIRM_OVERRIDE,
	POPUP_EXIT_OVERRIDE,
	POPUP_CONFIRM_CHANGE_FLOW,
	POPUP_CONFIRM_CHANGE_ALARMS,
	POPUP_CONFIRM_CHANGE_NEBULISER,
	POPUP_CONFIRM_AC_UNPLUGGED,
	POPUP_REDO_STARTUP_CAL,
	POPUP_CONFIRM_CHANGE_NEBULISER_FLOW,
	POPUP_STARTUP_CAL_FAILURE,
	POPUP_START_THERAPY_NO_AIR,
	POPUP_START_THERAPY_NO_O2,
	POPUP_2GAS_SUPPLY_AVAIL,
	POPUP_START_THERAPY_NO_GAS,

}POPUP_TYPE_enum;

typedef enum{
	POPUP_BACK,
	POPUP_FORWARD,
}POPUP_FB_enum;
//********************************************************************************************************
typedef enum{
	BUTTON_OFF,
	BUTTON_ADD,
	BUTTON_ON,
	BUTTON_REMOVE,
}Removable_button_enum;

typedef struct{
	Removable_button_enum 	mode;
	uint32_t				time;
}Removable_button_st;

typedef enum{
	UI_WELCOME = 	0x00,
	UI_WELCOME_WAIT,

	UI_TOUCH_CALIBRATE,
	UI_TOUCH_CALIBRATE_WAIT,

	UI_ENG_MODE,
	UI_ENG_MODE_WAIT,
	UI_ENG_CALIBRATE_SENSOR,
	UI_ENG_CALIBRATE_SENSOR_WAIT,

	UI_SELFCHECK,
	UI_SELFCHECK_WAIT,

	UI_PARACUBE_CAL,
	UI_PARACUBE_CAL_WAIT,

	UI_MODE_SETUP,
	UI_MODE_SETUP_WAIT,
	UI_FLOW_SETUP,
	UI_FLOW_SETUP_WAIT,
	UI_ALARM_SETUP,
	UI_ALARM_SETUP_WAIT,
	UI_MAIN,
	UI_MAIN_WAIT,

	UI_POPUP,
	UI_POPUP_WAIT,

	UI_SETTINGS,
	UI_SETTINGS_WAIT,

	UI_FLOW_OVERRIDE,
	UI_FLOW_OVERRIDE_WAIT,

	UI_AUTOOFF_120,
	UI_AUTOOFF_10,
	UI_AUTOOFF,
	UI_AUTOOFF_WAIT,

	//used in popup return
	UI_THIS_SCREEN	= 0xff,
}UI_STATUS;

typedef enum{
	DISPLAY_LINE_ONLY,
	DISPLAY_NUMBERS_BELOW,
	DISPLAY_NUMBERS_ABOVE,
}SLIDER_OPTION_e;


typedef enum{
	OVERRIDE_NONE		=  0,
	OVERRIDE_AVAIL		=  1,
	OVERRIDE_ACTIVE		=  2,
	OVERRIDE_ACTIVE1	=  3,
	OVERRIDE_UNKNOWN	=  0xff,
}OVERRIDE_enum;


/*********************************************************************************************************
 *		Structs / Unions
 ********************************************************************************************************/
typedef struct{
	int16_t			end_min;			//end of bar
	int16_t			end_max;			//end of bar
	int16_t			limit_min;			//limits before the end of bar
	int16_t			limit_max;			//limits before the end of bar

	float			pix_m;				//convert from number to pixcels

	int16_t			end_min_pix;		//pixcels	end of bar
	int16_t			end_max_pix;		//pixcels	end of bar
	int16_t			limit_min_pix;		//pixcels	limits before the end of bar
	int16_t			limit_max_pix;		//pixcels	limits before the end of bar

	int16_t			val;				//current value of variable
	EN_DIS_t		en_dis;				//used when slider has a disabled option at end of slider
	int16_t			val_last;			//current displayed value of variable

	uint16_t		val_last_pix;		//current displayed value of variable
}SLIDER_t;

#define	UI_SETTINGS_VALID				0x1C

typedef struct{
	uint8_t				valid;
	UI_STATUS			screen;
	UI_STATUS			screen_last;

	uint8_t				but_used;	//0 released, 1 press, 2 press and held
	uint8_t				touch_used;

	LANG_SELECT_enum	language;
	uint8_t				volume_amp;
	uint8_t				volume_chirp;
	uint8_t				volume_alarm;
	uint8_t				brightness;

	uint8_t 			demo_mode;				//demo mode enabled

	struct{
		uint8_t			endis;				//is sidebar displayed
		uint8_t			but1;				//what is in this button		usually mode
		uint8_t			but2;				//what is in this button		usually settings / lock
		uint8_t			but3;				//what is in this button		usually next or start or stop
		uint32_t		timer_n;			//refresh rate of variables on sidebar
		uint8_t			start_stop;			//which mode for but3
	}sidebar;

	struct{
		POPUP_TYPE_enum		type;
		UI_STATUS			back;
		UI_STATUS			next;
		uint32_t			timeout;
		uint32_t			no_return_timer;
	}popup;

	uint32_t			page_timer_n_0;			//counts down
	uint32_t			page_timer_n_1;			//counts down
	uint32_t			page_timer_main_return;	//counts down
	uint32_t			screen_lock_timer;		//count down
	uint32_t			no_return_timer;		//count down

	OVERRIDE_enum		flow_override;
	uint16_t			flowRate_override;		//override flow rate, so after plimit we can return to override value
	uint16_t			flowRate;				//normal flow rate, so after override we can return to original value

	SLIDER_t			FLOW;					//holds all info on slider and value while on setup screen
	SLIDER_t			O2;						//holds all info on slider and value while on setup screen
	SLIDER_t			Pmax;
	SLIDER_t			Pmin;
	SLIDER_t			Apnoea;
	SLIDER_t			Breathing_Freq;
	uint8_t				nebuliser;


	uint8_t				app_Pmode_available[PMODE_COUNT];
	app_Pmode_enum		select_mode;
	uint8_t				pneumatics_mode;
}UI_t;




/*********************************************************************************************************
 *		Public Variables
 ********************************************************************************************************/
extern UI_t		UI;

/*********************************************************************************************************
 *		Public functions
 ********************************************************************************************************/
void	app_UI(uint8_t *touch_status,uint8_t *but_status);

void	app_UI_parameters_rd(void);
void	app_UI_parameters_wr(void);

void 	app_UI_screen(UI_STATUS screen);

void 	app_UI_screen_lock_timer_reset(uint8_t unlock);

void 	app_UI_stop_therapy(uint8_t setFlag);

void	app_UI_irq(void);

uint8_t app_UI_mode_availability_set_menu(void);

#endif
/**********************************************************************************************************
**********************************************************************************************************/
//end of file

