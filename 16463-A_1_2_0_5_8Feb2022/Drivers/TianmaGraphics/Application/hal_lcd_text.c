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
 * Filename    :  hal_lcd_text.c
 * Date Created:  Tue 19 Dec 2017 03:51:57 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "hal_lcd_text.h"
#include "hal_lcd.h"
#include "fonts.h"
#include "Colours.h"

#include "bfcfont.h"

//pick the LCD driver here
#include "csp_LCD_SSD1963.h"


/*********************************************************************************************************
 *		Local Functions
 ********************************************************************************************************/

U16					LCD_GetTextHeight(	const BFC_FONT *pFont);
const BFC_CHARINFO*	LCD_GetCharInfo(	const BFC_FONT *pFont, uint16_t ch);

int					LCD_DrawChar_RowRowPacked(int x0, int y0, LCD_COLOR colfg,LCD_COLOR colbg, const BFC_FONT *pFont, uint16_t ch);

uint16_t			UTF8_DecodeChar(	PSZ *text);

uint8_t				debug_textActiveArea	=  0;

/*********************************************************************************************************
 ********************************************************************************************************/


/*************************************************************************************************
* Function Name :	UTF8_DecodeChar
* Description   : 	This function decodes a single UTF-8 encoded character starting at *text,
*                   advances *text past the bytes consumed, and returns the
*                   decoded Unicode code point.
* Arguments     : 	PSZ *text
* Returns       : 	UTF8_DecodeChar
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/07/26	A. Kane			Created
*************************************************************************************************/

uint16_t UTF8_DecodeChar(PSZ *text)
{
	unsigned char	b0	=  (unsigned char)**text;
	uint32_t		cp;
	int				extra;

	if(b0 < 0x80){
		//1-byte sequence: 0xxxxxxx  (plain ASCII)
		(*text)++;
		return (uint16_t)b0;
	}
	else if( (b0 & 0xE0) == 0xC0 ){
		//2-byte sequence: 110xxxxx 10xxxxxx
		cp		=  b0 & 0x1F;
		extra	=  1;
	}
	else if( (b0 & 0xF0) == 0xE0 ){
		//3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
		cp		=  b0 & 0x0F;
		extra	=  2;
	}
	else{
		
		(*text)++;
		return (uint16_t)'?';
	}

	(*text)++;

	while(extra--){
		unsigned char b = (unsigned char)**text;

		if( (b & 0xC0) != 0x80 ){
			
		}

		cp = (cp << 6) | (b & 0x3F);
		(*text)++;
	}

	return (uint16_t)cp;
}


/*************************************************************************************************
* Function Name :	LCD_DispText_option
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		03/10/17	W. Paul			Created
* 0.2.0		05/07/26	A. Kane			Decode UTF-8 before glyph lookup
*************************************************************************************************/
U16 LCD_DispText_option(U16 startX, U16 startY, U16 startW, U16 startH, TXT_JUSTIFICATION_enum Justification, LCD_COLOR colfg,LCD_COLOR colbg, const BFC_FONT *pFont, PSZ text)
{
 	const BFC_CHARINFO*		char_info;
	U16 	jusy 			=  0;
	U16 	jusx 			=  0;
	U16 	posx 			=  0;
	U16 	textwidth		=  0;
	U16 	textheight		=  0;
	U16 	total_width		=  0;
	uint16_t				codepoint;
	PSZ 	hold_text_ptr	=  text;

	//Debug box
	if(debug_textActiveArea){
		LCD_DrawRect_1(startX, startY, startW, startH, colfg);
	}

	textwidth	=  LCD_GetStringWidth(	pFont,text);
	textheight	=  LCD_GetTextHeight(	pFont);

	switch(Justification){
		case CENTER_LEFT:	jusy = (startH/2) - (textheight/2);	jusx = 0;							break;
		case CENTER:		jusy = (startH/2) - (textheight/2);	jusx = (startW/2)- (textwidth/2);	break;
		case CENTER_RIGHT:	jusy = (startH/2) - (textheight/2);	jusx = startW 	 - (textwidth);		break;
		case TOP_LEFT:		jusy = 0;							jusx = 0;							break;
		case TOP_CENTER:	jusy = 0;							jusx = (startW/2)- (textwidth/2);	break;
		case TOP_RIGHT:		jusy = 0;							jusx = startW 	 - (textwidth);		break;
		case BOTTOM_LEFT:	jusy = startH - textheight;			jusx = 0;							break;
		case BOTTOM_CENTER:	jusy = startH - textheight;			jusx = (startW/2)- (textwidth/2);	break;
		case BOTTOM_RIGHT:	jusy = startH - textheight;			jusx = startW 	 - (textwidth);		break;
		default:			jusy = 0;							jusx = 0;
	}

	while(*text != 0){
		
		codepoint	=  UTF8_DecodeChar(&text);		//advances text by 1-3 bytes as needed
		char_info	=  LCD_GetCharInfo(pFont, codepoint);
		total_width	+= char_info->Width;
		if( total_width > startW){
			if(debug_textActiveArea){
				printf("\r\nText string too wide! (%s)(x,y,w,h -%d %d %d %d)",hold_text_ptr,startX,startY,startW,startH);
			}
			break;
		}
	}

	text	=  hold_text_ptr;
	while(*text != 0){
		codepoint	=  UTF8_DecodeChar(&text);		//advances text by 1-3 bytes as needed
		posx		+= LCD_DrawChar_RowRowPacked(startX + jusx + posx , startY + jusy, colfg, colbg, pFont, codepoint);
	}

	return(posx + startX + jusx);
}




/*************************************************************************************************
* Function Name : 	LCD_GetStringWidth
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		19/12/17	W. Paul			Created
* 0.2.0		05/07/26	A. Kane			Decode UTF-8 before glyph lookup
*************************************************************************************************/
U16 LCD_GetStringWidth(const BFC_FONT *pFont,	PSZ text)
{
	U16 	    		width	=  0;
	const BFC_CHARINFO*	char_info;
	uint16_t			codepoint;


	if (pFont != NULL){
			// sum all chars widths
		while (*text != 0){
			codepoint	=  UTF8_DecodeChar(&text);	//advances text by 1-3 bytes as needed
			char_info	=  LCD_GetCharInfo(pFont, codepoint);
			width 		+= char_info->Width;
		}
	}
	return width;
}


/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		19/12/17	W. Paul			Created
*************************************************************************************************/
U16 LCD_GetTextHeight(const BFC_FONT *pFont)
{
	U16 height = 0;

	if (pFont != NULL){
		height = pFont->FontHeight;
	}
	return height;
}



/*************************************************************************************************
* Function Name : 	LCD_GetCharInfo
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		19/12/17	W. Paul			Created
* 0.2.0		05/07/26	A. Kane			ch widened from char (8-bit) to uint16_t (16-bit)
*************************************************************************************************/
const BFC_CHARINFO* LCD_GetCharInfo(const BFC_FONT *pFont, uint16_t ch)
{
	const BFC_CHARINFO	    *pCharInfo 	=  0;
	const BFC_FONT_PROP 	*pProp 		=  pFont->p.pProp;
	unsigned short 		    first_char;
	unsigned short 		    last_char;
    unsigned short 		    lookup_char;

    lookup_char     =  ch;		

	if(pFont == 0 || pFont->p.pProp == 0)
		return 0;

	while(pProp != 0)
	{
		first_char	= pProp->FirstChar;
		last_char	= pProp->LastChar;
		pCharInfo	= pProp->pFirstCharInfo;

		if( lookup_char >= first_char && lookup_char <= last_char ){

			pCharInfo = pCharInfo + (lookup_char - first_char);

			return pCharInfo;
		}
		else{

			pProp = pProp->pNextProp;
		}
	}


	if( pCharInfo == 0 ){
		pProp = pFont->p.pProp;
		pCharInfo = pProp->pFirstCharInfo;
    }

    if( pProp == 0 ){
		printf("\r\nChar not avail in this font  (0x%04x)",lookup_char);
	}



	return pCharInfo;
}

/*************************************************************************************************
* Function Name : 	LCD_DrawChar_RowRowPacked
* Description   : 	This Function unpacks the data for each charater and sends it to the screen
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		20/12/17	W. Paul			Created
* 0.2.0		05/07/26	A. Kane			ch widened from char to uint16_t
*************************************************************************************************/
int LCD_DrawChar_RowRowPacked(int x0, int y0, LCD_COLOR colfg,LCD_COLOR colbg, const BFC_FONT *pFont, uint16_t ch)
{
	// 1. find the character information first
	const BFC_CHARINFO *pCharInfo = LCD_GetCharInfo(pFont, ch);

	if( pCharInfo != 0 )
	{
		int height 					= pFont->FontHeight;
		int width 					= pCharInfo->Width;
		const unsigned char *pData	= pCharInfo->p.pData8;						// pointer to data array

		int 			x, y;
		LCD_COLOR       colour;
		uint8_t			mask = 0x80;

		// 2. draw all the pixels in this character
		for(y=0; y<height; y++){
			for(x=0; x<width; x++){
				if(*pData & mask){	colour =  colfg;	}
				else{				colour =  colbg;	}

				mask = mask >> 1;
				if (mask == 0){
					mask = 0x80;
					pData++;
				}

				if(colour != TRANSPARENT){
					LCD_SetCursor(x0+x, y0+y);
					LCD_GRAM_wr_cmd();
					LCD_RAM_VALUE	=  colour;
				}
			}
		}

		return width;
	}

	return 0;
}


/*************************************************************************************************
* Function Name :	LCD_DrawChar_RowRowUnpacked
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		20/12/17	W. Paul			Created
*************************************************************************************************/
/*
int LCD_DrawChar_RowRowUnpacked(int x0, int y0,LCD_COLOR colfg,LCD_COLOR colbg, const BFC_FONT *pFont, unsigned short ch)
{
	// 1. find the character information first
	const BFC_CHARINFO *pCharInfo = GetCharInfo(pFont, ch);

	if( pCharInfo != 0 )
	{
		int height = pFont->FontHeight;
		int width = pCharInfo->Width;
		int data_size = pCharInfo->DataSize;                // # bytes of the data array
		const unsigned char *pData = pCharInfo->p.pData8;   // pointer to data array

		int bpp = GetFontBpp(pFont->FontType);              // how many bits per pixel
		int bytesPerLine = (width * bpp + 7) / 8;           // # bytes in a row
		int bLittleEndian = (GetFontEndian(pFont->FontType)==1);

		int 			x, y, col;
		unsigned char 	data, pixel, bit;
		LCD_COLOR 		colour;

		// 2. draw all the pixels in this character
		for(y=0; y<height; y++)
		{
			for(x=0; x<width; x++)
			{
				col = (x * bpp) / 8;       // byte index in the line
				data = pData[y * bytesPerLine + col];

				// every BYTE (8 bits) data includes 8/bpp pixels,
				// we need to get each pixel color index (0,1,2,3... based on bpp) from the BYTE data
				pixel = data;

				// bit index in the BYTE
				// For 1-bpp: bit =  x % 8 (Big Endian),   7 -  x % 8 (Little Endian)
				// For 2-bpp: bit = 2x % 8 (Big Endian),   6 - 2x % 8 (Little Endian)
				// For 4-bpp: bit = 4x % 8 (Big Endian),   4 - 4x % 8 (Little Endian)
				bit = bLittleEndian ? (8-bpp)-(x*bpp)%8 : (x*bpp)%8;

				pixel = pixel<<bit;               // clear left pixels
				pixel = pixel>>(8/bpp-1)*bpp;     // clear right pixels

				if(pixel){	colour =  colfg;	}
				else{		colour =  colbg;	}

				// draw this pixel
				if(colour != TRANSPARENT){
					LCD_SetCursor(x0+x, y0+y);
					LCD_GRAM_wr_cmd();
					LCD_RAM_VALUE	=  colour;
				}
			}
		}

		return width;
	}

	return 0;
}
*/


/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		20/12/17	W. Paul			Created
*************************************************************************************************/
/*
int LCD_DrawChar_ColColUnpacked(HDC hdc, int x0, int y0,LCD_COLOR colfg,LCD_COLOR colbg, const BFC_FONT *pFont, unsigned short ch)
{
	// 1. find the character information first
	const BFC_CHARINFO *pCharInfo = GetCharInfo(pFont, ch);

	if( pCharInfo != 0 )
	{
		int height = pFont->FontHeight;
		int width = pCharInfo->Width;
		int data_size = pCharInfo->DataSize;                // # bytes of the data array
		const unsigned char *pData = pCharInfo->p.pData8;   // pointer to data array

		int bpp = GetFontBpp(pFont->FontType);              // how many bits per pixel
		int bytesPerLine = (height * bpp + 7) / 8;          // # bytes in a row
		int bLittleEndian = (GetFontEndian(pFont->FontType)==1);

		int 			x, y, col;
		unsigned char 	data, pixel, bit;
		LCD_COLOR 		color;

		// 2. draw all the pixels in this character
		for(x=0; x<width; x++)
		{
			for(y=0; y<height; y++)
			{
				col = (y * bpp) / 8;       // byte index in the line
				data = pData[x * bytesPerLine + col];

				// every BYTE (8 bits) data includes 8/bpp pixels,
				// we need to get each pixel color index (0,1,2,3... based on bpp) from the BYTE data
				pixel = data;

				// bit index in the BYTE
				// For 1-bpp: bit =  y % 8 (Big Endian),   7 -  y % 8 (Little Endian)
				// For 2-bpp: bit = 2y % 8 (Big Endian),   6 - 2y % 8 (Little Endian)
				// For 4-bpp: bit = 4y % 8 (Big Endian),   4 - 4y % 8 (Little Endian)
				bit = bLittleEndian ? (8-bpp)-(y*bpp)%8 : (y*bpp)%8;

				pixel = pixel<<bit;               // clear left pixels
				pixel = pixel>>(8/bpp-1)*bpp;     // clear right pixels

				if(pixel){	colour =  colfg;	}
				else{		colour =  colbg;	}

				// draw this pixel
				if(colour != TRANSPARENT){
					LCD_SetCursor(x0+x, y0+y);
					LCD_GRAM_wr_cmd();
					LCD_RAM_VALUE	=  colour;
				}
			}
		}

		return width;
	}

	return 0;
}
*/

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
