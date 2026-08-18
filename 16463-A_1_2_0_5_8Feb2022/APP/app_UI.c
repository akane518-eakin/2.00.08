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
 * Filename    :  app_UI.c
 * Date Created:  Tue 04 Apr 2017 09:26:24 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "app_UI.h"
#include "app_touchscreen.h"
#include "app_system.h"
#include "app_patient_pressure.h"
#include "app_button.h"
#include "app_pneumatic_ctrl.h"
#include "app_selfcheck.h"
#include "app_battery_fuel_guage.h"

#include "api_STM32_touchscreen.h"
#include "api_RTC.h"
#include "api_LEDs.h"
#include "api_calibrate.h"
#include "api_stopwatch.h"

#include "api_IO.h"
#include "api_audio.h"
#include "api_SerialNo.h"

#include "hal_ltc2943.h"				//fuel gauge

#include "csp_S25FL0xx.h"				//mem
#include "csp_STM32_delay.h"
#include "csp_paracube_O2.h"

#include "hal_lcd.h"					//LCD
#include "hal_lcd_text.h"				//LCD
#include "csp_LCD_SSD1963.h"
#include "fonts.h"
#include "bitmaps.h"
#include "Colours.h"
#include "Language.h"

#include "csp_STM32_uart.h"
#include "hal_STM32_uart.h"


/**********************************************************************************************************
 *		Public VARIABLES
 **********************************************************************************************************/
UI_t		UI;

/**********************************************************************************************************
 *		LOCAL VARIABLES
 **********************************************************************************************************/
LCD_COLOR 	colour_mode[6]		=  {	 COLOUR_M_PURPLE		//MODE_CPAP
										,COLOUR_M_GREY			//MODE_CPAP_PAED
										,COLOUR_M_YELLOW		//MODE_CPAP_HELMET
										,COLOUR_M_GREEN			//MODE_BUBBLE_PAP
										,COLOUR_M_LIGHT_BLUE	//MODE_HFOT
										,COLOUR_M_DARK_BLUE		//MODE_POINT
									};


LCD_COLOR	colour_R_G[2]		=  {	COLOUR_RED		,COLOUR_GREEN		};
LCD_COLOR	colour_G_R[2]		=  {	COLOUR_GREEN	,COLOUR_RED			};
LCD_COLOR	colour_B_W[2]		=  {	COLOUR_BLACK	,COLOUR_WHITE		};
LCD_COLOR	colour_W_B[2]		=  {	COLOUR_WHITE	,COLOUR_BLACK		};
LCD_COLOR	colour_W_R[2]		=  {	COLOUR_WHITE	,COLOUR_RED 		};
LCD_COLOR	colour_W_G[2]		=  {	COLOUR_WHITE	,COLOUR_GREEN 		};
LCD_COLOR	colour_G_Y_Y_R_R[6]	=  {	COLOUR_ORANGE	,COLOUR_GREEN	,COLOUR_ORANGE	,COLOUR_ORANGE	,COLOUR_RED	,COLOUR_RED		};
//										Unset			,Pass			,Note			,Warning		,fault		,Critical

LCD_COLOR	breath_in_out_colour[2]	=  	{COLOUR_GRAY_80,COLOUR_GREY_3C};
LCD_COLOR	breath_alm_en_colour[3]	=	{COLOUR_RED,COLOUR_YELLOW,COLOUR_GREEN};
uint16_t	autooff_time_period	= 120;
uint16_t	gas_debounce_timer	=  0;

const	uint8_t	str_a[23]	=  {
	TEST_CAL_FLOW_O2_a,
	TEST_CAL_FLOW_O2_b,
	TEST_CAL_FLOW_O2_c,
	TEST_CAL_FLOW_O2_d,
	TEST_CAL_FLOW_O2_e,
	TEST_CAL_FLOW_AIR_a,
	TEST_CAL_FLOW_AIR_b,
	TEST_CAL_FLOW_AIR_c,
	TEST_CAL_FLOW_AIR_d,
	TEST_CAL_FLOW_AIR_e,
	TEST_CAL_SENSOR_O2,
	TEST_CAL_SENSOR_PP,
	
	TEST_AC_SUPPLY,
	TEST_BATTERY_FITTED,
	TEST_BATTERY_CHARGE,
	TEST_5V,
	TEST_SUPPLY_AIR,
	TEST_SUPPLY_O2,
	TEST_O2_SENSOR,
	TEST_SENSOR_PP,
	TEST_MEM_RDWR,
	TEST_RTC,
	TEST_HELD_KEY,
};

LCD_COLOR   colour_alarm_priority[3] = {
    COLOUR_CYAN,    // ALARM_PRIORITY_LOW
    COLOUR_YELLOW,  // ALARM_PRIORITY_MEDIUM
    COLOUR_RED,     // ALARM_PRIORITY_HIGH
};


/**********************************************************************************************************
 *		LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************/




void	app_UI_screen_welcome(			uint8_t touch_status, uint8_t but_status, bool init);
void    app_UI_screen_selfcheck(		uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_calibrate(		uint8_t touch_status, uint8_t but_status, bool init);
void	app_UI_screen_eng_mode(			uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_calibrate_sensors(uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_mode_setup(		uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_flow_setup(		uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_alarm_setup(		uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_main(				uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_settings(			uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_popup(			uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_paracube_cal(		uint8_t touch_status, uint8_t but_status, bool init);
void 	app_UI_screen_autooff(			uint8_t touch_status, uint8_t but_status, bool init);

void 	app_UI_screen_side_bar(			uint8_t build_type);

void 		app_line_slider_draw(		uint16_t x,uint16_t y,uint16_t w,uint16_t val_min,uint16_t val_max,uint16_t marker_gap,LCD_COLOR color,SLIDER_OPTION_e option,SLIDER_t *info);
void		app_line_slider_wOFF_draw(	uint16_t x,uint16_t y,uint16_t w,uint16_t val_min,uint16_t val_max,uint16_t marker_gap,LCD_COLOR color,SLIDER_OPTION_e option,SLIDER_t *info);
uint16_t 	app_line_slider_val2pix(SLIDER_t *info,float val);
float 		app_line_slider_pix2val(SLIDER_t *info,float pix);

void 	app_UI_button(uint8_t id,uint16_t x,uint16_t y,uint16_t w,uint16_t h,LCD_COLOR colour );

void 	app_UI_screen_lock_timer_reset(uint8_t unlock);
uint8_t app_UI_screen_lock_read(void);

void app_UI_screen_flow_override(uint8_t touch_status, uint8_t but_status, bool init);

void app_UI_screen_popup_setup(	POPUP_TYPE_enum type,UI_STATUS back,UI_STATUS next,uint32_t	timeout);
void app_UI_screen_popup_exit(	POPUP_FB_enum forw_back);

void app_UI_silence_alarm(void);
void app_UI_NoTherapy_Popup_Alarm(void);
/**********************************************************************************************************
 **********************************************************************************************************/



/*************************************************************************************************
 * Function Name :		app_UI
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0		23/12/11	W. Paul		Created
 *
 *************************************************************************************************/
void app_UI(uint8_t *touch_status,uint8_t *but_status)
{
	// Local Variables
	uint8_t				res;
	static uint8_t		init_UI_vars_done	=  0;
	static uint8_t		waitForKeyRelease	=  0;

	PinSet(TP6,1);

	// Code
	if(init_UI_vars_done == 0){
		init_UI_vars_done	=  1;
		//get UI vars from flash
		app_UI_parameters_rd();
		//initialise vars

		UI.screen	=  UI_WELCOME;

		//get touch calibration
		res	=  api_touch_config_rd();
		if(res == 1){
			UI.screen	=  UI_TOUCH_CALIBRATE;
		}
	}

	if((*touch_status == 0xff)&&(*but_status == 0)){
		//no touch detected
	}
	else{
		app_UI_screen_lock_timer_reset(0);	//a touch will not unlock if timer has expired
		if( app_UI_screen_lock_read() ){	//if already locked
		  	if((UI.screen ==  UI_POPUP)||( UI.screen ==  UI_POPUP_WAIT)){
				//allow key press throught to this screen only
		  	}
		  	else if((*touch_status == BUTTON_UNLOCK)||((*but_status & BUT_PWR)==BUT_PWR) ){
		  		//allow only the following buttons through
		  		//UNLOCK icon
		  		//POWER BUTTON on membrane
		  	}
//			else if(app_selfcheck_SysAlarm_Rd()){
//				//allow button press through if alarm is sounding , but not touch screen
//				*touch_status	=  0xff;
//			}
		  	else{
				*touch_status	=  0xff;
				*but_status		=  0;
			//this is done in app_UI_screen_side_bar()
				app_touchscreen_button_disable_all();
				app_touchscreen_button_add(BUTTON_UNLOCK,655, 271, 140, 100);		//lock key location is enabled (should be enabled elsewhere)
		  	}
		   	if(	( UI.screen !=  UI_POPUP) 		 &&
			   	( UI.screen !=  UI_POPUP_WAIT)	 &&
			   	( *touch_status == BUTTON_UNLOCK) &&
				( UI.no_return_timer ==  0)				)
			{		//if timer has expired ignore all key inputs
				app_UI_screen_popup_setup(POPUP_SCREEN_UNLOCK,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_TIMEOUT);
			}
		}
	}

	//if change screen touch screen must be released
	if(UI.screen%2 == 0){  		waitForKeyRelease	=  1;	}	//this is an init==1 screen
	if(*touch_status == 0xff){ 	waitForKeyRelease	=  0;	}
	if(waitForKeyRelease){
		app_touchscreen_no_button_press();
		*touch_status =  0xfe;
	}

	if(UI.screen == UI_WELCOME){
		if(PinRead(LTC4009_ACP) == 1){		//battery mode
			hal_ltc2943_charge_read();
			if((FuelGauge.act_mAh < 50)||(FuelGauge.percentage < 1.0)){	//battery too low
				printf("\r\nBattery is too low (%fmAh %3.1f%% remaining)",FuelGauge.act_mAh,FuelGauge.percentage);
				UI.screen	=  UI_AUTOOFF_10;
			}
		}
	}
	if( (UI.pneumatics_mode == 0)&&(UI.screen>=UI_MODE_SETUP)){	//if not in therapy mode and not in eng/calibration screens
		app_UI_NoTherapy_Popup_Alarm();		//popup alarms
	}

	switch(UI.screen){
		case UI_WELCOME:						app_UI_screen_welcome(			*touch_status, *but_status, true);		break;
		case UI_WELCOME_WAIT:					app_UI_screen_welcome(			*touch_status, *but_status, false);		break;

		case UI_SELFCHECK:						app_UI_screen_selfcheck(		*touch_status, *but_status, true);		break;
		case UI_SELFCHECK_WAIT:					app_UI_screen_selfcheck(		*touch_status, *but_status, false);		break;

		case UI_TOUCH_CALIBRATE:				app_UI_screen_calibrate(		*touch_status, *but_status, true);		break;
		case UI_TOUCH_CALIBRATE_WAIT:			app_UI_screen_calibrate(		*touch_status, *but_status, false);		break;

		case UI_ENG_MODE:						app_UI_screen_eng_mode(			*touch_status, *but_status, true);		break;
		case UI_ENG_MODE_WAIT:					app_UI_screen_eng_mode(			*touch_status, *but_status, false);		break;
		case UI_ENG_CALIBRATE_SENSOR:			app_UI_screen_calibrate_sensors(*touch_status, *but_status, true);		break;
		case UI_ENG_CALIBRATE_SENSOR_WAIT:		app_UI_screen_calibrate_sensors(*touch_status, *but_status, false);		break;

		case UI_PARACUBE_CAL:	 				app_UI_screen_paracube_cal(		*touch_status, *but_status, true);		break;
		case UI_PARACUBE_CAL_WAIT: 				app_UI_screen_paracube_cal(		*touch_status, *but_status, false);		break;

		case UI_MODE_SETUP:						app_UI_screen_mode_setup(		*touch_status, *but_status, true);		break;
		case UI_MODE_SETUP_WAIT:				app_UI_screen_mode_setup(		*touch_status, *but_status, false);		break;
		case UI_FLOW_SETUP:						app_UI_screen_flow_setup(		*touch_status, *but_status, true);		break;
		case UI_FLOW_SETUP_WAIT:				app_UI_screen_flow_setup(		*touch_status, *but_status, false);		break;
		case UI_ALARM_SETUP:					app_UI_screen_alarm_setup(		*touch_status, *but_status, true);		break;
		case UI_ALARM_SETUP_WAIT:				app_UI_screen_alarm_setup(		*touch_status, *but_status, false);		break;
		case UI_MAIN:							app_UI_screen_main(				*touch_status, *but_status, true);		break;
		case UI_MAIN_WAIT:						app_UI_screen_main(				*touch_status, *but_status, false);		break;
		case UI_FLOW_OVERRIDE:					app_UI_screen_flow_override(	*touch_status, *but_status, true);		break;
		case UI_FLOW_OVERRIDE_WAIT:				app_UI_screen_flow_override(	*touch_status, *but_status, false);		break;

		case UI_POPUP:							app_UI_screen_popup(			*touch_status, *but_status, true);		break;
		case UI_POPUP_WAIT:						app_UI_screen_popup(			*touch_status, *but_status, false);		break;

		case UI_SETTINGS:		 				app_UI_screen_settings(			*touch_status, *but_status, true);		break;
		case UI_SETTINGS_WAIT:	 				app_UI_screen_settings(			*touch_status, *but_status, false);		break;

		case UI_AUTOOFF_120:		 			autooff_time_period	= 120+1;
												app_UI_screen_autooff(			*touch_status, *but_status, true);		break;

		case UI_AUTOOFF_10:						autooff_time_period	= 10+1;
												app_UI_screen_autooff(			*touch_status, *but_status, true);		break;
		case UI_AUTOOFF:	 					app_UI_screen_autooff(			*touch_status, *but_status, true);		break;
		case UI_AUTOOFF_WAIT:	 				app_UI_screen_autooff(			*touch_status, *but_status, false);		break;



		default:			printf("\r\nErr UI.screen %d",UI.screen);		break;
	}

	PinSet(TP6,0);

	return;
}


/**********************************************************************************************************
 * Function Name : app_UI_parameters_rd
 * Description   : This function is used to read the setup parameters from mem
 * Arguments     : None
 * Returns       : void
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 7/ 6/06	William Paul	Original Created
 * 1.0.0		 6/07/12	William Paul	Updated
 **********************************************************************************************************/
void app_UI_parameters_rd(void)
{
	app_Pmode_enum		i;

	//read settings from memory
	csp_mem_rd((uint8_t*)&UI, MEM_ADD_UI_VARS, sizeof(UI_t));

	//does data need initialised
	if(UI.valid != UI_SETTINGS_VALID){
		UI.valid						=  UI_SETTINGS_VALID;

		UI.language						=  Lang_English;

		UI.volume_amp					=  44;
		UI.volume_chirp					=  50;
		UI.volume_alarm					=  50;

		UI.brightness					=  50;

		UI.app_Pmode_available[PMODE_CPAP]				=  1,
		UI.app_Pmode_available[PMODE_CPAP_PAED]			=  1;
		UI.app_Pmode_available[PMODE_CPAP_HELMET]		=  1;
		UI.app_Pmode_available[PMODE_BUBBLE_PAP]		=  1;
		UI.app_Pmode_available[PMODE_HFOT]				=  1;
		UI.app_Pmode_available[PMODE_POINT]				=  1;

		app_UI_parameters_wr();
	}

	//On start up we always want to ...
	UI.screen						=  UI_WELCOME;
	UI.screen_last					=  UI_WELCOME;

	UI.but_used						=  1;	//0 released, 1 press, 2 press and held
	UI.touch_used					=  1;

	UI.demo_mode					=  0;

	UI.sidebar.endis				=  0;
	UI.sidebar.but1					=  BUT_BLANK;
	UI.sidebar.but2					=  BUT_BLANK;
	UI.sidebar.but3					=  BUT_BLANK;
	UI.sidebar.timer_n				=  10;

	UI.popup.type					=  POPUP_UNKNOWN;
	UI.popup.back					=  UI_WELCOME;
	UI.popup.next					=  UI_WELCOME;
	UI.popup.timeout				=  0;	//0= timer diabled
	UI.popup.no_return_timer		=  0;

	UI.page_timer_n_0				=  10;	//counts down
	UI.page_timer_main_return		=  0;	//0= timer diabled
	UI.screen_lock_timer			=  SCREEN_LOCK_TIME;
	UI.no_return_timer				=  0;

	UI.select_mode					=  PMODE_CPAP;
	for(i=PMODE_CPAP;i<=PMODE_POINT;i++){
		if(UI.app_Pmode_available[i]){
			UI.select_mode		=  i;				//first mode that is available
			i					=  PMODE_POINT;		//quit loop
		}
	}

	UI.flow_override				=  OVERRIDE_NONE;
	UI.flowRate						=  0;
	UI.flowRate_override			=  0;
	UI.pneumatics_mode				=  0;
	UI.nebuliser					=  0;

	UI.volume_chirp					=  50;
	UI.volume_alarm					=  50;

	Language_set(UI.language);
	LCD_brightness(UI.brightness);

	api_audio_volume_per_wr(AUDIO_AMP,UI.volume_amp);
	api_audio_volume_per_wr(AUDIO_DIG1,UI.volume_chirp);
	api_audio_volume_per_wr(AUDIO_DIG2,UI.volume_alarm);

	return;
}


/**********************************************************************************************************
 * Function Name : app_UI_parameters_wr
 * Description   : This function is used to save the setup parameters
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 9/ 6/06	William Paul	Original Created
 * 1.0.0		10/01/12	William Paul	Updated
 **********************************************************************************************************/
void app_UI_parameters_wr(void)
{
	csp_sys_mem_wr((uint8_t*)&UI, MEM_ADD_UI_VARS, sizeof(UI_t));

	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_screen
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		02/01/18	W. Paul			Created
*************************************************************************************************/
void app_UI_screen(UI_STATUS screen)
{
	UI.screen	=  screen;

	return;
}



/**********************************************************************************************************
 * Function Name : app_UI_screen_welcome
 * Description   : This function is used to handle the welcome screen
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		28/11/15	Tim Barr		Original Created
 **********************************************************************************************************/
void app_UI_screen_welcome(uint8_t touch_status, uint8_t but_status, bool init)
{
	// Local Variables
	char				str[30];
	static	uint8_t		selfcheck_done;
	static	uint8_t		progress		=  0;
	uint8_t				buttons_held	=  0;

	// Code
	if(init){
		UI.screen			=  UI_WELCOME_WAIT;
		UI.screen_last		=  UI_WELCOME;
		app_selfcheck_AudioMute(1);				//audio mute

		selfcheck_done	= 0;

		app_UI_stop_therapy(1);
		app_selfcheck_clearRuntimeAlarms();

		app_touchscreen_button_disable_all();
		app_touchscreen_button_add(	BUTTON_0,	0,	0,	LCD_DISP_WIDTH-1, LCD_DISP_HEIGHT-1);					//id,x,y,w,h
	//background
		LCD_FillRect(	0	,0						,LCD_DISP_WIDTH-1	,LCD_DISP_HEIGHT-100	,COLOUR_WELCOME_BACKGROUND	);
		LCD_FillRect(	0	,LCD_DISP_HEIGHT-100	,LCD_DISP_WIDTH		,100					,COLOUR_BLACK				);

	//Sidebar
		UI.sidebar.endis	=  0;

		LCD_DispMonoBitmapTrans(	 20, 50, "IMG_Armstrong_Logo"	,COLOUR_ARMSTRONG_LOGO);
		LCD_DispMonoBitmapTrans(	180, 50, "IMG_Armstrong"		,COLOUR_ARMSTRONG_LOGO);
		LCD_DispMonoBitmapTrans(	180,100, "IMG_Medical"			,COLOUR_ARMSTRONG_LOGO);
		LCD_DispMonoBitmapTrans(	500,150, "IMG_FD140i"			,COLOUR_ARMSTRONG_LOGO);

	//	LCD_DispMonoBitmapTrans(	300,300, "IMG_Progress_01"		,COLOUR_ARMSTRONG_LOGO);


	//version
		LCD_DispText_option(20,400,100,35,CENTER_RIGHT,COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Serial));	//x,y,w,h,justification,col,font,txt
		if(api_SerialNo_read((uint8_t*)str) == 0){
			sprintf(str,"- - -");
		}
		LCD_DispText_option(150,400,200,35,CENTER_LEFT,COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt

	//serial no
		LCD_DispText_option(20,440,100,35,CENTER_RIGHT,COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Ver));	//x,y,w,h,justification,col,font,txt
		//sprintf(str,"%d.%d.%d.%d"		,firmware_version_st_glb.a
		//								,firmware_version_st_glb.b
		//								,firmware_version_st_glb.c
		//								,firmware_version_st_glb.d		);
		sprintf(str,"%d.%02d"			,firmware_version_st_glb.a
										,firmware_version_st_glb.b		);
		LCD_DispText_option(150,440,200,35,CENTER_LEFT,COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(400,400,275,65,CENTER_RIGHT,COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h,LanguageStr(LangStr_Result));	//x,y,w,h,justification,col,font,txt

		UI.page_timer_n_0	=  20000;	//ms
		api_audio_chirp(1);

	}
	else{
		//selfcheck
		switch(selfcheck_done){
			case 0:
				app_selfcheck_mode(0);		//start full startup self test sequence
				selfcheck_done	=  1;
				UI.page_timer_n_1	=  200;
				progress			=  0;
				break;
			case 1:
				if(app_selfcheck_progress() ){
					selfcheck_done	=  2;
				}
				if(UI.page_timer_n_1 == 1){
					UI.page_timer_n_1	=  200;
					switch(progress){
						case 0:		LCD_DrawCircle(710, 420, 3, COLOUR_WHITE);	break;
						case 1:		LCD_DrawCircle(720, 420, 3, COLOUR_WHITE);	break;
						case 2:		LCD_DrawCircle(730, 420, 3, COLOUR_WHITE);	break;
						case 3:		LCD_DrawCircle(740, 420, 3, COLOUR_WHITE);	break;
						case 4:		LCD_DrawCircle(710, 420, 3, COLOUR_BLACK);	break;
						case 5:		LCD_DrawCircle(720, 420, 3, COLOUR_BLACK);	break;
				        case 6:		LCD_DrawCircle(730, 420, 3, COLOUR_BLACK);	break;
				    	case 7:		LCD_DrawCircle(740, 420, 3, COLOUR_BLACK);	break;
				    }
					if(++progress > 7){	progress = 0;	}
				}
				break;
			case 2:
				//if reach here, selfcheck is finished
				printf("\r\nSelfcheck UI done");
				selfcheck_done	=  3;

				api_audio_chirp(1);

				if(app_selfcheck_startup_status() == 0){
					LCD_DispMonoBitmap(		700,400,	"IMG_Tic_65_65"  	,COLOUR_GREEN	,COLOUR_BLACK);
				}
				else{
					LCD_DispMonoBitmap(		700,400,	"IMG_Cross_65_65"  	,COLOUR_RED		,COLOUR_BLACK);
				}
				UI.page_timer_n_0	=  2000;
//				if(app_selfcheck_startup_status() == 0){
//					app_selfcheck_UserQuietAlarmStart();
//				}
				break;
			case 3:
                break;
		}
		if(UI.page_timer_n_0 == 0){
			if(app_selfcheck_startup_status() == 0){
				app_selfcheck_UserQuietAlarmStart();
				
				app_selfcheck_mode(1);		//end full startup self test sequence - back to normal test mode
				//UI.screen		= UI_MODE_SETUP;
				UI.screen		= UI_PARACUBE_CAL;
			}
			else{
				UI.screen		= UI_SELFCHECK;
			}
		}

	}

	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
		case BUT_ALL_HOLD:
			buttons_held	=  1;
			break;
	}
	switch(touch_status){
		case BUTTON_0:
		case BUTTON_0_HOLD:
			app_selfcheck_SettleQuietAlarmStart();
			if(buttons_held){
				app_selfcheck_mode(1);
				//UI.screen		= UI_MODE_SETUP;
				UI.screen		= UI_PARACUBE_CAL;
				UI.demo_mode	=  1;
			}
			break;
		case BUTTON_RELEASED:
			break;
	}

	return;
}

/**********************************************************************************************************
 * Function Name : app_UI_screen_selfcheck
 * Description   : This function is used to handle the welcome screen
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		10/4/18		William Paul		Original Created
 **********************************************************************************************************/
void app_UI_screen_selfcheck(uint8_t touch_status, uint8_t but_status, bool init)
{
	// Local Variables
	uint16_t			y;
	uint16_t			x;

	uint8_t				valid;
	uint8_t				str_no_i;
	uint8_t				str_no;
	char*				lead_str;
	char*				res_str;
	uint8_t				res;
	static	uint8_t		last_res[NoOfSelfTests];
	static	uint8_t		last_icon		= 0xff;
	uint8_t				icon;
	uint8_t				buttons_held	=  0;
	
	// Code
	if(init){
		UI.screen			=  UI_SELFCHECK_WAIT;
		UI.screen_last		=  UI_SELFCHECK;
		app_selfcheck_AudioMute(1);		//audio mute
		app_selfcheck_AutoClear(1);
		app_selfcheck_mode(0);			//start full startup self test sequence
		last_icon		= 0xff;

		app_touchscreen_button_disable_all();
		app_touchscreen_button_add(	BUTTON_0,	0,	0,	LCD_DISP_WIDTH-1, LCD_DISP_HEIGHT-1);					//id,x,y,w,h
	//background
		LCD_FillRect(	0,	0,	LCD_DISP_WIDTH, LCD_DISP_HEIGHT,	COLOUR_BLACK);
	//Sidebar
		UI.sidebar.endis	=  0;
		LCD_DispText_option( 30, 20,400,35	,CENTER_LEFT	,COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Result));	//check_height
	//	sprintf(str,"Self Check Result");
		LCD_DispText_option(200,400,475,65	,CENTER_RIGHT	,COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Result));

		memset(last_res,0xff,sizeof(last_res));
	}

	x	=  0;
	y	=  60;
	for(str_no_i=0;str_no_i<sizeof(str_a);str_no_i++){
		str_no	=  str_a[str_no_i];
		valid	=  app_selfcheck_result_str(STATUS_LIVE,str_no,&lead_str,&res_str,&res);

		//if((valid) && (res > 0)){
		if(valid){
			if(last_res[str_no]	!= res){
				if(last_res[str_no] != 0xff){
					LCD_FillRect(	x		,y,140,25,	COLOUR_BLACK);
				}
				last_res[str_no]	=  res;
				LCD_DispText_option(x		,y,140,25,CENTER		,colour_G_Y_Y_R_R[res]	,TRANSPARENT, &fontArial22h, res_str);
				LCD_DispText_option(x+140	,y,260,25,CENTER_LEFT	,COLOUR_WHITE			,TRANSPARENT, &fontArial22h, lead_str);
			}
			if(str_no == 11){
				x	=  400;
				y	=  60;
			}
			else{
				y	+= 26;
			}
		}
	}

	icon	=  app_selfcheck_startup_status();
	if(last_icon != icon){
		last_icon	=  icon;

		if(app_selfcheck_startup_status() == 0){	//no errors
			LCD_DispMonoBitmap(		700,400,	"IMG_Tic_65_65"  	,COLOUR_GREEN	,COLOUR_BLACK);
			UI.page_timer_n_0	=  2000;
		}
		else{
			LCD_DispMonoBitmap(		700,400,	"IMG_Cross_65_65"  	,COLOUR_RED		,COLOUR_BLACK);
		}
	}

	if((UI.page_timer_n_0 == 0)&&(last_icon==0)){
		app_selfcheck_UserQuietAlarmStart();
		app_selfcheck_AutoClear(0);
		app_selfcheck_mode(1);
		
		//UI.screen		= UI_MODE_SETUP;
		UI.screen		= UI_PARACUBE_CAL;
	}
	
	//need a shortcut to move on in demo mode
	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
		case BUT_ALL_HOLD:
			buttons_held	=  1;
			break;
	}
	switch(touch_status){
		case BUTTON_0:
		case BUTTON_0_HOLD:
			app_selfcheck_SettleQuietAlarmStart();
			if(buttons_held){
				app_selfcheck_mode(1);
				//UI.screen		= UI_MODE_SETUP;
				UI.screen		= UI_PARACUBE_CAL;
				UI.demo_mode	=  1;
			}
			break;
		case BUTTON_RELEASED:
			break;
	}

	return;
}



/**********************************************************************************************************
 * Function Name : app_UI_screen_welcome
 * Description   : This function is used to handle the welcome screen
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		28/11/15	Tim Barr		Original Created
 **********************************************************************************************************/
void app_UI_screen_eng_mode(uint8_t touch_status, uint8_t but_status, bool init)
{
	// Local Variables
	static uint32_t			sys_tic_timer_1s;
	static uint8_t			timer_5s;

	slider_t			slider_info;
	char				str[30];

	static uint16_t		last_O2					= 0xffff;
	static uint16_t		last_AIR				= 0xffff;
	static uint16_t		last_Venturi_Flow		= 0xffff;
	static uint16_t		last_target_O2			= 0xffff;
	static uint16_t		last_target_Flow		= 0xffff;
	static float		last_act_Flow			= 0xffff;
	static float		last_act_O2				= 0xffff;

	static uint16_t		last_O2_dis				= 0xffff;
	static uint16_t		last_AIR_dis			= 0xffff;
	static uint16_t		last_Venturi_Flow_dis	= 0xffff;
	static uint16_t		last_target_O2_dis		= 0xffff;
	static uint16_t		last_target_Flow_dis	= 0xffff;
	static float		last_act_Flow_dis		= 0xffff;
	static float		last_act_O2_dis			= 0xffff;

	static uint8_t		last_Venturi_EN;
	static uint8_t		last_Cal_EN;
	static uint8_t		last_Neb_EN;
	static uint8_t		last_CutOut;

	static uint8_t		fan_status				= 2;
	static uint8_t		volume					= 0xff;
	static float		O2flow;
	static float		totalflow;
	static float		Airflow;
	static float		Max250_O2conc;
	static float		Paracube_O2conc;
	static float		thermistor;
	static float		test5V;
	static uint32_t		ADC_reset;
	static int16_t		last_apnoea;

	static uint8_t		last_CTRL_MODE			= 0xff;

	uint8_t				air_supply;
	uint8_t				O2_supply;
	time_t 				rawtime;
	struct tm 			*timeinfo;
	uint8_t				hr,min,sec,swatch_running;
	uint16_t			day;
	uint16_t			x;
	uint16_t			last_pix,new_pix;



	// Code
	if(init){
		UI.screen				=  UI_ENG_MODE_WAIT;
		UI.screen_last			=  UI_ENG_MODE;
		app_selfcheck_AudioMute(1);

		UI.touch_used			=  1;
		UI.but_used				=  1;
		last_Venturi_EN			=  0xff;
		last_Cal_EN				=  0xff;
		last_Neb_EN				=  0xff;
		last_CutOut				=  0xff;

		last_O2					= 0xffff;
		last_AIR				= 0xffff;
		last_Venturi_Flow		= 0xffff;
		last_target_O2			= 0xffff;
		last_target_Flow		= 0xffff;
		last_act_Flow			= 0xffff;
		last_act_O2				= 0xffff;

		last_O2_dis				= 0xffff;
		last_AIR_dis			= 0xffff;
		last_Venturi_Flow_dis	= 0xffff;
		last_target_O2_dis		= 0xffff;
		last_target_Flow_dis	= 0xffff;
		last_act_Flow_dis		= 0xffff;
		last_act_O2_dis			= 0xffff;
		ADC_reset				= 0xffffffff;
		last_apnoea				= -1.0;
		volume					= 0xff;

		last_CTRL_MODE			= 0xff;

		sys_tic_timer_1s		=  sys_tic_rd();



		app_touchscreen_button_disable_all();

		//background
		LCD_FillRect(0,	0,	LCD_DISP_WIDTH, LCD_DISP_HEIGHT, COLOUR_GRAY_8A);

		//Sidebar
		UI.sidebar.endis		=  0;

	//sliders
		//O2
		LCD_DrawSoftRect_1(						10,156,60,304,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_0,	10,158,60,300);						//id,x,y,w,h
		LCD_DispText_option(					5,135,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "O ");	//x,y,w,h,justification,col,font,txt

		//AIR
		LCD_DrawSoftRect_1(						80,156,60,304,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_1,	80,158,60,300);						//id,x,y,w,h
		LCD_DispText_option(					75,135,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_Air));	//x,y,w,h,justification,col,font,txt

		//VenturiFlow
		LCD_DrawSoftRect_1(						150,156,60,304,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_2,	150,158,60,300);					//id,x,y,w,h
		LCD_DispText_option(					145,135,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "Venturi");	//x,y,w,h,justification,col,font,txt

	//1st column buttons
		//Venturi EN
		app_touchscreen_button_add(	BUTTON_3,	220,400,60,60);						//id,x,y,w,h
		LCD_DispText_option(					215,380,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "Vent EN");	//x,y,w,h,justification,col,font,txt

		//Cal EN
		app_touchscreen_button_add(	BUTTON_4,	220,320,60,60);						//id,x,y,w,h
		LCD_DispText_option(					215,300,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_CalEn));	//x,y,w,h,justification,col,font,txt

		//Neb EN
		app_touchscreen_button_add(	BUTTON_5,	220,240,60,60);						//id,x,y,w,h
		LCD_DispText_option(					215,220,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "Neb EN");	//x,y,w,h,justification,col,font,txt

		//Safty cutout
		app_touchscreen_button_add(	BUTTON_6,	220,160,60,60);						//id,x,y,w,h
		LCD_DispText_option(					215,140,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_CutOut));	//x,y,w,h,justification,col,font,txt

	//buttons
		//off button
		LCD_DrawSoftRect_1(						10,50,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_7,	10,50,50,50);						//id,x,y,w,h
		LCD_DispText_option(					10,50,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "1/0");	//x,y,w,h,justification,col,font,txt

		//stopwatch
		LCD_DrawSoftRect_1(						685,350,110,45,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_8,	685,350,110,45);					//id,x,y,w,h
		LCD_DispText_option(					685,350,110,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_Timer));	//x,y,w,h,justification,col,font,txt

		//fan
		LCD_DrawSoftRect_1(						685,300,110,45,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_9,	685,300,110,45);					//id,x,y,w,h
		LCD_DispText_option(					685,300,110,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_Fan));	//x,y,w,h,justification,col,font,txt

		//audio
		LCD_DrawSoftRect_1(						685,200,50,45,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_A,	685,200,50,45);						//id,x,y,w,h
		LCD_DispText_option(					685,200,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_Audio));	//x,y,w,h,justification,col,font,txt

		//Vol+
		LCD_DrawSoftRect_1(						745,250,50,45,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_B,	745,250,50,45);						//id,x,y,w,h
		LCD_DispText_option(					745,250,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_VolP));	//x,y,w,h,justification,col,font,txt

		//Vol-
		LCD_DrawSoftRect_1(						685,250,50,45,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_C,	685,250,50,45);						//id,x,y,w,h
		LCD_DispText_option(					685,250,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_VolN));	//x,y,w,h,justification,col,font,txt

		//Graph rescale
		LCD_DrawSoftRect_1(						720, 10,50,40,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_D,	720, 10,50,40);						//id,x,y,w,h
		LCD_DispText_option(					720, 10,50,40,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_Scale));	//x,y,w,h,justification,col,font,txt

		//BACK to Main code
		LCD_DrawSoftRect_1(						70,50,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_E,	70,50,50,50);						//id,x,y,w,h
		LCD_DispText_option(					70,50,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "UI");	//x,y,w,h,justification,col,font,txt


		//graph area
		LCD_FillRect(300, 10, PP_HIS_LEN, 100, COLOUR_BLUE);
		app_PP_data_init(0,	100, 0.1, -0.05);

	//Sliders
		//FLOW
		LCD_DrawSoftRect_1(						290,156,60,304,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_F,	290,158,60,300);					//id,x,y,w,h
		LCD_DispText_option(					285,135,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_Flow));	//x,y,w,h,justification,col,font,txt
		// OXYGEN
		LCD_DrawSoftRect_1(						360,156,60,304,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_10,	360,158,60,300);					//id,x,y,w,h
		LCD_DispText_option(					355,135,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_Oxygen));	//x,y,w,h,justification,col,font,txt

	//Buttons
		//Ctrl Normal
		app_touchscreen_button_add(	BUTTON_11,	430,160,60,60);						//id,x,y,w,h
		LCD_DispText_option(					425,140,70,20,CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_CAll));	//x,y,w,h,justification,col,font,txt

		//Ctrl Venturi
		app_touchscreen_button_add(	BUTTON_12,	430,240,60,60);						//id,x,y,w,h
		LCD_DispText_option(					425,220,70,20,CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_CVent));	//x,y,w,h,justification,col,font,txt

		//Ctrl Flow only
		app_touchscreen_button_add(	BUTTON_13,	430,320,60,60);						//id,x,y,w,h
		LCD_DispText_option(					425,300,70,20,CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_AFlow));	//x,y,w,h,justification,col,font,txt

		//Ctrl O2 conc only
		app_touchscreen_button_add(	BUTTON_14,	430,400,60,60);						//id,x,y,w,h
		LCD_DispText_option(					425,380,70,20,CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_OFlow));	//x,y,w,h,justification,col,font,txt

		//Calibrate screen
		LCD_DrawSoftRect_1(						500,400,150,50,10, COLOUR_BLACK);	//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_15,	500,400,150,50);					//id,x,y,w,h
		LCD_DispText_option(					500,400,150,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_CalSens));	//x,y,w,h,justification,col,font,txt

	//TEXT
		LCD_DispText_option(540,140,70,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_O2Conc));		//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(540,160,70,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_250Max));		//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(540,180,70,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, "-Paracube");						//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(540,200,70,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_CalmFlow));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(540,220,70,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_dAir));		//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(540,240,70,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_dO2));		//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(540,260,70,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_dTot));		//x,y,w,h,justification,col,font,txt


	}
	app_UI_screen_lock_timer_reset(1);

	if(sys_tic_rd() > sys_tic_timer_1s){
		sys_tic_timer_1s	=  sys_tic_rd() + 1000;
	//every 1 second

	//Time update
		rawtime	= time(0);
		timeinfo = localtime (&rawtime);
		sprintf(str,"%2d:%02d.%02d"	, timeinfo->tm_hour
									, timeinfo->tm_min
									, timeinfo->tm_sec			);
		LCD_DispText_option(10,5,130,20,CENTER,COLOUR_BLACK,COLOUR_WHITE, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt

		sprintf(str,"%s %d %s %d "	, wday_str[timeinfo->tm_wday]
									, timeinfo->tm_mday
									, mon_str[timeinfo->tm_mon]
									, timeinfo->tm_year + EPOCH_YEAR	);
		LCD_DispText_option(10,25,130,20,CENTER,COLOUR_BLACK,COLOUR_WHITE, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt

	//O2/Air suppy flags
		O2_supply	=  api_io_rd_O2Supply(0);
		LCD_FillCircle(		40,120,10, (LCD_COLOR) colour_R_G[O2_supply]);
		air_supply	=  api_io_rd_AirSupply(0);
		LCD_FillCircle(		110,120,10, (LCD_COLOR) colour_R_G[air_supply]);

	//Apnoea
		if(last_apnoea != PP_data.BreathRate.len_s ){
			last_apnoea	= PP_data.BreathRate.len_s;
			sprintf(str," Apnoea = %2ds ",last_apnoea);
			LCD_DispText_option(700,70,100,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		}

	//PSU
		if(thermistor != io_status_st_glb.Temperature ){
			thermistor	= io_status_st_glb.Temperature;
			sprintf(str," T %3.3fC ",io_status_st_glb.Temperature);
			LCD_DispText_option(700,90,100,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		}
		if(test5V != io_status_st_glb.Test_5V ){
			test5V	= io_status_st_glb.Test_5V;
			sprintf(str," 5V = %2.1fV ",test5V);
			LCD_DispText_option(700,110,100,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		}
		if(ADC_reset != app_Pneumatics.ADC_reset_cnt ){
			ADC_reset	= app_Pneumatics.ADC_reset_cnt;
			sprintf(str," %d ",ADC_reset);
			LCD_DispText_option(700,130,100,15,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		}


	//every 5 seconds
		if(++timer_5s >= 5){
			timer_5s	=  0;
	//battery data
			sprintf(str," %s %4.0fmAh %2.1fC", LanguageStr(LangStr_Bat),FuelGauge.act_mAh
													,FuelGauge.act_temperature);
			LCD_DispText_option(150,5,130,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_WHITE, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
			sprintf(str," %2.1fV %1.3fA"			,FuelGauge.act_voltage
													,FuelGauge.act_current);
			LCD_DispText_option(150,25,130,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_WHITE, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
			switch(battery_charger.mode){
				case BAT_CHARGING:
					sprintf(str," %s  %3.1f%%   ", LanguageStr(LangStr_Charging)	,battery_charger.percentage);
					break;
				case BAT_CHARGED:
					sprintf(str," %s ", LanguageStr(LangStr_Charged));
					break;
				case BAT_DISCHARGING:
					sprintf(str," %2d%s %02d%s %3.1f%%  "	,battery_charger.sec_remaining / 3600, LanguageStr(LangStr_Hr),
															(battery_charger.sec_remaining%3600) / 60, LanguageStr(LangStr_Min),
															battery_charger.percentage);
					break;
				default:
					sprintf(str," -- ");
			}
			LCD_DispText_option(150,45,130,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_WHITE, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt

		}
	}

	//stopwatch
	if(api_Stopwatch_read(&day,&hr,&min,&sec) ){
		sprintf(str,"%d %2d:%02d:%02d ",day,hr,min,sec);
		swatch_running	=  api_Stopwatch_running_read();
		LCD_DispText_option(685,400,110,50,CENTER_LEFT,COLOUR_BLACK,colour_R_G[swatch_running], &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
	}
	//fan
	if(fan_status != api_io_fan(0xff)){
		fan_status	= api_io_fan(0xff);
		if(fan_status){		LCD_FillCircle(		690,305,5, (LCD_COLOR) colour_R_G[1]);	}
		else{				LCD_FillCircle(		690,305,5, (LCD_COLOR) colour_R_G[0]);	}
	}
	//volume
	if(volume != api_audio_volume_per_rd(AUDIO_AMP) ){
		volume	= api_audio_volume_per_rd(AUDIO_AMP);
		sprintf(str," %2d ", volume);
		LCD_DispText_option(745,205,60,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_Vol));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(745,225,60,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);								//x,y,w,h,justification,col,font,txt
	}

	//MAX 250
	if(Max250_O2conc != io_status_st_glb.Max250_O2_value ){
		Max250_O2conc	= io_status_st_glb.Max250_O2_value;
		sprintf(str,"%2.3f ",Max250_O2conc);
		LCD_DispText_option(615,160,50,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//Paracube
	if(Paracube_O2conc != io_status_st_glb.Paracube_O2_value ){
		Paracube_O2conc	= io_status_st_glb.Paracube_O2_value;
		sprintf(str,"%2.3f ",Paracube_O2conc);
		LCD_DispText_option(615,180,50,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//Air flow
	if(Airflow != io_status_st_glb.Flow_Air ){
		Airflow	= io_status_st_glb.Flow_Air;
		sprintf(str,"%2.3f ",Airflow);
		LCD_DispText_option(615,220,50,15,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//O2 flow
	if(O2flow != io_status_st_glb.Flow_O2 ){
		O2flow	= io_status_st_glb.Flow_O2;
		sprintf(str,"%2.3f ",O2flow);
		LCD_DispText_option(615,240,50,15,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//total flow
	if(totalflow != app_Pneumatics.flow_rate_actual ){
		totalflow	= app_Pneumatics.flow_rate_actual;
		sprintf(str,"%2.3f ",totalflow);
		LCD_DispText_option(615,260,50,15,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}

	//graph data
	if( app_PP_graph_start_refresh()){

		for(x=0;x<PP_HIS_LEN;x++){
			if(	app_PP_data_graph_get(x,&last_pix,&new_pix) ){
              LCD_DrawDot(x+300, 110-last_pix, COLOUR_BLUE);
				LCD_DrawDot(x+300, 110-new_pix, COLOUR_WHITE);
			}
		}

	}

	//button status
	if(last_CutOut != io_status_st_glb.safety_cutout){
		last_CutOut =  io_status_st_glb.safety_cutout;
		LCD_FillCircle(	250,190,30, (LCD_COLOR) colour_R_G[!io_status_st_glb.safety_cutout]		);
	}
	if(last_Neb_EN != io_status_st_glb.valve_nebuliser){
		last_Neb_EN = io_status_st_glb.valve_nebuliser;
		LCD_FillCircle(	250,270,30, (LCD_COLOR) colour_R_G[io_status_st_glb.valve_nebuliser]	);
	}
	if(last_Cal_EN != io_status_st_glb.valve_O2_calibrate){
		last_Cal_EN	= io_status_st_glb.valve_O2_calibrate;
		LCD_FillCircle(	250,350,30, (LCD_COLOR) colour_R_G[io_status_st_glb.valve_O2_calibrate]	);
	}
	if(last_Venturi_EN != io_status_st_glb.valve_venturi_en){
		last_Venturi_EN	= io_status_st_glb.valve_venturi_en;
		LCD_FillCircle(	250,430,30, (LCD_COLOR) colour_R_G[io_status_st_glb.valve_venturi_en]	);
	}
	if(last_CTRL_MODE != app_pneumatic_pid_mode_read() ){
		last_CTRL_MODE  = app_pneumatic_pid_mode_read();

		LCD_FillCircle(	460,190,30, COLOUR_RED	);
		LCD_FillCircle(	460,270,30, COLOUR_RED	);
		LCD_FillCircle(	460,350,30, COLOUR_RED	);
		LCD_FillCircle(	460,430,30, COLOUR_RED	);

		switch(last_CTRL_MODE){
			case PNEUMATIC_CTRL_PID_MATH:		LCD_FillCircle(	460,190,30, COLOUR_YELLOW	);		break;
			case PNEUMATIC_CTRL_PID_VENTURI:	LCD_FillCircle(	460,270,30, COLOUR_GREEN	);		break;
			case PNEUMATIC_CTRL_TEST_AIR:		LCD_FillCircle(	460,350,30, COLOUR_GREEN	);		break;
			case PNEUMATIC_CTRL_TEST_O2:		LCD_FillCircle(	460,430,30, COLOUR_GREEN	);		break;
		}
	}
//	PinSet(TP5,1);
//sliders updated
	if(last_O2 != io_status_st_glb.pwm_O2){
		last_O2	= io_status_st_glb.pwm_O2;
		if(last_O2_dis != 0xffff){
			LCD_FillRect(	12,last_O2_dis, 54, 2, COLOUR_GRAY_8A);
		}
		last_O2_dis		=  (uint16_t)( 458 - ((float)last_O2/100*3) );
		LCD_FillRect(	12,last_O2_dis, 54, 2, COLOUR_RED);

		sprintf(str,"  %.1f%% ",(float)last_O2/100);
        LCD_DispText_option(	5,460,70,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
    }
   	if(last_AIR != io_status_st_glb.pwm_Air){
   		last_AIR	= io_status_st_glb.pwm_Air;
		if(last_AIR_dis != 0xffff){
			LCD_FillRect(	82,last_AIR_dis, 54, 2, COLOUR_GRAY_8A);
		}
		last_AIR_dis		=  (uint16_t)( 458 - ((float)last_AIR/100*3) );
		LCD_FillRect(	82,last_AIR_dis, 54, 2, COLOUR_RED);

		sprintf(str,"  %.1f%% ",(float)last_AIR/100);
        LCD_DispText_option(	75,460,70,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
    }
    if(last_Venturi_Flow != io_status_st_glb.pwm_VenturiFlow){
    	last_Venturi_Flow	= io_status_st_glb.pwm_VenturiFlow;
		if(last_O2_dis != 0xffff){
			LCD_FillRect(	152,last_Venturi_Flow_dis, 54, 2, COLOUR_GRAY_8A);
		}
		last_Venturi_Flow_dis		=  (uint16_t)( 458 - ((float)last_Venturi_Flow/100*3) );
		LCD_FillRect(	152,last_Venturi_Flow_dis, 54, 2, COLOUR_RED);

		sprintf(str,"  %.1f%% ",(float)last_Venturi_Flow/100);
        LCD_DispText_option(	145,460,70,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
    }

    //CTRL sliders
    if(last_target_Flow != app_Pneumatics.flow_rate_target){
    	last_target_Flow	= app_Pneumatics.flow_rate_target;
		if(last_O2_dis != 0xffff){
			LCD_FillRect(	292,last_target_Flow_dis, 50, 2, COLOUR_GRAY_8A);
		}
		last_target_Flow_dis		=  (uint16_t)( 458 - ((float)last_target_Flow/150*300) );
		LCD_FillRect(	292,last_target_Flow_dis, 50, 2, COLOUR_RED);

		sprintf(str,"  %d%s ",last_target_Flow, LanguageStr(LangStr_Lmin));
        LCD_DispText_option(	285,460,70,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
    }
    if(last_target_O2 != app_Pneumatics.O2_conc_target){
    	last_target_O2	= app_Pneumatics.O2_conc_target;
		if(last_O2_dis != 0xffff){
			LCD_FillRect(	362,last_target_O2_dis, 50, 2, COLOUR_GRAY_8A);
		}
		last_target_O2_dis		=  (uint16_t)( 458 - ((float)last_target_O2*3) );
		LCD_FillRect(	362,last_target_O2_dis, 50, 2, COLOUR_RED);

		sprintf(str,"  %d%%%s ",last_target_O2, LanguageStr(LangStr_calmOxygen));
        LCD_DispText_option(	355,460,70,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
    }

	if(last_act_Flow != app_Pneumatics.flow_rate_actual){
    	last_act_Flow	= app_Pneumatics.flow_rate_actual;
		if(last_act_Flow_dis != 0xffff){
			LCD_FillRect(	342,(U16)last_act_Flow_dis,  4, 2, COLOUR_GRAY_8A);
		}
		last_act_Flow_dis		=  (uint16_t)( 458 - ((float)last_act_Flow/150*300) );
		LCD_FillRect(	342,(U16)last_act_Flow_dis,  4, 2, COLOUR_YELLOW);
    }
    if(last_act_O2 != app_Pneumatics.O2_conc_actual){
    	last_act_O2	= app_Pneumatics.O2_conc_actual;
		if(last_act_O2_dis != 0xffff){
			LCD_FillRect(	412,(U16)last_act_O2_dis, 4, 2, COLOUR_GRAY_8A);
		}
		last_act_O2_dis		=  (uint16_t)( 458 - ((float)last_act_O2*3) );
		LCD_FillRect(	412,(U16)last_act_O2_dis,  4, 2, COLOUR_YELLOW);
    }
//	PinSet(TP5,0);



	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	// User Input
	switch(touch_status){
        case BUTTON_0:
		case BUTTON_0_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
       			api_pwm_O2((uint16_t)( (1-slider_info.y_ee_f) * 10000) );
       		}
			break;
		case BUTTON_1:
		case BUTTON_1_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
        		api_pwm_Air((uint16_t)( (1-slider_info.y_ee_f) * 10000) );
        	}
			break;
		case BUTTON_2:
		case BUTTON_2_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
        		api_pwm_VenturiFlow((uint16_t)( (1-slider_info.y_ee_f) * 10000) );
        	}
			break;
		case BUTTON_3:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_valve_Venturi_En( VALVE_Venturi_TOG );
			}
			break;
		case BUTTON_4:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_valve_O2_calibrate(VALVE_O2_CAL_TOG );
			}
			break;
		case BUTTON_5:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
			//	api_valve_nebuliser( VALVE_NEBLISER_TOG );
				if(UI.nebuliser){		UI.nebuliser = 0;	}
				else{					UI.nebuliser = 1;	}
				app_pneumatic_set_nebuliser(UI.nebuliser);
			}
			break;
		case BUTTON_6:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_io_safety_cutout( SAFTY_CUTOUT_TOG );
			}
			break;
		case BUTTON_7:
			if(UI.touch_used == 0){
				printf("\r\nSwitch Off");
				system_shutdown(RESET_SYSTEM);
			}
			break;
		case BUTTON_8:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_Stopwatch_mode_set(STOPWATCH_MODE_START_STOP);
			}
			break;
		case BUTTON_8_HOLD:
			if(UI.touch_used == 1){
				UI.touch_used	=  2;
				api_Stopwatch_mode_set(STOPWATCH_MODE_STOP);
				api_Stopwatch_mode_set(STOPWATCH_MODE_RESET);
			}
			break;
		case BUTTON_9:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_io_fan(2);		//toggle status
			}
			break;
		case BUTTON_A:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_audio_play_file_stop();
			}
			break;
		case BUTTON_A_HOLD:
			if(UI.touch_used == 1){
				UI.touch_used	=  2;
				api_audio_play_file(FLASH_ADD_AUDIO_FILE_A);
			}
			break;
		case BUTTON_B:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_audio_volume_per_change(AUDIO_AMP,2);
			}
			break;
		case BUTTON_C:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_audio_volume_per_change(AUDIO_AMP,-2);
			}
			break;
		case BUTTON_D:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				app_PP_data_max_min();
			}
			break;

	//----------------------
		case BUTTON_E:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				UI.screen		=  UI_WELCOME;
				UI.nebuliser 	=  0;
				app_pneumatic_set_nebuliser(UI.nebuliser);
			}
			break;

		case BUTTON_F:
		case BUTTON_F_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
				app_Pneumatics.flow_rate_target	=  (uint16_t)( (1-slider_info.y_ee_f) * 150) ;
				app_pneumatic_set_flow(app_Pneumatics.flow_rate_target,app_Pneumatics.O2_conc_target);
			}
			break;
		case BUTTON_10:
		case BUTTON_10_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
				app_Pneumatics.O2_conc_target	=  (uint16_t)( (1-slider_info.y_ee_f) * 100) ;
				app_Pneumatics.pid_conc_output	=  (float)app_Pneumatics.O2_conc_target;
				app_pneumatic_set_flow(app_Pneumatics.flow_rate_target,app_Pneumatics.O2_conc_target);
			}
			break;
		case BUTTON_11:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(app_pneumatic_pid_mode_read() != PNEUMATIC_CTRL_PID_MATH){
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_MATH);
					UI.pneumatics_mode	=  1;
				}
				else{
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);
					UI.pneumatics_mode	=  0;
				}
			}
			break;
		case BUTTON_12:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(app_pneumatic_pid_mode_read() != PNEUMATIC_CTRL_PID_VENTURI){
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_VENTURI);
					UI.pneumatics_mode	=  1;
				}
				else{
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);
					UI.pneumatics_mode	=  0;
				}
			}
			break;
		case BUTTON_13:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(app_pneumatic_pid_mode_read() != PNEUMATIC_CTRL_TEST_AIR){
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_TEST_AIR);
					UI.pneumatics_mode	=  1;
				}
				else{
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);
					UI.pneumatics_mode	=  0;
				}
			}
			break;
		case BUTTON_14:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(app_pneumatic_pid_mode_read() != PNEUMATIC_CTRL_TEST_O2){
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_TEST_O2);
					UI.pneumatics_mode	=  1;
				}
				else{
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);
					UI.pneumatics_mode	=  0;
				}
			}
			break;
    	case BUTTON_15:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				UI.screen			=  UI_ENG_CALIBRATE_SENSOR;
			}
			break;

		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			break;
	}



	return;
}


/**********************************************************************************************************
 * Function Name : app_UI_screen_calibrate_sensors
 * Description   : This function is used to handle the welcome screen
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		12/4/18		W Paul			Original Created
 **********************************************************************************************************/
void app_UI_screen_calibrate_sensors(uint8_t touch_status, uint8_t but_status, bool init)
{
	// Local Variables
	static uint32_t		sys_tic_timer_1s;
	slider_t			slider_info;
	char				str[30];

	static uint16_t		last_O2					= 0xffff;
	static uint16_t		last_AIR				= 0xffff;
	static uint16_t		last_O2_dis				= 0xffff;
	static uint16_t		last_AIR_dis			= 0xffff;
	static uint8_t		last_CutOut				= 0xff;

	static uint32_t		O2flow_adc;
	static uint32_t		Airflow_adc;
	static uint32_t		Max250_adc;
	static float		Paracube_O2conc;
	static uint32_t		PP_adc;
	static uint8_t		last_CTRL_MODE;
	static uint32_t		last_O2Raw;
	static uint16_t		last_O2Raw_dis;
	static uint32_t		last_AIR_Raw;
	static uint16_t		last_AIR_Raw_dis;
	static uint16_t		AirPWM;
	static uint16_t		O2PWM;

	uint8_t				air_supply;
	uint8_t				O2_supply;

	uint16_t			x;
	uint16_t			last_pix,new_pix;
	
	LCD_COLOR			pwm_colour				=  COLOUR_BLACK;

	// Code
	if(pwm_calibration.calib_status				== CALIB_SUCCESS){
		pwm_colour			=  COLOUR_GREY_3C;
	}
	if(init){
		UI.screen			=  UI_ENG_CALIBRATE_SENSOR_WAIT;
		UI.screen_last		=  UI_ENG_CALIBRATE_SENSOR;
		app_selfcheck_AudioMute(1);
		UI.touch_used		=  1;
		UI.but_used			=  1;

		AirPWM				=  0xFFFF;
		O2PWM				=  0xFFFF;

		last_CutOut			=  0xff;
		last_O2				=  0xffff;
		last_AIR			=  0xffff;
		last_O2				=  0xffff;
		last_AIR			=  0xffff;
		last_O2_dis			=  0xffff;
		last_AIR_dis		=  0xffff;

		last_O2Raw			=  0x008fffff;
		last_O2Raw_dis		=  0xffff;
		last_AIR_Raw		=  0x008fffff;
		last_AIR_Raw_dis	=  0xffff;

		last_CTRL_MODE		=  0xff;

		app_touchscreen_button_disable_all();

		//background
		LCD_FillRect(0,	0,	LCD_DISP_WIDTH, LCD_DISP_HEIGHT, COLOUR_GRAY_8A);

		//Sidebar
		UI.sidebar.endis	=  0;
	//graph area
		LCD_FillRect(440, 10, PP_HIS_LEN, 100, COLOUR_BLUE);
		app_PP_data_init(0,	100, 25, -25);
	//sliders
		//O2
		LCD_DrawSoftRect_1(						10,156+96+10,60,150+45+4,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_0,	10,158+96+10,60,150+45);					//id,x,y,w,h
		LCD_DispText_option(					10,135+96+10,60,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "O ");	//x,y,w,h,justification,col,font,txt

		//AIR
		LCD_DrawSoftRect_1(						80,156+96+10,60,150+45+4,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_1,	80,158+96+10,60,150+45);					//id,x,y,w,h
		LCD_DispText_option(					80,135+96+10,60,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_Air));	//x,y,w,h,justification,col,font,txt

	//Buttons
		//BACK to Main code
		LCD_DrawSoftRect_1(						160,  5,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_2,	160,  5,50,50);					//id,x,y,w,h
		LCD_DispText_option(					160,  5,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "UI");	//x,y,w,h,justification,col,font,txt

		//Safty cutout
		app_touchscreen_button_add(	BUTTON_3,	150,400,60,60);					//id,x,y,w,h
		LCD_DispText_option(					145,380,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, LanguageStr(LangStr_CutOut));	//x,y,w,h,justification,col,font,txt


		//calibration
	//-------------------

		//calibrate flow O2 off
		LCD_DrawSoftRect_1(						450,150,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_4,	450,150,50,45);						//id,x,y,w,h
		LCD_DispText_option(					450,150,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "0");		//x,y,w,h,justification,col,font,txt

		//calibrate flow O2 10 l/min
		LCD_DrawSoftRect_1(						450,200,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_5,	450,200,50,45);						//id,x,y,w,h
		LCD_DispText_option(					450,200,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "10");	//x,y,w,h,justification,col,font,txt

		//calibrate flow O2 30 l/min
		LCD_DrawSoftRect_1(						450,250,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_6,	450,250,50,45);						//id,x,y,w,h
		LCD_DispText_option(					450,250,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "30");	//x,y,w,h,justification,col,font,txt

		//calibrate flow O2 80 l/min
		LCD_DrawSoftRect_1(						450,300,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_7,	450,300,50,45);						//id,x,y,w,h
		LCD_DispText_option(					450,300,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "80");	//x,y,w,h,justification,col,font,txt

		//calibrate flow O2 120 l/min
		LCD_DrawSoftRect_1(						450,350,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_8,	450,350,50,45);						//id,x,y,w,h
		LCD_DispText_option(					450,350,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "120");	//x,y,w,h,justification,col,font,txt

		//calibrate flow O2 140 l/min
		LCD_DrawSoftRect_1(						450,400,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_25,	450,400,50,45);						//id,x,y,w,h
		LCD_DispText_option(					450,400,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "140");	//x,y,w,h,justification,col,font,txt

	//-------------------
		//calibrate flow Air off
		LCD_DrawSoftRect_1(						520,150,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_9,	520,150,50,45);						//id,x,y,w,h
		LCD_DispText_option(					520,150,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "0");		//x,y,w,h,justification,col,font,txt

		//calibrate flow Air 10 l/min
		LCD_DrawSoftRect_1(						520,200,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add( BUTTON_A,	520,200,50,45);						//id,x,y,w,h
		LCD_DispText_option(					520,200,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "10");	//x,y,w,h,justification,col,font,txt

		//calibrate flow Air 30 l/min
		LCD_DrawSoftRect_1(						520,250,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_B,	520,250,50,45);						//id,x,y,w,h
		LCD_DispText_option(					520,250,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "30");	//x,y,w,h,justification,col,font,txt

		//calibrate flow Air 80 l/min
		LCD_DrawSoftRect_1(						520,300,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add( BUTTON_C,	520,300,50,45);						//id,x,y,w,h
		LCD_DispText_option(					520,300,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "80");	//x,y,w,h,justification,col,font,txt

		//calibrate flow Air 120 l/min
		LCD_DrawSoftRect_1(						520,350,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_D,	520,350,50,45);						//id,x,y,w,h
		LCD_DispText_option(					520,350,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "120");	//x,y,w,h,justification,col,font,txt

		//calibrate flow Air 140 l/min
		LCD_DrawSoftRect_1(						520,400,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_26,	520,400,50,45);						//id,x,y,w,h
		LCD_DispText_option(					520,400,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "140");	//x,y,w,h,justification,col,font,txt

	//-------------------
		//calibrate max250 20%
		LCD_DrawSoftRect_1(						590,150,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_E,	590,150,50,45);					//id,x,y,w,h
		LCD_DispText_option(					590,150,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "21%");	//x,y,w,h,justification,col,font,txt

		//calibrate max250 100%
		LCD_DrawSoftRect_1(						590,200,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_F,	590,200,50,45);					//id,x,y,w,h
		LCD_DispText_option(					590,200,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "100%");	//x,y,w,h,justification,col,font,txt

	//-------------------
		//calibrate paracube 20%
		LCD_DrawSoftRect_1(						660,150,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_10,	660,150,50,45);					//id,x,y,w,h
		LCD_DispText_option(					660,150,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "21%");	//x,y,w,h,justification,col,font,txt

		//calibrate paracube 100%
		LCD_DrawSoftRect_1(						660,200,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_11,	660,200,50,45);					//id,x,y,w,h
		LCD_DispText_option(					660,200,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "100%");	//x,y,w,h,justification,col,font,txt

	//-------------------
		//calibrate PP 0cmH2O
		LCD_DrawSoftRect_1(						730,150,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_12,	730,150,50,45);					//id,x,y,w,h
		LCD_DispText_option(					730,150,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "0");	//x,y,w,h,justification,col,font,txt

		//calibrate PP 40cmH2O
		LCD_DrawSoftRect_1(						730,200,50,45,5, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_13,	730,200,50,45);					//id,x,y,w,h
		LCD_DispText_option(					730,200,50,45,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "40");	//x,y,w,h,justification,col,font,txt

	//-------------------
		//slider O2 +
		LCD_DrawSoftRect_1(						15,  5,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_14,	15,  5,50,50);					//id,x,y,w,h
		LCD_DispText_option(					15,  5,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "+");	//x,y,w,h,justification,col,font,txt

		//slider O2 -
		LCD_DrawSoftRect_1(						15, 60,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_15,	15, 60,50,50);					//id,x,y,w,h
		LCD_DispText_option(					15, 60,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "-");	//x,y,w,h,justification,col,font,txt

		//slider Air +
		LCD_DrawSoftRect_1(						85,  5,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_16,	85,  5,50,50);					//id,x,y,w,h
		LCD_DispText_option(					85,  5,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "+");	//x,y,w,h,justification,col,font,txt

		//slider Air -
		LCD_DrawSoftRect_1(						85, 60,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_17,	85, 60,50,50);					//id,x,y,w,h
		LCD_DispText_option(					85, 60,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "-");	//x,y,w,h,justification,col,font,txt

		//slider O2 10 +
		LCD_DrawSoftRect_1(						15,  60+55,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_21,	15,  60+55,50,50);					//id,x,y,w,h
		LCD_DispText_option(					15,  60+55,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "+");	//x,y,w,h,justification,col,font,txt

		//slider O2 10 -
		LCD_DrawSoftRect_1(						15, 60+55+55,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_22,	15, 60+55+55,50,50);					//id,x,y,w,h
		LCD_DispText_option(					15, 60+55+55,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "-");	//x,y,w,h,justification,col,font,txt

		//slider Air 10 +
		LCD_DrawSoftRect_1(						85,  60+55,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_23,	85,  60+55,50,50);					//id,x,y,w,h
		LCD_DispText_option(					85,  60+55,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "+");	//x,y,w,h,justification,col,font,txt

		//slider Air 10 -
		LCD_DrawSoftRect_1(						85, 60+55+55,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_24,	85, 60+55+55,50,50);					//id,x,y,w,h
		LCD_DispText_option(					85, 60+55+55,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "-");	//x,y,w,h,justification,col,font,txt
	//-------------------
	//sliders
		//O2 RAW
		LCD_DrawSoftRect_1(						220,156,60,304,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_18,	220,158,60,300);					//id,x,y,w,h
		LCD_DispText_option(					220,135,60,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "O  raw");	//x,y,w,h,justification,col,font,txt

		//AIR RAW
		LCD_DrawSoftRect_1(						290,156,60,304,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_19,	290,158,60,300);					//id,x,y,w,h
		LCD_DispText_option(					290,135,60,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "Air Raw");	//x,y,w,h,justification,col,font,txt

	//-------------------
		//slider O2 raw +
		LCD_DrawSoftRect_1(						225,  5,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_1A,	225,  5,50,50);					//id,x,y,w,h
		LCD_DispText_option(					225,  5,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "+");	//x,y,w,h,justification,col,font,txt

		//slider O2 raw -
		LCD_DrawSoftRect_1(						225, 60,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_1B,	225, 60,50,50);					//id,x,y,w,h
		LCD_DispText_option(					225, 60,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "-");	//x,y,w,h,justification,col,font,txt

		//slider Air raw +
		LCD_DrawSoftRect_1(						295,  5,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_1C,	295,  5,50,50);					//id,x,y,w,h
		LCD_DispText_option(					295,  5,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "+");	//x,y,w,h,justification,col,font,txt

		//slider Air raw -
		LCD_DrawSoftRect_1(						295, 60,50,50,10, COLOUR_BLACK);		//x,y,w,h,corner,col
		app_touchscreen_button_add(	BUTTON_1D,	295, 60,50,50);					//id,x,y,w,h
		LCD_DispText_option(					295, 60,50,50,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "-");	//x,y,w,h,justification,col,font,txt

	//-------------------
		//Start raw ctrl O2
		app_touchscreen_button_add(	BUTTON_1E,	150,160,60,60);					//id,x,y,w,h
		LCD_DispText_option(					145,140,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "Ctrl O2");	//x,y,w,h,justification,col,font,txt

		//Start raw ctrl Air
		app_touchscreen_button_add(	BUTTON_1F,	150,240,60,60);					//id,x,y,w,h
		LCD_DispText_option(					145,220,70,20,CENTER,COLOUR_BLACK,TRANSPARENT, &fontArial16h, "Ctrl Air");	//x,y,w,h,justification,col,font,txt

	//-------------------
		//Calibrate O2 Valve Minimum
		LCD_DrawSoftRect_1(						590,350,85,45,5,	pwm_colour);		//x,y,w,h,corner,col
		if(pwm_calibration.calib_status			!= CALIB_SUCCESS){
			app_touchscreen_button_add(			BUTTON_27,	590,350,85,45);				//id,x,y,w,h
		}
		LCD_DispText_option(					590,350,85,45,CENTER,pwm_colour,TRANSPARENT, &fontArial16h, "O2");	//x,y,w,h,justification,col,font,txt

		//Calibrate Air Valve Minimum
		LCD_DrawSoftRect_1(						695,350,85,45,5,	pwm_colour);		//x,y,w,h,corner,col
		if(pwm_calibration.calib_status			!= CALIB_SUCCESS){
			app_touchscreen_button_add(			BUTTON_28,	695,350,85,45);				//id,x,y,w,h
		}
		LCD_DispText_option(					695,350,85,45,CENTER,pwm_colour,TRANSPARENT, &fontArial16h, "Air");	//x,y,w,h,justification,col,font,txt

		//Lock Values
		LCD_DrawSoftRect_1(						590,400,190,45,5,	pwm_colour);		//x,y,w,h,corner,col
		if(pwm_calibration.calib_status			!= CALIB_SUCCESS){
			app_touchscreen_button_add(			BUTTON_29,	590,400,190,45);			//id,x,y,w,h
		}
		LCD_DispText_option(					590,400,190,45,CENTER,pwm_colour,TRANSPARENT, &fontArial16h, "Lock Values");	//x,y,w,h,justification,col,font,txt

	//-------------------


	//TEXT
		LCD_DispText_option(450,115,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_CalmFlow));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(450,132,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_dO2));		//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(520,115,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_CalmFlow));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(520,132,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_dAir));		//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(590,115,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_O2Conc));		//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(590,132,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_250Max));		//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(660,115,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_O2Conc));		//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(660,132,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, "-Paracube");						//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(730,115,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_Patient));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(730,132,70,17,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, LanguageStr(LangStr_Pressure));	//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(590,332,210,17,CENTER_LEFT,pwm_colour,COLOUR_GRAY_8A, &fontArial16h, "Valve Minimum");				//x,y,w,h,justification,col,font,txt
	}

	app_UI_screen_lock_timer_reset(1);

	if(sys_tic_rd() > sys_tic_timer_1s){
		sys_tic_timer_1s	=  sys_tic_rd() + 1000;

	//O2/Air suppy flags
		O2_supply	=  api_io_rd_O2Supply(0);
		LCD_FillCircle(		40,125+96+12,10, (LCD_COLOR) colour_R_G[O2_supply]);
		air_supply	=  api_io_rd_AirSupply(0);
		LCD_FillCircle(		110,125+96+12,10, (LCD_COLOR) colour_R_G[air_supply]);
	}

	//button status
	if(last_CutOut != io_status_st_glb.safety_cutout){
		last_CutOut =  io_status_st_glb.safety_cutout;
		LCD_FillCircle(	180,430,30, (LCD_COLOR) colour_R_G[!io_status_st_glb.safety_cutout]		);
	}
	if(last_CTRL_MODE != app_pneumatic_pid_mode_read() ){
		last_CTRL_MODE  = app_pneumatic_pid_mode_read();

		LCD_FillCircle(	180,190,30, COLOUR_RED	);
		LCD_FillCircle(	180,270,30, COLOUR_RED	);

		switch(last_CTRL_MODE){
			case PNEUMATIC_CTRL_RAW_O2:		LCD_FillCircle(	180,190,30, COLOUR_GREEN	);		break;
			case PNEUMATIC_CTRL_RAW_AIR:	LCD_FillCircle(	180,270,30, COLOUR_GREEN	);		break;
		}
	}

	//O2 flow
	if(O2flow_adc != io_status_st_glb.ADC_Pressure_O2 ){
		O2flow_adc	= io_status_st_glb.ADC_Pressure_O2;
		sprintf(str,"%2.5f ",io_status_st_glb.Flow_O2);
		LCD_DispText_option(450,446,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		sprintf(str,"%d ",io_status_st_glb.ADC_Pressure_O2);
		LCD_DispText_option(450,460,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//Air flow
	if(Airflow_adc != io_status_st_glb.ADC_Pressure_Air ){
		Airflow_adc	= io_status_st_glb.ADC_Pressure_Air;
		sprintf(str,"%2.5f ",io_status_st_glb.Flow_Air);
		LCD_DispText_option(520,446,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		sprintf(str,"%d ",io_status_st_glb.ADC_Pressure_Air);
		LCD_DispText_option(520,460,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//MAX 250
	if(Max250_adc != io_status_st_glb.ADC_O2_Sensor ){
		Max250_adc	= io_status_st_glb.ADC_O2_Sensor;
		sprintf(str,"%2.2f ",io_status_st_glb.Max250_O2_value);
		LCD_DispText_option(590,250,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		sprintf(str,"%d ",io_status_st_glb.ADC_O2_Sensor);
		LCD_DispText_option(590,270,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//Paracube
	if(Paracube_O2conc != io_status_st_glb.Paracube_O2_value ){
		Paracube_O2conc	= io_status_st_glb.Paracube_O2_value;
		sprintf(str,"%2.2f ",Paracube_O2conc);
		LCD_DispText_option(660,250,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//PPressure
	if(PP_adc != io_status_st_glb.ADC_patientP ){
		PP_adc	= io_status_st_glb.ADC_patientP;
		sprintf(str,"%2.2f",io_status_st_glb.patientP);
		LCD_DispText_option(730,250,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		sprintf(str,"%d ",io_status_st_glb.ADC_patientP);
		LCD_DispText_option(730,270,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//O2PWM
	if(O2PWM	!= pwm_calibration.pwm_O2){
		O2PWM	=  pwm_calibration.pwm_O2;
		sprintf(str,"%d ",pwm_calibration.pwm_O2);
		LCD_DispText_option(590,460,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	//AirPWM
	if(AirPWM	!= pwm_calibration.pwm_air){
		AirPWM	=  pwm_calibration.pwm_air;
		sprintf(str,"%d ",pwm_calibration.pwm_air);
		LCD_DispText_option(695,460,65,20,CENTER_LEFT,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}


	//graph data
	if( app_PP_graph_start_refresh()){

		for(x=0;x<PP_HIS_LEN;x++){
			if(	app_PP_data_graph_get(x,&last_pix,&new_pix) ){
				LCD_DrawDot(x+440, 110-last_pix, COLOUR_BLUE);
				LCD_DrawDot(x+440, 110-new_pix, COLOUR_WHITE);
			}
		}
	}


//sliders updated
	if(last_O2 != io_status_st_glb.pwm_O2){
		last_O2	= io_status_st_glb.pwm_O2;
		if(last_O2_dis != 0xffff){
			LCD_FillRect(	12,last_O2_dis, 54, 2, COLOUR_GRAY_8A);
		}
		last_O2_dis		=  (uint16_t)( 458 - ((float)last_O2*0.65/100*3) );
		LCD_FillRect(	12,last_O2_dis, 54, 2, COLOUR_RED);

		sprintf(str,"  %.1f%% ",(float)last_O2/100);
		LCD_DispText_option(	10,460,60,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	if(last_AIR != io_status_st_glb.pwm_Air){
		last_AIR	= io_status_st_glb.pwm_Air;
		if(last_AIR_dis != 0xffff){
			LCD_FillRect(	82,last_AIR_dis, 54, 2, COLOUR_GRAY_8A);
		}
		last_AIR_dis		=  (uint16_t)( 458 - ((float)last_AIR*0.65/100*3) );
		LCD_FillRect(	82,last_AIR_dis, 54, 2, COLOUR_RED);

		sprintf(str,"  %.1f%% ",(float)last_AIR/100);
		LCD_DispText_option(	80,460,60,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}

	if(last_O2Raw != app_Pneumatics.flow_rate_O2_raw){
		last_O2Raw	= app_Pneumatics.flow_rate_O2_raw;
		if(last_O2Raw_dis != 0xffff){
			LCD_FillRect(	222,last_O2Raw_dis, 54, 2, COLOUR_GRAY_8A);
		}
		last_O2Raw_dis		=  (uint16_t)( 458 - ( (0x00ffffff - last_O2Raw)/0xDB00) );	//300 * 0xDB00 = 0xffffff (approx)
		LCD_FillRect(	222,last_O2Raw_dis, 54, 2, COLOUR_RED);

		sprintf(str,"  %d ",last_O2Raw);
		LCD_DispText_option(	220,460,60,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}
	if(last_AIR_Raw != app_Pneumatics.flow_rate_air_raw){
		last_AIR_Raw	= app_Pneumatics.flow_rate_air_raw;
		if(last_AIR_Raw_dis != 0xffff){
			LCD_FillRect(	292,last_AIR_Raw_dis, 54, 2, COLOUR_GRAY_8A);
		}
		last_AIR_Raw_dis		=  (uint16_t)( 458 - ( (0x00ffffff - last_AIR_Raw)/0xDB00) );//300 * 0xDB00 = 0xffffff (approx)
		LCD_FillRect(	292,last_AIR_Raw_dis, 54, 2, COLOUR_RED);

		sprintf(str,"  %d ",last_AIR_Raw);
		LCD_DispText_option(	290,460,60,20,CENTER,COLOUR_BLACK,COLOUR_GRAY_8A, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
	}



	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	// User Input
	switch(touch_status){
		case BUTTON_0:
		case BUTTON_0_HOLD:
			if( app_touchscreen_slider_info(&slider_info)	){
				api_pwm_O2((uint16_t)( (1-slider_info.y_ee_f) * 10000) );
			}
			break;
		case BUTTON_1:
		case BUTTON_1_HOLD:
			if(	app_touchscreen_slider_info(&slider_info)	){
				api_pwm_Air((uint16_t)( (1-slider_info.y_ee_f) * 10000) );
			}
			break;
		case BUTTON_2:
		case BUTTON_2_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				UI.screen			= UI_ENG_MODE;
			}
			break;
		case BUTTON_3:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_io_safety_cutout( SAFTY_CUTOUT_TOG );
			}
			break;

	//----------------------
		case BUTTON_4:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_set_points(	CAL_FLOW_O2_a,0,10);			// low point 0  flow,hi point 50l/min
				api_calibrate_cmd(			CAL_FLOW_O2_a,CAL_INIT);
				api_calibrate_set_points(	CAL_FLOW_O2_b,10,30);
				api_calibrate_cmd(			CAL_FLOW_O2_b,CAL_INIT);
				api_calibrate_set_points(	CAL_FLOW_O2_c,30,80);
				api_calibrate_cmd(			CAL_FLOW_O2_c,CAL_INIT);
				api_calibrate_set_points(	CAL_FLOW_O2_d,80,120);
				api_calibrate_cmd(			CAL_FLOW_O2_d,CAL_INIT);
				api_calibrate_set_points(	CAL_FLOW_O2_e,120,140);
				api_calibrate_cmd(			CAL_FLOW_O2_e,CAL_INIT);

				api_calibrate_cmd(			CAL_FLOW_O2_a,CAL_POINT_LOW);
				printf("\r\nO2 Flow Cal 0 l/min point");
				LCD_FillCircle(450-5, 150+20, 4, COLOUR_GREEN);
				LCD_FillCircle(450-5, 200+20, 4, COLOUR_RED);
				LCD_FillCircle(450-5, 250+20, 4, COLOUR_RED);
				LCD_FillCircle(450-5, 300+20, 4, COLOUR_RED);
				LCD_FillCircle(450-5, 350+20, 4, COLOUR_RED);
				LCD_FillCircle(450-5, 400+20, 4, COLOUR_RED);
				
				cal_temperature_O2			=  io_status_st_glb.Temperature;
				csp_sys_mem_wr((uint8_t*)(&cal_temperature_O2), MEM_ADD_CALIBATION + 0x404, sizeof(float));
			}
			break;
		case BUTTON_5:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_O2_b,CAL_POINT_LOW);

				api_calibrate_cmd(			CAL_FLOW_O2_a,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_O2_a,CAL_CALCULATE);

				printf("\r\nO2 Flow Cal 10 l/min point");
				LCD_FillCircle(450-5, 200+20, 4, COLOUR_GREEN);
			}
			break;
		case BUTTON_6:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_O2_c,CAL_POINT_LOW);

				api_calibrate_cmd(			CAL_FLOW_O2_b,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_O2_b,CAL_CALCULATE);

				printf("\r\nO2 Flow Cal 30 l/min point");
				LCD_FillCircle(450-5, 250+20, 4, COLOUR_GREEN);
			}
			break;
		case BUTTON_7:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_O2_d,CAL_POINT_LOW);

				api_calibrate_cmd(			CAL_FLOW_O2_c,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_O2_c,CAL_CALCULATE);

				printf("\r\nO2 Flow Cal 80 l/min point");
				LCD_FillCircle(450-5, 300+20, 4, COLOUR_GREEN);
			}
			break;
		case BUTTON_8:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_O2_e,CAL_POINT_LOW);

				api_calibrate_cmd(			CAL_FLOW_O2_d,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_O2_d,CAL_CALCULATE);
				printf("\r\nO2 Flow Cal 120 l/min point");
				LCD_FillCircle(450-5, 350+20, 4, COLOUR_GREEN);
			}
			break;
		case BUTTON_25:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_O2_e,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_O2_e,CAL_CALCULATE);
				printf("\r\nO2 Flow Cal 140 l/min point");
				LCD_FillCircle(450-5, 400+20, 4, COLOUR_GREEN);
			}
			break;
		//----------------------
		case BUTTON_9:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_set_points(	CAL_FLOW_AIR_a,0,10);
				api_calibrate_cmd(			CAL_FLOW_AIR_a,CAL_INIT);
				api_calibrate_set_points(	CAL_FLOW_AIR_b,10,30);
				api_calibrate_cmd(			CAL_FLOW_AIR_b,CAL_INIT);
				api_calibrate_set_points(	CAL_FLOW_AIR_c,30,80);
				api_calibrate_cmd(			CAL_FLOW_AIR_c,CAL_INIT);
				api_calibrate_set_points(	CAL_FLOW_AIR_d,80,120);
				api_calibrate_cmd(			CAL_FLOW_AIR_d,CAL_INIT);
				api_calibrate_set_points(	CAL_FLOW_AIR_e,120,140);
				api_calibrate_cmd(			CAL_FLOW_AIR_e,CAL_INIT);

				api_calibrate_cmd(			CAL_FLOW_AIR_a,CAL_POINT_LOW);
				printf("\r\nAir Flow Cal 0 l/min point");
				LCD_FillCircle(520-5, 150+20, 4, COLOUR_GREEN);
				LCD_FillCircle(520-5, 200+20, 4, COLOUR_RED);
				LCD_FillCircle(520-5, 250+20, 4, COLOUR_RED);
				LCD_FillCircle(520-5, 300+20, 4, COLOUR_RED);
				LCD_FillCircle(520-5, 350+20, 4, COLOUR_RED);
				LCD_FillCircle(520-5, 400+20, 4, COLOUR_RED);
				
				cal_temperature_air		=  io_status_st_glb.Temperature;
				csp_sys_mem_wr((uint8_t*)(&cal_temperature_air), MEM_ADD_CALIBATION + 0x400, sizeof(float));
			}
			break;
		case BUTTON_A:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_AIR_b,CAL_POINT_LOW);

				api_calibrate_cmd(			CAL_FLOW_AIR_a,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_AIR_a,CAL_CALCULATE);

				printf("\r\nAir Flow Cal 10 l/min point");
				LCD_FillCircle(520-5, 200+20, 4, COLOUR_GREEN);
			}
			break;
		case BUTTON_B:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_AIR_c,CAL_POINT_LOW);

				api_calibrate_cmd(			CAL_FLOW_AIR_b,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_AIR_b,CAL_CALCULATE);

				printf("\r\nAir Flow Cal 30 l/min point");
				LCD_FillCircle(520-5, 250+20, 4, COLOUR_GREEN);
			}
			break;
		case BUTTON_C:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_AIR_d,CAL_POINT_LOW);

				api_calibrate_cmd(			CAL_FLOW_AIR_c,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_AIR_c,CAL_CALCULATE);

				printf("\r\nAir Flow Cal 80 l/min point");
				LCD_FillCircle(520-5, 300+20, 4, COLOUR_GREEN);
			}
			break;
		case BUTTON_D:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_AIR_e,CAL_POINT_LOW);

				api_calibrate_cmd(			CAL_FLOW_AIR_d,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_AIR_d,CAL_CALCULATE);
				printf("\r\nAir Flow Cal 120 l/min point");
				LCD_FillCircle(520-5, 350+20, 4, COLOUR_GREEN);
			}
			break;
		case BUTTON_26:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_FLOW_AIR_e,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_FLOW_AIR_e,CAL_CALCULATE);
				printf("\r\nAir Flow Cal 140 l/min point");
				LCD_FillCircle(520-5, 400+20, 4, COLOUR_GREEN);
			}
			break;

		//----------------------
		case BUTTON_E:	//MAX250 21%
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_set_points(	CAL_SENSOR_O2,21,100);
				api_calibrate_cmd(			CAL_SENSOR_O2,CAL_INIT);
				api_calibrate_cmd(			CAL_SENSOR_O2,CAL_POINT_LOW);
				printf("\r\nMax O2 Cal 21%% point");
			}
			break;
		case BUTTON_F:	//MAX250 100%
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_SENSOR_O2,CAL_POINT_HIGH);
				api_calibrate_cmd(			CAL_SENSOR_O2,CAL_CALCULATE);
				printf("\r\nMax O2 Cal 100%% point");
			}
			break;
		//----------------------
		case BUTTON_10:	//PARACUBE   21%
			csp_paracube_command(PARACUBE_2_POINT_CAL_LOW, 21.0);
			break;
		case BUTTON_11:	//PARACUBE   100%
			csp_paracube_command(PARACUBE_2_POINT_CAL_HIGH, 100.0);
			break;
		//----------------------
		case BUTTON_12:	//Patient 0cm H20
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_set_points(	CAL_SENSOR_PP,0,40);				// low point 0 cmH2O ,hi point 40 cmH2O
				api_calibrate_cmd(			CAL_SENSOR_PP,CAL_INIT);			//(CAL_FLOW_O2,CAL_FLOW_AIR, CAL_SENSOR_O2)(CAL_POINT_LOW,CAL_POINT_HIGH,CAL_CALCULATE)
				api_calibrate_cmd(			CAL_SENSOR_PP,CAL_POINT_LOW);		//(CAL_FLOW_O2,CAL_FLOW_AIR, CAL_SENSOR_O2)(CAL_POINT_LOW,CAL_POINT_HIGH,CAL_CALCULATE)
				printf("\r\nPatient Pressure  Cal 0 cmH2O point");
			}
			break;
		case BUTTON_13:	//Patient 40cm H20
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				api_calibrate_cmd(			CAL_SENSOR_PP,CAL_POINT_HIGH);		//(CAL_FLOW_O2,CAL_FLOW_AIR, CAL_SENSOR_O2)(CAL_POINT_LOW,CAL_POINT_HIGH,CAL_CALCULATE)
				api_calibrate_cmd(			CAL_SENSOR_PP,CAL_CALCULATE);		//(CAL_FLOW_O2,CAL_FLOW_AIR, CAL_SENSOR_O2)(CAL_POINT_LOW,CAL_POINT_HIGH,CAL_CALCULATE)
				printf("\r\nPatient Pressure  Cal 40 cmH2O point");
			}
			break;

		//----------------------
		case BUTTON_14:
		case BUTTON_14_HOLD:
			if(io_status_st_glb.pwm_O2	<= 9990){	api_pwm_O2(io_status_st_glb.pwm_O2+10);		}
			else{									api_pwm_O2(10000);							}
			break;
		case BUTTON_15:
		case BUTTON_15_HOLD:
			if(io_status_st_glb.pwm_O2	>= 10){		api_pwm_O2(io_status_st_glb.pwm_O2-10);		}
			else{									api_pwm_O2(0);								}
			break;
		case BUTTON_16:
		case BUTTON_16_HOLD:
			if(io_status_st_glb.pwm_Air	<= 9990){	api_pwm_Air(io_status_st_glb.pwm_Air+10);	}
			else{									api_pwm_Air(10000);							}
			break;
		case BUTTON_17:
		case BUTTON_17_HOLD:
			if(io_status_st_glb.pwm_Air	>= 10){		api_pwm_Air(io_status_st_glb.pwm_Air-10);	}
			else{									api_pwm_Air(0);								}
			break;

		//----------------------
		case BUTTON_18:
		case BUTTON_18_HOLD:
			if(	app_touchscreen_slider_info(&slider_info)	){
				app_Pneumatics.flow_rate_O2_raw		=  (uint32_t)(0x00ffffff - ( (1 - slider_info.y_ee_f) * 0xffff00) ) ;
			}
			break;
		case BUTTON_19:
		case BUTTON_19_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
        		app_Pneumatics.flow_rate_air_raw	=  (uint32_t)(0x00ffffff - ( (1 - slider_info.y_ee_f) * 0xffff00) ) ;
        	}
			break;
		//----------------------
		case BUTTON_1B:			// -
			if(app_Pneumatics.flow_rate_O2_raw < 0xffff00){		app_Pneumatics.flow_rate_O2_raw += 0x000100;		}
			break;
		case BUTTON_1B_HOLD:	// - hold
			if(app_Pneumatics.flow_rate_O2_raw < 0xfff000){		app_Pneumatics.flow_rate_O2_raw += 0x000f00;		}
			break;
		case BUTTON_1A:			// +
			if(app_Pneumatics.flow_rate_O2_raw > 0x000000){		app_Pneumatics.flow_rate_O2_raw -= 0x000100;		}
			break;
		case BUTTON_1A_HOLD:	// + hold
			if(app_Pneumatics.flow_rate_O2_raw > 0x000f00){		app_Pneumatics.flow_rate_O2_raw -= 0x000f00;		}
			break;

		case BUTTON_1D:			// -
			if(app_Pneumatics.flow_rate_air_raw < 0xffff00){	app_Pneumatics.flow_rate_air_raw += 0x000100;	}
			break;
		case BUTTON_1D_HOLD:	// -  hold
			if(app_Pneumatics.flow_rate_air_raw < 0xfff000){	app_Pneumatics.flow_rate_air_raw += 0x000f00;	}
			break;
		case BUTTON_1C:			// +
			if(app_Pneumatics.flow_rate_air_raw > 0x000000){	app_Pneumatics.flow_rate_air_raw -= 0x000100;	}
			break;
		case BUTTON_1C_HOLD:	// + hold
			if(app_Pneumatics.flow_rate_air_raw > 0x000f00){	app_Pneumatics.flow_rate_air_raw -= 0x000f00;	}
			break;

		case BUTTON_1E:
		case BUTTON_1E_HOLD:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(	!(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_RAW_O2) ){
					app_Pneumatics.flow_rate_O2_raw		=  io_status_st_glb.ADC_Pressure_O2;
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_RAW_O2);
					UI.pneumatics_mode	=  1;
				}
				else{
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);
					UI.pneumatics_mode	=  0;
				}
			}
			break;
		case BUTTON_1F:
		case BUTTON_1F_HOLD:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(	!(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_RAW_AIR) ){
					app_Pneumatics.flow_rate_air_raw	=  io_status_st_glb.ADC_Pressure_Air;
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_RAW_AIR);
					UI.pneumatics_mode	=  1;
				}
				else{
					app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);
					UI.pneumatics_mode	=  0;
				}
			}
			break;

//----------------------
		case BUTTON_21:
		case BUTTON_21_HOLD:
			if(io_status_st_glb.pwm_O2 <= 9900){		api_pwm_O2(io_status_st_glb.pwm_O2+100);	}
			else{										api_pwm_O2(10000);							}
			break;
		case BUTTON_22:
		case BUTTON_22_HOLD:
			if(io_status_st_glb.pwm_O2 >= 100) {		api_pwm_O2(io_status_st_glb.pwm_O2-100);	}
			else{										api_pwm_O2(0);								}
			break;
		case BUTTON_23:
		case BUTTON_23_HOLD:
			if(io_status_st_glb.pwm_Air <= 9900){		api_pwm_Air(io_status_st_glb.pwm_Air+100);	}
			else{										api_pwm_Air(10000);							}
			break;
		case BUTTON_24:
		case BUTTON_24_HOLD:
			if(io_status_st_glb.pwm_Air >= 100){		api_pwm_Air(io_status_st_glb.pwm_Air-100);	}
			else{										api_pwm_Air(0);								}
			break;
		case BUTTON_27:
			pwm_calibration.pwm_O2						=  io_status_st_glb.pwm_O2	- 50;
			break;
		case BUTTON_28:
			pwm_calibration.pwm_air						=  io_status_st_glb.pwm_Air	- 50;
			break;
		case BUTTON_29:
			pwm_calibration.calib_status				=  CALIB_SUCCESS;
			api_calibrate_pwm_wr();
			break;
		//----------------------
		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			break;
	}


	return;
}
/**********************************************************************************************************
 * Function Name : app_UI_screen_calibrate
 * Description   : This function is used to handle the 'calibrate' screen
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		28/11/15	Tim Barr		Original Created
 **********************************************************************************************************/
void app_UI_screen_calibrate(uint8_t touch_status, uint8_t but_status, bool init)
{
	// Local Variables
	static uint8_t	cal_point;
	static uint8_t	cal_state;
	static tPoint	DP[3];
	static tPoint	TP[3];
	char			str[10];
	int16_t			X,Y;
	uint16_t			TouchPressure;

	// Code
 	if(init){
 		UI.screen			=  UI_TOUCH_CALIBRATE_WAIT;
		UI.screen_last		=  UI_TOUCH_CALIBRATE;
 		app_selfcheck_AudioMute(1);
 		//var init
 		DP[0].x		=  LCD_DISP_WIDTH	* 0.1;
 		DP[0].y		=  LCD_DISP_HEIGHT	* 0.1;
 		DP[1].x		=  LCD_DISP_WIDTH	* 0.5;
 		DP[1].y		=  LCD_DISP_HEIGHT	* 0.9;
 		DP[2].x		=  LCD_DISP_WIDTH	* 0.9;
 		DP[2].y		=  LCD_DISP_HEIGHT	* 0.5;
		cal_state	=  0;
		cal_point	=  0;

		//Sidebar
		UI.sidebar.endis	=  0;

		//buttons
		app_touchscreen_button_disable_all();
		app_touchscreen_button_add(BUTTON_0,0,0,4096,4096);	//not detecting a calibrated points

		//screen graphics
		LCD_FillRect(	0,	0,	LCD_DISP_WIDTH,LCD_DISP_HEIGHT,			COLOUR_BLACK);
		LCD_DispText_option(220, 100,300,35,CENTER_LEFT, COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_TCal));		//check_height

		sprintf(str,"%s %d", LanguageStr(LangStr_Point), cal_point+1);
		LCD_DispText_option(220, 130,300,35,CENTER_LEFT, COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, str);
		LCD_FillRect(DP[cal_point].x-1, DP[cal_point].y-1, 3, 3, COLOUR_WHITE);

		api_touch_config_default();
	}

	app_UI_screen_lock_timer_reset(1);	//unlock screen

	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}
	// User Input
	switch(touch_status){
        case BUTTON_0:
        case BUTTON_0_HOLD:
			switch(cal_state){
				case 0:	//get adc for point 0
				case 2:	//get adc for point 1
				case 4:	//get adc for point 2
					api_touch_raw_rd(&TP[cal_point].x,&TP[cal_point].y);
					printf("\r\nCalibration pt %d xraw= %d yraw = %d",cal_point,TP[cal_point].x,TP[cal_point].y);

					LCD_FillRect(DP[cal_point].x - 1, DP[cal_point].y - 1, 3, 3, COLOUR_BLACK);

					cal_point	+= 1;

					if(cal_state == 4){
						api_touch_CalculateCalibrationConstants(DP,TP);
                        LCD_DispText_option(220, 130,300,35,CENTER_LEFT, COLOUR_WHITE,COLOUR_BLACK, &fontArialNarrowBold31h, LanguageStr(LangStr_CComplete));
                        LCD_DispText_option(220, 160,300,35,CENTER_LEFT, COLOUR_WHITE,COLOUR_BLACK, &fontArialNarrowBold31h, LanguageStr(LangStr_UOff));
					}
					else{
						sprintf(str,"%s %d ", LanguageStr(LangStr_Point), cal_point+1);
						LCD_DispText_option(220, 130,300,35,CENTER_LEFT, COLOUR_WHITE,COLOUR_BLACK, &fontArialNarrowBold31h, str);
					}

					cal_state	+= 1;

					break;
				case 1:
				case 3:
					//wait for key release
					break;
				case 5:
					api_touch_CoOrds_rd(&X,&Y,&TouchPressure);
					LCD_DrawDot(X,Y,COLOUR_WHITE);
					break;
				default:
					printf("\r\nUnknown calibration state %d",cal_state);
			}
	        break;
	    case BUTTON_PRE_HOLD:
	    case BUTTON_JUST_RELEASED:
	    	break;
		case BUTTON_RELEASED:
			if((cal_state == 1)||(cal_state == 3)){
				LCD_FillRect(DP[cal_point].x-1, DP[cal_point].y-1, 3, 3, COLOUR_WHITE);
				cal_state	+= 1;
			}
			break;
		default:
			printf("\r\nUnregistered soft button detected %d",touch_status);
	}

	return;
}

/**********************************************************************************************************
 * Function Name : app_UI_screen_mode_setup
 * Description   : This function is used to select the mode of operation
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		28/11/15	Tim Barr		Original Created
 **********************************************************************************************************/
// 									CPAP	,CPAP_PAED	,CPAP_HELMET	,BUBBLE_PAP	,HFOT	,POINT
const uint16_t UI_PMODE_X[6]	=  { 50		,350		,50				,350		,50		,350};
const uint16_t UI_PMODE_Y[6]	=  { 20		,20			,175			,175		,330	,330};
void app_UI_screen_mode_setup(uint8_t touch_status, uint8_t but_status, bool init)
{
	uint8_t			i;
	LCD_COLOR		col_a;
	LCD_COLOR		col_b;


	if(init){
		UI.screen		=  UI_MODE_SETUP_WAIT;
		UI.screen_last	=  UI_MODE_SETUP;

		app_selfcheck_AudioMute(0);
		UI.but_used		=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used	=  1;

		app_touchscreen_button_disable_all();

	//background
		LCD_FillRect(	0,	0,	650, LCD_DISP_HEIGHT,	COLOUR_SCREEN_BACKGROUND);
		if(UI.demo_mode){
			LCD_DrawRect_n(1,	1,	650-2, LCD_DISP_HEIGHT-1-2, 4, COLOUR_RED);
		}

        UI.sidebar.endis	=  1;
		UI.sidebar.but1		=  BUT_BLANK;
		UI.sidebar.but2		=  BUT_SETTINGS;
		UI.sidebar.but3		=  BUT_NEXT;
		app_UI_screen_side_bar(BUILD_ALL_NEW);
		app_touchscreen_button_disable(BUTTON_SIDE1);

		for(i=0;i<=(uint8_t)PMODE_POINT;i++){
			if(UI.app_Pmode_available[i]){		col_a	=  colour_mode[i];
												col_b	=  COLOUR_WHITE;
			}
			else{								col_a	=  COLOUR_ICON_A_DISABLE;
												col_b	=  COLOUR_ICON_B_DISABLE;
			}

			LCD_FillSoftRect( 						UI_PMODE_X[i]+2, UI_PMODE_Y[i]+2, 250-4, 130-4, 10, col_a );
			LCD_DrawSoftRect_n(						UI_PMODE_X[i],   UI_PMODE_Y[i],   250,   130,   10, 3, col_b);
			app_touchscreen_button_add(BUTTON_6+i,	UI_PMODE_X[i],   UI_PMODE_Y[i],   250,   130);
			if(UI.app_Pmode_available[i] == 0){
				app_touchscreen_button_audio_option(BUTTON_6+i,1);	//if button is not available   play a different tone
			}

		}
		LCD_DrawSoftRect_n(			UI_PMODE_X[UI.select_mode],  UI_PMODE_Y[UI.select_mode], 250, 130, 10, 3, COLOUR_BLACK);
		LCD_DispText_option(				50,  20, 250, 130,		CENTER,			colour_W_B[UI.app_Pmode_available[PMODE_CPAP]],			TRANSPARENT, &fontArialNarrowBold31h, "CPAP");		//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(				350, 20, 250,  65,		BOTTOM_CENTER,	colour_W_B[UI.app_Pmode_available[PMODE_CPAP_PAED]],	TRANSPARENT, &fontArialNarrowBold31h, "CPAP");		//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(				350, 85, 250,  65,		TOP_CENTER,		colour_W_B[UI.app_Pmode_available[PMODE_CPAP_PAED]],	TRANSPARENT, &fontArialNarrowBold31h, "Paed");		//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(				50, 175, 250,  65,		BOTTOM_CENTER,	colour_W_B[UI.app_Pmode_available[PMODE_CPAP_HELMET]],	TRANSPARENT, &fontArialNarrowBold31h, "CPAP");		//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(				50, 240, 250,  65,		TOP_CENTER,		colour_W_B[UI.app_Pmode_available[PMODE_CPAP_HELMET]],	TRANSPARENT, &fontArialNarrowBold31h, "Helmet");	//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(				350,175, 250,  65,		BOTTOM_CENTER,	colour_W_B[UI.app_Pmode_available[PMODE_BUBBLE_PAP]],	TRANSPARENT, &fontArialNarrowBold31h, "Bubble");	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(				350,240, 250,  65,		TOP_CENTER,		colour_W_B[UI.app_Pmode_available[PMODE_BUBBLE_PAP]],	TRANSPARENT, &fontArialNarrowBold31h, "PAP");		//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(				50, 330, 250, 130,		CENTER,			colour_W_B[UI.app_Pmode_available[PMODE_HFOT]],			TRANSPARENT, &fontArialNarrowBold31h, "HFOT");		//x,y,w,h,justification,col,font,txt

		LCD_DispText_option(				350,330, 250, 130,		CENTER,			colour_W_B[UI.app_Pmode_available[PMODE_POINT]],		TRANSPARENT, &fontArialNarrowBold31h, "POINT");		//x,y,w,h,justification,col,font,txt

	}
	else{
		app_UI_screen_side_bar(BUILD_REFRESH);
	}

	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	switch(touch_status){
//		case BUTTON_TIME:
		case BUTTON_TIME_HOLD:
		case BUTTON_SIDE2:
		case BUTTON_SIDE2_HOLD:
			UI.screen			=  UI_SETTINGS;
			break;
//
//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:
//
//		case BUTTON_SIDE1:
//		case BUTTON_SIDE1_HOLD:

		case BUTTON_SIDE3:
			if(UI.touch_used == 0){
				UI.touch_used		=  1;
				if(app_pneumatic_set_mode(UI.select_mode,0) == 1){		//set up pneumatics t the selected mode
					api_Stopwatch_mode_set(STOPWATCH_MODE_RESET);
				}
				UI.screen			=  UI_FLOW_SETUP;
			}
			break;
	//	case BUTTON_SIDE3_HOLD:
	//		break;

		case BUTTON_6:
		case BUTTON_7:
		case BUTTON_8:
		case BUTTON_20: // override testing
		case BUTTON_9:
		case BUTTON_A:
		case BUTTON_B:
			if(UI.touch_used == 0){
				UI.touch_used		=  1;
				if(UI.app_Pmode_available[touch_status - BUTTON_6]){
					UI.select_mode	= (app_Pmode_enum)(touch_status - BUTTON_6);
					for(i=0;i<=(uint8_t)PMODE_POINT;i++){
						LCD_DrawSoftRect_n(		UI_PMODE_X[i],  UI_PMODE_Y[i], 250, 130, 10, 3, COLOUR_WHITE);
					}
					LCD_DrawSoftRect_n(			UI_PMODE_X[UI.select_mode],  UI_PMODE_Y[UI.select_mode], 250, 130, 10, 3, COLOUR_BLACK);
				}
			//	app_UI_screen_side_bar(BUILD_NEW);
			}
			break;

//		case BUTTON_JUST_RELEASED:
		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			break;
	}

	return;
}

/**********************************************************************************************************
 * Function Name : app_UI_screen_flow_setup
 * Description   : This function is used to set the desired flow and O2 coincentration
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		28/11/15	Tim Barr		Original Created
 **********************************************************************************************************/
void app_UI_screen_flow_setup(uint8_t touch_status, uint8_t but_status, bool init)
{
	static uint8_t		rebuild_screen;
	char				str[30];
	slider_t			slider_info;
	uint16_t			line_width;
	uint16_t			spacing;
	uint16_t			line_max;
	uint16_t			posx;

	if(init){
    	UI.screen			=  UI_FLOW_SETUP_WAIT;
    	UI.screen_last		=  UI_FLOW_SETUP;
    	app_selfcheck_AudioMute(0);
		UI.but_used		=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used	=  1;

//		UI.nebuliser		=  app_Pneumatics.nebuliser_state;

		app_touchscreen_button_disable_all();

	//background

		LCD_FillRect(	0,	0,	650, LCD_DISP_HEIGHT,	colour_mode[UI.select_mode]);
		if(UI.demo_mode){
			LCD_DrawRect_n(1,	1,	650-2, LCD_DISP_HEIGHT-1-2, 4, COLOUR_RED);
		}

	//************************************************
	//Sidebar
		UI.sidebar.endis	=  1;
		if(UI.pneumatics_mode == 0){
	//	if(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_IDLE){	//not running pneumatics
			UI.sidebar.but1	=  BUT_MODE;
			UI.sidebar.but2	=  BUT_SETTINGS;
			UI.sidebar.but3	=  BUT_NEXT;
		}
		else{
			UI.sidebar.but1	=  BUT_BLANK;
			UI.sidebar.but2	=  BUT_BLANK;
			UI.sidebar.but3	=  BUT_NEXT;
		}


		app_UI_screen_side_bar(BUILD_ALL_NEW);

	//************************************************
	//Title
		switch(UI.select_mode){
			case PMODE_CPAP:			sprintf(str, "CPAP");			break;
			case PMODE_CPAP_PAED:		sprintf(str, "CPAP Paed");	break;
			case PMODE_CPAP_HELMET:		sprintf(str, "CPAP Helmet");	break;
			case PMODE_BUBBLE_PAP:		sprintf(str, "Bubble PAP");		break;
			case PMODE_HFOT:			sprintf(str, "HFOT");			break;
			case PMODE_POINT:			sprintf(str, "POINT");			break;
		}
		posx = LCD_DispText_option(	25,  	5, 600, 50,				BOTTOM_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h	, str);
		LCD_DispText_option(		posx,  	5, 600-25-posx, 47,		BOTTOM_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow37h	, LanguageStr(LangStr_FSetting));

	//************************************************
	//O2
		//Text
		LCD_DispText_option(				 25, 285, 200, 40,		CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, 	LanguageStr(LangStr_Oxygen));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(				 100,325, 200, 25,		CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArial22h, 	LanguageStr(LangStr_pOxygen));	//x,y,w,h,justification,col,font,txt

		//- button
		LCD_FillSoftRect(					20,  335, 75, 75, 10, 	COLOUR_BUTTON_PAIR1);
		LCD_DrawSoftRect_1(					20,  335, 75, 75, 10, 	COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_9,20,  335, 75, 75);
		LCD_DispText_option(				20,  335, 75, 75,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrow60h, "-");	//x,y,w,h,justification,col,font,txt

		//+ button
		LCD_FillSoftRect(					480, 335, 75, 75, 10, 	COLOUR_BUTTON_PAIR1);
		LCD_DrawSoftRect_1(					480, 335, 75, 75, 10, 	COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_B,480, 335, 75, 75);
		LCD_DispText_option(				480, 335, 75, 75,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrow60h, "+");	//x,y,w,h,justification,col,font,txt

		//slider
		app_line_slider_draw(				110, 380,350,0,100,20,COLOUR_BLACK,DISPLAY_NUMBERS_ABOVE,&UI.O2);	//x,y,w,minx,max,x,spacing,info

		UI.O2.limit_min		=  app_Pneumatics.limits.O2_min;
		UI.O2.limit_max		=  app_Pneumatics.limits.O2_max;
		if(UI.O2.limit_min < app_Pneumatics.O2_conc_min){
			UI.O2.limit_min	=  app_Pneumatics.O2_conc_min;
		}
		if(UI.O2.limit_max > app_Pneumatics.O2_conc_max){
			UI.O2.limit_max	=  app_Pneumatics.O2_conc_max;
		}

		if((api_io_rd_AirSupply(0)				== 0)&&(UI.demo_mode == 0)){
			if(UI.O2.limit_max < app_Pneumatics.O2_conc_max){
				printf("\r\n O2 Max limit - has been pushed up");
			}
			UI.O2.limit_min			=  app_Pneumatics.O2_conc_max;
			UI.O2.limit_max			=  app_Pneumatics.O2_conc_max;
		}
		else if((api_io_rd_O2Supply(0)			== 0)&&(UI.demo_mode == 0)){
			if(UI.O2.limit_min > app_Pneumatics.O2_conc_min){
				printf("\r\n O2 Max limit - has been pushed down");
			}
			UI.O2.limit_min			=  app_Pneumatics.O2_conc_min;
			UI.O2.limit_max			=  app_Pneumatics.O2_conc_min;
		}


		UI.O2.limit_min_pix		=  app_line_slider_val2pix( &UI.O2	,UI.O2.limit_min);
		UI.O2.limit_max_pix		=  app_line_slider_val2pix( &UI.O2	,UI.O2.limit_max);

		app_touchscreen_button_add(BUTTON_A,110+UI.O2.end_min_pix, 380-50, UI.O2.end_max_pix - UI.O2.end_min_pix, 150);

		//endstops
		LCD_DispMonoBitmapTrans(			110-10+UI.O2.limit_min_pix, 380+7, "IMG_EndStopL",COLOUR_BLACK);
		LCD_DispMonoBitmapTrans(			110   +UI.O2.limit_max_pix, 380+7, "IMG_EndStopR",COLOUR_BLACK);


		UI.O2.val				=  app_Pneumatics.O2_conc_target;
		if(UI.O2.val < UI.O2.limit_min){	UI.O2.val = UI.O2.limit_min;	}
		if(UI.O2.val > UI.O2.limit_max){	UI.O2.val = UI.O2.limit_max;	}

		UI.O2.val_last		=  uINIT_s16;
		UI.FLOW.val			=  app_Pneumatics.flow_rate_target;
		UI.FLOW.val_last	=  uINIT_s16;

		rebuild_screen 	=  1;
	}

	if(rebuild_screen){
		rebuild_screen	=  0;
		LCD_FillRect(	115-15, 100-50-1+100,	450, 50,colour_mode[UI.select_mode]);
	//************************************************
	//FLOW
		//Text
		LCD_DispText_option(				35,   105,200, 40,		CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Flow));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(				110,  145,200, 25,		CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArial22h, LanguageStr(LangStr_Lmin));	//x,y,w,h,justification,col,font,txt


		//- button
		LCD_FillSoftRect(					 20,  155, 75, 75, 10, 	COLOUR_BUTTON_PAIR2);
		LCD_DrawSoftRect_1(					 20,  155, 75, 75, 10, 	COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_6, 20,  155, 75, 75);
		LCD_DispText_option(				 20,  155, 75, 75,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrow60h, "-");	//x,y,w,h,justification,col,font,txt

		//+ button
		LCD_FillSoftRect(					480,  155, 75, 75, 10, 	COLOUR_BUTTON_PAIR2);
		LCD_DrawSoftRect_1(					480,  155, 75, 75, 10, 	COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_8,480,  155, 75, 75);
		LCD_DispText_option(				480,  155, 75, 75,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrow60h, "+");	//x,y,w,h,justification,col,font,txt

		//slider
		switch(UI.select_mode){
			case PMODE_CPAP:
			case PMODE_CPAP_HELMET:	line_width = 308; line_max = 140; spacing = 20;	break;
			case PMODE_CPAP_PAED:
			case PMODE_HFOT:
			case PMODE_POINT:		line_width = 320; line_max =  80; spacing = 10;	break;
			case PMODE_BUBBLE_PAP:	line_width = 320; line_max =  20; spacing =  5;	break;
		}
		app_line_slider_draw(				125, 200,line_width,  0,line_max,spacing,COLOUR_BLACK,DISPLAY_NUMBERS_ABOVE,&UI.FLOW);	//x,y,w,minx,max,x,spacing

		UI.FLOW.limit_min	=  app_Pneumatics.limits.flow_min;
		if((UI.nebuliser == 1)&&(Nebuliser_min_flow > app_Pneumatics.limits.flow_min)){
			UI.FLOW.limit_min	=  Nebuliser_min_flow;
		}
		printf("\r\nlimit_min =%d",UI.FLOW.limit_min);
		UI.FLOW.limit_max	=  app_Pneumatics.limits.flow_max;
		UI.FLOW.limit_min_pix	=  app_line_slider_val2pix( &UI.FLOW	,UI.FLOW.limit_min);
		UI.FLOW.limit_max_pix	=  app_line_slider_val2pix( &UI.FLOW	,UI.FLOW.limit_max);


		app_touchscreen_button_add(BUTTON_7,125+UI.FLOW.end_min_pix,  200-50,UI.FLOW.end_max_pix - UI.FLOW.end_min_pix, 150);

		//endstops
		LCD_DispMonoBitmapTrans(			125-10+UI.FLOW.limit_min_pix, 200+7, "IMG_EndStopL",COLOUR_BLACK);
		LCD_DispMonoBitmapTrans(			125   +UI.FLOW.limit_max_pix, 200+7, "IMG_EndStopR",COLOUR_BLACK);

		UI.FLOW.val_last	= uINIT_s16;

	}

	//auto return to main screen
	if(UI.page_timer_main_return == 1){
		UI.page_timer_main_return	=  0;
		UI.screen		=  UI_MAIN;
	}

	app_UI_screen_side_bar(BUILD_REFRESH);


	//curser on slider
	if(UI.FLOW.val_last != UI.FLOW.val){
		if(UI.FLOW.val_last != uINIT_s16){
			LCD_DispMonoBitmapTrans(		125+UI.FLOW.val_last_pix-11, 200+22, "IMG_SliderIconHandU",colour_mode[UI.select_mode]);
		}
		if(UI.FLOW.val < UI.FLOW.limit_min){	UI.FLOW.val =  UI.FLOW.limit_min;	}
		if(UI.FLOW.val > UI.FLOW.limit_max){	UI.FLOW.val =  UI.FLOW.limit_max;	}

		UI.FLOW.val_last_pix	=  app_line_slider_val2pix( &UI.FLOW	,UI.FLOW.val);
		LCD_DispMonoBitmapTrans(			125+UI.FLOW.val_last_pix-11, 200+22, "IMG_SliderIconHandU",COLOUR_BLACK);

		sprintf(str," %3d ",UI.FLOW.val);
		LCD_DispText_option(				560,  155, 85, 75,CENTER,COLOUR_BLACK,colour_mode[UI.select_mode], &fontArialNarrow50h, str);	//x,y,w,h,justification,col,font,txt
		UI.FLOW.val_last	=  UI.FLOW.val;	//save of var
	}

	//curser on slider
	if(UI.O2.val_last != UI.O2.val){
		if(UI.O2.val_last != uINIT_s16){
			LCD_DispMonoBitmapTrans(		110+UI.O2.val_last_pix-11, 380+22, "IMG_SliderIconHandU",colour_mode[UI.select_mode]);
		}
		if(UI.O2.val < UI.O2.limit_min){	UI.O2.val =  UI.O2.limit_min;	}
		if(UI.O2.val > UI.O2.limit_max){	UI.O2.val =  UI.O2.limit_max;	}
		UI.O2.val_last_pix	=  app_line_slider_val2pix( &UI.O2	,UI.O2.val);
		LCD_DispMonoBitmapTrans(			110+UI.O2.val_last_pix-11, 380+22, "IMG_SliderIconHandU",COLOUR_BLACK);

		sprintf(str," %3d ",UI.O2.val);
		LCD_DispText_option(				560,  335,85, 75,CENTER,COLOUR_BLACK,colour_mode[UI.select_mode], &fontArialNarrow50h, str);	//x,y,w,h,justification,col,font,txt
		UI.O2.val_last	=  UI.O2.val;	//save of var
	}


	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	switch(touch_status){
//		case BUTTON_TIME:
		case BUTTON_TIME_HOLD:
		case BUTTON_SIDE2:
		case BUTTON_SIDE2_HOLD:
			UI.screen			=  UI_SETTINGS;
			break;
//
//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:
//
		case BUTTON_SIDE1:
		case BUTTON_SIDE1_HOLD:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				UI.screen			=  UI_MODE_SETUP;
				
			}
			break;

//		case BUTTON_SIDE2:
//		case BUTTON_SIDE2_HOLD:
//			break;

		case BUTTON_SIDE3:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(UI.pneumatics_mode == 0){
			//	if(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_IDLE){
					//save the settings that have been chosen
					UI.flowRate	= UI.FLOW.val;
					app_pneumatic_set_flow(UI.flowRate, UI.O2.val);

					if( (UI.select_mode == PMODE_HFOT)||(UI.select_mode == PMODE_POINT) ){
						//no alarm settings in these modes
						//set the values to default for this mode
						UI.screen		=  UI_MAIN;

						UI.Pmin.en_dis			=  PP_settings.Patient_Pressure_min.en_dis;
						UI.Pmin.val				=  PP_settings.Patient_Pressure_min.value;
						UI.Pmax.en_dis			=  PP_settings.Patient_Pressure_max.en_dis;
						UI.Pmax.val				=  PP_settings.Patient_Pressure_max.value;
						UI.Apnoea.val			=  PP_settings.Apnoea_alarm_wait;
						UI.Breathing_Freq.val	=  PP_settings.Respiration_rate_max;
						app_PP_set_alarm_patient_pressure(	UI.Pmin.en_dis	,UI.Pmin.val,
															UI.Pmax.en_dis	,UI.Pmax.val	);
						app_PP_set_alarm_apnoea(			UI.Apnoea.val);
						app_PP_set_alarm_resperation_rate(	UI.Breathing_Freq.val);
					}
					else{
						UI.screen		=  UI_ALARM_SETUP;
					}
				}
				else{
					app_UI_screen_popup_setup(POPUP_CONFIRM_CHANGE_FLOW,UI_MAIN,UI_MAIN,POPUP_NOTIMEOUT);
				}
			}
			break;
	//	case BUTTON_SIDE3_HOLD:
	//		break;

		//************************************************
		//FLOW
		case BUTTON_6:
		case BUTTON_6_HOLD:
			if(UI.FLOW.val > UI.FLOW.limit_min){
				UI.FLOW.val	-= 1;
			}
			break;

		case BUTTON_7:		//Flow slider
		case BUTTON_7_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
				UI.FLOW.val	=  (uint16_t)app_line_slider_pix2val(&UI.FLOW, slider_info.x_ee_f);
			}
			break;
		case BUTTON_8:
		case BUTTON_8_HOLD:
			if(UI.FLOW.val < UI.FLOW.limit_max){
				UI.FLOW.val	+= 1;
			}
			break;
		//************************************************
		//O2
		case BUTTON_9:
		case BUTTON_9_HOLD:
			if(UI.O2.val > app_Pneumatics.O2_conc_min){
				UI.O2.val	-= 1;
			}
			break;
		case BUTTON_A:		//Flow slider
		case BUTTON_A_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
				UI.O2.val	=  (uint16_t)app_line_slider_pix2val(&UI.O2, slider_info.x_ee_f);
			}
			break;
		case BUTTON_B:
		case BUTTON_B_HOLD:
			if(UI.O2.val < app_Pneumatics.O2_conc_max){
				UI.O2.val	+= 1;
			}
			break;
		//************************************************
//		case BUTTON_C:
//			if(UI.touch_used == 0){
//				UI.touch_used	=  1;
//			}
//			break;
		//************************************************
		//confirm change
//		case BUTTON_D:
//			break;
//		case BUTTON_D_HOLD:
//			if(UI.touch_used == 0){
//				UI.touch_used	=  1;
//
//				//save the settings that have been chosen
//				UI.flowRate	= UI.FLOW.val;
//				app_pneumatic_set_flow(UI.FLOW.val, UI.O2.val);
//				UI.screen		=  UI_MAIN;
//			}
//			break;
//		case BUTTON_E:
//			break;
//		case BUTTON_E_HOLD:
//			if(UI.touch_used == 0){
//				UI.touch_used	=  1;
//				//return without save
//				UI.screen		=  UI_MAIN;
//			}
//			break;

//		case BUTTON_JUST_RELEASED:
		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			break;
	}

	return;
}

/**********************************************************************************************************
 * Function Name : app_UI_screen_alarm_setup
 * Description   : This function is used to set the desired flow and O2 coincentration
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		28/11/15	Tim Barr		Original Created
 **********************************************************************************************************/
void app_UI_screen_alarm_setup(uint8_t touch_status, uint8_t but_status, bool init)
{
	char				str[30];
	slider_t			slider_info;
	static uint8_t		rebuild_screen;

	static LCD_COLOR	pressure_colour;
	static LCD_COLOR	apnoea_colour;
	static LCD_COLOR	breathing_freq_colour;
	static uint8_t		last_pmax_image;
	static uint8_t		last_pmin_image;
	uint16_t			posx;


	if(init){
    	UI.screen			=  UI_ALARM_SETUP_WAIT;
    	UI.screen_last		=  UI_ALARM_SETUP;
    	app_selfcheck_AudioMute(0);
		UI.but_used		=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used	=  1;

		last_pmax_image	=  0;
		last_pmin_image	=  0;

		app_touchscreen_button_disable_all();

	//background
		LCD_FillRect(	0,	0,	650, LCD_DISP_HEIGHT,	colour_mode[UI.select_mode]);
		if(UI.demo_mode){
			LCD_DrawRect_n(1,	1,	650-2, LCD_DISP_HEIGHT-1-2, 4, COLOUR_RED);
		}
	//************************************************
	//Sidebar
		UI.sidebar.endis	=  1;
		if(UI.pneumatics_mode == 0){
			UI.sidebar.but1	=  BUT_MODE;
			UI.sidebar.but2	=  BUT_SETTINGS;
			UI.sidebar.but3	=  BUT_NEXT;
		}
		else{
			UI.sidebar.but1	=  BUT_BLANK;
			UI.sidebar.but2	=  BUT_BLANK;
			UI.sidebar.but3	=  BUT_NEXT;
		}
		app_UI_screen_side_bar(BUILD_ALL_NEW);
	//************************************************
	//Title
		switch(UI.select_mode){
			case PMODE_CPAP:			sprintf(str, "CPAP");			break;
			case PMODE_CPAP_PAED:		sprintf(str, "CPAP Paed");	break;
			case PMODE_CPAP_HELMET:		sprintf(str, "CPAP Helmet");	break;
			case PMODE_BUBBLE_PAP:		sprintf(str, "Bubble PAP");		break;
			case PMODE_HFOT:			sprintf(str, "HFOT");			break;
			case PMODE_POINT:			sprintf(str, "POINT");			break;
		}
		posx = LCD_DispText_option(	25,  	5, 600, 50,				BOTTOM_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h	, str);
		LCD_DispText_option(		posx,  	5, 600-25-posx, 47,		BOTTOM_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow37h	, LanguageStr(LangStr_AlarmSet));

	//************************************************

		if(app_Pneumatics.limits.pressure_alarm_avail){			pressure_colour	=  	COLOUR_BLACK;					}
		else{													pressure_colour	=  	colour_mode[UI.select_mode] & FILTER_RGB;	}
		//Text

		LCD_DispText_option(				 95,   65, 350, 40,		CENTER			,pressure_colour,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_CPAPPres));
		LCD_DispText_option(				  5,   80,  95, 35,		BOTTOM_CENTER	,pressure_colour,TRANSPARENT, &fontArialNarrowBold31h,  LanguageStr(LangStr_Pmax));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(				  5,  310,  95, 35,		TOP_CENTER		,pressure_colour,TRANSPARENT, &fontArialNarrowBold31h,  LanguageStr(LangStr_Pmin));	//x,y,w,h,justification,col,font,txt


		//- button
		LCD_FillSoftRect(					20, 112, 75, 75, 10, 	COLOUR_BUTTON_PAIR1);
		LCD_DrawSoftRect_1(					20, 112, 75, 75, 10, 	pressure_colour);
		app_touchscreen_button_add(BUTTON_6,20, 112, 75, 75);
		LCD_DispText_option(				20, 112, 75, 75,		CENTER,		pressure_colour,TRANSPARENT, &fontArialNarrow60h, "-");	//x,y,w,h,justification,col,font,txt

		//+ button
		LCD_FillSoftRect(					500,112, 75, 75, 10, 	COLOUR_BUTTON_PAIR1);
		LCD_DrawSoftRect_1(					500,112, 75, 75, 10, 	pressure_colour);
		app_touchscreen_button_add(BUTTON_8,500,112, 75, 75);
		LCD_DispText_option(				500,112, 75, 75,		CENTER,		pressure_colour,TRANSPARENT, &fontArialNarrow60h, "+");	//x,y,w,h,justification,col,font,txt

		//- button
		LCD_FillSoftRect(					20,  235, 75, 75, 10, 	COLOUR_BUTTON_PAIR2);
		LCD_DrawSoftRect_1(					20,  235, 75, 75, 10, 	pressure_colour);
		app_touchscreen_button_add(BUTTON_9,20,  235, 75, 75);
		LCD_DispText_option(				20,  235, 75, 75,		CENTER,		pressure_colour,TRANSPARENT, &fontArialNarrow60h, "-");	//x,y,w,h,justification,col,font,txt

		//+ button
		LCD_FillSoftRect(					500, 235, 75, 75, 10, 	COLOUR_BUTTON_PAIR2);
		LCD_DrawSoftRect_1(					500, 235, 75, 75, 10, 	pressure_colour);
		app_touchscreen_button_add(BUTTON_B,500, 235, 75, 75);
		LCD_DispText_option(				500, 235, 75, 75,		CENTER,		pressure_colour,TRANSPARENT, &fontArialNarrow60h, "+");	//x,y,w,h,justification,col,font,txt

		//slider
		UI.Pmin.val		=  PP_settings.Patient_Pressure_min.value;
		UI.Pmin.en_dis	=  PP_settings.Patient_Pressure_min.en_dis;
		UI.Pmin.val_last		=  0xffff;

		UI.Pmax.val		=  PP_settings.Patient_Pressure_max.value;
		UI.Pmax.en_dis	=  PP_settings.Patient_Pressure_max.en_dis;
		UI.Pmax.val_last		=  0xffff;

		if(app_Pneumatics.limits.pressure_alarm_avail == 0){
			app_touchscreen_button_disable(BUTTON_6);
			app_touchscreen_button_disable(BUTTON_7);
			app_touchscreen_button_disable(BUTTON_8);
			app_touchscreen_button_disable(BUTTON_9);
			app_touchscreen_button_disable(BUTTON_A);
			app_touchscreen_button_disable(BUTTON_B);
		}

		//draw lines
		LCD_FillRect(20, 340, 610, 3, COLOUR_BLACK);
        LCD_FillRect(325, 350, 3, 120, COLOUR_BLACK);

		//Apnoea
		UI.Apnoea.val			=  PP_settings.Apnoea_alarm_wait;
		UI.Apnoea.val_last		=  uINIT_s16;
		UI.Apnoea.limit_min	=  app_Pneumatics.limits.apnoea_alarm_min;
		UI.Apnoea.limit_max	=  app_Pneumatics.limits.apnoea_alarm_max;
		if(app_Pneumatics.limits.apnoea_alarm_avail){	apnoea_colour	=  COLOUR_BLACK;				}
		else{											apnoea_colour	=  colour_mode[UI.select_mode] & FILTER_RGB;	}
		LCD_DispText_option(				 20, 345,270, 40,		CENTER		,apnoea_colour,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_APNOEA));

		//- button
		LCD_FillSoftRect(					 20, 390, 75, 75, 10, 	COLOUR_BUTTON_PAIR2);
		LCD_DrawSoftRect_1(					 20, 390, 75, 75, 10, 	apnoea_colour);
		app_touchscreen_button_add(BUTTON_C, 20, 390, 75, 75);
		LCD_DispText_option(				 20, 390, 75, 75,		CENTER,		apnoea_colour,TRANSPARENT, &fontArialNarrow60h, "-");	//x,y,w,h,justification,col,font,txt

		//+ button
		LCD_FillSoftRect(					215, 390, 75, 75, 10, 	COLOUR_BUTTON_PAIR2);
		LCD_DrawSoftRect_1(					215, 390, 75, 75, 10, 	apnoea_colour);
		app_touchscreen_button_add(BUTTON_D,215, 390, 75, 75);
		LCD_DispText_option(				215, 390, 75, 75,		CENTER,		apnoea_colour,TRANSPARENT, &fontArialNarrow60h, "+");	//x,y,w,h,justification,col,font,txt

		if(app_Pneumatics.limits.apnoea_alarm_avail == 0){
			app_touchscreen_button_disable(BUTTON_C);
			app_touchscreen_button_disable(BUTTON_D);
		}

		//Breathing frequency
		UI.Breathing_Freq.val			=  PP_settings.Respiration_rate_max;
		UI.Breathing_Freq.val_last		=  uINIT_s16;
		UI.Breathing_Freq.limit_min		=  5;
		UI.Breathing_Freq.limit_max		=  60;
		if(app_Pneumatics.limits.breath_freq_measured){	breathing_freq_colour	=  COLOUR_BLACK;				}
		else{											breathing_freq_colour	=  colour_mode[UI.select_mode] & FILTER_RGB;	}
		LCD_DispText_option(				360, 345,270, 40,		CENTER		,breathing_freq_colour,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_F_max));

		//- button
		LCD_FillSoftRect(					360, 390, 75, 75, 10, 	COLOUR_BUTTON_PAIR1);
		LCD_DrawSoftRect_1(					360, 390, 75, 75, 10, 	breathing_freq_colour);
		app_touchscreen_button_add(BUTTON_E,360, 390, 75, 75);
		LCD_DispText_option(				360, 390, 75, 75,		CENTER,		breathing_freq_colour,TRANSPARENT, &fontArialNarrow60h, "-");	//x,y,w,h,justification,col,font,txt

		//+ button
		LCD_FillSoftRect(					555, 390, 75, 75, 10, 	COLOUR_BUTTON_PAIR1);
		LCD_DrawSoftRect_1(					555, 390, 75, 75, 10, 	breathing_freq_colour);
		app_touchscreen_button_add(BUTTON_F,555, 390, 75, 75);
		LCD_DispText_option(				555, 390, 75, 75,		CENTER,		breathing_freq_colour,TRANSPARENT, &fontArialNarrow60h, "+");	//x,y,w,h,justification,col,font,txt

		if(app_Pneumatics.limits.breath_freq_measured == 0){
			app_touchscreen_button_disable(BUTTON_E);
			app_touchscreen_button_disable(BUTTON_F);
		}

		rebuild_screen	=  1;
	}//init

	if(rebuild_screen){
		rebuild_screen	=  0;

		app_line_slider_wOFF_draw(				125				, 175   ,350,0,25,5,pressure_colour,DISPLAY_NUMBERS_BELOW,&UI.Pmax);		//x,y,w,minx,max,x,spacing
		UI.Pmax.limit_min	=  UI.Pmin.val;
		UI.Pmax.limit_max	=  app_Pneumatics.limits.pressure_alarm_max;
		UI.Pmax.limit_min_pix	=  app_line_slider_val2pix( &UI.Pmax	,UI.Pmax.limit_min);
		UI.Pmax.limit_max_pix	=  app_line_slider_val2pix( &UI.Pmax	,UI.Pmax.limit_max);

		app_line_slider_wOFF_draw(				125				, 175+35+30,350,0,25,5,pressure_colour,DISPLAY_LINE_ONLY,&UI.Pmin);	//x,y,w,minx,max,x,spacing
		UI.Pmin.limit_min	=  app_Pneumatics.limits.pressure_alarm_min;
		if(UI.Pmax.en_dis == 1){	UI.Pmin.limit_max	=  UI.Pmax.val;	}
		else{						UI.Pmin.limit_max	=  UI.Pmax.end_max - 3;	}
		UI.Pmin.limit_min_pix	=  app_line_slider_val2pix( &UI.Pmin	,UI.Pmin.limit_min);
		UI.Pmin.limit_max_pix	=  app_line_slider_val2pix( &UI.Pmin	,UI.Pmin.limit_max);

		if(UI.Pmax.val	< UI.Pmax.end_min){	UI.Pmax.val	= UI.Pmax.end_min;	}
		if(UI.Pmax.val	> UI.Pmax.end_max){	UI.Pmax.val	= UI.Pmax.end_min;	}	//not a mistake prob >end_max because we want it to be in the off state

		if(UI.Pmin.val	< UI.Pmin.end_min){	UI.Pmin.val	= UI.Pmin.end_min;	}
		if(UI.Pmin.val	> UI.Pmin.end_max){	UI.Pmin.val	= UI.Pmin.end_max;	}


		app_touchscreen_button_add(BUTTON_7,	125+UI.Pmax.end_min_pix, 175-90, UI.Pmax.end_max_pix - UI.Pmax.end_min_pix, 100);
	//	LCD_FillRect(							125+UI.Pmax.limit_min_pix, 175-90, UI.Pmax.limit_max_pix - UI.Pmax.limit_min_pix, 100,	colour_mode[UI.select_mode] & FILTER_RGB);
		app_touchscreen_button_add(BUTTON_A,	125+UI.Pmin.end_min_pix, 175+25+30, UI.Pmin.end_max_pix - UI.Pmin.end_min_pix, 100);
	//	LCD_FillRect(							125+UI.Pmin.limit_min_pix, 175+25, UI.Pmin.limit_max_pix - UI.Pmin.limit_min_pix, 100,	colour_mode[UI.select_mode] & FILTER_RGB);

		//endstops
		LCD_DispMonoBitmapTrans(			125   +UI.Pmax.limit_max_pix	,175-17			,"IMG_EndStopR",pressure_colour);
		LCD_DispMonoBitmapTrans(			125-10+UI.Pmin.limit_min_pix+2	,175+35+5+30	,"IMG_EndStopL",pressure_colour);


//		LCD_DispMonoBitmapTrans(			125-10+UI.Pmin.limit_min_pix	,175-17		,"IMG_EndStopL",pressure_colour);
//		LCD_DispMonoBitmapTrans(			125   +UI.Pmax.limit_max_pix+2	,175+35+5	,"IMG_EndStopR",pressure_colour);

	}

	//auto return to main screen
	if(UI.page_timer_main_return == 1){
		UI.page_timer_main_return	=  0;
		UI.screen		=  UI_MAIN;
	}

	app_UI_screen_side_bar(BUILD_REFRESH);

	//curser on slider
	if(UI.Pmax.val_last != UI.Pmax.val){

		if(UI.Pmax.val < 0){	UI.Pmax.en_dis	=  DIS;	}
		else{					UI.Pmax.en_dis	=  EN;	}

		if(UI.Pmax.val_last != uINIT_s16){
			LCD_DispMonoBitmapTrans(		125+UI.Pmax.val_last_pix-22, 175-50-22, "IMG_SliderIconHandD",colour_mode[UI.select_mode]);	//-13= image width, -50 = image hight
		}
		UI.Pmax.val_last_pix	=  app_line_slider_val2pix( &UI.Pmax	,UI.Pmax.val);
		LCD_DispMonoBitmapTrans(			125+UI.Pmax.val_last_pix-22, 175-50-22, "IMG_SliderIconHandD",pressure_colour);	//-13= image width, -50 = image hight

	//	if((UI.Pmax.val >= UI.Pmax.limit_min)&&(UI.Pmax.val >= 0)){
		if(UI.Pmax.en_dis	==  EN){
			if(last_pmax_image != 1){
				last_pmax_image	=  1;
				LCD_FillRect(580+10, 112+15, 45,45, colour_mode[UI.select_mode]);
			}
			sprintf(str, " %2d ", UI.Pmax.val);
			LCD_DispText_option(			580, 112, 70, 75,CENTER,pressure_colour,colour_mode[UI.select_mode], &fontArialNarrow50h, str);
		}
		else{
			if(last_pmax_image != 2){
				last_pmax_image	=  2;
				LCD_FillRect(580+10, 112+15, 45,45, colour_mode[UI.select_mode]);
			}
			LCD_DispMonoBitmapTrans(		580+10, 112+15, "IMG_AlarmSilence",COLOUR_BLACK);
		}

		UI.Pmax.val_last	=  UI.Pmax.val;	//save of var
	}
	//curser on slider
	if(UI.Pmin.val_last != UI.Pmin.val){

		if(UI.Pmin.val < 0){	UI.Pmin.en_dis	=  DIS;	}
		else{					UI.Pmin.en_dis	=  EN;	}

		if(UI.Pmin.val_last != uINIT_s16){
			LCD_DispMonoBitmapTrans(		125+UI.Pmin.val_last_pix-11, 175+35+22+30, "IMG_SliderIconHandU",colour_mode[UI.select_mode]);	//-13= image width, -50 = image hight
		}
		UI.Pmin.val_last_pix	=  app_line_slider_val2pix( &UI.Pmin	,UI.Pmin.val);
		LCD_DispMonoBitmapTrans(			125+UI.Pmin.val_last_pix-11, 175+35+22+30, "IMG_SliderIconHandU",pressure_colour);	//-13= image width, -50 = image hight

	//	if(UI.Pmin.val >= UI.Pmin.limit_min){
		if(UI.Pmin.en_dis	==  EN){
			if(last_pmin_image != 1){
				last_pmin_image	=  1;
				LCD_FillRect(580+10, 235+15, 45,45, colour_mode[UI.select_mode]);
			}
			sprintf(str, " %2d ", UI.Pmin.val);
			LCD_DispText_option(				580, 235, 70,  75,CENTER,pressure_colour,colour_mode[UI.select_mode], &fontArialNarrow50h, str);
		}
		else{
			if(last_pmin_image != 2){
				last_pmin_image	=  2;
				LCD_FillRect(580+10, 235+15, 45,45, colour_mode[UI.select_mode]);
			}
			LCD_DispMonoBitmapTrans(		580+10, 235+15, "IMG_AlarmSilence",COLOUR_BLACK);
		}

		UI.Pmin.val_last	=  UI.Pmin.val;	//save of var
	}



	if(UI.Apnoea.val_last != UI.Apnoea.val){
		if(UI.Apnoea.val < UI.Apnoea.limit_min){	UI.Apnoea.val	=  UI.Apnoea.limit_min;	}
		if(UI.Apnoea.val > UI.Apnoea.limit_max){	UI.Apnoea.val	=  UI.Apnoea.limit_max;	}

		UI.Apnoea.val_last		= UI.Apnoea.val;
		if(app_Pneumatics.limits.apnoea_alarm_avail	!= 0){
			sprintf(str,"%d s",UI.Apnoea.val);
			LCD_FillRect(				96,  390, 118, 75,colour_mode[UI.select_mode]);
			LCD_DispText_option(		96,  390, 118, 75,CENTER,apnoea_colour,colour_mode[UI.select_mode], &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
		}
	}
	if(UI.Breathing_Freq.val_last != UI.Breathing_Freq.val){
		UI.Breathing_Freq.val_last		= UI.Breathing_Freq.val;
	//	if(app_Pneumatics.limits.breath_freq_measured == 0){		sprintf(str,LanguageStr(LangStr_Disabled));					}
		if(app_Pneumatics.limits.breath_freq_measured == 0){		sprintf(str,"-");											}
		else if(UI.Breathing_Freq.val != 0){						sprintf(str,"%d %s",UI.Breathing_Freq.val, LanguageStr(LangStr_perMin));	}
		else{													sprintf(str,LanguageStr(LangStr_Off));							}
		LCD_FillRect(					436,  390, 118, 75,	colour_mode[UI.select_mode]);
		LCD_DispText_option(			436,  390, 118, 75,CENTER,breathing_freq_colour,colour_mode[UI.select_mode], &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
	}

	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	switch(touch_status){
//		case BUTTON_TIME:
		case BUTTON_TIME_HOLD:
		case BUTTON_SIDE2:
		case BUTTON_SIDE2_HOLD:
			UI.screen			=  UI_SETTINGS;
			break;
//
//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:
//
		case BUTTON_SIDE1:
		case BUTTON_SIDE1_HOLD:
			if(UI.pneumatics_mode == 0){
		//	if(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_IDLE){
				if(UI.touch_used == 0){
					UI.touch_used	=  1;
					UI.screen			=  UI_MODE_SETUP;
			
				}
			}
			break;
//		case BUTTON_SIDE2:
//		case BUTTON_SIDE2_HOLD:
//			if(UI.touch_used == 0){
//				UI.touch_used	=  1;
//				UI.screen			=  UI_FLOW_SETUP;
//			}
//			break;

		case BUTTON_SIDE3:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;


				//save the settings that have been chosen
				if(UI.pneumatics_mode == 0){
			//	if(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_IDLE){
					//save the settings that have been chosen

					app_PP_set_alarm_patient_pressure(	UI.Pmin.en_dis	,UI.Pmin.val,
														UI.Pmax.en_dis	,UI.Pmax.val	);
					app_PP_set_alarm_apnoea(			UI.Apnoea.val);
					app_PP_set_alarm_resperation_rate(	UI.Breathing_Freq.val);
					UI.screen		=  UI_MAIN;
				}
				else{
					app_UI_screen_popup_setup(POPUP_CONFIRM_CHANGE_ALARMS,UI_MAIN,UI_MAIN,POPUP_NOTIMEOUT);
				}

			}
			break;
	//	case BUTTON_SIDE3_HOLD:
	//		break;
		//************************************************
		//MAX
		case BUTTON_6:
		case BUTTON_6_HOLD:
			if(UI.Pmax.val > UI.Pmin.limit_min+3){
				if(UI.Pmax.val > UI.Pmin.val+3){				UI.Pmax.val	-= 1;	}	//move down
				else if(UI.Pmin.val > UI.Pmin.limit_min){		UI.Pmax.val	-= 1;
																UI.Pmin.val	-= 1;	}	//push Pmin down too
			}
			else{												UI.Pmax.val	=  UI.Pmax.end_min;	}	//go to Off position
			break;
		case BUTTON_7:		//Flow slider
		case BUTTON_7_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
				UI.Pmax.val	=  (uint16_t)app_line_slider_pix2val(&UI.Pmax, slider_info.x_ee_f);

				if( UI.Pmax.val < UI.Pmax.limit_min){			UI.Pmax.val	=  UI.Pmax.end_min;		}	//go to Off position
				else if( UI.Pmax.val > UI.Pmax.limit_max){		UI.Pmax.val	=  UI.Pmax.limit_max;		}	//go to max
				else if( UI.Pmax.val < UI.Pmax.limit_min){		UI.Pmax.val	=  UI.Pmax.limit_min;	}	//go to min value
				else if(UI.Pmax.val < UI.Pmin.val+3){													//below Pmin value push pmin value
				  	if(	UI.Pmax.val - 3 < UI.Pmin.limit_min){	UI.Pmin.val	=  UI.Pmin.limit_min;			//as fas as pmax limit
																UI.Pmax.val	=  UI.Pmin.limit_min + 3;	}
					else{										UI.Pmin.val	=  UI.Pmax.val - 3;			}	//not as afar as limit
				}
			}
			break;
		case BUTTON_8:
		case BUTTON_8_HOLD:
			if(UI.Pmax.val < UI.Pmax.limit_max){		UI.Pmax.val	+= 1;					}
			if(UI.Pmax.val < UI.Pmin.limit_min+3){
			  	if(UI.Pmin.val != UI.Pmin.end_min){
					UI.Pmax.val	=  UI.Pmin.val + 3;
					if(UI.Pmax.val > UI.Pmax.limit_max){
						UI.Pmax.val =  UI.Pmax.limit_max;
						UI.Pmin.val	=  UI.Pmax.val - 3;
					}
				}
				else{									UI.Pmax.val	=  UI.Pmin.limit_min + 3;	}
			}
			break;

	
		//************************************************
		//MIN
		case BUTTON_9:
		case BUTTON_9_HOLD:
			if(UI.Pmin.val > UI.Pmin.limit_min){		UI.Pmin.val	-= 1;				}
			else{										UI.Pmin.val	=  UI.Pmin.end_min;	}
			break;

		case BUTTON_A:		//Flow slider
		case BUTTON_A_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
				UI.Pmin.val	=  (uint16_t)app_line_slider_pix2val(&UI.Pmin, slider_info.x_ee_f);

				if(UI.Pmin.val < UI.Pmin.limit_min-2){			UI.Pmin.val	=  UI.Pmin.end_min;		}	//go to Off position
				else if(UI.Pmin.val < UI.Pmin.limit_min){		UI.Pmin.val	=  UI.Pmin.limit_min;		}	//go to min value
				else if(UI.Pmin.val >= UI.Pmax.limit_max-3){	UI.Pmin.val	=  UI.Pmax.limit_max-3;	}	//go to max value

				if(UI.Pmin.val > UI.Pmax.val-3){														//above Pmax value push pmax value
					if(	UI.Pmin.val + 3 > UI.Pmax.limit_max){
					  	if(UI.Pmax.val != UI.Pmax.end_min){		UI.Pmax.val	=  UI.Pmax.limit_max;}			//as fas as pmax limit
																UI.Pmin.val	=  UI.Pmax.val - 3;
					}
					else if(UI.Pmax.val != UI.Pmax.end_min){	UI.Pmax.val	=  UI.Pmin.val + 3;		}	//not as afar as limit
				}
			}
			break;
		case BUTTON_B:
		case BUTTON_B_HOLD:
			if(UI.Pmin.val < UI.Pmin.limit_min){			UI.Pmin.val 	=  UI.Pmin.limit_min;	}
			else if(UI.Pmin.val >= UI.Pmax.limit_max-3){	UI.Pmin.val 	=  UI.Pmax.limit_max-3;	}
			else if(UI.Pmin.val < UI.Pmax.val-3){			UI.Pmin.val	+= 1;	}
			else if(UI.Pmax.val < UI.Pmax.limit_max){
					 								 		UI.Pmin.val	+= 1;
				if(UI.Pmax.val != UI.Pmax.end_min){			UI.Pmax.val	+= 1;	}
			}
			break;

		//************************************************
		case BUTTON_C:
		case BUTTON_C_HOLD:
//			if(UI.touch_used == 0){
//  				UI.touch_used	=  1;
				if(UI.Apnoea.val > UI.Apnoea.limit_min){		UI.Apnoea.val	-= 1;	}
//			}
			break;

		case BUTTON_D:
		case BUTTON_D_HOLD:
//		  	if(UI.touch_used == 0){
//  				UI.touch_used	=  1;
				if(UI.Apnoea.val < UI.Apnoea.limit_max){		UI.Apnoea.val += 1;		}
//			}
			break;

		case BUTTON_E:
		case BUTTON_E_HOLD:
//		  	if(UI.touch_used == 0){
//  				UI.touch_used	=  1;
				if(UI.Breathing_Freq.val == UI.Breathing_Freq.limit_min){			UI.Breathing_Freq.val =  0;	}
				else if(UI.Breathing_Freq.val > UI.Breathing_Freq.limit_min){		UI.Breathing_Freq.val -= 5;	}
//			}
			break;

		case BUTTON_F:
		case BUTTON_F_HOLD:
//			if(UI.touch_used == 0){
//  				UI.touch_used	=  1;
				if(UI.Breathing_Freq.val == 0){										UI.Breathing_Freq.val =  UI.Breathing_Freq.limit_min;	}
				else if(UI.Breathing_Freq.val < UI.Breathing_Freq.limit_max){		UI.Breathing_Freq.val += 5;	}
//			}
			break;

		case BUTTON_JUST_RELEASED:
		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			break;
	}

	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_screen_main
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		07/12/17	W. Paul			Created
* 0.1.1         12/06/26        A. Kane                 Updated to call correct alarm colour
*************************************************************************************************/
void 	app_UI_screen_main(			uint8_t touch_status, uint8_t but_status, bool init)
{
	char						str[50];
	uint8_t						hr,min,sec;
	uint16_t					err_str_len;
	uint16_t					day;
	uint16_t					x,last_pix,new_pix;
	static uint8_t				nebuliser_status_last;
	static uint8_t				breathingRate_last;
	static int16_t				patientPressure_last;
	static uint8_t				PP_negflag;
	static uint16_t				y_val_min_alarm;
	static uint16_t				y_val_max_alarm;
	static OVERRIDE_enum		override_last;
	static uint8_t				display_flow_target;
	static uint8_t				display_O2_conc_target;
	static uint8_t				breathingRate_detect_flag_last;
	static uint8_t				breathingRate_event_settle_status_last;
	static uint8_t				breathingRate_no_breath_det_last;

	uint16_t					y_val1,y_val2;
	LCD_COLOR					fg_endis_colour;
	LCD_COLOR					graph_endis_colour;
	LCD_COLOR					pix_colour;
	static LCD_COLOR			fault_background_colour;
	static Removable_button_st	err_message_but;
	static uint16_t				graph_lines[8];
	uint8_t						i;
	uint8_t						j;
	uint32_t					rd_alarm_tmr;

	uint8_t						fault_status;
	static uint8_t				fault_status_last;

	uint8_t						fault_no;
	static uint8_t				last_fault_no;
	static uint8_t				next_fault_tog	= 0;
	char* 						lead_str[2];
	char* 						res_str;
	uint8_t 					res;
	static uint32_t				flasher_tmr;
    uint8_t             		new_build    =  0;
	float						tempf;
	static uint8_t				last_day	=  0xff;
	static uint8_t				last_hr		=  0xff;
	static uint8_t				hold_cnt	=  0;
	
	// Code
	if((api_io_rd_AirSupply(0)	== 0) && (api_io_rd_O2Supply(0)	== 0)){
	}
	else if((api_io_rd_AirSupply(0)	== 0) && (app_Pneumatics.O2_conc_target < app_Pneumatics.O2_conc_max)){
		if(gas_debounce_timer	== 0){
			UI.O2.val	=  app_Pneumatics.O2_conc_max;
			app_pneumatic_set_flow(app_Pneumatics.flow_rate_target, app_Pneumatics.O2_conc_max);
			init	=  1;
		}
	}
	else if((api_io_rd_O2Supply(0)	== 0) && (app_Pneumatics.O2_conc_target > app_Pneumatics.O2_conc_min)){
		if(gas_debounce_timer	== 0){
			UI.O2.val	=  app_Pneumatics.O2_conc_min;
			app_pneumatic_set_flow(app_Pneumatics.flow_rate_target, app_Pneumatics.O2_conc_min);
			init	=  1;
		}
	}
	else{
		gas_debounce_timer	=  2000;
	}
	
	if(init){
		UI.screen			=  UI_MAIN_WAIT;
		UI.screen_last		=  UI_MAIN;
		app_selfcheck_AudioMute(0);
		UI.but_used			=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used		=  1;

		UI.nebuliser			=  app_Pneumatics.nebuliser_state;
		nebuliser_status_last	=  0xff;
		breathingRate_last		=  0xff;
		patientPressure_last	=  260;
		PP_negflag				=  2;
		err_message_but.mode 	=  BUTTON_OFF;
		last_fault_no			=  0xff;
		fault_status_last		=  0xff;
		fault_background_colour	=  0;
		flasher_tmr				=  0;
		display_flow_target		=  0xff;
		display_O2_conc_target	=  0xff;
		override_last			=  OVERRIDE_UNKNOWN;
		last_day				=  0xff;
		last_hr					=  0xff;
		breathingRate_detect_flag_last			=  0;
		breathingRate_event_settle_status_last	=  0xff;
		breathingRate_no_breath_det_last		=  0xff;
		app_touchscreen_button_disable_all();

	//background
		LCD_FillRect(	0,	0,	650, LCD_DISP_HEIGHT-1,	colour_mode[UI.select_mode]);
		if(UI.demo_mode){
			LCD_DrawRect_n(1,	1,	650-2, LCD_DISP_HEIGHT-1-2, 4, COLOUR_RED);
		}
	//************************************************
	//Sidebar
		if(UI.pneumatics_mode == 0){
			UI.sidebar.but1	=  BUT_MODE;
			UI.sidebar.but2	=  BUT_SETTINGS;
			UI.sidebar.but3	=  BUT_START;

		}
		else{
			UI.sidebar.but1	=  BUT_BLANK;
			UI.sidebar.but2	=  BUT_SETTINGS;
			UI.sidebar.but3	=  BUT_STOP;

		}

		app_UI_screen_side_bar(BUILD_ALL_NEW);

	//************************************************
	//Title
		switch(UI.select_mode){
			case PMODE_CPAP:			sprintf(str, "CPAP");			break;
			case PMODE_CPAP_PAED:		sprintf(str, "CPAP Paed");	break;
			case PMODE_CPAP_HELMET:		sprintf(str, "CPAP Helmet");	break;
			case PMODE_BUBBLE_PAP:		sprintf(str, "Bubble PAP");		break;
			case PMODE_HFOT:			sprintf(str, "HFOT");			break;
			case PMODE_POINT:			sprintf(str, "POINT");			break;
		}

		LCD_DispText_option(	25,  5, 600, 50,		BOTTOM_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h, str);



	//************************************************

	//buttons - all moved down by 90 from 180 to 270 (10 above screen bottom)
		LCD_FillRect(				 10, 60     ,200,200,COLOUR_WHITE);			//patient 						//id,x,y,w,h
		LCD_DrawRect_1(				 10, 60     ,200,200,COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_6,10, 60     ,200,200);
		app_touchscreen_button_disable(BUTTON_6);	//disable button
		app_UI_button(	BUTTON_7,	 10,270     ,200,200,COLOUR_BUTTON_BACK);	//flow settings					//id,x,y,w,h
		app_UI_button(	BUTTON_8,	225,270     ,200, 95,COLOUR_BUTTON_BACK);	//timer							//id,x,y,w,h
		app_UI_button(	BUTTON_9,	225,270+105 , 95, 95,COLOUR_BUTTON_BACK);	//nebuliser						//id,x,y,w,h
		app_UI_button(	BUTTON_A,	330,270+105 , 95, 95,COLOUR_BUTTON_BACK);	//alarm settings				//id,x,y,w,h

		if( (UI.select_mode == PMODE_HFOT)||(UI.select_mode == PMODE_POINT) ){
			fg_endis_colour		=  COLOUR_GRAY_80;
			graph_endis_colour	=  COLOUR_GREY_20;
			//uninitialised buttons
			app_touchscreen_button_disable(	BUTTON_A);
		}
		else{
			fg_endis_colour		=  COLOUR_BLACK;
			graph_endis_colour	=  COLOUR_WHITE;
		}


	//icon images
		LCD_DispMonoBitmapTrans(	 10+125,	65,  	"IMG_Smell50"			,fg_endis_colour);
		LCD_DispMonoBitmapTrans(	 10+125,	275, 	"IMG_Adjust50"			,COLOUR_BLACK);
		LCD_DispMonoBitmapTrans(		380,	292, 	"IMG_Hourglass50"		,COLOUR_BLACK);
		LCD_DispMonoBitmapTrans(		330+8,	375+10,	"IMG_Alarmbell" 		,fg_endis_colour);

		LCD_DispText_option(	10+135	,60 + 60	,65,60		,BOTTOM_LEFT,fg_endis_colour,COLOUR_WHITE, &fontArial22h, LanguageStr(LangStr_CmH20));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(	10+  1	,60 + 60	,134,65		,BOTTOM_RIGHT,fg_endis_colour,COLOUR_WHITE, &fontArialNarrow60h, "0.0");	//x,y,w,h,justification,col,font,txt

		if(UI.select_mode	== PMODE_BUBBLE_PAP){
			LCD_DispText_option(10+135	,60 +130	,65,60		,BOTTOM_LEFT,COLOUR_GRAY_80,COLOUR_WHITE, &fontArial22h, LanguageStr(LangStr_RR));	//x,y,w,h,justification,col,font,txt
			LCD_DispText_option(10+  1	,60 +130	,134,65		,BOTTOM_RIGHT,COLOUR_GRAY_80,COLOUR_WHITE, &fontArialNarrow60h, "0");	//x,y,w,h,justification,col,font,txt
		}
		else{
			LCD_DispText_option(10+135	,60 +130	,65,60		,BOTTOM_LEFT,fg_endis_colour,COLOUR_WHITE, &fontArial22h, LanguageStr(LangStr_RR));	//x,y,w,h,justification,col,font,txt
			LCD_DispText_option(10+  1	,60 +130	,134,65		,BOTTOM_RIGHT,fg_endis_colour,COLOUR_WHITE, &fontArialNarrow60h, "0");	//x,y,w,h,justification,col,font,txt
		}

		LCD_DispText_option(	10+135	,270 + 60	,65,60		,BOTTOM_LEFT,COLOUR_BLACK,COLOUR_BUTTON_BACK, &fontArial22h, LanguageStr(LangStr_Lmin));	//x,y,w,h,justification,col,font,txt
		sprintf(str,"%% O ");
		LCD_DispText_option(	10+135	,270 +130	,65,60		,BOTTOM_LEFT,COLOUR_BLACK,COLOUR_BUTTON_BACK, &fontArial22h, str);	//x,y,w,h,justification,col,font,txt



	//graph area

//		LCD_DrawRect_1(	224,	 59,417,202, 	COLOUR_WHITE);	//box edge
		LCD_FillRect(	225,	 60, 30,200,	COLOUR_BLACK);	//left text area
		LCD_FillRect(	225+385, 60, 30,200,	COLOUR_BLACK);	//right text area

		app_PP_data_init(0		,200-20, PP_MAX_Pressure,  PP_MIN_Pressure);	//mode,pix range,max,min
		if(PP_settings.Patient_Pressure_min.en_dis == EN){		tempf	=  PP_settings.Patient_Pressure_min.value;	}
		else{													tempf	=  PP_MIN_Pressure;							}
		y_val_min_alarm	=  app_PP_data_graph_scale( tempf);

		if(PP_settings.Patient_Pressure_max.en_dis == EN){		tempf	=  PP_settings.Patient_Pressure_max.value;	}
		else{													tempf	=  PP_MAX_Pressure;							}
		y_val_max_alarm	=  app_PP_data_graph_scale( tempf);


	//	LCD_DrawRect_1(	 	 224+30	,  59						,355+2	,202								,COLOUR_WHITE);		//graph surround
		LCD_DrawVHLine(225+30-1			, 60, 200, LCD_VERT_LINE, graph_endis_colour);
		LCD_DrawVHLine(225+30+PP_HIS_LEN, 60, 200, LCD_VERT_LINE, graph_endis_colour);

		LCD_FillRect(		 225+30	,  60 						,355	,200								,COLOUR_BLACK);		//graph area
		if(y_val_max_alarm != y_val_min_alarm){
			LCD_FillRect(	 225+30	,  60+180+10-y_val_max_alarm	,355		,y_val_max_alarm-y_val_min_alarm+1	,graph_endis_colour);		//no alarm area
		}
		//draw graph lines and x axis
		for(i=0;i<=7;i++){
			graph_lines[i]	=  app_PP_data_graph_scale( (float)(5*(i-2)) );
			if((graph_lines[i] < y_val_min_alarm)||(graph_lines[i] > y_val_max_alarm)){	pix_colour	=  COLOUR_GREY_3C;	}
			else{																		pix_colour=  COLOUR_GRAY_80;	}	//inside max and min
            LCD_DrawVHLine(225+30, 59+180+10-graph_lines[i], PP_HIS_LEN, LCD_HORIZ_LINE, pix_colour);
			LCD_DrawVHLine(225+30, 60+180+10-graph_lines[i], PP_HIS_LEN, LCD_HORIZ_LINE, pix_colour);

			sprintf(str,"%d",5*(i-2));
			LCD_DispText_option(225,	60+180+10-graph_lines[i]-10,30,20			,BOTTOM_CENTER,graph_endis_colour,TRANSPARENT, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		}

		//min & max text on right axis
		if(y_val_min_alarm != y_val_max_alarm){
			y_val1	=  60+180+10-y_val_min_alarm -10;
			y_val2	=  60+180+10-y_val_max_alarm -10;
//			xoffset		=  0;
//			if(y_val1 - y_val2 < 10){			xoffset	= 10;		}

			if(PP_settings.Patient_Pressure_min.en_dis == EN){		sprintf(str,"%d",PP_settings.Patient_Pressure_min.value);	}
			else{													sprintf(str,"%d",PP_MIN_Pressure);					}
			LCD_DispText_option(225+30+PP_HIS_LEN+1	,y_val1	,30,20			,BOTTOM_CENTER,graph_endis_colour,TRANSPARENT, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt

			if(PP_settings.Patient_Pressure_max.en_dis == EN){		sprintf(str,"%d",PP_settings.Patient_Pressure_max.value);	}
			else{													sprintf(str,"%d",PP_MAX_Pressure);					}
			LCD_DispText_option(225+30+PP_HIS_LEN+1	,y_val2	,30,20			,BOTTOM_CENTER,graph_endis_colour,TRANSPARENT, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		}

        new_build   =  1;
	}


	if(display_flow_target != app_Pneumatics.flow_rate_target){
		display_flow_target	=  app_Pneumatics.flow_rate_target;
		LCD_FillRect(			10+1,		270 + 60,134,65		,COLOUR_BUTTON_BACK);
		sprintf(str,"%d",app_Pneumatics.flow_rate_target);
		LCD_DispText_option( 	10+1,		270 + 60,134,65		,BOTTOM_RIGHT,COLOUR_BLACK,COLOUR_BUTTON_BACK, &fontArialNarrow60h, str);	//x,y,w,h,justification,col,font,txt
	}

	if(display_O2_conc_target != app_Pneumatics.O2_conc_target){
		display_O2_conc_target	=  app_Pneumatics.O2_conc_target;
		LCD_FillRect(			10+1,		270 +130,134,65		,COLOUR_BUTTON_BACK);
		sprintf(str,"%d",app_Pneumatics.O2_conc_target);
		LCD_DispText_option( 	10+1,		270 +130,134,65		,BOTTOM_RIGHT,COLOUR_BLACK,COLOUR_BUTTON_BACK, &fontArialNarrow60h, str);	//x,y,w,h,justification,col,font,txt
	}


	//stopwatch
	if(api_Stopwatch_read(&day,&hr,&min,&sec) || new_build ){
		if(day			!= last_day){
			last_day	=  day;
			LCD_FillRect(		230,272,150,91	,COLOUR_BUTTON_BACK);
		}
		if(	(hr			<  10)		&&
			(last_hr	>= 10)		){
			LCD_FillRect(		230,272,150,91	,COLOUR_BUTTON_BACK);
		}
		last_hr			=  hr;		// should be outside 'if'
		
	//	swatch_running	=  api_Stopwatch_running_read();	//colour_W_G[swatch_running]
		if(day>0){
			if(day==1){	sprintf(str," %d %s "	,day, LanguageStr(LangStr_Day));	}
			else{		sprintf(str," %d %s "	,day, LanguageStr(LangStr_Days));	}
			LCD_DispText_option(230,275,150,37,CENTER,COLOUR_WHITE,COLOUR_BUTTON_BACK, &fontArialNarrow37h, str);	//x,y,w,h,justification,col,font,txt

			sprintf(str,"%2d:%02d:%02d",hr,min,sec);
			LCD_DispText_option(230,313,150,50,CENTER,COLOUR_WHITE,COLOUR_BUTTON_BACK, &fontArialNarrow50h, str);	//x,y,w,h,justification,col,font,txt
		}
		else{
			sprintf(str,"%2d:%02d:%02d",hr,min,sec);
			LCD_DispText_option(230,272,150,91,CENTER,COLOUR_WHITE,COLOUR_BUTTON_BACK, &fontArialNarrow50h, str);	//x,y,w,h,justification,col,font,txt
		}
	}

	if( (UI.select_mode == PMODE_HFOT)||(UI.select_mode == PMODE_POINT) ){
		//don't update graph, PP or RR
	}
	else{
		//graph data
		if( app_PP_graph_start_refresh() ){
			for(x=0;x<PP_HIS_LEN;x++){
				if(	app_PP_data_graph_get(x,&last_pix,&new_pix) || new_build){
					//graph width is 400 pixcels -> 385 pixels
					//xy position is of the bottom left of graph area

				//	if(last_pix == y_val_min_alarm){		LCD_DrawDot(75+50+x, 5+155-y_val_min_alarm	,COLOUR_ORANGE);		}
				//	else if(last_pix == y_val_max_alarm){	LCD_DrawDot(75+50+x, 5+155-y_val_max_alarm	,COLOUR_YELLOW);		}
				//	else{									LCD_DrawDot(75+50+x, 5+155-last_pix, 	COLOUR_WHITE);			}

					for(j=0;j<DOT_SIZE;j++){
						pix_colour	=  COLOUR_WHITE;
						if(y_val_max_alarm == y_val_min_alarm){		pix_colour	=  COLOUR_BLACK;	}
						else if((last_pix-j) > y_val_max_alarm){	pix_colour	=  COLOUR_BLACK;	}
						else if((last_pix-j) < y_val_min_alarm){	pix_colour	=  COLOUR_BLACK;	}
						for(i=0;i<=7;i++){
							if((last_pix-j)	== graph_lines[i] ){
								if((graph_lines[i] < y_val_min_alarm)||(graph_lines[i] > y_val_max_alarm)){	pix_colour	=  COLOUR_GREY_3C;	}
								else{																		pix_colour	=  COLOUR_GRAY_80;	}	//inside max and min
							}
						}
						LCD_DrawDot(225+30+x, j+59+180+10-last_pix			,pix_colour);
					}
					
					for(j=0;j<DOT_SIZE;j++){
						pix_colour	=  COLOUR_BLACK;
						if(		(new_pix-j) > y_val_max_alarm){		pix_colour	=  COLOUR_WHITE;		}
						else if((new_pix-j) < y_val_min_alarm){		pix_colour	=  COLOUR_WHITE;		}
						
						LCD_DrawDot(225+30+x, j+59+180+10-new_pix			,pix_colour);
					}
				}
			}
			
		} 
		//Breathin / out marker
		if(PP_settings.debug){
			if(PP_data.BreathRate.detect_flag != breathingRate_detect_flag_last){
				breathingRate_detect_flag_last	= PP_data.BreathRate.detect_flag;

				LCD_FillCircle(10+150	,60 +150,10, breath_in_out_colour[breathingRate_detect_flag_last]);
			}
			if(PP_data.Apnoea.event_alarm_no_breathing_detected != breathingRate_no_breath_det_last){
				breathingRate_no_breath_det_last	= PP_data.Apnoea.event_alarm_no_breathing_detected;

				LCD_FillCircle(10+150+30	,60 +150,10, colour_G_R[PP_data.Apnoea.event_alarm_no_breathing_detected]);
			}
			if(PP_data.Apnoea.event_settle_status != breathingRate_event_settle_status_last){
				breathingRate_event_settle_status_last	= PP_data.Apnoea.event_settle_status;

				LCD_FillCircle(10+150+30	,60 +150+30,10, breath_alm_en_colour[PP_data.Apnoea.event_settle_status]);
			}
		}
		//RR
		if(UI.select_mode	!= PMODE_BUBBLE_PAP){
			if(PP_data.BreathRate.dis_val != breathingRate_last){
				breathingRate_last	= PP_data.BreathRate.dis_val;
	
				sprintf(str,"%d",(uint8_t)PP_data.BreathRate.dis_val);
				LCD_FillRect(		10+  1,60 +130,134,65 								,COLOUR_WHITE);
				LCD_DispText_option(10+  1,60 +130,134,65	,BOTTOM_RIGHT,COLOUR_BLACK	,COLOUR_WHITE, &fontArialNarrow60h, str);	//x,y,w,h,justification,col,font,txt
			}
		}
		//patient Pressure
		if(PP_data.PatientPressureAv_x10 != patientPressure_last){
			patientPressure_last	=  PP_data.PatientPressureAv_x10;
			if(PP_data.PatientPressureAv_x10 < -99){	PP_data.PatientPressureAv_x10	= -99;	}
			if(PP_data.PatientPressureAv_x10 > 250){	PP_data.PatientPressureAv_x10	= 250;	}

			if(PP_data.PatientPressureAv_x10 < 0){		PP_negflag	=  1;	}
			else if(PP_negflag == 1){
				PP_negflag	=  0;
				LCD_FillRect(		10+  1,60 + 60,134,65 	,COLOUR_WHITE);
			}

			if(PP_data.PatientPressureAv_x10 >= 100){	sprintf(str," %2d",PP_data.PatientPressureAv_x10/10);	}
			else{										sprintf(str," %1.1f",(float)(PP_data.PatientPressureAv_x10)/10);	}
			LCD_DispText_option(	10+  1,60 + 60,134,65		,BOTTOM_RIGHT,COLOUR_BLACK	,COLOUR_WHITE, &fontArialNarrow60h, str);	//x,y,w,h,justification,col,font,txt
		}
	}

	if(app_selfcheck_cfp_status() && (err_message_but.mode == BUTTON_OFF) ){
//		UI.screen		=  UI_MAIN;	//rebuild screen so background colour is correct
		err_message_but.mode	= BUTTON_ADD;
	}

	if(err_message_but.mode){
		//add button
		if(err_message_but.mode == BUTTON_ADD){
			err_message_but.mode	=  BUTTON_ON;
			
			app_touchscreen_button_add(	BUTTON_C,	440,270	,200, 200);							//id,x,y,w,h
			err_message_but.time	=  sys_tic_rd();	//
		}
			

		//time out remove button
		if(sys_tic_rd() > err_message_but.time){
			err_message_but.time	=  sys_tic_rd() + 1000;

			next_fault_tog	^= 0x01;
			if((next_fault_tog)||(fault_status_last == 0)){
				fault_no	=  app_selfcheck_next_fault();
			}
			else{
				fault_no	=  last_fault_no;
			}
			fault_status	= 0;	//no faults of ack needed

			if( fault_no != 0xff){
				app_selfcheck_result_str(STATUS,fault_no,&lead_str[0],&res_str,&res);
				if(		SelfCheckRes[fault_no].flags.bits.single_alert){
					if(SelfCheckRes[fault_no].flags.bits.status_live	>  TEST_PASS){	fault_status	= 3;	}	// dont display single alert timers
					else{																fault_status	= 2;	}	// fixed need ack alarm
				}
				else if(SelfCheckRes[fault_no].flags.bits.status_live	>  TEST_PASS){	fault_status	= 1;	}	// not fixed so need timer
				else{																	fault_status	= 2;	}	// fixed need ack alarm


				if((last_fault_no != fault_no)||(fault_status_last != fault_status)){
					last_fault_no		=  fault_no;


                                        LCD_COLOR fault_priority_colour = colour_alarm_priority[SelfCheckRes[fault_no].flags.bits.alarm_priority];
                                        if(fault_background_colour != fault_priority_colour){
                                                fault_background_colour = fault_priority_colour;
						LCD_FillSoftRect(		440		,270	,200	,200		, 9		,fault_background_colour);
						LCD_DrawSoftRect_1(		440		,270	,200	,200		, 10	,COLOUR_BLACK);
						LCD_DispMonoBitmapTrans(440+10	,270+10	,	"IMG_WarningIcon"  		,COLOUR_BLACK);
					}

					LCD_FillRect(			440+70	,270+10	,200-70	,45		,fault_background_colour);
					sprintf(str,"#%d",fault_no);
					LCD_DispText_option(	440+70	,270+10	,200-70	,45		,CENTER_LEFT	,COLOUR_BLACK	,fault_background_colour, &fontArialNarrowBold31h, str);
					LCD_FillRect(			440+5	,270+60	,200-5	,50		,fault_background_colour);
					if(LCD_GetStringWidth(	&fontArial22h, lead_str[0])	>  (200-5)){
						strcpy(str,										lead_str[0]);
						lead_str[0]										=  str;
						err_str_len										=  strlen(lead_str[0]);
						err_str_len										/= 2;
						strtok(&lead_str[0][err_str_len],				" ");
						lead_str[1]										=  lead_str[0] + strlen(lead_str[0]) + 1;
						LCD_DispText_option(	440+5 	,270+60	,200-5	,25	,CENTER			,COLOUR_BLACK	,fault_background_colour, &fontArial22h, lead_str[0]);
						LCD_DispText_option(	440+5 	,270+85	,200-5	,25	,CENTER			,COLOUR_BLACK	,fault_background_colour, &fontArial22h, lead_str[1]);
					}
					else{
						LCD_DispText_option(	440+5 	,270+60	,200-5	,25	,CENTER			,COLOUR_BLACK	,fault_background_colour, &fontArial22h, lead_str[0]);
					}
				}

				if(fault_status_last != fault_status){
					fault_status_last	=  fault_status;
					LCD_FillRect(		450+1	,270+140	,180-2	,60,		fault_background_colour);
				}

				if(fault_status == 1){ //
					rd_alarm_tmr	=  app_selfcheck_SysAlarm_Quiet_Tmr();
					if(rd_alarm_tmr){
						rd_alarm_tmr	/= 1000;
						sprintf(str," %d:%02d "		,rd_alarm_tmr /60
													,rd_alarm_tmr %60);
						LCD_DispText_option(	440+70	,270+140	,190-70	,45	,BOTTOM_LEFT		,COLOUR_BLACK	,fault_background_colour, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
						LCD_DispMonoBitmapTrans(440+10	,270+140,	"IMG_AlarmMuteIcon"  	,COLOUR_BLACK);
						
					}
				}
				else if(fault_status == 2){
					LCD_DispText_option(450		,270+140	,180	,31,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Ack));
					LCD_DispText_option(450		,270+140+31	,180	,31,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Alarm));
				}
				else if(fault_status == 3){
					if(SelfCheckRes[fault_no].flags.bits.userSilenceCnt	>= 1){
						LCD_DispMonoBitmapTrans(440+10	,270+140,	"IMG_AlarmMuteIcon"  	,COLOUR_BLACK);
					}
				}

			}
			else if(last_fault_no != 0xff){
				last_fault_no			= 0xff;
				fault_background_colour	=  colour_mode[UI.select_mode];
				LCD_FillSoftRect( 		440		,270	,200	,200		, 9		,colour_mode[UI.select_mode]);
			}

		}
		if(err_message_but.mode == BUTTON_REMOVE){
			err_message_but.mode	=  BUTTON_OFF;
			app_touchscreen_button_disable(BUTTON_C);
			LCD_FillSoftRect( 		440	,270	,200	,200		, 9		,colour_mode[UI.select_mode]);
			fault_background_colour	=  0;
		}
	}

	// yellow border(flash) for override on flow settings button -- only at low pressure warning (fault no 12)
	if(	(SelfCheckRes[TEST_PP_MIN].flags.bits.status <= TEST_PASS)	||
		(SelfCheckRes[TEST_PP_MIN].flags.bits.settle_quiet	== 1 )		)
	{
		if(UI.flow_override == OVERRIDE_AVAIL){	UI.flow_override	=  OVERRIDE_NONE;	}
		//don't clear if active
	}
	else{
		if(UI.flow_override == OVERRIDE_NONE){	UI.flow_override	=  OVERRIDE_AVAIL;	}
	}


	if((UI.flow_override != override_last)||(UI.flow_override >= OVERRIDE_ACTIVE)){
		override_last	=  UI.flow_override;
		switch(UI.flow_override){
			case OVERRIDE_NONE:		//no low pressure fault
				LCD_DrawSoftRect_n(8	,268	,204	,204	,10	,4,	colour_mode[UI.select_mode]);
				LCD_DrawSoftRect_1(10	,270	,200	,200	,10	,COLOUR_WHITE);
				break;
			case OVERRIDE_AVAIL:		//flash border to show option
				LCD_DrawSoftRect_n(8	,268	,204	,204	,10	,4,COLOUR_YELLOW);
				break;
			case OVERRIDE_ACTIVE:		//flash in override mode
			case OVERRIDE_ACTIVE1:
				if(sys_tic_rd() > flasher_tmr){
					flasher_tmr	= sys_tic_rd() +500;
					if(UI.flow_override == OVERRIDE_ACTIVE){
						LCD_DrawSoftRect_n(8	,268	,204	,204	,10	,4,COLOUR_YELLOW);
						UI.flow_override	=  OVERRIDE_ACTIVE1;
					}
					else{
					 	LCD_DrawSoftRect_n(8	,268	,204	,204	,10	,4,	colour_mode[UI.select_mode]);
						LCD_DrawSoftRect_1(10	,270	,200	,200	,10	,COLOUR_WHITE);
						UI.flow_override	=  OVERRIDE_ACTIVE;
					}
				}
				break;
			default:	UI.flow_override	=  OVERRIDE_NONE;
		}
	}

	//nebuliser button
	if(nebuliser_status_last != UI.nebuliser){
		nebuliser_status_last	= UI.nebuliser;
		//image 60 high     110 wide
		//space 75 high     200 wide
		if(app_Pneumatics.limits.nebuliser_avail == 0){
				LCD_DispMonoBitmap(	228,400, "IMG_NebuliserDeactivated"  ,COLOUR_GREY_3C,COLOUR_BUTTON_BACK);
				app_touchscreen_button_disable(BUTTON_9);	//disable button
		}
		else{
			if(UI.nebuliser == 0){
				LCD_DispMonoBitmap(	228,400, "IMG_NebuliserDeactivated"  ,COLOUR_BLACK,COLOUR_BUTTON_BACK);
			}
			else{
				LCD_DispMonoBitmap(	228,400, "IMG_NebuliserActivated"    ,COLOUR_GREEN,COLOUR_BUTTON_BACK);
			}
		//	LCD_FillSoftRect(		440+150,305+10, 30, 55, 10,		colour_W_G[UI.nebuliser]);
		}
	}

	app_UI_screen_side_bar(BUILD_REFRESH);


	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	switch(touch_status){
//		case BUTTON_TIME:
//		case BUTTON_TIME_HOLD:

//
//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:
//
		case BUTTON_SIDE1:
		case BUTTON_SIDE1_HOLD:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(UI.pneumatics_mode == 0){
			//	if(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_IDLE){
					UI.screen		=  UI_MODE_SETUP;
				
					//app_UI_screen_mode_setup(BUILD_NEW);
				}
			}
			break;

		case BUTTON_SIDE2:
		case BUTTON_SIDE2_HOLD:
			UI.screen			=  UI_SETTINGS;
			if(UI.pneumatics_mode != 0){
				UI.page_timer_main_return	=  RET_MAIN_SCR;
			}
			break;

		case BUTTON_SIDE3:
		case BUTTON_SIDE3_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;

				if(UI.sidebar.but3	==  BUT_STOP){			app_UI_screen_popup_setup(POPUP_STOP_THERAPY			,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);		}
				else{
					if(UI.demo_mode){						app_UI_screen_popup_setup(POPUP_START_THERAPY			,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);		}
					else 	if( (api_io_rd_AirSupply(0)	== 0)&&(api_io_rd_O2Supply(0)	== 0) ){
															app_UI_screen_popup_setup(POPUP_START_THERAPY_NO_GAS	,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);		}
					else if(api_io_rd_O2Supply(0)	==0 ){	app_UI_screen_popup_setup(POPUP_START_THERAPY_NO_O2		,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);		}
					else if(api_io_rd_AirSupply(0)	==0 ){	app_UI_screen_popup_setup(POPUP_START_THERAPY_NO_AIR	,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);		}
					else {									app_UI_screen_popup_setup(POPUP_START_THERAPY			,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);		}
				}
			}
			break;

		//************************************************
		//patient stats
	
		case BUTTON_20: // override testing
			break;
		case BUTTON_6:		//disabled

			break;
		case BUTTON_7:
		case BUTTON_7_HOLD:		//flow settings
			if(UI.touch_used == 0){
				UI.touch_used	=  2;
				switch(UI.flow_override){
					case OVERRIDE_NONE:
						UI.screen			=  UI_FLOW_SETUP;
						if(UI.pneumatics_mode != 0){
							UI.page_timer_main_return	=  RET_MAIN_SCR;
						}
						break;
					case OVERRIDE_AVAIL:
//						UI.flowRate			=  UI.FLOW.val;				//is this correct
						UI.screen			=  UI_FLOW_OVERRIDE;
						if(UI.pneumatics_mode != 0){
							UI.page_timer_main_return	=  RET_MAIN_SCR;
						}
						break;
					case OVERRIDE_ACTIVE:
					case OVERRIDE_ACTIVE1:
						app_UI_screen_popup_setup(POPUP_EXIT_OVERRIDE,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);
						break;
				}
			}
			break;

		//stopwatch
		case BUTTON_8:	//press
			if(UI.touch_used					== 0){
				UI.touch_used					=  2;
				if(api_Stopwatch_running_read()	==1){
					api_Stopwatch_mode_set(		STOPWATCH_MODE_STOP);
					hold_cnt					|= 0x80;		// do not restart on button release
				}
				hold_cnt						++;
			}
			break;
		case BUTTON_8_HOLD:
			if((hold_cnt & 0x7F)				== 8){
				if(api_Stopwatch_running_read()	== 0){
					api_Stopwatch_mode_set(		STOPWATCH_MODE_RESET);
				}
			}
			if((hold_cnt & 0x7F)				<  9){
				hold_cnt						++;
			}
			break;

		//nebuliser
		case BUTTON_9:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				if(UI.pneumatics_mode == 0){
			//	if(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_IDLE){
					if((UI.flowRate < Nebuliser_min_flow)&&(UI.nebuliser == 0)){
						app_UI_screen_popup_setup(POPUP_CONFIRM_CHANGE_NEBULISER_FLOW,UI_MAIN,UI_FLOW_SETUP,POPUP_NOTIMEOUT);
					}
					else{
						if(UI.nebuliser){		UI.nebuliser = 0;	}
						else{					UI.nebuliser = 1;	}
						app_pneumatic_set_nebuliser(UI.nebuliser);
						if(UI.flowRate < Nebuliser_min_flow){
							UI.screen			=  UI_FLOW_SETUP;
						}
					}
				}
				else{
					if((UI.flowRate < Nebuliser_min_flow)&&(UI.nebuliser == 0)){
						app_UI_screen_popup_setup(POPUP_CONFIRM_CHANGE_NEBULISER_FLOW,UI_MAIN,UI_FLOW_SETUP,POPUP_NOTIMEOUT);
					}
					else{
						app_UI_screen_popup_setup(POPUP_CONFIRM_CHANGE_NEBULISER,UI_MAIN,UI_MAIN,POPUP_NOTIMEOUT);
					}
				}
			}
			break;
	//************************************************
		case BUTTON_A: // alarm settings
			UI.screen			=  UI_ALARM_SETUP;
			if(UI.pneumatics_mode != 0){
				UI.page_timer_main_return	=  RET_MAIN_SCR;
			}
			break;
	//************************************************
		//error message button
		case BUTTON_C:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;
				
				if(	(SelfCheckRes[TEST_O2_STARTUP_CAL].flags.bits.status > TEST_PASS)	&&
					(api_io_rd_AirSupply(0)	== 1)	&&
					(api_io_rd_O2Supply(0)	== 1)	)
				{
					app_UI_screen_popup_setup(POPUP_REDO_STARTUP_CAL,UI_MAIN,UI_PARACUBE_CAL,POPUP_NOTIMEOUT);
				}
				
//				if(UI.pneumatics_mode){		//make only avail if in therapy mode
					if(	(api_io_rd_AirSupply(0)	== 1)	&&
						(api_io_rd_O2Supply(0)	== 1)	&&
						(	(SelfCheckRes[TEST_SUPPLY_AIR].flags.bits.status > TEST_PASS) ||
							(SelfCheckRes[TEST_SUPPLY_O2].flags.bits.status > TEST_PASS)	)	)
					{
						app_UI_screen_popup_setup(POPUP_2GAS_SUPPLY_AVAIL,UI_MAIN,UI_FLOW_SETUP,POPUP_NOTIMEOUT);
					}
//				}
				app_selfcheck_AckClearStatus_all();
				last_fault_no		= 0xff;
				fault_status_last	= 0xff;
				err_message_but.mode	=  BUTTON_REMOVE;
			}
			break;
	//************************************************
		case BUTTON_JUST_RELEASED:
		case BUTTON_RELEASED:
			UI.touch_used								=  0;
			if(hold_cnt									>  0){
				if(hold_cnt								<  4){		// less than 1 second
					if(	(api_Stopwatch_running_read()	== 0)	&&
						(UI.pneumatics_mode				== 1)	){
						api_Stopwatch_mode_set(		STOPWATCH_MODE_START);
					}
				}
				hold_cnt			=  0;
			}
			break;
	}
	return;
}


/*************************************************************************************************
* Function Name : 	app_UI_screen_settings
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		07/12/17	W. Paul			Created
* 0.2.0         11/06/26        A.Kane                  Update to Alarm Audio Call
*************************************************************************************************/
void 	app_UI_screen_settings(	uint8_t touch_status, uint8_t but_status, bool init)
{
	char				str[30];
	time_t 				rawtime;
	static struct tm 	*timeinfo;
	uint8_t				time_change		=  0;
	uint8_t				buttons_held	=  0;
	static uint8_t		hold_slow_cnt	=  0;
	uint8_t				month_d;
	static uint8_t		queue_tone;

	static uint8_t		scr_volume_chirp;
	static uint8_t		scr_volume_alarm;
	static uint8_t		scr_brightness;
	static uint8_t		scr_language;

	if(init){
    	UI.screen			=  UI_SETTINGS_WAIT;
    //	UI.screen_last		=  UI_SETTINGS;		//not here because we need screen_last in this screen to know where to return to
    //	app_selfcheck_AudioMute(0);		//if silenced here.. if during threatment an alarm is sounding it can be bypassed here
    	UI.page_timer_n_1	=  0;
    	queue_tone			=  0;

		UI.but_used			=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used		=  1;
		scr_volume_chirp	=  0xff;
		scr_volume_alarm	=  0xff;
		scr_brightness		=  0xff;
		scr_language		=  0xff;

		app_touchscreen_button_disable_all();

	//background
		LCD_FillRect(	0,	0,	650, LCD_DISP_HEIGHT-1,	COLOUR_SCREEN_BACKGROUND);
		if(UI.demo_mode){
			LCD_DrawRect_n(1,	1,	650-2, LCD_DISP_HEIGHT-1-2, 4, COLOUR_RED);
		}
	//************************************************
	//Sidebar
		UI.sidebar.endis	=  1;
		UI.sidebar.but1		=  BUT_BLANK;
		UI.sidebar.but2		=  BUT_BLANK;
		UI.sidebar.but3		=  BUT_BACK;
		app_UI_screen_side_bar(BUILD_ALL_NEW);

	//************************************************

	//	LCD_DispText_option(				  5, 5,150,50,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, "Settings");	//x,y,w,h,justification,col,font,txt

	//buttons
		app_UI_button(			BUTTON_6,	 65,40,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				 65,40,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_7,	155,40,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				155,40,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_8,	295,40,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		
		LCD_DispText_option(				295,40,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_9,	410,40,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				410,40,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_A,	525,40,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				525,40,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt

		app_UI_button(			BUTTON_B,	 65,140,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				 65,140,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_C,	155,140,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				155,140,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_D,	295,140,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				295,140,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_E,	410,140,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				410,140,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_F,	525,140,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				525,140,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt


		app_UI_button(			BUTTON_10,	 65,290,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				 65,290,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_11,	155,290,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				155,290,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_12,	295,290,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				295,290,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_13,	525,290,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				525,290,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "+");	//x,y,w,h,justification,col,font,txt

		app_UI_button(			BUTTON_14,	 65,390,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				 65,390,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_15,	155,390,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				155,390,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_16,	295,390,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				295,390,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt
		app_UI_button(			BUTTON_17,	525,390,60,50,COLOUR_BUTTON_BACK);											//id,x,y,w,h,backgound colour
		LCD_DispText_option(				525,390,60,50,		CENTER,		COLOUR_WHITE,TRANSPARENT, &fontArialNarrowBold31h, "-");	//x,y,w,h,justification,col,font,txt

	//icon images
		LCD_DispMonoBitmapTrans(	 65+10	,235, "IMG_Beep50"			,COLOUR_BLACK);		//50high 40 wide
		LCD_DispMonoBitmapTrans(	155+5+5	,235, "IMG_AlarmVol"		,COLOUR_BLACK);		//50high 50 wide	(but wants bell shape centred
		LCD_DispMonoBitmapTrans(	295+5	,235, "IMG_Brightness51"	,COLOUR_BLACK);
		LCD_DispMonoBitmapTrans(	525-4	,235, "IMG_Language50"		,COLOUR_BLACK);

		//update time
		UI.page_timer_n_0		=  0;
	}

	if(UI.page_timer_n_0 == 0){
		rawtime	= time(0);
		timeinfo = localtime (&rawtime);
		UI.page_timer_n_0	= (60 - timeinfo->tm_sec)*1000;

		sprintf(str," %2d  "	, timeinfo->tm_hour);
		LCD_DispText_option( 65,95,60,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option( 125,95,30,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, ":");	//x,y,w,h,justification,col,font,txt
		sprintf(str," %02d "	, timeinfo->tm_min	);
		LCD_DispText_option( 155,95,60,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt

		sprintf(str,"  %d  "	, timeinfo->tm_mday		);
		LCD_DispText_option(295,95,60,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
		sprintf(str,"  %s  "	, LanguageStr( Lang_array_Month3[timeinfo->tm_mon])	);
		LCD_DispText_option(410-20,95,100,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
		sprintf(str,"  %d  "	, timeinfo->tm_year + EPOCH_YEAR	);
		LCD_DispText_option(525,95,60,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
	}


	if(scr_volume_chirp != UI.volume_chirp){
		if(UI.volume_chirp > UI_VOLUME_MAX){					UI.volume_chirp =  UI_VOLUME_MAX;		}
		if(UI.volume_chirp < UI_VOLUME_MIN){					UI.volume_chirp =  UI_VOLUME_MIN;		}
		scr_volume_chirp 		=  UI.volume_chirp;
		api_audio_volume_per_wr(AUDIO_DIG1,UI.volume_chirp);
		sprintf(str,"  %3d%%  "	, scr_volume_chirp	);
		LCD_DispText_option( 50,345,90,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
		if(init == 0){
			api_audio_chirp(1);
		}
	}
	if(scr_volume_alarm != UI.volume_alarm){
		if(UI.volume_alarm > UI_VOLUME_MAX){					UI.volume_alarm =  UI_VOLUME_MAX;		}
		if(UI.volume_alarm < UI_VOLUME_MIN){					UI.volume_alarm =  UI_VOLUME_MIN;		}
		scr_volume_alarm 		=  UI.volume_alarm;
		api_audio_volume_per_wr(AUDIO_DIG2,UI.volume_alarm);
		sprintf(str,"  %3d%%  "	, scr_volume_alarm	);
		LCD_DispText_option( 140,345,90,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt

		if(	(api_audio_play_status() != AUDIO_IDLE) && 		//if audio is not running
		   	(UI.page_timer_n_1	>  500)  					//and lots of play time remaining
		){}
		else if(init == 0){
			queue_tone	=  1;
		}
	}

	if((api_audio_play_status() == AUDIO_IDLE)&&(queue_tone== 1)){
		queue_tone	=  0;
		UI.page_timer_n_1	=  1900;
		if(api_audio_alarm(ALARM_PRIORITY_MEDIUM, 0)){					// Start audio alarm
			queue_tone		=  0;						// Audio not updated, setup to redo
		}
	}

	if(UI.page_timer_n_1 == 1){
		UI.page_timer_n_1	=  0;
		api_audio_play_file_stop();
	}


	if(scr_brightness != UI.brightness){
		scr_brightness	=  UI.brightness;
		LCD_brightness(UI.brightness);
		sprintf(str,"  %3d%%  "	, scr_brightness	);
		LCD_DispText_option(280,345,90,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt
	}
	if(scr_language != (uint8_t)UI.language){
		scr_language	=  UI.language;
		Language_set(UI.language);

		LCD_FillRect(		475,345,160,40,  	COLOUR_SCREEN_BACKGROUND);
		sprintf(str,"%s"	, LanguageStr( Lang_array_Lang[UI.language]) );
		LCD_DispText_option(475,345,160,40,CENTER,COLOUR_BLACK,COLOUR_SCREEN_BACKGROUND, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt

		UI.page_timer_n_0		=  10;		//rebuild screen language
		app_UI_screen_side_bar(BUILD_NEW);	//rebuild screen language
	}

	//auto return to main screen
	if(UI.page_timer_main_return == 1){
		UI.page_timer_main_return	=  0;
		UI.screen		=  UI_MAIN;	//this timer always sends us back to main
		app_UI_parameters_wr();
	}


	app_UI_screen_side_bar(BUILD_REFRESH);

	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;

		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
		case BUT_ALL_HOLD:
			buttons_held	=  1;
	}

	switch(touch_status){
//		case BUTTON_TIME:
//		case BUTTON_TIME_HOLD:
//
//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:
//
//		case BUTTON_SIDE1:
//		case BUTTON_SIDE1_HOLD:
//			if(UI.touch_used == 0){
//				UI.touch_used	=  1;
//				UI.screen		=  UI_MODE_SETUP;
//			}
//			break;
//		case BUTTON_SIDE2:
//		case BUTTON_SIDE2_HOLD:
//			if(UI.touch_used == 1){
//				UI.touch_used	=  2;
//				UI.screen		=  UI_ALARM_SETUP;
//			}
//			break;

		case BUTTON_SIDE3:
			if(UI.touch_used == 0){
				UI.touch_used	=  1;

				if(UI.page_timer_n_1){				//if tone started by vol change then stop tone here
					UI.page_timer_n_1	=  0;
					api_audio_play_file_stop();
				}

				UI.screen		=  UI.screen_last;
				//save any changes
				app_UI_parameters_wr();
			}
			break;
	//	case BUTTON_SIDE3_HOLD:
	//		break;

		//************************************************
		case BUTTON_6:	//hr+
		case BUTTON_6_HOLD:	//hr+
			if(timeinfo->tm_hour < 23){	timeinfo->tm_hour++;		}
			else{						timeinfo->tm_hour	= 0;	}
			time_change		=  1;
			break;
		case BUTTON_7:	//min+
		case BUTTON_7_HOLD:
			if(timeinfo->tm_min < 59){	timeinfo->tm_min++;			}
			else{						timeinfo->tm_min	= 0;	}
			time_change		=  1;
			break;
		case BUTTON_8:	//day+
		case BUTTON_8_HOLD:
			month_d	=  no_days_in_month[timeinfo->tm_mon];
			if((timeinfo->tm_year % 4 == 0)&&(timeinfo->tm_mon == 1)){month_d = 29;}
			if(timeinfo->tm_mday < month_d){	timeinfo->tm_mday++;			}
			else{								timeinfo->tm_mday		= 1;	}
			time_change		=  1;
			break;
		case BUTTON_9:	//month+
		case BUTTON_9_HOLD:
			if(hold_slow_cnt == 0){
				if(timeinfo->tm_mon < 11){	timeinfo->tm_mon++;			}
				else{						timeinfo->tm_mon	= 0;	}
				time_change		=  1;
			}
			if(++hold_slow_cnt > 3){	hold_slow_cnt	=  0;	}
			break;
		case BUTTON_A:	//year+
		case BUTTON_A_HOLD:
			if(timeinfo->tm_year < TIME_MAX_YEAR){	timeinfo->tm_year++;	}
			time_change		=  1;
			break;

		case BUTTON_B:	//hr-
		case BUTTON_B_HOLD:
			if(timeinfo->tm_hour > 0){	timeinfo->tm_hour--;		}
			else{						timeinfo->tm_hour	= 23;	}
			time_change		=  1;
			break;
		case BUTTON_C:	//min-
		case BUTTON_C_HOLD:
			if(timeinfo->tm_min > 0){	timeinfo->tm_min--;			}
			else{						timeinfo->tm_min	= 59;	}
			time_change		=  1;
			break;
		case BUTTON_D:	//day-
		case BUTTON_D_HOLD:
			month_d	=  no_days_in_month[timeinfo->tm_mon];
			if((timeinfo->tm_year % 4 == 0)&&(timeinfo->tm_mon == 1)){month_d = 29;}
			if(timeinfo->tm_mday > 1 ){	timeinfo->tm_mday--;			}
			else{						timeinfo->tm_mday		= month_d;	}
			time_change		=  1;
			break;
		case BUTTON_E:	//month-
		case BUTTON_E_HOLD:
			if(hold_slow_cnt == 0){
				if(timeinfo->tm_mon > 0){	timeinfo->tm_mon--;			}
				else{						timeinfo->tm_mon	= 11;	}
				time_change		=  1;
			}
			if(++hold_slow_cnt > 3){	hold_slow_cnt	=  0;	}
			break;
		case BUTTON_F:	//year-
		case BUTTON_F_HOLD:
			if( (timeinfo->tm_year + EPOCH_YEAR) > 2017 ){	timeinfo->tm_year--;	}
			time_change		=  1;
			break;
        //************************************************
        //VOLUME Chirp
        case BUTTON_10:
        case BUTTON_10_HOLD:
        //	api_audio_play_file_stop();
			if(UI.volume_chirp < UI_VOLUME_MAX - UI_VOLUME_STEP){	UI.volume_chirp += UI_VOLUME_STEP;		}
			else{													UI.volume_chirp =  UI_VOLUME_MAX;		}
			break;
        case BUTTON_14:
        case BUTTON_14_HOLD:
        //	api_audio_play_file_stop();
        	if(UI.volume_chirp > UI_VOLUME_MIN + UI_VOLUME_STEP){	UI.volume_chirp -= UI_VOLUME_STEP;		}
        	else{													UI.volume_chirp =  UI_VOLUME_MIN;		}
			break;
		//************************************************
        //VOLUME Alarm
        case BUTTON_11:
        case BUTTON_11_HOLD:
        //	api_audio_play_file_stop();
			if(UI.volume_alarm < UI_VOLUME_MAX - UI_VOLUME_STEP){	UI.volume_alarm += UI_VOLUME_STEP;		}
			else{													UI.volume_alarm =  UI_VOLUME_MAX;		}
			break;
        case BUTTON_15:
        case BUTTON_15_HOLD:
        //	api_audio_play_file_stop();
        	if(UI.volume_alarm > UI_VOLUME_MIN + UI_VOLUME_STEP){	UI.volume_alarm -= UI_VOLUME_STEP;		}
        	else{													UI.volume_alarm =  UI_VOLUME_MIN;		}
			break;
		//************************************************
        //BRIGHTNESS
        case BUTTON_12:
        case BUTTON_12_HOLD:
			if(UI.brightness < UI_BRIGHTNESS_MAX - UI_BRIGHTNESS_STEP){	UI.brightness += UI_BRIGHTNESS_STEP;	}
			else{														UI.brightness =  UI_BRIGHTNESS_MAX;		}

			if(UI.brightness > UI_BRIGHTNESS_MAX){						UI.brightness =  UI_BRIGHTNESS_MAX;		}
			if(UI.brightness < UI_BRIGHTNESS_MIN){						UI.brightness =  UI_BRIGHTNESS_MIN;		}
			break;
        case BUTTON_16:
        case BUTTON_16_HOLD:
        	if(UI.brightness > UI_BRIGHTNESS_MIN + UI_BRIGHTNESS_STEP){	UI.brightness -= UI_BRIGHTNESS_STEP;	}
        	else{														UI.brightness =  UI_BRIGHTNESS_MIN;		}

        	if(UI.brightness > UI_BRIGHTNESS_MAX){						UI.brightness =  UI_BRIGHTNESS_MAX;		}
			if(UI.brightness < UI_BRIGHTNESS_MIN){						UI.brightness =  UI_BRIGHTNESS_MIN;		}
			break;
		//************************************************
        //Lanuguage
        case BUTTON_13:
			if(UI.language < Lang_Max){		UI.language += 1;	}
			else{							UI.language =  (LANG_SELECT_enum)(Lang_Max-1);	}
			if(UI.language >= Lang_Max){	UI.language =  (LANG_SELECT_enum)(Lang_Max-1);	}
			break;
        case BUTTON_17:
        	if(UI.language){				UI.language -= 1;	}
        	else{							UI.language =  (LANG_SELECT_enum)(0);		    }
        	if(UI.language >= Lang_Max){	UI.language =  (LANG_SELECT_enum)(Lang_Max-1);	}
			break;
		//************************************************
		case BUTTON_JUST_RELEASED:
		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			hold_slow_cnt	=  0;
			break;
		case BUTTON_PRESSED_SOMEWHERE_ELSE:
			if(buttons_held){
				if(UI.pneumatics_mode	== 0){
					UI.screen			=  UI_ENG_MODE;
				}
			}
			break;
	}

	if(time_change){
		month_d	=  no_days_in_month[timeinfo->tm_mon];
		if((timeinfo->tm_year % 4 == 0)&&(timeinfo->tm_mon == 1)){month_d = 29;}
		if(timeinfo->tm_mday > month_d){
			timeinfo->tm_mday = month_d;
		}

		rawtime	=  mktime ( timeinfo );
		time(&rawtime);
		UI.page_timer_n_0	=  10;
	}

	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_screen_flow_override
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	    Date d/m/y		Programmer		Reason for Change
* 0.1.0		    31/07/18 		J. Park			Created
*************************************************************************************************/
void app_UI_screen_flow_override(uint8_t touch_status, uint8_t but_status, bool init)
{
	static uint8_t		rebuild_screen;
	char				str[40];
	slider_t			slider_info;
	uint16_t			line_width;
	uint16_t			spacing;
	uint16_t			line_max;
	uint16_t			posx;


	if(init){
    	UI.screen			=  UI_FLOW_OVERRIDE_WAIT;
    	UI.screen_last		=  UI_FLOW_OVERRIDE;
    	app_selfcheck_AudioMute(0);
		UI.but_used		=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used	=  1;

		app_touchscreen_button_disable_all();

	//background
		LCD_FillRect(	0,	0,	650, LCD_DISP_HEIGHT,	COLOUR_YELLOW);
		if(UI.demo_mode){
			LCD_DrawRect_n(1,	1,	650-2, LCD_DISP_HEIGHT-1-2, 4, COLOUR_RED);
		}
	//************************************************
	//Sidebar
		UI.sidebar.but1	=  BUT_BLANK;
		UI.sidebar.but2	=  BUT_BLANK;
		UI.sidebar.but3	=  BUT_NEXT;

		app_UI_screen_side_bar(BUILD_ALL_NEW);

	//************************************************
	//Title
		switch(UI.select_mode){
			case PMODE_CPAP:			sprintf(str, "CPAP");			break;
			case PMODE_CPAP_PAED:		sprintf(str, "CPAP Paed");	break;
			case PMODE_CPAP_HELMET:		sprintf(str, "CPAP Helmet");	break;
			case PMODE_BUBBLE_PAP:		sprintf(str, "Bubble PAP");		break;
			case PMODE_HFOT:			sprintf(str, "HFOT");			break;
			case PMODE_POINT:			sprintf(str, "POINT");			break;
		}
		posx = LCD_DispText_option(	25,  	5, 600, 50,				BOTTOM_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h	, str);
		LCD_DispText_option(		posx,  	5, 600-25-posx, 47,		BOTTOM_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow37h	, LanguageStr(LangStr_Override));

	//************************************************
		UI.FLOW.val		=  app_Pneumatics.flow_rate_target;
		UI.FLOW.val_last		=  0xffff;
		rebuild_screen = 1;
	}

	if(rebuild_screen){
		rebuild_screen	=  0;
		LCD_FillRect(	115-15, 100-50-1+100,	450, 50, COLOUR_YELLOW);
	//************************************************
	//FLOW
		//Text
		LCD_DispText_option(				25,  105, 120, 40,		CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Flow));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(				150,  105,200, 40,		CENTER_LEFT,COLOUR_BLACK,TRANSPARENT, &fontArial22h, LanguageStr(LangStr_Lmin));	//x,y,w,h,justification,col,font,txt


		//- button
		LCD_FillSoftRect(					 10,  155, 75, 75, 10, 	COLOUR_BUTTON_PAIR2);
		LCD_DrawSoftRect_1(					 10,  155, 75, 75, 10, 	COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_6, 10,  155, 75, 75);
		LCD_DispText_option(				 10,  155, 75, 75,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrow60h, "-");	//x,y,w,h,justification,col,font,txt

		//+ button
		LCD_FillSoftRect(					470,  155, 75, 75, 10, 	COLOUR_BUTTON_PAIR2);
		LCD_DrawSoftRect_1(					470,  155, 75, 75, 10, 	COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_8,470,  155, 75, 75);
		LCD_DispText_option(				470,  155, 75, 75,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrow60h, "+");	//x,y,w,h,justification,col,font,txt

		//slider
		switch(UI.select_mode){
			case PMODE_CPAP:
			case PMODE_CPAP_HELMET:	line_width = 308; line_max = 140; spacing = 20;	break;
			case PMODE_CPAP_PAED:
			case PMODE_HFOT:
			case PMODE_POINT:		line_width = 320; line_max =  80; spacing = 10;	break;
			case PMODE_BUBBLE_PAP:	line_width = 320; line_max =  20; spacing =  5;	break;
		}
		app_line_slider_draw(				115, 200,line_width,  0,line_max,spacing,COLOUR_BLACK,DISPLAY_NUMBERS_ABOVE,&UI.FLOW);	//x,y,w,minx,max,x,spacing
		UI.FLOW.limit_min	=  app_Pneumatics.limits.flow_min;
		UI.FLOW.limit_max	=  app_Pneumatics.limits.flow_max;
		UI.FLOW.limit_min_pix	=  app_line_slider_val2pix( &UI.FLOW	,UI.FLOW.limit_min);
		UI.FLOW.limit_max_pix	=  app_line_slider_val2pix( &UI.FLOW	,UI.FLOW.limit_max);

		app_touchscreen_button_add(BUTTON_7,115+UI.FLOW.end_min_pix,  200-50,UI.FLOW.end_max_pix - UI.FLOW.end_min_pix, 150);

		//endstops
		LCD_DispMonoBitmapTrans(			115-10+UI.FLOW.limit_min_pix, 200+7, "IMG_EndStopL",COLOUR_BLACK);
		LCD_DispMonoBitmapTrans(			115   +UI.FLOW.limit_max_pix, 200+7, "IMG_EndStopR",COLOUR_BLACK);
		UI.FLOW.val_last	= 0xffff;


	}

	//auto return to main screen
	if(UI.page_timer_main_return == 1){
		UI.page_timer_main_return	=  0;
		UI.screen		=  UI_MAIN;
	}

	app_UI_screen_side_bar(BUILD_REFRESH);

	//curser on slider
	if(UI.FLOW.val_last != UI.FLOW.val){
		if(UI.FLOW.val_last != uINIT_s16){
			UI.FLOW.val_last	=  app_line_slider_val2pix( &UI.FLOW	,UI.FLOW.val_last);
//			temp_f		=  UI.FLOW.val_last;
//			temp_f		*= UI.FLOW.pix_m;
//			UI.FLOW.val_last	=  (uint16_t)temp_f;

			//Note 11 is the offset for the image to pointer center
			LCD_DispMonoBitmapTrans(		115+UI.FLOW.val_last-11, 200+22, "IMG_SliderIconHandU",COLOUR_YELLOW);
		}
//		UI.FLOW.val_last	=  UI.FLOW.val;
//		temp_f		=  UI.FLOW.val_last;
//		temp_f		*= UI.FLOW.pix_m;
//		UI.FLOW.val_last	=  (uint16_t)temp_f;
		UI.FLOW.val_last	=  app_line_slider_val2pix( &UI.FLOW	,UI.FLOW.val);
		LCD_DispMonoBitmapTrans(			115+UI.FLOW.val_last-11, 200+22, "IMG_SliderIconHandU",COLOUR_BLACK);

		sprintf(str," %3d ",UI.FLOW.val);
		LCD_DispText_option(				550,  80+100, 95, 50,CENTER,COLOUR_BLACK,COLOUR_YELLOW, &fontArialNarrow50h, str);	//x,y,w,h,justification,col,font,txt   height_check
		UI.FLOW.val_last	=  UI.FLOW.val;	//save of var
	}


	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	switch(touch_status){
//		case BUTTON_TIME:
		case BUTTON_TIME_HOLD:
		case BUTTON_SIDE2:
		case BUTTON_SIDE2_HOLD:
			UI.screen			=  UI_SETTINGS;
			break;
//
//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:
//
		case BUTTON_SIDE1:
		case BUTTON_SIDE1_HOLD:
			break;

		case BUTTON_SIDE3:
		case BUTTON_SIDE3_HOLD:
			//save the settings that have been chosen
			app_UI_screen_popup_setup(POPUP_CONFIRM_OVERRIDE,UI_THIS_SCREEN,UI_MAIN,POPUP_NOTIMEOUT);
			break;

		//************************************************
		//FLOW
		case BUTTON_6:
		case BUTTON_6_HOLD:
			if(UI.FLOW.val > UI.FLOW.limit_min){
				UI.FLOW.val	-= 1;
			}
			break;

		case BUTTON_7:		//Flow slider
		case BUTTON_7_HOLD:
			if( app_touchscreen_slider_info(&slider_info) ){
				UI.FLOW.val	=  (uint16_t)app_line_slider_pix2val(&UI.FLOW, slider_info.x_ee_f);
				if(UI.FLOW.val < UI.FLOW.limit_min){	UI.FLOW.val =  UI.FLOW.limit_min;	}
				if(UI.FLOW.val > UI.FLOW.limit_max){	UI.FLOW.val =  UI.FLOW.limit_max;	}
			}
			break;
		case BUTTON_8:
		case BUTTON_8_HOLD:
			if(UI.FLOW.val < UI.FLOW.limit_max){
				UI.FLOW.val	+= 1;
			}
			break;
	}

	return;
}


/*************************************************************************************************
* Function Name : 	app_UI_screen_paracube_cal
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	    Date d/m/y		Programmer		Reason for Change
* 0.1.0		    27/08/18 		W. Paul			Created
*************************************************************************************************/
void app_UI_screen_paracube_cal(uint8_t touch_status, uint8_t but_status, bool init)
{
	static uint8_t		rebuild_screen;
	char				str[100];
	static uint32_t		sys_tic_timer_1s;
	static uint8_t		time_rem_s;


	if(init){
		UI.screen			=  UI_PARACUBE_CAL_WAIT;
		UI.screen_last		=  UI_PARACUBE_CAL;
		app_selfcheck_AudioMute(0);
		
		UI.but_used		=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used	=  1;
		
		app_touchscreen_button_disable_all();
		
		app_selfcheck_AckClearStatus_all();
		
		//background
		LCD_FillRect(	0,	0,	650, LCD_DISP_HEIGHT,	COLOUR_WELCOME_BACKGROUND);
		if(UI.demo_mode){
			LCD_DrawRect_n(1,	1,	650-2, LCD_DISP_HEIGHT-1-2, 4, COLOUR_RED);
		}
	//************************************************
	//Sidebar
		UI.sidebar.endis	=  1;
		UI.sidebar.but1		=  BUT_BLANK;
		UI.sidebar.but2		=  BUT_BLANK;
		UI.sidebar.but3		=  BUT_BLANK;

		app_UI_screen_side_bar(BUILD_ALL_NEW);

	//************************************************
	//Title
		LCD_DispText_option(	25,  5, 600, 50,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h, LanguageStr(LangStr_CalO2));

	//************************************************
		rebuild_screen = 1;
	}

	if(rebuild_screen == 1){
		rebuild_screen	=  2;
		//Text
		LCD_DispText_option(				 0,  105+50, 650, 50,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow37h, LanguageStr(LangStr_Query));	//x,y,w,h,justification,col,font,txt
		LCD_DispText_option(				 0,  170+50, 650, 50,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow37h, LanguageStr(LangStr_CalTime));	//x,y,w,h,justification,col,font,txt

		//- button
		LCD_DrawSoftRect_1(					100,  350, 150, 75, 10, 	COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_6,100,  350, 150, 75);
		LCD_DispText_option(				100,  350, 150, 75,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h, LanguageStr(LangStr_Yes));	//x,y,w,h,justification,col,font,txt

		//+ button
		LCD_DrawSoftRect_1(					400,  350, 150, 75, 10, 	COLOUR_BLACK);
		app_touchscreen_button_add(BUTTON_7,400,  350, 150, 75);
		LCD_DispText_option(				400,  350, 150, 75,		CENTER,		COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h, LanguageStr(LangStr_No));	//x,y,w,h,justification,col,font,txt

		sys_tic_timer_1s	= 0xffffffff;

	}
	if(rebuild_screen == 3){
		rebuild_screen	=  4;
		//Text
		LCD_FillRect(	0	,105+50,	650, 50,	COLOUR_WELCOME_BACKGROUND);	//cover text
		LCD_FillRect(	0	,170+50,	650, 50,	COLOUR_WELCOME_BACKGROUND);	//cover text
		LCD_FillRect(	100-1	,350-1,	 150+2,  75+2,	COLOUR_WELCOME_BACKGROUND);	//cover button1
		LCD_FillRect(	400-1	,350-1,	 150+2,  75+2,	COLOUR_WELCOME_BACKGROUND);	//cover button1
		LCD_DispText_option(				 0,  105+50, 650, 50,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow37h, LanguageStr(LangStr_Wait));	//x,y,w,h,justification,col,font,txt
		sys_tic_timer_1s	=  0;
		time_rem_s			=  76;
	}


	if(sys_tic_rd() > sys_tic_timer_1s){
		sys_tic_timer_1s	=  sys_tic_rd() + 1000;
		if(time_rem_s >0){		time_rem_s			-= 1;	}
		//every 1 second
		sprintf(str,"  %ds %s  ",time_rem_s, LanguageStr(LangStr_Remaining));
		LCD_DispText_option(				 0,  170+50, 650, 50,		CENTER,COLOUR_BLACK,COLOUR_WELCOME_BACKGROUND, &fontArialNarrow37h, str);	//x,y,w,h,justification,col,font,txt

		if(app_Pneumatics.Cal_status == O2_CAL_FINISHED){
			app_Pneumatics.Cal_status	=  O2_CAL_IDLE;	//end the process
			app_UI_screen_lock_timer_reset(1);	//unlock screen
			app_selfcheck_AckClearStatus_all();
			UI.screen		= UI_MODE_SETUP;
		}
		else if((app_Pneumatics.Cal_status	== O2_CAL_ERR_NO_GAS)			||
				(app_Pneumatics.Cal_status	== O2_CAL_ERR_NO_GAS_ATSTART)	||
				(app_Pneumatics.Cal_status	== O2_CAL_ERR_NO_SENSOR)		){
			app_Pneumatics.Cal_status	=  O2_CAL_IDLE;	//end the process
			app_UI_screen_lock_timer_reset(1);	//unlock screen
			app_selfcheck_AckClearStatus_all();
			app_UI_screen_popup_setup(POPUP_STARTUP_CAL_FAILURE,UI_MODE_SETUP,UI_MODE_SETUP,POPUP_NOTIMEOUT);
		}
	}

	app_UI_screen_side_bar(BUILD_REFRESH);



	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
		//	app_UI_silence_alarm();
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	switch(touch_status){
//		case BUTTON_TIME:
//		case BUTTON_TIME_HOLD:

//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:

		case BUTTON_SIDE1:
		case BUTTON_SIDE1_HOLD:
			break;
		case BUTTON_SIDE2:
		case BUTTON_SIDE2_HOLD:
			break;
		case BUTTON_SIDE3:
		case BUTTON_SIDE3_HOLD:
			break;

		//************************************************
		case BUTTON_6:
		case BUTTON_6_HOLD:
			app_Pneumatics.Cal_status	=  O2_CAL_START;
			app_selfcheck_AudioMute(1);
				//because cal runs pid control modes, all alarms are being monitored
				//as there is no patient, we get alarms going off
			
			csp_paracube_timeout_read(1);		// Reset paracube error timeout
			
			rebuild_screen	=  3;
			break;

		case BUTTON_7:
		case BUTTON_7_HOLD:
//			app_Pneumatics.startup_o2_cal_status		=  2;	//enabling this will mean system will alarm and hold a yellow allert until calibration has been done
//																// will only alarm once as single alert flag is set
			UI.screen		= UI_MODE_SETUP;
			break;

		//************************************************
		case BUTTON_JUST_RELEASED:
		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			break;
	}

	return;
}


/*************************************************************************************************
* Function Name :       app_UI_screen_autooff
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		24/10/18	W. Paul			Created
* 0.2.0         11/06/26        A.Kane                  Update to Alarm Audio Call
*************************************************************************************************/
void 	app_UI_screen_autooff(		uint8_t touch_status, uint8_t but_status, bool init)
{
	char				str[100];
	static uint32_t		sys_tic_timer_1s;


	if(init){
		UI.screen_last		=  UI_AUTOOFF;
		UI.screen			=  UI_AUTOOFF_WAIT;

		app_selfcheck_AudioMute(1);
		UI.screen_lock_timer	=  SCREEN_LOCK_TIME;

		UI.but_used		=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used	=  1;

		app_touchscreen_button_disable_all();


	//background
		LCD_FillRect(	0,	0,	650, LCD_DISP_HEIGHT,	COLOUR_RED);
		if(UI.demo_mode){
			LCD_DrawRect_n(1,	1,	650-2, LCD_DISP_HEIGHT-1-2, 4, COLOUR_RED);
		}
	//************************************************
	//Sidebar
		UI.sidebar.endis	=  1;
		UI.sidebar.but1		=  BUT_BLANK;
		UI.sidebar.but2		=  BUT_BLANK;
		UI.sidebar.but3		=  BUT_BLANK;

		app_UI_screen_side_bar(BUILD_ALL_NEW);

	//************************************************
	//Title
		LCD_DispMonoBitmapTrans(	325-35,	50	,	"IMG_WarningIcon"  		,COLOUR_BLACK);
		LCD_DispText_option(		25, 100, 600, 50,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h, LanguageStr(LangStr_BatLevCrit) );		//height_check
		if(UI.pneumatics_mode){
			app_UI_stop_therapy(0);			// Do not remove flag of therapy stopped
			LCD_DispText_option(	25,	150, 600, 50,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h, LanguageStr(LangStr_TherHasStopped) );	//height_check
		}
		LCD_DispText_option(		25, 300, 600, 50,		CENTER,COLOUR_BLACK,TRANSPARENT, &fontArialNarrow50h, LanguageStr(LangStr_SwitchOffIn) );		//height_check

		app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_SHUTDOWN);
		api_audio_alarm(ALARM_PRIORITY_HIGH, 0);	//start audio alarm

		sys_tic_timer_1s	=  sys_tic_rd();
	//************************************************

	}

	if(sys_tic_rd() >= sys_tic_timer_1s){
		sys_tic_timer_1s	=  sys_tic_rd() + 1000;
		if(autooff_time_period >0){
			autooff_time_period			-= 1;
			sprintf(str,"  %ds  ",autooff_time_period);
			LCD_DispText_option(	 25,  350, 600, 50,		CENTER,COLOUR_BLACK,COLOUR_RED, &fontArialNarrow50h, str);	//x,y,w,h,justification,col,font,txt
		}
		else{
			system_shutdown(RESET_SYSTEM);
		}
	}
	app_UI_screen_side_bar(BUILD_REFRESH);
	app_UI_screen_lock_timer_reset(1);		//don't lock screen


	if(battery_charger.status.bits.ACP){
		app_UI_stop_therapy(1);			// Update flag of therapy stopped
		if(autooff_time_period == 10){	UI.screen	=  UI_WELCOME;		}
		else{							UI.screen	=  UI_MODE_SETUP;	}
		api_audio_play_file_stop();
	}


	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:

			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			if(UI.but_used == 0){
				UI.but_used	=  1;
				app_UI_screen_popup_setup(POPUP_SHUTDOWN,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_SHUTDOWN_TIMEOUT);
			}
			break;
	}

	switch(touch_status){
//		case BUTTON_TIME:
//		case BUTTON_TIME_HOLD:

//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:

		case BUTTON_SIDE1:
		case BUTTON_SIDE1_HOLD:
			break;
		case BUTTON_SIDE2:
		case BUTTON_SIDE2_HOLD:
			break;
		case BUTTON_SIDE3:
		case BUTTON_SIDE3_HOLD:
			break;



		//************************************************
		case BUTTON_JUST_RELEASED:
		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			break;
	}

	return;
}


/*************************************************************************************************
* Function Name : 	app_UI_screen_popup
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/01/18	W. Paul			Created
* 0.2.0         12/06/26        A. Kane                 Update to implement three alarm priority colours
*************************************************************************************************/
void app_UI_screen_popup(uint8_t touch_status, uint8_t but_status, bool init)
 {
	char				str1[70];
	char				str2[70];
	char				str3[70];
	char 				str4[70];			// added string for override change
	uint16_t			err_str_len;
	uint16_t			pop_x	=  30;
	uint16_t			pop_y	=  140;
	uint16_t			pop_w	=  590;
	uint16_t			pop_h	=  200;

	uint8_t				but_y	=  0;
	uint8_t				but_y1	=  0;
	uint8_t				but_y2	=  0;		// added another y button for override change
	uint8_t				but_n	=  0;
	uint8_t				full_scr;
	static LCD_COLOR	bg_colour;
	static LCD_COLOR	txt_colour;
	static LCD_COLOR	last_bg_colour;
	static uint32_t		sys_tic_time;

	static uint32_t		last_fault_no;
	uint8_t				fault_no;
	char* 				lead_str;
	char* 				res_str;
	char				fault_str[50];
	uint8_t 			res;
	uint8_t				noofstrings	=  1;
	static uint16_t		        fill_width;
        static ALARM_PRIORITY_e         last_alarm_priority;



	if(init){
		UI.screen			=  UI_POPUP_WAIT;
		//UI.screen_last		=  UI_POPUP;		//not here because we need screen_last in this screen to know where to return to
		UI.but_used		=  1;	//0 released, 1 press, 2 press and held
		UI.touch_used	=  1;
		last_fault_no	=  0xff;
                last_alarm_priority =  (ALARM_PRIORITY_e)0xff;
		sys_tic_time	=  0;

		if(UI.screen_last <= UI_ENG_CALIBRATE_SENSOR_WAIT){
			fill_width	= 800;
			UI.sidebar.endis	=  0;
		}
		else{
			fill_width	= 650;
			UI.sidebar.endis	=  1;
		}

		UI.sidebar.but1		=  BUT_BLANK;
		UI.sidebar.but2		=  BUT_BLANK;
		UI.sidebar.but3		=  BUT_BLANK;

		last_bg_colour		=  (LCD_COLOR)0xfffe;

		UI.page_timer_main_return	=  0;	//disable return to main timeout	causes problems when leaving popup

		app_touchscreen_button_disable_all();
		app_UI_screen_side_bar(BUILD_ALL_NEW);
	}

	if(UI.popup.type == POPUP_SYSTEM_ALARM){
                if(last_alarm_priority != app_selfcheck_SysAlarm_Priority()){
                        init = 1;
                        }
		if(sys_tic_rd() >= sys_tic_time){
			sys_tic_time	=  sys_tic_rd() + 2000;

			fault_no	=  app_selfcheck_next_fault();
			if( SelfCheckRes[fault_no].flags.bits.status_live < SelfCheckRes[fault_no].flags.bits.status){
				fault_no		= 0xff;
			}

			if( fault_no != 0xff){
				if( (last_fault_no    != fault_no) || (last_alarm_priority != app_selfcheck_SysAlarm_Priority()) ){
                                      init = 1;
                                }
			}
			else{
				UI.screen			=  UI.popup.back;
				UI.popup.timeout	=  0;
				init 				=  0;
				return;
			}

			printf("\r\nDisplaying Fault =%d",fault_no);
		}

	}

	if(init){
		switch(UI.popup.type){
			case POPUP_UNKNOWN:
				printf("\r\nUnknown Popup type!");
				sprintf(str1,LanguageStr(LangStr_Unknown));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_RED;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  1;
				break;
			case POPUP_SHUTDOWN:
				sprintf(str1,LanguageStr(LangStr_Shutdown));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_RED;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_SYSTEM_ALARM:
				sprintf(str1,LanguageStr(LangStr_Silence));
				but_y		=  1;
				but_n		=  1;
				last_fault_no	=  fault_no;
                                last_alarm_priority =  app_selfcheck_SysAlarm_Priority();
				app_selfcheck_result_str(STATUS,fault_no,&lead_str,&res_str,&res);
				bg_colour       =  colour_alarm_priority[SelfCheckRes[fault_no].flags.bits.alarm_priority];
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  1;
				break;
			case POPUP_SCREEN_UNLOCK:
				sprintf(str1,LanguageStr(LangStr_Unlock));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_SCREEN_BACKGROUND;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_START_THERAPY:
				switch(UI.select_mode){
					case PMODE_CPAP:			sprintf(str1, LanguageStr(LangStr_StartCpap));		break;
					case PMODE_CPAP_PAED:		sprintf(str1, LanguageStr(LangStr_StartPaed));		break;
					case PMODE_CPAP_HELMET:		sprintf(str1, LanguageStr(LangStr_StartHelmet));	break;
					case PMODE_BUBBLE_PAP:		sprintf(str1, LanguageStr(LangStr_StartBubble));	break;
					case PMODE_HFOT:			sprintf(str1, LanguageStr(LangStr_StartHFOT));		break;
					case PMODE_POINT:			sprintf(str1, LanguageStr(LangStr_StartPoint));		break;
				}

			//	if(battery_charger.mode < BAT_DISCHARGING){		//slower to respond as there is filtering
				if(battery_charger.status.bits.ACP){			//should be imediate
					noofstrings	=  1;
				}
				else{
					sprintf(str2, LanguageStr(LangStr_BatteryOnly));
					noofstrings	=  2;
				}
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_GREEN;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_STOP_THERAPY:
				sprintf(str1, LanguageStr(LangStr_Stop));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_CONFIRM_OVERRIDE:
				sprintf(str1, LanguageStr(LangStr_FlowOver));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_EXIT_OVERRIDE:
				sprintf(str1, LanguageStr(LangStr_FlowOverExitAdapt)); // <--- added this line to test
				sprintf(str2, LanguageStr(LangStr_FlowOverExit));
				sprintf(str3, LanguageStr(LangStr_FlowOverAdjust)); /// this is where override change is needed - ss
				sprintf(str4, LanguageStr(LangStr_NoChange));
				noofstrings	=  4;

				//	pop_x	=  100;
				pop_y	=  30;
				//	pop_w	=  450;
				pop_h	=  420; //370
                but_y       =  1;	    //exit and adopt   but 20
				but_y1		=  1;		//Exit			   but 6
				but_y2		=  1;		//Adjust		   but 8
				but_n		=  1;		//No change		   but 7
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_CONFIRM_CHANGE_FLOW:
				sprintf(str1, LanguageStr(LangStr_FlowSetting));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_CONFIRM_CHANGE_ALARMS:
				sprintf(str1, LanguageStr(LangStr_AlarmChange));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_CONFIRM_CHANGE_NEBULISER:
				sprintf(str1, LanguageStr(LangStr_NebuliserChange));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_CONFIRM_AC_UNPLUGGED:
				sprintf(str1, LanguageStr(LangStr_MainSupply));
				sprintf(str2, LanguageStr(LangStr_BatteryOnly));
				noofstrings	=  2;
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_REDO_STARTUP_CAL:
				sprintf(str1, LanguageStr(LangStr_Cal02Now));
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_CONFIRM_CHANGE_NEBULISER_FLOW:
				sprintf(str1, LanguageStr(LangStr_ConNebChange));
				sprintf(str2, LanguageStr(LangStr_FlowInc));
				noofstrings	=  2;
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_STARTUP_CAL_FAILURE:
				sprintf(str1, LanguageStr(LangStr_CalFail));
				but_y		=  1;
				but_n		=  0;
				bg_colour	=  COLOUR_RED;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_START_THERAPY_NO_AIR:
				switch(UI.select_mode){
					case PMODE_CPAP:			sprintf(str1, LanguageStr(LangStr_StartCpap));			break;
					case PMODE_CPAP_PAED:		sprintf(str1, LanguageStr(LangStr_StartPaed));			break;
					case PMODE_CPAP_HELMET:		sprintf(str1, LanguageStr(LangStr_StartHelmet));		break;
					case PMODE_BUBBLE_PAP:		sprintf(str1, LanguageStr(LangStr_StartBubble));		break;
					case PMODE_HFOT:			sprintf(str1, LanguageStr(LangStr_StartHFOT));			break;
					case PMODE_POINT:			sprintf(str1, LanguageStr(LangStr_StartPoint));			break;
				}
				sprintf(str2, LanguageStr(LangStr_NoAir));
				noofstrings =  2;
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  1;
				break;
			case POPUP_START_THERAPY_NO_O2:
				switch(UI.select_mode){
					case PMODE_CPAP:			sprintf(str1, LanguageStr(LangStr_StartCpap));			break;
					case PMODE_CPAP_PAED:		sprintf(str1, LanguageStr(LangStr_StartPaed));			break;
					case PMODE_CPAP_HELMET:		sprintf(str1, LanguageStr(LangStr_StartHelmet));		break;
					case PMODE_BUBBLE_PAP:		sprintf(str1, LanguageStr(LangStr_StartBubble));		break;
					case PMODE_HFOT:			sprintf(str1, LanguageStr(LangStr_StartHFOT));			break;
					case PMODE_POINT:			sprintf(str1, LanguageStr(LangStr_StartPoint));			break;
				}
				sprintf(str2, LanguageStr(LangStr_NoO2));
				noofstrings =  2;
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  1;
				break;
			case POPUP_2GAS_SUPPLY_AVAIL:
				sprintf(str1, LanguageStr(LangStr_FlowQuery));
				sprintf(str2, LanguageStr(LangStr_TwoGas));
				noofstrings =  2;
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_SCREEN_BACKGROUND;	//COLOUR_YELLOW;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;
				break;
			case POPUP_START_THERAPY_NO_GAS:
				switch(UI.select_mode){
					case PMODE_CPAP:			sprintf(str1, LanguageStr(LangStr_StartCpap));			break;
					case PMODE_CPAP_PAED:		sprintf(str1, LanguageStr(LangStr_StartPaed));			break;
					case PMODE_CPAP_HELMET:		sprintf(str1, LanguageStr(LangStr_StartHelmet));		break;
					case PMODE_BUBBLE_PAP:		sprintf(str1, LanguageStr(LangStr_StartBubble));		break;
					case PMODE_HFOT:			sprintf(str1, LanguageStr(LangStr_StartHFOT));			break;
					case PMODE_POINT:			sprintf(str1, LanguageStr(LangStr_StartPoint));			break;
				}
				sprintf(str2, LanguageStr(LangStr_NoGAS_Supply));
				noofstrings =  2;
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_ORANGE;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  1;
				break;
			default:
				sprintf(str1,"unknown Fault");
				sprintf(str2,"type %d",UI.popup.type);
				noofstrings =  2;
				but_y		=  1;
				but_n		=  1;
				bg_colour	=  COLOUR_SCREEN_BACKGROUND;	//COLOUR_YELLOW;
				txt_colour	=  COLOUR_BLACK;
				full_scr	=  0;

		}


	//background



		if(bg_colour !=  last_bg_colour){
			last_bg_colour	=  bg_colour;
			if(full_scr == 1){
				LCD_FillRect(							0					,0					,fill_width		,LCD_DISP_HEIGHT,	bg_colour);
			}
			else{
				LCD_FillRect(							0					,0					,fill_width		,LCD_DISP_HEIGHT,	COLOUR_SCREEN_BACKGROUND);
				LCD_FillSoftRect(						pop_x				,pop_y				,pop_w		,pop_h		,10		,bg_colour);
			}
			LCD_DrawSoftRect_n(							pop_x				,pop_y				,pop_w		,pop_h		,10	,3	,COLOUR_WHITE);
		}
		//specific txt
		if(noofstrings == 1){
			if(LCD_GetStringWidth(&fontArialNarrow50h, str1)			>  (pop_w-10)){
				err_str_len												=  strlen(str1);
				err_str_len												/= 2;
				strtok(&str1[err_str_len],								" ");
				strcpy(str2,											(str1 + strlen(str1) + 1));
				noofstrings												=  2;
			}
		}// Do not join - can result in noofstrings == 2
		
		if(noofstrings == 1){
			LCD_DispText_option( 					pop_x+5				,pop_y+25			,pop_w-10	,50		,CENTER	,txt_colour,TRANSPARENT, &fontArialNarrow50h, str1);	//x,y,w,h,justification,col,font,txt
		}
		else if(noofstrings == 2){										// Issue will only be with second string
			if(LCD_GetStringWidth(&fontArialNarrow50h, str2)			>  (pop_w-10)){
				LCD_DispText_option( 				pop_x+5				,pop_y+10			,pop_w-10	,40		,CENTER	,txt_colour,TRANSPARENT, &fontArialNarrow37h, str1);	//x,y,w,h,justification,col,font,txt
				LCD_DispText_option( 				pop_x+5				,pop_y+50			,pop_w-10	,40		,CENTER	,txt_colour,TRANSPARENT, &fontArial26h, str2);			//x,y,w,h,justification,col,font,txt
			}
			else{
				LCD_DispText_option( 				pop_x+5				,pop_y+10			,pop_w-10	,40		,CENTER	,txt_colour,TRANSPARENT, &fontArialNarrow37h, str1);	//x,y,w,h,justification,col,font,txt
				LCD_DispText_option( 				pop_x+5				,pop_y+50			,pop_w-10	,40		,CENTER	,txt_colour,TRANSPARENT, &fontArialNarrow37h, str2);	//x,y,w,h,justification,col,font,txt
			}
		}
		else if (noofstrings == 3) {
			LCD_DispText_option( 					pop_x+25+85+10		,pop_y+pop_h-85-25-200	,pop_w-25-85-20	,85		,CENTER_LEFT	,txt_colour,TRANSPARENT, &fontArialNarrowBold31h, str1);	//x,y,w,h,justification,col,font,txt
			LCD_DispText_option( 					pop_x+25+85+10		,pop_y+pop_h-85-25-100	,pop_w-25-85-20	,85		,CENTER_LEFT	,txt_colour,TRANSPARENT, &fontArialNarrowBold31h, str2);	//x,y,w,h,justification,col,font,txt
			LCD_DispText_option( 					pop_x+25+85+10		,pop_y+pop_h-85-25		,pop_w-25-85-20	,85		,CENTER_LEFT	,txt_colour,TRANSPARENT, &fontArialNarrowBold31h, str3);	//x,y,w,h,justification,col,font,txt
		}
		else if (noofstrings == 4) {
			LCD_DispText_option( 					pop_x+25+85+10		,pop_y+pop_h-85-25-255	,pop_w-25-85-20	,85		,CENTER_LEFT	,txt_colour,TRANSPARENT, &fontArialNarrowBold31h, str1);	//x,y,w,h,justification,col,font,txt
			LCD_DispText_option( 					pop_x+25+85+10		,pop_y+pop_h-85-25-165	,pop_w-25-85-20	,85		,CENTER_LEFT	,txt_colour,TRANSPARENT, &fontArialNarrowBold31h, str2);	//x,y,w,h,justification,col,font,txt //SS Added another string for override
			LCD_DispText_option( 					pop_x+25+85+10		,pop_y+pop_h-85-25-75	,pop_w-25-85-20	,85		,CENTER_LEFT	,txt_colour,TRANSPARENT, &fontArialNarrowBold31h, str3);	//x,y,w,h,justification,col,font,txt
			LCD_DispText_option( 					pop_x+25+85+10		,pop_y+pop_h-85-25+15   ,pop_w-25-85-20	,85		,CENTER_LEFT	,txt_colour,TRANSPARENT, &fontArialNarrowBold31h, str4);	//x,y,w,h,justification,col,font,txt
		}
		

	//buttons
		if(noofstrings <= 2){	//hor option pair
			if(but_y){
				app_UI_button(				BUTTON_6,	pop_x+50			,pop_y+pop_h-85-25,85			,85					,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+50+10			,pop_y+pop_h-85-25+10, 			"IMG_Tic_65_65"  	,COLOUR_GREEN,COLOUR_BUTTON_BACK);
			}
			if(but_n){
				app_UI_button(				BUTTON_7,	pop_x+pop_w-50-85	,pop_y+pop_h-85-25	,85			,85					,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+pop_w-50+10-85,pop_y+pop_h-85-25+10, 			"IMG_Cross_65_65"  	,COLOUR_RED,COLOUR_BUTTON_BACK);
			}
			sprintf(str1, LanguageStr(LangStr_ConAct));
			LCD_DispText_option( 						pop_x+50+85			,pop_y+pop_h-85-25				,pop_w-((50+85)*2)	,85		,CENTER	,COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, str1);	//x,y,w,h,justification,col,font,txt
		}
		if(noofstrings == 3){	//vertical list
			if(but_y){
				app_UI_button(				BUTTON_6,	pop_x+25			,pop_y+pop_h-85-25-200			,85			,85			,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+25+10			,pop_y+pop_h-85-25-200+10		,"IMG_Tic_65_65"  		,COLOUR_GREEN,COLOUR_BUTTON_BACK);
			}
			if(but_y1){
				app_UI_button(				BUTTON_8,	pop_x+25			,pop_y+pop_h-85-25-100			,85			,85			,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+25+10			,pop_y+pop_h-85-25-100+10		,"IMG_Tic_65_65"  		,COLOUR_GREEN,COLOUR_BUTTON_BACK);
			}
			if(but_n){
				app_UI_button(				BUTTON_7,	pop_x+25			,pop_y+pop_h-85-25				,85			,85			,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+25+10			,pop_y+pop_h-85-25+10			,"IMG_Cross_65_65"  	,COLOUR_RED,COLOUR_BUTTON_BACK);
			}
			sprintf(str1, LanguageStr(LangStr_ConAct));
			LCD_DispText_option( 						pop_x+25			,pop_y+10	,pop_w-(25+25)		,40		,CENTER	,COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, str1);	//x,y,w,h,justification,col,font,txt
		}

		// added another if section for override change
			if(noofstrings == 4){	//vertical list
			if(but_y){
				app_UI_button(				BUTTON_20,	pop_x+25			,pop_y+pop_h-85-25-255			,85			,85			,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+25+10			,pop_y+pop_h-85-25-245		    ,"IMG_Tic_65_65"  		,COLOUR_GREEN,COLOUR_BUTTON_BACK);
			}
			if(but_y1){
				app_UI_button(				BUTTON_6,	pop_x+25			,pop_y+pop_h-85-25-165			,85			,85			,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+25+10			,pop_y+pop_h-85-25-155			,"IMG_Cross_65_65"  		,COLOUR_RED,COLOUR_BUTTON_BACK);
			}

			if(but_y2){
				app_UI_button(				BUTTON_8,	pop_x+25			,pop_y+pop_h-85-25-75			,85			,85			,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+25+10			,pop_y+pop_h-85-25-65			,"IMG_Tic_65_65"  		,COLOUR_GREEN,COLOUR_BUTTON_BACK);
			}         


			if(but_n){
				app_UI_button(				BUTTON_7,	pop_x+25			,pop_y+pop_h-85-25+15			,85			,85			,COLOUR_BUTTON_BACK);
				LCD_DispMonoBitmap(						pop_x+25+10			,pop_y+pop_h-85-25+25			,"IMG_Cross_65_65"  	,COLOUR_RED,COLOUR_BUTTON_BACK);
			}
			sprintf(str1, LanguageStr(LangStr_ConAct));
			LCD_DispText_option( 						pop_x+25			,pop_y+10	,pop_w-(25+25)		,40		,CENTER	,COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, str1);	//x,y,w,h,justification,col,font,txt
		}

	//POPUP_SYSTEM_ALARM err txt
		if(UI.popup.type == POPUP_SYSTEM_ALARM){
			LCD_DispMonoBitmapTrans(pop_x	,50	,	"IMG_Warning_Icon_60x60"		,COLOUR_BLACK);

			LCD_FillRect(			pop_x+80	,25	,pop_w-130	,100,bg_colour);
			LCD_DispText_option(	pop_x+80	,75	,pop_w-130	,50	,CENTER_LEFT	,COLOUR_BLACK	,TRANSPARENT, &fontArialNarrow50h, lead_str);
			sprintf(fault_str,		"#%d",		fault_no);
			LCD_DispText_option(	pop_x+80	,25	,pop_w-130	,50	,CENTER_LEFT	,COLOUR_BLACK	,TRANSPARENT, &fontArialNarrow50h, fault_str);
		}

//		if(UI.popup.type == POPUP_SCREEN_UNLOCK){
//			UI.screen_lock_timer	=  0;	//lock screen enabled
//			UI.no_return_timer	=  2000;
//			app_touchscreen_button_disable_all();
//		}
	}


	//auto return to main screen
	if(UI.popup.timeout == 1){
		UI.popup.timeout	=  0;
		UI.screen			=  UI.popup.back;
	}


	if((UI.sidebar.endis)){	//&&(full_scr	== 0)
		app_UI_screen_side_bar(BUILD_REFRESH);
	}

	switch(but_status){
		case BUT_NONE:
			UI.but_used	= 0;
			break;
		case BUT_ALARM:
		case BUT_ALARM_HOLD:
			break;
		case BUT_PWR:
			break;
		case BUT_PWR_HOLD:
			break;
	}

	switch(touch_status){
		case BUTTON_TIME:
		case BUTTON_TIME_HOLD:
			break;
//		case BUTTON_BATTERY:
//		case BUTTON_BATTERY_HOLD:
		case BUTTON_SIDE1:
		case BUTTON_SIDE1_HOLD:
		case BUTTON_SIDE2:
		case BUTTON_SIDE2_HOLD:
		case BUTTON_SIDE3:
		case BUTTON_SIDE3_HOLD:
			break;

		//************************************************
		// BUTTON 20 for new override setting
		case BUTTON_20:
		case BUTTON_20_HOLD:
			if(UI.touch_used == 0){
				UI.touch_used	=  2;
				switch(UI.popup.type){
					case POPUP_UNKNOWN:
						printf("\r\nUnknown Popup type!");
						break;
					case POPUP_EXIT_OVERRIDE:
						UI.flow_override	=  OVERRIDE_NONE;
						app_pneumatic_set_flow(UI.flowRate_override, app_Pneumatics.O2_conc_target);
						break;
						
					default:
						printf("\r\nDecline option not avail");

				}
				app_UI_screen_popup_exit(POPUP_FORWARD);	//POPUP_FORWARD
			}
			break;
				

		
		//accept
		case BUTTON_6:
		case BUTTON_6_HOLD:
			if(UI.touch_used == 0){
				UI.touch_used	=  2;
				switch(UI.popup.type){
					case POPUP_UNKNOWN:
						printf("\r\nUnknown Popup type!");
					case POPUP_SHUTDOWN:
						system_shutdown(RESET_SYSTEM);
						break;
					case POPUP_SYSTEM_ALARM:
						app_selfcheck_UserQuietAlarmStart();
						break;
					case POPUP_SCREEN_UNLOCK:
						app_UI_screen_lock_timer_reset(1);		//screen can't lock while in popup
						break;
					case POPUP_START_THERAPY:
					case POPUP_STOP_THERAPY:
					case POPUP_START_THERAPY_NO_O2:
					case POPUP_START_THERAPY_NO_AIR:
						if(UI.pneumatics_mode == 0){
//						if(	!(	(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_MATH) ||
//								(app_pneumatic_pid_mode_read() == PNEUMATIC_CTRL_PID_NORMAL)	) )
//						{
							if(battery_charger.status.bits.ACP == 0){
								app_selfcheck_start_therapy_in_Bat_mode();
							}
							if(UI.demo_mode == 0){
								app_pneumatic_pid_mode_set(PNEUMATIC_CTRL_PID_MATH);
								app_PP_start_treatment();
							}

							UI.pneumatics_mode	=  1;
							if(api_Stopwatch_running_read()==0){
//									api_Stopwatch_mode_set(STOPWATCH_MODE_RESET);
								api_Stopwatch_mode_set(STOPWATCH_MODE_START);
							}
							app_touchscreen_button_disable(BUTTON_SIDE1);
							UI.sidebar.but3	=  BUT_STOP;
						}
						else{
							app_UI_stop_therapy(1);
							app_touchscreen_button_enable(BUTTON_SIDE1);
							UI.sidebar.but3	=  BUT_START;
						}
						app_UI_screen_side_bar(BUILD_NEW);				//updatebutton on sidebar
						break;
					case POPUP_CONFIRM_OVERRIDE:
						UI.flow_override		=  OVERRIDE_ACTIVE;
						UI.flowRate_override	=  UI.FLOW.val;
						app_pneumatic_set_flow(UI.flowRate_override, app_Pneumatics.O2_conc_target);
						break;
					case POPUP_EXIT_OVERRIDE:
						UI.flow_override	=  OVERRIDE_NONE;
						app_pneumatic_set_flow(UI.flowRate, app_Pneumatics.O2_conc_target);
						break;
					case POPUP_CONFIRM_CHANGE_FLOW:
						UI.flowRate		= UI.FLOW.val;
						app_pneumatic_set_flow(		UI.flowRate,	UI.O2.val);
						app_pneumatic_set_nebuliser(UI.nebuliser);
						break;
					case POPUP_CONFIRM_CHANGE_ALARMS:
						app_PP_set_alarm_patient_pressure(	UI.Pmin.en_dis,	UI.Pmin.val,
															UI.Pmax.en_dis,	UI.Pmax.val		);
						app_PP_set_alarm_apnoea(			UI.Apnoea.val);
						app_PP_set_alarm_resperation_rate(	UI.Breathing_Freq.val);
						break;
					case POPUP_CONFIRM_CHANGE_NEBULISER:
						if(UI.nebuliser){		UI.nebuliser = 0;	}
						else{					UI.nebuliser = 1;	}
						app_pneumatic_set_nebuliser(UI.nebuliser);
						break;
					case POPUP_REDO_STARTUP_CAL:
						break;
					case POPUP_CONFIRM_CHANGE_NEBULISER_FLOW:
						if(UI.nebuliser){		UI.nebuliser = 0;	}
						else{					UI.nebuliser = 1;	}
						app_pneumatic_set_nebuliser(UI.nebuliser);
						break;
					case POPUP_STARTUP_CAL_FAILURE:
						break;
					case POPUP_2GAS_SUPPLY_AVAIL:
						break;
					case POPUP_START_THERAPY_NO_GAS:

						break;
					default:
						printf("\r\nAccept option not avail");
				}
				app_UI_screen_popup_exit(POPUP_FORWARD);	//POPUP_FORWARD
			}
			break;
	//decline
		case BUTTON_7:
		case BUTTON_7_HOLD:
			if(UI.touch_used == 0){
				UI.touch_used	=  2;
				switch(UI.popup.type){
					case POPUP_UNKNOWN:
						printf("\r\nUnknown Popup type!");
						break;
					case POPUP_CONFIRM_CHANGE_FLOW:
					case POPUP_CONFIRM_CHANGE_ALARMS:
					case POPUP_CONFIRM_CHANGE_NEBULISER:
					case POPUP_SHUTDOWN:
					case POPUP_SYSTEM_ALARM:
					case POPUP_REDO_STARTUP_CAL:
						break;
					case POPUP_SCREEN_UNLOCK:
						UI.screen_lock_timer	=  0;	//lock screen enabled
						UI.no_return_timer		=  2000;
						app_touchscreen_button_disable_all();
						break;
					case POPUP_CONFIRM_OVERRIDE:
						UI.flow_override =  OVERRIDE_AVAIL;
						break;
					case POPUP_EXIT_OVERRIDE:
						UI.flow_override =  OVERRIDE_ACTIVE;
						break;
					case POPUP_CONFIRM_CHANGE_NEBULISER_FLOW:
					case POPUP_STARTUP_CAL_FAILURE:
					case POPUP_START_THERAPY:
					case POPUP_STOP_THERAPY:
					case POPUP_START_THERAPY_NO_O2:
					case POPUP_START_THERAPY_NO_AIR:
					case POPUP_2GAS_SUPPLY_AVAIL:
					case POPUP_START_THERAPY_NO_GAS:
						break;
					default:
						printf("\r\nDecline option not avail");

				}
				app_UI_screen_popup_exit(POPUP_BACK);	//POPUP_FORWARD
			}
			break;
		case BUTTON_8:
		case BUTTON_8_HOLD:
			if(UI.touch_used == 0){
				UI.touch_used	=  2;
				switch(UI.popup.type){
					case POPUP_UNKNOWN:
						printf("\r\nUnknown Popup type!");
						break;
					case POPUP_EXIT_OVERRIDE:
						UI.screen			=  UI_FLOW_OVERRIDE;
						if(UI.pneumatics_mode != 0){
							UI.page_timer_main_return	=  RET_MAIN_SCR;
						}
						break;
					default:
						printf("\r\nDecline option not avail");

				}
			//	app_UI_screen_popup_exit(POPUP_FORWARD);	//POPUP_FORWARD
			}
			break;
//		case BUTTON_JUST_RELEASED:
		case BUTTON_RELEASED:
			UI.touch_used	=  0;
			break;
	}

	return;
}




/*************************************************************************************************
* Function Name : 	app_UI_screen_side_bar
* Description   : 	This Function build the UI side bar
* Arguments     : 	uint8_t build_type
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/10/17	W. Paul			Created
*************************************************************************************************/
void app_UI_screen_side_bar(uint8_t build_type)
{
	char					str[30];
	time_t 					rawtime;
	struct tm 				*timeinfo;
	static battery_mode_e	last_charging_status		=  BAT_UNKNOWN;
	static float			last_charging_percentage	=  101.1;
	uint16_t				pos_x;
	uint16_t				pos_y;
	uint8_t					i;
	LCD_COLOR				colour;
	static uint32_t         wheel_timeout	=  0xffffffff;
	static uint8_t			wheel_idx		=  0;
	
	if(UI.sidebar.endis	== 0)	return;

	if(build_type == BUILD_ALL_NEW){
		last_charging_status		=  BAT_UNKNOWN;
		last_charging_percentage	=  101.1;

		LCD_FillRect(	650,	0,	150, LCD_DISP_HEIGHT,	COLOUR_BLACK);

		//but time
		app_touchscreen_button_add(BUTTON_TIME,655,   5, 140, 60);
		app_touchscreen_button_disable(BUTTON_TIME);					//not active

		//but battery
		app_touchscreen_button_add(BUTTON_BATTERY,655,   69, 140, 60);
		app_touchscreen_button_disable(BUTTON_BATTERY);					//not active

		build_type = BUILD_NEW;
	}

	if(app_UI_screen_lock_read()){
		if(	(UI.sidebar.but2 != BUT_LOCKED)	&&
		   	(UI.screen != UI_POPUP)			&&
			(UI.screen != UI_POPUP_WAIT)		)
		{		//move to locked screen, remove all other buttons, enable unlock screen icon only
			UI.sidebar.but1	=  BUT_BLANK;
			UI.sidebar.but2	=  BUT_LOCKED;
			UI.sidebar.but3	=  BUT_BLANK;
			build_type		=  BUILD_NEW;
		}
	}

	if(build_type == BUILD_NEW){
		//but1
		LCD_FillSoftRect(		655, 167, 140, 100,10,	COLOUR_BLACK);
		if(UI.sidebar.but1){	// MENU
			LCD_DrawSoftRect_1(	655, 167, 140, 100, 10, COLOUR_WHITE);
			app_touchscreen_button_add(BUTTON_SIDE1,655, 167, 140, 100);
		}
		switch(UI.sidebar.but1){
			case BUT_BLANK:	
					break;
			case BUT_MODE:
				LCD_FillSoftRect(		655, 167, 140, 100, 10,	COLOUR_SCREEN_BACKGROUND);
				LCD_DispText_option(	655, 167, 140, 100,	CENTER,			COLOUR_BLACK,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Menu));
				break;
			default:
				printf("\r\nSidebar but1 = %d",UI.sidebar.but1);
		}

	//But2
		LCD_FillSoftRect(		655, 271, 140,100,10,COLOUR_BLACK);
		if(UI.sidebar.but2){	// settings
			LCD_DrawSoftRect_1(	655, 271, 140,100, 10, COLOUR_WHITE);
			if(UI.sidebar.but2 == BUT_LOCKED){	app_touchscreen_button_add(BUTTON_UNLOCK	,655, 271, 140, 100);	}	//same loc, but global button name
			else{								app_touchscreen_button_add(BUTTON_SIDE2		,655, 271, 140, 100);	}
		}
		switch(UI.sidebar.but2){
			case BUT_BLANK:			break;
			case BUT_LOCKED:	LCD_DispMonoBitmapTrans(656+30, 272+9, "IMG_Locked"		,COLOUR_RED);		break;
			case BUT_SETTINGS:	LCD_DispMonoBitmapTrans(656+37, 272+16, "IMG_Gear1"		,COLOUR_WHITE); 	break;
			default:
				printf("\r\nSidebar but2 = %d",UI.sidebar.but2);
		}


	//But3
		LCD_FillSoftRect(		655, 375, 140,100,10,	COLOUR_BLACK);
		if(UI.sidebar.but3){	// ok, start/stop, back(in setting)
			LCD_DrawSoftRect_1(	655, 375, 140,100, 10, COLOUR_WHITE);
			app_touchscreen_button_add(BUTTON_SIDE3,655, 375, 140, 100);
		}
		switch(UI.sidebar.but3){
			case BUT_BLANK:			break;
			case BUT_NEXT:	LCD_DispText_option(	656		,376, 138,98,	CENTER,			COLOUR_WHITE,TRANSPARENT, &fontArial48h, LanguageStr(LangStr_Ok));		break;
			case BUT_BACK:	LCD_DispMonoBitmapTrans(656+29	,379+9, 	"IMG_BackArrow",COLOUR_WHITE);		break;
			case BUT_START:	LCD_DispMonoBitmapTrans(656+29	,379+8, 	"IMG_Circle_Go",COLOUR_GREEN);		break;
			case BUT_STOP:	LCD_DispMonoBitmapTrans(656+29	,379+8, 	"IMG_Circle_X",	COLOUR_RED);		break;
			default:
				printf("\r\nSidebar but3 = %d",UI.sidebar.but3);
		}
		
	// Running logo initialised timers
		wheel_idx		=  0;
		wheel_timeout	=  sys_tic_rd() + 50;


		build_type = BUILD_REFRESH;
		UI.sidebar.timer_n	=  0;
	}
	if(build_type == BUILD_REFRESH){
		if(UI.sidebar.timer_n == 0){
			UI.sidebar.timer_n	= 1000;

		//demo_mode
			switch(UI.demo_mode){
				case 1:		LCD_DispText_option(	655, 130, 140, 35,	CENTER,			COLOUR_RED,TRANSPARENT, &fontArialNarrowBold31h, LanguageStr(LangStr_Demo));		UI.demo_mode	=  2;	break;
				case 2:		UI.demo_mode	=  1;
				default:	LCD_FillRect(			655, 130, 140, 35 ,COLOUR_BLACK);
			}

		//time and date
			rawtime	= time(0);
			timeinfo = localtime (&rawtime);

			sprintf(str," %2d:%02d.%02d "	, timeinfo->tm_hour
											, timeinfo->tm_min
											, timeinfo->tm_sec			);
			LCD_DispText_option(660, 8,130,35,CENTER,COLOUR_WHITE,COLOUR_BLACK, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt

			//sprintf(str,"  %s %2d %s %d "	, LanguageStr( Lang_array_Day3[timeinfo->tm_wday]) /// Replaced this line with below to remove Day from UI 
			sprintf(str,"   %2d %s %d "		, timeinfo->tm_mday
											, LanguageStr( Lang_array_Month3[timeinfo->tm_mon])
											, timeinfo->tm_year + EPOCH_YEAR	);
			
			LCD_DispText_option(660,45,130,20,CENTER,COLOUR_WHITE,COLOUR_BLACK, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt

		//battery



			pos_x	=  660;
			pos_y	=  76;
			if((battery_charger.mode == BAT_DISCHARGING)&&(battery_charger.percentage < 21.0)){
				battery_charger.flash	^= 0x01;
			}
			else{
			  	battery_charger.flash	=  0;
			}
			if(battery_charger.percentage >= 21.0){	colour	=  COLOUR_GREEN;	}
			else{
			  	if(battery_charger.flash){			colour	=  COLOUR_RED;		}
			  	else{								colour	=  COLOUR_RED50;	}
			}
			if(battery_charger.mode == BAT_DISCHARGING){		//if not plugged in
				battery_charger.unplugged_icon	^= 0x01;
			}
			else{
				if(battery_charger.unplugged_icon){
					LCD_FillRect(pos_x,pos_y,150,25 ,COLOUR_BLACK);
				}
				battery_charger.unplugged_icon	=  0;
			}


			if(battery_charger.unplugged_icon){
				LCD_FillRect(pos_x,pos_y,150,25 ,COLOUR_BLACK);
				LCD_DispMonoBitmapTrans(	pos_x+30,	pos_y, "IMG_Unplugged_Icon"			,COLOUR_WHITE);
			}
			else{
				if(battery_charger.mode == BAT_DISCHARGING){
					LCD_FillRect(pos_x,pos_y,150,25 ,COLOUR_BLACK);
				}
				LCD_DrawRect_n(pos_x	,pos_y	, 56, 20, 2 ,COLOUR_WHITE);
				LCD_DrawRect_n(pos_x+56	,pos_y+5,  4, 10, 2 ,COLOUR_WHITE);
				for(i=0;i<(battery_charger.percentage/2);i++){
					LCD_FillRect(pos_x+3 +(i),pos_y+3, 1, 14 ,colour);
				}
				for(i=(uint8_t)(battery_charger.percentage/2);i<50;i++){
					LCD_FillRect(pos_x+3 +(i),pos_y+3, 1, 14 ,COLOUR_BLACK);
				}

				if(last_charging_percentage != battery_charger.percentage){
					last_charging_percentage = battery_charger.percentage;
					LCD_FillRect(pos_x+60,pos_y,70,25 ,COLOUR_BLACK);
				}
				if(	(battery_charger.mode			== BAT_CHARGING)	&&
					(battery_charger.percentage		>  99.0)			){
					hal_ltc2943_set_charge(0.99);
					sprintf(str, " %3.0f%%", 99.0);
				}
				else if(battery_charger.percentage	>= 10.0){
					sprintf(str, " %3.0f%%", battery_charger.percentage);
				}
				else{
					sprintf(str, " %2.1f%%", battery_charger.percentage);
				}
				LCD_DispText_option(pos_x+60,pos_y-5,70,35,CENTER_RIGHT,colour,COLOUR_BLACK, &fontArialNarrowBold31h, str);	//x,y,w,h,justification,col,font,txt		//check height
			}

			//bottom line
			pos_x	=  660;
			pos_y	=  106;
			if(last_charging_status != battery_charger.mode){
				last_charging_status = battery_charger.mode;
				LCD_FillRect(pos_x+2,pos_y, 126, 15 ,COLOUR_BLACK);
			}

			switch(battery_charger.mode){
				case BAT_CHARGING:
					sprintf(str," %s ", LanguageStr(LangStr_Charging) );
					break;
				case BAT_CHARGED:
					sprintf(str," %s ", LanguageStr(LangStr_Charged));
					break;
				case BAT_DISCHARGING:
					sprintf(str,"  %2d%s %02d%s  "		,battery_charger.sec_remaining / 3600, LanguageStr(LangStr_Hr)
														,(battery_charger.sec_remaining%3600) / 60, LanguageStr(LangStr_Min )
														);
					break;
				default:
					sprintf(str," -- ");
			}
			LCD_DispText_option(pos_x,pos_y,130,20,CENTER,COLOUR_WHITE,COLOUR_BLACK, &fontArial16h, str);	//x,y,w,h,justification,col,font,txt
		}
		
	// Running logo

	
		if(UI.pneumatics_mode !=0){  // in therapy mode
			if(sys_tic_rd()		>= wheel_timeout){
				wheel_timeout	=  sys_tic_rd() + 40;

				sprintf(str,"IMG_Progress_%02d", (wheel_idx+1));
				LCD_DispMonoBitmap(685, 167, str, COLOUR_GREEN, COLOUR_BLACK);

				if(++wheel_idx	>= 16){
					wheel_idx	=  0;
				}
			}
		}
		else if(wheel_timeout	<  0xffffffff){
			wheel_idx			=  0;
			wheel_timeout		=  0xffffffff;
		}
	}
	
	return;
}


/*************************************************************************************************
* Function Name : 	app_UI_silence_alarm
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		29/08/18	W. Paul			Created
*************************************************************************************************/
void app_UI_silence_alarm(void)
{
	if(app_selfcheck_SysAlarm_Rd()  ){	//alarm has been set off
		if( (UI.screen == UI_POPUP) || (UI.screen == UI_POPUP_WAIT) ){
			//alarm popup already here
		}
		else{
		    app_UI_screen_popup_setup(POPUP_SYSTEM_ALARM,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);
		}
	}
	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_NoTherapy_Popup_Alarm
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		29/08/18	W. Paul			Created
*************************************************************************************************/
void app_UI_NoTherapy_Popup_Alarm(void)
{
	if(	(app_selfcheck_SysAlarm_Rd() > 1 )	&&		//new alarm has been set off
	   	(UI.popup.no_return_timer == 0)			)
	{
		if(		(UI.screen	== UI_POPUP)		||
		   		(UI.screen	== UI_POPUP_WAIT)	){
			//alarm popup already here
		}
		else if((UI.screen	== UI_AUTOOFF_120)	||
				(UI.screen	== UI_AUTOOFF_10)	||
				(UI.screen	== UI_AUTOOFF)		||
				(UI.screen	== UI_AUTOOFF_WAIT)	){
			//do not interrupt autooff
		}
		else{
		    app_UI_screen_popup_setup(POPUP_SYSTEM_ALARM,UI_THIS_SCREEN,UI_THIS_SCREEN,POPUP_NOTIMEOUT);
		}
	}
	return;
}

/*************************************************************************************************
* Function Name : 	app_line_slider_draw
* Description   : 	This Function draws the slider bar
* Arguments     : 	uint16_t x				start x pos
					uint16_t y				start y pos
					uint16_t w				line width in pix
					uint16_t val_min		min value on the line
					uint16_t val_max		max value on the line
					uint16_t marker_gap		how often a markerr is placed on the line
					SLIDER_OPTION_e option	DISPLAY_LINE_ONLY,DISPLAY_NUMBERS_BELOW,DISPLAY_NUMBERS_ABOVE
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		24/10/17	W. Paul			Created
*************************************************************************************************/
void app_line_slider_draw(uint16_t x,uint16_t y,uint16_t w,uint16_t val_min,uint16_t val_max,uint16_t marker_gap,LCD_COLOR color,SLIDER_OPTION_e option,SLIDER_t *info)
{
	uint16_t	i;
	char		str[10];
	uint8_t		markers;
	uint16_t	x_gap;
	uint16_t	y_offset	=  0;

	//line
	LCD_FillRect(	x-1, y, w+2, 3,  		color);

	markers		=  val_max - val_min;
	markers		/=  marker_gap;
	x_gap		=  w;
	x_gap		/= markers;

	info->end_min		=  val_min;									//lowest value on line slider
	info->end_max		=  val_max;									//highest value on line slider
	info->pix_m			=  (float)w / (markers * marker_gap);		//convert from number to pixcels

	info->end_min_pix		=  app_line_slider_val2pix( info	,info->end_min);
	info->end_max_pix		=  app_line_slider_val2pix( info	,info->end_max);


	if(option == DISPLAY_NUMBERS_BELOW){	y_offset	=  y+10;	}
	if(option == DISPLAY_NUMBERS_ABOVE){	y_offset	=  y-25;	}
	for(i=0;i<=markers;i++){
		LCD_FillRect(	x-1+(i*x_gap), y-2, 2, 8,  	color);
		if( (option == DISPLAY_NUMBERS_BELOW)||(option == DISPLAY_NUMBERS_ABOVE) ){
			sprintf(str,"%d",val_min+(marker_gap*i) );
			LCD_DispText_option(x-20+(i*x_gap),y_offset,40,25,CENTER,color,TRANSPARENT, &fontArial22h, str);	//x,y,w,h,justification,col,font,txt		check_height
		}
	}

	return;
}

/*************************************************************************************************
* Function Name : 	app_line_slider_val2pix
* Description   : 	This Function converts the real value to pix loc fo rthe line sliders
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/08/18	W. Paul			Created
*************************************************************************************************/
uint16_t app_line_slider_val2pix(SLIDER_t *info,float val)
{
	float		temp_f;

	temp_f		=  val;
	temp_f		-= info->end_min;
	temp_f		*= info->pix_m;

	return((uint16_t)temp_f);
}

/*************************************************************************************************
* Function Name : 	app_line_slider_val2pix
* Description   : 	This Function converts the real value to pix loc fo rthe line sliders
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/08/18	W. Paul			Created
*************************************************************************************************/
float app_line_slider_pix2val(SLIDER_t *info,float pix)
{
	float		temp_f;

	temp_f		=  pix;
	temp_f		*= (float)(info->end_max - info->end_min);
	temp_f		+= info->end_min;

	return(temp_f);
}

/*************************************************************************************************
* Function Name : 	app_line_slider_wOFF_draw
* Description   : 	This Function draws the slider bar with OFF option
* Arguments     : 	uint16_t x				start x pos
					uint16_t y				start y pos
					uint16_t w				line width in pix
					uint16_t val_min		min value on the line
					uint16_t val_max		max value on the line
					uint16_t marker_gap		how often a markerr is placed on the line
					SLIDER_OPTION_e option	DISPLAY_LINE_ONLY,DISPLAY_NUMBERS_BELOW,DISPLAY_NUMBERS_ABOVE
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		24/10/17	W. Paul			Created
*************************************************************************************************/
void app_line_slider_wOFF_draw(uint16_t x,uint16_t y,uint16_t w,uint16_t val_min,uint16_t val_max,uint16_t marker_gap,LCD_COLOR color,SLIDER_OPTION_e option,SLIDER_t *info)
{
	uint16_t	i;
	char		str[10];
	uint8_t		markers;
	uint16_t	x_gap;
	uint16_t	y_offset	=  0;

	//line
	LCD_FillRect(	x-1, y, w+2, 3,  		color);

	markers		=  val_max - val_min;
	markers		/= marker_gap;
	markers		+= 1;
	x_gap		=  w;
	x_gap		/= markers;

	info->end_min		=  -marker_gap;								//lowest value on line slider
	info->end_max		=  val_max;									//highest value on line slider
	info->pix_m			=  (float)w / (float)(markers * (float)marker_gap);		//convert from number to pixcels

	info->end_min_pix		=  app_line_slider_val2pix( info	,info->end_min);
	info->end_max_pix		=  app_line_slider_val2pix( info	,info->end_max);

	if(option == DISPLAY_NUMBERS_BELOW){	y_offset	=  y+10;	}
	if(option == DISPLAY_NUMBERS_ABOVE){	y_offset	=  y-25;	}

	for(i=0;i<=markers;i++){
		LCD_FillRect(	x-1+(i*x_gap), y-2, 2, 8,  	color);
		if( (option == DISPLAY_NUMBERS_BELOW)||(option == DISPLAY_NUMBERS_ABOVE) ){
			if(i==0){
				LCD_DispMonoBitmapTrans(x-23,y_offset+1, "IMG_AlarmSilence",COLOUR_BLACK);
//				sprintf(str, LanguageStr(LangStr_Off) );
//				LCD_DispText_option(x-50+(i*x_gap),y_offset,100,22,CENTER,color,TRANSPARENT, &fontArial22h, str);	//x,y,w,h,justification,col,font,txt	//check_height
			}
			else{
				sprintf(str,"%d",val_min+(marker_gap*(i-1)) );
				LCD_DispText_option(x-20+(i*x_gap),y_offset+10,40,30,CENTER,color,TRANSPARENT, &fontArial26h, str);	//x,y,w,h,justification,col,font,txt	//check_height
			}

		}
	}

	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_irq
* Description   : 	This Function is the ms timer for this code
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		16/10/17	W. Paul			Created
*************************************************************************************************/
void	app_UI_irq(void)
{
	if(UI.page_timer_n_0){								UI.page_timer_n_0--;				}
	if(UI.page_timer_n_1					>  1){		UI.page_timer_n_1--;				}
	if(UI.sidebar.timer_n){								UI.sidebar.timer_n--;				}
	if(UI.screen_lock_timer){							UI.screen_lock_timer--;				}
	if(UI.no_return_timer){								UI.no_return_timer--;				}
	if(UI.popup.no_return_timer){						UI.popup.no_return_timer--;			}
	if(gas_debounce_timer){								gas_debounce_timer--;				}
	
	//only cound down to 1   0 = not active
	if(UI.page_timer_main_return			>  1){		UI.page_timer_main_return--;		}
	if(UI.popup.timeout						>  1){		UI.popup.timeout--;					}
	
	if(UI.pneumatics_mode					== 0){
		if(io_status_st_glb.Flow_Of_timer	<  7000){	io_status_st_glb.Flow_Of_timer++;	}
	}
	else{
		io_status_st_glb.Flow_Of_timer		=  0;
	}
	
	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_button
* Description   : 	This Function places a button on the screen
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/12/17	W. Paul			Created
*************************************************************************************************/
void app_UI_button(uint8_t id,uint16_t x,uint16_t y,uint16_t w,uint16_t h,LCD_COLOR colour )
{
	app_touchscreen_button_add(	id,	x,y,w,h);							//id,x,y,w,h
	LCD_FillSoftRect( 				x,y,w,h, 10, 	colour);
	LCD_DrawSoftRect_1(				x,y,w,h, 10, 	COLOUR_WHITE);

	return;
}



/*************************************************************************************************
* Function Name : 	app_UI_screen_lock_timer_reset
* Description   : 	This Function is called when UI is used to reset the auto lock timer
* Arguments     : 	uint8_t unlock	only reset timer if timer has not expired unless unlock input is set
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		29/06/18	W. Paul			Created
*************************************************************************************************/
void app_UI_screen_lock_timer_reset(uint8_t unlock)
{
	if((UI.screen_lock_timer != 0)||(unlock == 1)){
		if(UI.screen_lock_timer	== 0){
			printf("\r\nLock timer reset when at 0");
			UI.screen &= 0xFE;	//rebuild screen
		}
		UI.screen_lock_timer	=  SCREEN_LOCK_TIME;
	}
	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_screen_lock_read
* Description   : 	This Function reads the screen lock status
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		29/06/18	W. Paul			Created
*************************************************************************************************/
uint8_t app_UI_screen_lock_read(void)
{
  if(UI.screen_lock_timer){	return( 0);	}
	return(1);	//lock screen
}




/*************************************************************************************************
* Function Name : 	app_UI_screen_popup_setup
* Description   : 	This Function helps with the setup of popup windows
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/08/18	W. Paul			Created
*************************************************************************************************/
void app_UI_screen_popup_setup(	POPUP_TYPE_enum type,UI_STATUS back,UI_STATUS next,uint32_t	timeout)
{
	UI.popup.type			=  type;

	if(back == UI_THIS_SCREEN){	UI.popup.back	=  (UI_STATUS)(UI.screen & 0xFE); 	}  //drop last bit
	else{						UI.popup.back	=  back;							}
	if(next == UI_THIS_SCREEN){	UI.popup.next	=  (UI_STATUS)(UI.screen & 0xFE); 	}  //drop last bit
	else{						UI.popup.next	=  next;							}
	UI.popup.timeout		=  timeout;

	UI.screen				=  UI_POPUP;
	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_screen_popup_exit
* Description   : 	This Function helps with the setup of popup windows
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/08/18	W. Paul			Created
*************************************************************************************************/
void app_UI_screen_popup_exit(	POPUP_FB_enum forw_back)
{
	if(forw_back == POPUP_BACK){	UI.screen	=  UI.popup.back;	}
	else{							UI.screen	=  UI.popup.next;	}

	UI.popup.type				=  POPUP_UNKNOWN;
	UI.popup.timeout			=  0;
	UI.popup.no_return_timer	=  2000;
	return;
}

/*************************************************************************************************
* Function Name : 	app_UI_stop_therapy
* Description   : 	This Function stops therapy and clars relavent flags
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/12/18	W. Paul			Created
*************************************************************************************************/
void app_UI_stop_therapy(uint8_t setFlag)
{
	app_pneumatic_pid_mode_set(		PNEUMATIC_CTRL_PID_SHUTDOWN);
	if(setFlag){
		UI.pneumatics_mode			=  0;
	}
	UI.flow_override				=  OVERRIDE_NONE;
	if(api_Stopwatch_running_read()	==1){
		api_Stopwatch_mode_set(		STOPWATCH_MODE_STOP);
	}
	app_PP_stop_treatment();
	app_selfcheck_AckClearStatus_all();		//clear all current alarms

	return;
}


/*************************************************************************************************
* Function Name : 	app_UI_mode_availability_set_menu
* Description   : 	This Function is used to set what modes of operation are avaialble
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		12/08/19	W. Paul			Created
*************************************************************************************************/
uint8_t app_UI_mode_availability_set_menu(void)
{
/* Local Variables */
	uint8_t				rec_status_u8;
	uint8_t				rec_char_u8;

/* Code */
	rec_char_u8 = debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){

		switch(rec_char_u8){
			case ' ':
				printf("\r\n");
				printf("\r\n****Mode Availability menu****");
				printf("\r\nEsc  Exit Menu");
				printf("\r\nv    View current settings");
				printf("\r\ns    Save settings");
				printf("\r\n1    CPAP");
				printf("\r\n2    CPAP_PAED");
				printf("\r\n3    CPAP_HELMET");
				printf("\r\n4    BUBBLE_PAP");
				printf("\r\n5    HFOT");
				printf("\r\n6    POINT");

				break;

			case 0x1b:
				app_UI_parameters_rd();	//unsaved settings will be lost
				return(0);
				//break;
			case 's':
				printf("\r\nSave Settings");
				printf("\r\n...");
				app_UI_parameters_wr();
				printf("Done");
				//break;
			case 'v':
				printf("\r\nCurrent Cettings");
				printf("\r\n -CPAP         %d",UI.app_Pmode_available[PMODE_CPAP]			);
				printf("\r\n -CPAP Paed    %d",UI.app_Pmode_available[PMODE_CPAP_PAED]		);
				printf("\r\n -CPAP Helmet  %d",UI.app_Pmode_available[PMODE_CPAP_HELMET]	);
				printf("\r\n -Bubble PAP   %d",UI.app_Pmode_available[PMODE_BUBBLE_PAP]		);
				printf("\r\n -HFOT         %d",UI.app_Pmode_available[PMODE_HFOT]			);
				printf("\r\n -POINT        %d",UI.app_Pmode_available[PMODE_POINT]			);
				printf("\r\nend");
				break;

			case '1':	UI.app_Pmode_available[PMODE_CPAP] 			^= 0x01;	break;
			case '2':	UI.app_Pmode_available[PMODE_CPAP_PAED]		^= 0x01;	break;
			case '3':	UI.app_Pmode_available[PMODE_CPAP_HELMET]	^= 0x01;	break;
			case '4':	UI.app_Pmode_available[PMODE_BUBBLE_PAP]	^= 0x01;	break;
			case '5':	UI.app_Pmode_available[PMODE_HFOT]			^= 0x01;	break;
			case '6':	UI.app_Pmode_available[PMODE_POINT]			^= 0x01;	break;

		}
	}
	return(1);
}
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
