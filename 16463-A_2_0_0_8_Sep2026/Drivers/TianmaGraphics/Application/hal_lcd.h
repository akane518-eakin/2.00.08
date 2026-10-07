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
 * Filename    :  hal_lcd.h
 * Date Created:  Tue 19 Dec 2017 09:40:41 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _HAL_LCD_H
#define _HAL_LCD_H

#include "globtypes.h"

typedef union
{
	U16	data;
	struct{
		U16		b	:5;
		U16		g	:6;
		U16		r	:5;
	}parts;
}pix565_u;


#define LCD_HORIZ_LINE     0x00
#define LCD_VERT_LINE      0x01

#ifndef LCD_RGB
#define LCD_RGB(COLOUR_RED,COLOUR_GREEN,BLUE)	(((COLOUR_RED & 0xF8) << 8) + ((COLOUR_GREEN & 0xFC) << 3) + ((BLUE & 0xF8) >> 3))

#define LCD_EXTRACT_RED(color) 		(((color >> 11) & 0x1F)<<3)		// COLOUR_RED is 5 MS bits
#define LCD_EXTRACT_GREEN(color) 	(((color >> 5) & 0x3F)<<2)		// COLOUR_GREEN is middle 6 bits
#define LCD_EXTRACT_BLUE(color) 	((color & 0x1F)<<3)				// blue is LS 5 bits

#define LCD_ASSEMBLE_RGB(COLOUR_RED,COLOUR_GREEN,BLUE)	(((COLOUR_RED & 0x1F) << 11) + ((COLOUR_GREEN & 0x3F) << 5) + (BLUE & 0x1F))
#endif

#define Horizontal     0x00
#define Vertical       0x01



//	Graphic bitmap definition
typedef const struct lcdbitmapdef {
	PSZ id;			// identifier (name)
	U8 bitsPerPixel; // currently 1 or 16
	U32 pixels;		// total pixels
	U16 height;		// height in pixels
	U16 width;		// width in pixels
	const U8* pData; // pointer to the pixel data
	struct lcdbitmapdef *next;	// list pointer
} LCDBITMAPDEF, *PLCDBITMAPDEF;

#define MONO_SPACED_WIDTH_INDIC	0xFFFF0000		// fixed width indicator


__packed typedef struct {
	char	id[2];
	long	filesize;
	char	reserved[4];
	long	headersize;
	long	infoSize;
	long	width;
	long	depth;
	short	biPlanes;
	short	bits;
	long	biCompression;
	long	biSizeImage;
	long	biXPelsPerMeter;
	long	biYPelsPerMeter;
	long	biClrUsed;
	long	biClrImportant;
} BMPHEAD;




void LCD_Clear(LCD_COLOR Color);
void LCD_DrawDot(U16 startX, U16 startY, LCD_COLOR Color);
void LCD_DrawLine(U16 startX, U16 startY, U16 endX, U16 endY, LCD_COLOR Color);

#define EDGE_BIAS_NONE		0
#define EDGE_BIAS_INSIDE	1
#define EDGE_BIAS_OUTSIDE	2
void LCD_DrawWuLine(U16 startX, U16 startY, U16 endX, U16 endY, LCD_COLOR BaseColor,LCD_COLOR BackColor, U8 edgeBias);


void LCD_DrawVHLine(U16 startX, U16 startY, U16 Length, U8 Direction, LCD_COLOR color);

void LCD_FillRect(U16 startX, U16 startY,  U16 Width,U16 Height, LCD_COLOR Color);
void LCD_DrawRect_1(U16 startX, U16 startY, U16 Width, U16 Height, LCD_COLOR Color);
void LCD_DrawRect_n(U16 startX, U16 startY, U16 Width, U16 Height, U16 thick, LCD_COLOR color);


void LCD_DrawCircle(U16 startX, U16 startY, U16 Radius, LCD_COLOR Color);
void LCD_FillCircle(U16 startX, U16 startY, U16 Radius, LCD_COLOR Color);
void LCD_DrawQuarter(U16 startX, U16 startY, U16 Radius, U8 quadrant, LCD_COLOR Color);
void LCD_FillQuarter(U16 startX, U16 startY, U16 Radius, U8 quadrant, LCD_COLOR Color);

void LCD_FillSoftRect(U16 startX, U16 startY, U16 width, U16 height,  U16 cornerSize, LCD_COLOR color);
void LCD_DrawSoftRect_1(U16 startX, U16 startY,  U16 width, U16 height,U16 cornerSize,LCD_COLOR color);
void LCD_DrawSoftRect_n(U16 startX, U16 startY, U16 Width, U16 Height, U16 cornerSize, U16 thick, LCD_COLOR color);

void LCD_DrawSoftRect_2(U16 startX, U16 startY, U16 width, U16 height, U16 cornerSize, LCD_COLOR color);
void LCD_DrawQuarter_2(U16 startX, U16 startY, U16 Radius, U8 quadrant, LCD_COLOR color);
//void LCD_DrawQuarter_n(U16 startX, U16 startY, U16 Radius1, U16 thick, U8 quadrant, LCD_COLOR color);

void LCD_DrawHTrap(U16 startX, U16 startY, U16 startWidth, U16 endX, U16 endY, U16 endWidth, LCD_COLOR color);
void LCD_FillHTrap(U16 startX, U16 startY, U16 startWidth, U16 endX, U16 endY, U16 endWidth, LCD_COLOR color);

void LCD_FillGradRect(U16 startX, U16 startY, U16 width, U16 height,  LCD_COLOR startColor,  LCD_COLOR endColor);
void LCD_FillGradSubRect(U16 orgStartY, U16 orgHeight, LCD_COLOR startColor,LCD_COLOR endColor, U16 startX, U16 startY, U16 height, U16 width);

void LCD_DispBitmap(U16 startX, U16 startY, const PSZ name);
void LCD_DispMonoBitmap(U16 startX, U16 startY,const PSZ bitmapName,LCD_COLOR foreColor,LCD_COLOR backColor);
void LCD_DispMonoBitmapTrans(U16 startX, U16 startY, const PSZ bitmapName, LCD_COLOR foreColor);



void 			LCD_PowerOn(void);
void 			LCD_DisplayStandby(void);
void 			LCD_DisplayResume(void);
void 			LCD_DisplayShutdown(void);

U16 			LCD_GetBitmapHeight(PSZ bitmapName);
U16 			LCD_GetBitmapWidth(PSZ bitmapName);
LCDBITMAPDEF* 	LCD_GetBitmapInfo( const PSZ name, U16* pixelX, U16* pixelY, U8* bitsPerPixel);
LCDBITMAPDEF* 	LCD_GetBitmapDef(const PSZ name);

void 			LCD_RenderColorBitmap(U16 startX, U16 startY,  U16 width,U16 height,  const U16* pData);
void 			LCD_RenderMonoBitmap(U16 startX, U16 startY, const U8* pData, U16 width, U16 height, LCD_COLOR foreColor, LCD_COLOR backColor, BOOLEAN packedBits);
LCD_COLOR 		LCD_GradColor(LCD_COLOR start, LCD_COLOR end, U16 grades, U16 number);

void 			LCD_Output_24BMP_FromScreen(U16 start_x, U16 start_y, U16 w, U16 h );

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
