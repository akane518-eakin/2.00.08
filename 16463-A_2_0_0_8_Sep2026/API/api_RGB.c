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
 * Filename    :  api_RGB.c
 * Date Created:  Tue 05 Sep 2017 02:24:44 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "api_RGB.h"
#include "csp_STM32_timer1.h"

#include "csp_STM32_uart.h"

/*********************************************************************************************************
 *		Local Variables
 ********************************************************************************************************/

/**********************************************************************************************************
 **********************************************************************************************************/
api_RGB_t	api_RGB;

/*************************************************************************************************
* Function Name : 	api_RGB_set
* Description   : 	This Function sets the colour and flash rate on the RGB LED
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/09/17	W. Paul			Created
*************************************************************************************************/
void api_RGB_set(uint32_t colour,api_RGB_speed_enum flash)
{
	api_RGB.col.a		=  colour;
	api_RGB.flash_rate	=  flash;

	api_RGB.flash_timer	=  0;
	timer1_ch2_config(api_RGB.col.b[0]);	//COLOUR_RED
	timer1_ch3_config(api_RGB.col.b[1]);	//COLOUR_GREEN
	timer1_ch4_config(api_RGB.col.b[2]);	//blue
	api_RGB.on_off		=  1;
	return;
}


/*************************************************************************************************
* Function Name : 	api_RGB_IRQ
* Description   : 	This Function should be put in an IRQ timer, so that the LEDS flash
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/09/17	W. Paul			Created
*************************************************************************************************/
void api_RGB_IRQ(void)
{
	if(api_RGB.flash_rate){
		api_RGB.flash_timer	+= 1;

		if(	(api_RGB.on_off	== 1)	&&
			(api_RGB.flash_timer >= (uint16_t)(api_RGB.flash_rate/2)	)	)
		{
			timer1_ch2_config(0);	//LED_BAT_R
			timer1_ch3_config(0);	//LED_BAT_G
			timer1_ch4_config(0);	//LED_BAT_B
			api_RGB.on_off		=  0;
		}
		else if(	(api_RGB.on_off	== 0)	&&
					(api_RGB.flash_timer >= (uint16_t)(api_RGB.flash_rate)	)	)
		{
			timer1_ch2_config(api_RGB.col.b[0]);	//LED_BAT_R
			timer1_ch3_config(api_RGB.col.b[1]);	//LED_BAT_G
			timer1_ch4_config(api_RGB.col.b[2]);	//LED_BAT_B
			api_RGB.on_off		=  1;
		}

		if(api_RGB.flash_timer >= (uint16_t)(api_RGB.flash_rate) ){
			api_RGB.flash_timer	=  0;
		}
	}
	return;
}


/**********************************************************************************************************
 * Function	Name :	api_IO_test
 * Description	 :	This function is used to test IO functions
 * Arguments	 : None
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		 8/10/12	William	Paul	Original Created
 **********************************************************************************************************/
uint8_t api_RGB_test(void)
{
/* Local Variables */
	uint8_t						rec_status_u8;
	uint8_t						rec_char_u8;
	static api_RGB_speed_enum	speed			=  RGB_SOLID;

/* Code	*/
	rec_char_u8	= debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){
		switch(rec_char_u8){
			case ' ':
				printf("\r\n");
				printf("\r\n****TST	RGB menu****");
				printf("\r\nEsc	 Exit Menu");
				printf("\r\n0    COLOUR_BLACK Off");
				printf("\r\nw    COLOUR_WHITE");
				printf("\r\nr    COLOUR_RED");
				printf("\r\ng    COLOUR_GREEN");
				printf("\r\nb    Blue");
				printf("\r\ny    COLOUR_YELLOW");
				printf("\r\nc    Cyan");
				printf("\r\nm    Magenta");
				printf("\r\n");
				printf("\r\n123  Flash speed");
				break;

			case 0x1b:
				return(0);
				//break;
			case '0':	api_RGB_set(RGB_BLACK,speed);		printf("\r\nBLACK   ");	break;
			case 'w':	api_RGB_set(RGB_WHITE,speed);		printf("\r\nWHITE   ");	break;
			case 'r':	api_RGB_set(RGB_RED,speed);			printf("\r\nRED     ");	break;
			case 'g':	api_RGB_set(RGB_GREEN,speed);		printf("\r\nGREEN   ");	break;
			case 'b':	api_RGB_set(RGB_BLUE,speed);		printf("\r\nBLUE    ");	break;
			case 'y':	api_RGB_set(RGB_YELLOW,speed);		printf("\r\nYELLOW  ");	break;
			case 'c':	api_RGB_set(RGB_CYAN,speed);		printf("\r\nCYAN    ");	break;
			case 'm':	api_RGB_set(RGB_MAGENTA,speed);		printf("\r\nMAGENTA ");	break;

			case '1':	speed	=  RGB_SOLID;				printf("\r\nSolid ");	break;
			case '2':	speed	=  RGB_SLOW_FLASH;			printf("\r\nSlow  ");	break;
			case '3':	speed	=  RGB_MED_FLASH;			printf("\r\nMed   ");	break;
			case '4':	speed	=  RGB_FAST_FLASH;			printf("\r\nFast  ");	break;
			default:
				printf("\r\n Invalid Command");
				break;
		}
	}

	return(1);
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
