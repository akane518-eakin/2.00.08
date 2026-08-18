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
 * Filename    :  api_RGB.h
 * Date Created:  Tue 05 Sep 2017 02:26:04 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _API_RGB_H_
#define _API_RGB_H_


/**********************************************************************************************************
 **********************************************************************************************************/
//							0x--RRGGBB
#define		RGB_RED			0x00FF0000
#define		RGB_GREEN		0x0000FF00
#define		RGB_BLUE		0x000000FF

#define		RGB_CYAN		0x0000FFFF
#define		RGB_MAGENTA		0x00FF00FF
#define		RGB_YELLOW		0x00FFFF00

#define		RGB_WHITE		0x00FFFFFF
#define		RGB_BLACK		0x00000000


typedef enum
{
	RGB_SOLID		=	0,
	RGB_SLOW_FLASH	= 	150,
	RGB_MED_FLASH	= 	75,
	RGB_FAST_FLASH	= 	25,
}api_RGB_speed_enum;

typedef struct
{
	union{
		uint32_t	a;
		uint8_t		b[4];
	}col;
	uint16_t			flash_timer;
	api_RGB_speed_enum	flash_rate;
	uint8_t				on_off;
}api_RGB_t;


/*********************************************************************************************************
 *		Functions
 ********************************************************************************************************/
void 	api_RGB_set(uint32_t colour,api_RGB_speed_enum flash);
void 	api_RGB_IRQ(void);

uint8_t	api_RGB_test(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
