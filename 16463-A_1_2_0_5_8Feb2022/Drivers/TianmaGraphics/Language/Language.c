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
 * Filename    :  Language.c
 * Date Created:  Mon 18 Dec 2017 09:54:10 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdint.h>
#include <stdio.h>

#include "Language.h"

#include "hal_lcd.h"					//LCD
#include "hal_lcd_text.h"				//LCD
#include "csp_LCD_SSD1963.h"
#include "fonts.h"
#include "bitmaps.h"
#include "Colours.h"
#include "Language.h"
#include "csp_STM32_delay.h"
#include "hal_STM32_uart.h"
#include "csp_STM32_uart.h"

#include "app_system.h"

#include 	"csp_STM32_iwdg.h"


LANG_SELECT_enum	    cur_language		=  Lang_English;

static char const**	    languages[]			=  {	EnglishPhrase,
					    							FrenchPhrase,
					    							GermanPhrase,
						    						SpanishPhrase,
						    						DutchPhrase,
						    						ItalianPhrase,
                                                                                                ArabicPhrase,
					    							FinnishPhrase,
					    							NorwegianPhrase,
						    						PortuguesePhrase,
						    						GreekPhrase,
						    						IndonesianPhrase,
                                                                                                LatvianPhrase,
						    						PolishPhrase,
						    						RomanianPhrase,
                                                                                                SwedishPhrase,
						    						TurkishPhrase,
						    						VietnamesePhrase,
								    			};

LANG_STR_enum			Lang_array_Lang[]	=  {    LangStr_English,
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
												};
LANG_STR_enum			Lang_array_Month3[]	=  {    LangStr_Jan,
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
													LangStr_Dec
												};
LANG_STR_enum			Lang_array_Day3[]	=  {    LangStr_Sun,
                                                    LangStr_Mon,
													LangStr_Tue,
													LangStr_Wed,
													LangStr_Thur,
                                                    LangStr_Fri,
													LangStr_Sat
												};
LANG_STR_enum			lang_array_test[]	={		LangStr_Unset,
													LangStr_Pass,
													LangStr_Notice,
													LangStr_Warning,
													LangStr_Fault,
													LangStr_Critical_Fault
												};

LANG_STR_enum			lang_array_selftest[]	={	LangStr_Mem_Rd_Wr,		//Selftest
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
												};

char*	LangStr_Err		= "ER";
/*************************************************************************************************
* Function Name :	Language_set
* Description   : 	This Function sets the language for the product UI
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/12/17	W. Paul			Created
*************************************************************************************************/
void Language_set(LANG_SELECT_enum Lang)
{
	if(Lang < Lang_Max){
		cur_language	=  	Lang;
	}

	return;
}

/*************************************************************************************************
* Function Name :   LanguageStr
* Description   : 	This Function returns the phrase in the correct language
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/12/17	W. Paul			Created
*************************************************************************************************/
char* LanguageStr(LANG_STR_enum LangStr)
{
	if(LangStr < LangStr_Max){
		return((char*)languages[cur_language][LangStr]);
	}
	else{
		return(LangStr_Err);
	}
}


/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/12/17	W. Paul			Created
*************************************************************************************************/
void Lanugage_test(void)
{
	LANG_SELECT_enum	lan;
	LANG_STR_enum		str;
	char				title[40];

	printf("\r\nLanguage String Listings");
	printf("\r\nLChoose Language");
	printf("\r\n0 English	");
	printf("\r\n1 French	");
	printf("\r\n2 German	");
	printf("\r\n3 Spanish   ");
	printf("\r\n4 Dutch	    ");
	printf("\r\n5 Italian   ");
        printf("\r\n6 Arabic	");
	printf("\r\n7 Finnish	");
	printf("\r\n8 Norwegian	");
	printf("\r\n9 Portuguese   ");
	printf("\r\na Greek	    ");
	printf("\r\nb Indonesian   ");
        printf("\r\nc Latvian   ");
	printf("\r\nd Polish	    ");
	printf("\r\ne Romanian   ");
        printf("\r\nf Swedish   ");
	printf("\r\ng Turkish	    ");
	printf("\r\nh Vietnamese   ");
	uint8_t	rec_status_u8   =  0;
        uint8_t	rec_char_u8	=  0;


        while(rec_status_u8 == 0){
            rec_char_u8 = debug_getchar(0,&rec_status_u8);
        }

        if(rec_char_u8 >= '0' && rec_char_u8 <= '9'){
            lan = (LANG_SELECT_enum)(rec_char_u8 - '0');
        }else if(rec_char_u8 >= 'a' && rec_char_u8 <= 'h'){
            lan = (LANG_SELECT_enum)(rec_char_u8 - 'a' + 10);
        }else if(rec_char_u8 >= 'A' && rec_char_u8 <= 'H'){
            lan = (LANG_SELECT_enum)(rec_char_u8 - 'A' + 10);
        }else{
            lan = (LANG_SELECT_enum)0;
        }
	printf("\r\nChoose :-\b");
	if(lan >= Lang_Max){	lan =  (LANG_SELECT_enum)(Lang_Max-1);	}

	Language_set(lan);

	for(str = (LANG_STR_enum)0;str<LangStr_Max;str++){
		printf("\r\n%d",str);

		printf("\r\n %s",LanguageStr(str) );
		sprintf(title,"%d %s",str,EnglishPhrase[lan]);

		LCD_FillRect(			10	,10		,780	,140,	COLOUR_BLACK);
		LCD_DispText_option( 	10	,10		,780	,20		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &fontArial16h, title);

		LCD_DispText_option( 	10	,30		,780	,20		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &fontArial16h, LanguageStr(str));
		LCD_DispText_option( 	10	,50		,780	,25		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &fontArial22h, LanguageStr(str));
		LCD_DispText_option( 	10	,75		,780	,30		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &fontArialNarrowBold31h, LanguageStr(str));
		LCD_DispText_option( 	10	,105	,780	,50		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &fontArialNarrow50h, LanguageStr(str));

		Delay(1000);
		app_sys_watchdog_reload();
		Delay(1000);
		app_sys_watchdog_reload();
	}
	printf("\r\nFinished\r\n");
	return;
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
