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
 * Filename    :  app_touchscreen.h
 * Date Created:  Thu 14 Sep 2017 11:29:50 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _APP_TOUCHSCREEN_H
#define _APP_TOUCHSCREEN_H

#include <stdint.h>

#define		MAX_SOFT_BUTTONS		0x2B

#define		PRESS_AND_HOLD_REPEAT	200
#define		PRESS_AND_HOLD_DURATION	500
#define		PRESS_DURATION			40
#define		PRESS_RELEASE			20
#define		PRESS_AND_HOLD_ERROR	20000

#define		TOUCH_AV_HIS			5

//********************************************************************************************************
//BUTTON definitions
#define	BUTTON_HOLD					0x80

#define	BUTTON_0					0x00
#define	BUTTON_1					0x01
#define	BUTTON_2					0x02
#define	BUTTON_3					0x03
#define	BUTTON_4					0x04
#define BUTTON_5					0x05
#define BUTTON_6					0x06
#define BUTTON_7					0x07
#define BUTTON_8					0x08
#define BUTTON_9					0x09
#define BUTTON_A					0x0A
#define BUTTON_B					0x0B
#define BUTTON_C					0x0C
#define BUTTON_D					0x0D
#define BUTTON_E					0x0E
#define BUTTON_F					0x0F
#define BUTTON_10					0x10
#define BUTTON_11					0x11
#define BUTTON_12					0x12
#define BUTTON_13					0x13
#define BUTTON_14					0x14
#define BUTTON_15					0x15
#define BUTTON_16					0x16
#define BUTTON_17					0x17
#define BUTTON_18					0x18
#define BUTTON_19					0x19
#define BUTTON_1A					0x1A
#define BUTTON_1B					0x1B
#define BUTTON_1C					0x1C
#define BUTTON_1D					0x1D
#define BUTTON_1E					0x1E
#define BUTTON_1F					0x1F
#define BUTTON_20					0x20
#define BUTTON_21					0x21
#define BUTTON_22					0x22
#define BUTTON_23					0x23
#define BUTTON_24					0x24
#define BUTTON_25					0x25
#define BUTTON_26					0x26
#define BUTTON_27					0x27
#define BUTTON_28					0x28
#define BUTTON_29					0x29


#define	BUTTON_0_HOLD				BUTTON_HOLD + BUTTON_0
#define	BUTTON_1_HOLD				BUTTON_HOLD + BUTTON_1
#define	BUTTON_2_HOLD				BUTTON_HOLD + BUTTON_2
#define	BUTTON_3_HOLD				BUTTON_HOLD + BUTTON_3
#define	BUTTON_4_HOLD				BUTTON_HOLD + BUTTON_4
#define	BUTTON_5_HOLD				BUTTON_HOLD + BUTTON_5
#define	BUTTON_6_HOLD				BUTTON_HOLD + BUTTON_6
#define	BUTTON_7_HOLD				BUTTON_HOLD + BUTTON_7
#define	BUTTON_8_HOLD				BUTTON_HOLD + BUTTON_8
#define	BUTTON_9_HOLD				BUTTON_HOLD + BUTTON_9
#define	BUTTON_A_HOLD				BUTTON_HOLD + BUTTON_A
#define	BUTTON_B_HOLD				BUTTON_HOLD + BUTTON_B
#define	BUTTON_C_HOLD				BUTTON_HOLD + BUTTON_C
#define	BUTTON_D_HOLD				BUTTON_HOLD + BUTTON_D
#define	BUTTON_E_HOLD				BUTTON_HOLD + BUTTON_E
#define	BUTTON_F_HOLD				BUTTON_HOLD + BUTTON_F
#define	BUTTON_10_HOLD				BUTTON_HOLD + BUTTON_10
#define	BUTTON_11_HOLD				BUTTON_HOLD + BUTTON_11
#define	BUTTON_12_HOLD				BUTTON_HOLD + BUTTON_12
#define	BUTTON_13_HOLD				BUTTON_HOLD + BUTTON_13
#define	BUTTON_14_HOLD				BUTTON_HOLD + BUTTON_14
#define	BUTTON_15_HOLD				BUTTON_HOLD + BUTTON_15
#define	BUTTON_16_HOLD				BUTTON_HOLD + BUTTON_16
#define	BUTTON_17_HOLD				BUTTON_HOLD + BUTTON_17
#define	BUTTON_18_HOLD				BUTTON_HOLD + BUTTON_18
#define	BUTTON_19_HOLD				BUTTON_HOLD + BUTTON_19
#define	BUTTON_1A_HOLD				BUTTON_HOLD + BUTTON_1A
#define	BUTTON_1B_HOLD				BUTTON_HOLD + BUTTON_1B
#define	BUTTON_1C_HOLD				BUTTON_HOLD + BUTTON_1C
#define	BUTTON_1D_HOLD				BUTTON_HOLD + BUTTON_1D
#define	BUTTON_1E_HOLD				BUTTON_HOLD + BUTTON_1E
#define	BUTTON_1F_HOLD				BUTTON_HOLD + BUTTON_1F
#define BUTTON_20_HOLD				BUTTON_HOLD + BUTTON_20
#define	BUTTON_21_HOLD				BUTTON_HOLD + BUTTON_21
#define	BUTTON_22_HOLD				BUTTON_HOLD + BUTTON_22
#define	BUTTON_23_HOLD				BUTTON_HOLD + BUTTON_23
#define	BUTTON_24_HOLD				BUTTON_HOLD + BUTTON_24
#define	BUTTON_25_HOLD				BUTTON_HOLD + BUTTON_25
#define	BUTTON_26_HOLD				BUTTON_HOLD + BUTTON_26
#define	BUTTON_27_HOLD				BUTTON_HOLD + BUTTON_27
#define	BUTTON_28_HOLD				BUTTON_HOLD + BUTTON_28
#define	BUTTON_29_HOLD				BUTTON_HOLD + BUTTON_29

#define	BUTTON_UNLOCK				BUTTON_0

#define	BUTTON_PRESSED_SOMEWHERE_ELSE	0xfc
#define	BUTTON_PRE_HOLD					0xfd
#define	BUTTON_JUST_RELEASED			0xfe
#define	BUTTON_RELEASED					0xff

#define	SOUND_PRESS			0
#define	SOUND_PRESS_HOLD	1

#define NO_OF_SOUND_OPTIONS				2
//********************************************************************************************************

typedef struct{
	uint8_t				valid;
	int16_t				x_s;
	int16_t				x_w;
	int16_t				x_e;
	int16_t				y_s;
	int16_t				y_h;
	int16_t				y_e;
	uint8_t				press_sound;
}soft_touch_but_t;

typedef struct{
	int32_t		tot;
	int16_t		his[TOUCH_AV_HIS];
	uint8_t		cnt;
	uint8_t		pos;
}touch_av_t;

typedef struct{
	uint8_t			init;
	uint8_t			cur_button;
	uint32_t		press_duration;
	uint32_t		release_duration;
	int16_t			press_start_x;
	int16_t			press_start_y;
	int16_t			press_current_x;
	int16_t			press_current_y;
	uint8_t			must_release;
}button_t;


typedef struct{
	int16_t			x_pix_diff;		//	cur - start
	int16_t			y_pix_diff;		//	cur - start
	float			x_ee_f;			//	slider length = 1 so cur pos = fraction of length
	float			y_ee_f;			//	slider length = 1 so cur pos = fraction of length

}slider_t;

/*********************************************************************************************************
 *		Global Functions
 ********************************************************************************************************/
uint8_t		app_touchscreen_handler(void);

void 		app_touchscreen_no_button_press(void);
uint8_t		app_touchscreen_slider_info(slider_t *slider);

void 		app_touchscreen_button_add(	uint8_t id,uint16_t x,uint16_t y,uint16_t w,uint16_t h);

void 		app_touchscreen_button_enable(	uint8_t id);
void 		app_touchscreen_button_disable(	uint8_t id);
void 		app_touchscreen_button_disable_all(void);

void 		app_touchscreen_button_audio_option(uint8_t id,uint8_t sound);

void 		app_touchscreen_irq(void);
uint8_t 	app_touch_hold_err(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
