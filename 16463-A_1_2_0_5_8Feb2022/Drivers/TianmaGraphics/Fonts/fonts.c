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
 *  Copyright 2018, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  fonts.c
 * Date Created:  Fri 07 Dec 2018 11:18:26 AM
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

#include "app_system.h"
#include "csp_STM32_delay.h"

#include 	"csp_STM32_iwdg.h"


/*************************************************************************************************
* Function Name : 	 Fonts_test()
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		07/12/18	W. Paul			Created
*************************************************************************************************/
void Fonts_test(void)
{
	char	c;
	char	str[40];
	uint8_t	i;
	BFC_FONT	font[]	=  {fontArial16h,fontArial22h,fontArialNarrowBold31h,fontArialNarrow50h};


	printf("\r\nFont Character Check");

	for(i=0;i<4;i++){
		str[0]	= 0;
	  	for(c = ' ';c<'/';c++){	sprintf(str,"%s%c",str,c);	}			//16
		LCD_FillRect(			10	,10		,780	,50		,COLOUR_BLACK);
		LCD_DispText_option( 	10	,10		,780	,50		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &font[i], str);

		str[0]	= 0;
		for(c = ':';c<'@';c++){	sprintf(str,"%s%c",str,c);	}			//7
		for(c = '[';c<'`';c++){	sprintf(str,"%s%c",str,c);	}			//7
		for(c = '{';c<'~';c++){	sprintf(str,"%s%c",str,c);	}			//7
		LCD_FillRect(			10	,60		,780	,50		,COLOUR_BLACK);
		LCD_DispText_option( 	10	,60		,780	,50		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &font[i], str);

		str[0]	= 0;	
		for(c = 'A';c<'Z';c++){	sprintf(str,"%s%c",str,c);	}			//26
		LCD_FillRect(			10	,110		,780	,50		,COLOUR_BLACK);
		LCD_DispText_option( 	10	,110		,780	,50		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &font[i], str);

		str[0]	= 0;		
		for(c = 'a';c<'z';c++){	sprintf(str,"%s%c",str,c);	}			//26
		LCD_FillRect(			10	,160	,780	,50		,COLOUR_BLACK);
		LCD_DispText_option( 	10	,160	,780	,50		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &font[i], str);

		Delay(1000);
		app_sys_watchdog_reload();
		Delay(1000);
		app_sys_watchdog_reload();
	}

	//numbers only
	str[0]	= 0;
	sprintf(str,"%c%c",' ','+');
	for(c = 0x2D;c<':';c++){		sprintf(str,"%s%c",str,c);	}
	LCD_FillRect(			10	,170	,780	,60		,COLOUR_BLACK);
	LCD_DispText_option( 	10	,170	,780	,60		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &fontArial48h, str);
	LCD_FillRect(			10	,230	,780	,60		,COLOUR_BLACK);
	LCD_DispText_option( 	10	,230	,780	,60		,CENTER_LEFT	,COLOUR_WHITE,COLOUR_BLACK, &fontArialNarrow60h, str);

	app_sys_watchdog_reload();
	printf("\r\nFinished\r\n");
	return;
}



/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
