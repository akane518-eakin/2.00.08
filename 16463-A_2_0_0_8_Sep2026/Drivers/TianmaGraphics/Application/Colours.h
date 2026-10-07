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
 * Filename    :  Colours.h
 * Date Created:  Mon 18 Dec 2017 10:57:59 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _COLOURS_H
#define _COLOURS_H

#define RGB(COLOUR_RED,COLOUR_GREEN,BLUE)	(((COLOUR_RED & 0xF8) << 8) + ((COLOUR_GREEN & 0xFC) << 3) + ((BLUE & 0xF8) >> 3))

#define		TRANSPARENT							0x1111
#define		COLOUR_GRID							0x1112

//colours										//	R,G,B		0-255

#define 	COLOUR_BLACK						0x0000				// RGB(0x00,0x00,0x00)
#define		COLOUR_GREY_20						0x2104				// RGB(0x20,0x20,0x20)
#define		COLOUR_GREY_3C						0x39E7				// RGB(0x3C,0x3C,0x3C)
#define 	COLOUR_GRAY_80						0x8410				// RGB(0x80,0x80,0x80)
#define 	COLOUR_GRAY_8A						0x8C51				// RGB(0x8A,0x8A,0x8A)
#define 	COLOUR_GRAY_E1						0xE71C				// RGB(0xE1,0xE1,0xE1)
#define 	COLOUR_WHITE						0xFFFF				// RGB(0xFF,0xFF,0xFF)


#define 	COLOUR_RED							0xF800				// RGB(0xFF,0x00,0x00)
#define 	COLOUR_GREEN						0x07E0				// RGB(0x00,0xFF,0x00)
#define 	COLOUR_BLUE							0x001F				// RGB(0x00,0x00,0xFF)

#define 	COLOUR_MAGENTA						0xF81F				// RGB(0xFF,0x00,0xFF)
#define 	COLOUR_CYAN							0x07FF				// RGB(0x00,0xFF,0xFF)
#define 	COLOUR_YELLOW						0xFFE0				// RGB(0xFF,0xFF,0x00)

#define 	COLOUR_RED50						RGB(0x80,0x00,0x00)
#define 	COLOUR_ORANGE						RGB(0xFF,0x85,0x00)
#define 	COLOUR_INDIGO						0x4810				// RGB(0x4B,0x00,0x82)
#define 	COLOUR_VIOLET						0xEC1D				// RGB(0xEE,0x80,0xEE)
#define 	COLOUR_LIME							0x07E0				// RGB(0x00,0xFF,0x00)
#define 	COLOUR_TURQUOISE					0x471A				// RGB(0x40,0xE0,0xD0)
#define 	COLOUR_TEAL							0x0410				// RGB(0x00,0x80,0x80)
#define 	COLOUR_PALETURQUOISE				0xAF7D				// RGB(0xAF,0xEE,0xEE)
#define 	COLOUR_PALEGREEN					0x9FD3				// RGB(0x98,0xFB,0x98)
#define 	COLOUR_PLUM							0xDD1B				// RGB(0xDD,0xA0,0xDD)


//EPAS
#define 	COLOUR_EPAS_BLUE              		RGB(19,24,251)//CE91

//ARMSTRONG
#define		COLOUR_WELCOME_BACKGROUND			COLOUR_WHITE
#define		COLOUR_SCREEN_BACKGROUND			RGB(180,180,180)
#define		COLOUR_ICON_A_DISABLE				RGB(200,200,200)
#define		COLOUR_ICON_B_DISABLE				RGB(220,220,220)
#define		COLOUR_ARMSTRONG_LOGO				RGB( 79, 45,127)		//Pantone 268	Dark blue violet
#define		COLOUR_BUTTON_BACK					RGB(100,100,100)
#define		COLOUR_BUTTON_BACK1					RGB(100,0,0)

#define		COLOUR_M_PURPLE						RGB(200,100,255)
#define		COLOUR_M_YELLOW						RGB(255,255,  0)
#define		COLOUR_M_GREEN						RGB(128,255,128)
#define		COLOUR_M_LIGHT_BLUE					RGB(  0,255,255)
#define		COLOUR_M_DARK_BLUE					RGB(100,100,255)
#define		COLOUR_M_GREY						RGB(160,160,160)

#define		COLOUR_BUTTON_PAIR1					RGB(140,216,240)	//blue butons          	173,216,240
#define		COLOUR_BUTTON_PAIR2					RGB(90,255,90)	//green buttons         142,255,142

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
