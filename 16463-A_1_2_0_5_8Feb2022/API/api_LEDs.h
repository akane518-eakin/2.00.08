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
 * Filename    :  api_LEDs.h
 * Date Created:  Tue 24 Apr 2018 03:29:57 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_LEDS_H_
#define _API_LEDS_H_


/**********************************************************************************************************
 **********************************************************************************************************/
#define	MAX_BRIGNNESS	3

#define	LED_FLICKER_TIME	400


typedef enum{
	LED_PWR		= 0,
	LED_BATG	= 1,
	LED_BATR	= 2,
	LED_ALM		= 3,
}LED_enum;


typedef enum{
	LED_OFF	= 0,
	LED_ON,
	LED_FLASH,
	LED_RAMP,
}LED_mode_enum;

typedef struct{
	LED_mode_enum		mode;
	uint8_t				max_brightness;

	uint8_t				cur_brightness;
	uint8_t				req_brightness;
	uint8_t				up_down;
	uint16_t			ms_timer;
        uint16_t                        flash_period_ms;
}LED_st;


/*********************************************************************************************************
 *		Functions
 ********************************************************************************************************/
void 	api_LED(LED_enum led,LED_mode_enum mode,uint8_t brightness);

void 	api_LED_manager(void);
void 	api_LED_irq(void);
uint8_t	api_LED_test(void);

void    api_LED_flash_rate(LED_enum led, uint16_t period_ms);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
