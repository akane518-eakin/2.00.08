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
 * Filename    :  api_LEDs.c
 * Date Created:  Tue 24 Apr 2018 03:30:05 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "api_leds.h"
#include "csp_STM32_timer1.h"

#include "csp_STM32_uart.h"

/*********************************************************************************************************
 *		Local Variables
 ********************************************************************************************************/

LED_st		led[4];


/**********************************************************************************************************
 **********************************************************************************************************/


/*************************************************************************************************
* Function Name :	api_LED
* Description   : 	This Function
* Arguments     : 	uint8_t brightness
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		29/08/18	W. Paul			Created
*************************************************************************************************/
void 	api_LED(LED_enum led_no,LED_mode_enum mode,uint8_t brightness)
{
	led[led_no].mode				=  mode;
	led[led_no].max_brightness		=  brightness;
        
        if(led[led_no].flash_period_ms == 0){
        led[led_no].flash_period_ms = 1000;   // default 0.5Hz, preserves existing behaviour
        }

	return;
}




/*************************************************************************************************
* Function Name : 	api_LED_manager
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		24/04/18	W. Paul			Created
*************************************************************************************************/
void api_LED_manager(void)
{
	uint8_t		i;

	for(i=0;i<4;i++){
		switch(led[i].mode){
			case LED_OFF:
				led[i].req_brightness	=  0;
				break;
			case LED_ON:
				led[i].req_brightness	=  led[i].max_brightness;
				break;
			case LED_FLASH:
				if(led[i].ms_timer == 0){
					if(led[i].up_down){
						led[i].req_brightness	=  led[i].max_brightness;
						led[i].up_down			=  0;
					}
					else{
						led[i].req_brightness	=  0;
						led[i].up_down			=  1;
					}
                                        led[i].ms_timer = (led[i].flash_period_ms > 0) ? led[i].flash_period_ms : 1000;
				}
				break;
			case LED_RAMP:
				if(led[i].ms_timer == 0){
					if(led[i].up_down){
						if(led[i].req_brightness < led[i].max_brightness){
							led[i].req_brightness += 1;
						}
						else{
							led[i].up_down			=  0;
						}
						led[i].ms_timer		=  10;
					}
					else{
						if(led[i].req_brightness > 0){
							led[i].req_brightness -= 1;
						}
						else{
							led[i].up_down			=  1;
						}
						led[i].ms_timer		=  10;
					}
				}
				break;
		}

		if(led[i].cur_brightness != led[i].req_brightness){
			led[i].cur_brightness	=  led[i].req_brightness;

			switch(i){
				case LED_PWR:      	timer1_ch1_config(	led[i].cur_brightness );	break;
				case LED_BATR:		timer1_ch2_config(	led[i].cur_brightness );	break;
				case LED_BATG:		timer1_ch3_config(	led[i].cur_brightness );	break;
				case LED_ALM:		timer1_ch4_config(	led[i].cur_brightness );	break;
			}

		}
	}

	return;
}

/*************************************************************************************************
* Function Name : 	api_LED_irq
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		24/04/18	W. Paul			Created
*************************************************************************************************/
void api_LED_irq(void)
{
	uint8_t	i;

	for(i=0;i<4;i++){
		if(led[i].ms_timer){
			led[i].ms_timer	-= 1;
		}
	}

	return;
}

/**********************************************************************************************************
 * Function	Name :	api_LED_test
 * Description	 :	This function is used to test LED functions
 * Arguments	 : None
 * Returns		 : None
 * Notes		 : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		24/04/18	William	Paul	Original Created
 **********************************************************************************************************/
uint8_t api_LED_test(void)
{
/* Local Variables */
	uint8_t						rec_status_u8;
	uint8_t						rec_char_u8;

/* Code	*/
	rec_char_u8	= debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){
		switch(rec_char_u8){
			case ' ':
				printf("\r\n");
				printf("\r\n****TST	LED menu****");
				printf("\r\nEsc	   Exit Menu");
				printf("\r\nqwer   Pwr LED");
				printf("\r\nasdf   Alarm LED");
				printf("\r\nzxcv   BAT LED Green");
				printf("\r\nZXCV   BAT LED RED");
				break;
			case 0x1b:
				return(0);
				//break;
			case 'q':		api_LED(LED_PWR		,LED_OFF	,0x00);		break;
			case 'w':		api_LED(LED_PWR		,LED_ON		,0x80);		break;
			case 'e':		api_LED(LED_PWR		,LED_FLASH	,0xff);		break;
			case 'r':		api_LED(LED_PWR		,LED_RAMP	,0xff);		break;

			case 'a':		api_LED(LED_ALM		,LED_OFF	,0x00);		break;
			case 's':		api_LED(LED_ALM		,LED_ON		,0x80);		break;
			case 'd':		api_LED(LED_ALM		,LED_FLASH	,0xff);		break;
			case 'f':		api_LED(LED_ALM		,LED_RAMP	,0xff);		break;

            case 'z':		api_LED(LED_BATG	,LED_OFF	,0x00);		break;
			case 'x':		api_LED(LED_BATG	,LED_ON		,0x80);		break;
			case 'c':		api_LED(LED_BATG	,LED_FLASH	,0xff);		break;
			case 'v':		api_LED(LED_BATG	,LED_RAMP	,0xff);		break;

			case 'Z':		api_LED(LED_BATR	,LED_OFF	,0x00);		break;
			case 'X':		api_LED(LED_BATR	,LED_ON		,0x80);		break;
			case 'C':		api_LED(LED_BATR	,LED_FLASH	,0xff);		break;
			case 'V':		api_LED(LED_BATR	,LED_RAMP	,0xff);		break;

			default:
				printf("\r\n Invalid Command");
				break;
		}
	}

	return(1);
}

/*************************************************************************************************
* Function Name : 	api_LED_flash_rate
* Description   : 	This Function sets flash rate
* Arguments     : 	LED_enum led_no, uint16_t period_ms
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		15/06/26	A. Kane			Created
*************************************************************************************************/

void api_LED_flash_rate(LED_enum led_no, uint16_t period_ms)
{
    led[led_no].flash_period_ms = period_ms;
    return;
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
