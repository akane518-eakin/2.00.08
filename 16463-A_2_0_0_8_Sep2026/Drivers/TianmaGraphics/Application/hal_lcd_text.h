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
 * Filename    :  hal_lcd_text.h
 * Date Created:  Tue 19 Dec 2017 05:04:13 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _HAL_LCD_TEXT_H
#define _HAL_LCD_TEXT_H

#include "stdint.h"
#include "bfcfont.h"
#include "globtypes.h"



typedef enum{
  CENTER,
  CENTER_LEFT,
  CENTER_RIGHT,
  TOP_LEFT,
  TOP_RIGHT,
  TOP_CENTER,
  BOTTOM_LEFT,
  BOTTOM_RIGHT,
  BOTTOM_CENTER,
}TXT_JUSTIFICATION_enum;


extern uint8_t debug_textActiveArea;

/*********************************************************************************************************
 *		Function prototypes
 ********************************************************************************************************/

U16 LCD_DispText_option(U16 startX, U16 startY, U16 startW, U16 startH, TXT_JUSTIFICATION_enum Justification, LCD_COLOR colfg,LCD_COLOR colbg, const BFC_FONT *pfont, PSZ text);
U16 LCD_GetStringWidth(	const BFC_FONT *pFont,	PSZ text);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
