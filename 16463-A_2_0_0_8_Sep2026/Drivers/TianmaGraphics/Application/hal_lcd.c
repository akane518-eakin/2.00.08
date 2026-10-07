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
* Filename    :  hal_lcd.c
* Date Created:  Tue 04 Apr 2017 03:12:20 PM
* Programmer  :  William Paul
* Description :  This module is used for
*
**********************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32f10x.h"

#include	"app_system.h"

#include "glb_typedefs.h"

#include "csp_STM32_delay.h"
#include "csp_STM32_timer3.h"
#include "csp_STM32_iwdg.h"
#include "csp_STM32_dma.h"

#include "hal_lcd.h"
#include "bitmaps.h"
#include "fonts.h"
#include "globtypes.h"
#include "stm32f10x_DMA.h"

#include "csp_LCD_SSD1963.h"


LCDBITMAPDEF* 	LCD_GetBitmapDef(PSZ name);



void LCD_WriteRAM_1(LCD_COLOR RGB_Code);
void LCD_WriteRAM_n(LCD_COLOR RGB_Code, uint32_t count);
void LCD_WriteRAM_n_loop(LCD_COLOR RGB_Code, uint32_t count);
void LCD_WriteRAM_n_dma(LCD_COLOR RGB_Code, uint32_t count);


void LCD_DrawVHLine(U16 startX, U16 startY, U16 Length, U8 Direction, U16 color);
void LCD_RenderColorBitmap(U16 startX, U16 startY, U16 width,U16 height,  const U16* pData);



/*******************************************************************************
* Function Name  : LCD_WriteRAM_1
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_1(LCD_COLOR RGB_Code)
{
	/* Write 16-bit GRAM Reg */
	LCD_RAM_VALUE = RGB_Code;
}

/*************************************************************************************************
* Function Name : 	LCD_WriteRAM_n
* Description   : 	This Function transfers data to LCD driver
* Arguments     : 	LCD_COLOR 	RGB_Code	data to transfer to LCD driver
*					uint32_t 	count		number of times tosend data
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/09/18	W. Paul			Created
*************************************************************************************************/
void LCD_WriteRAM_n(LCD_COLOR RGB_Code, uint32_t count)
{
	if(count > 30){	LCD_WriteRAM_n_dma(		RGB_Code, count);	}
	else{			LCD_WriteRAM_n_loop(	RGB_Code, count);	}

	return;
}


/*************************************************************************************************
* Function Name : 	LCD_WriteRAM_n_loop
* Description   : 	This Function transfers data to LCD driver using a loop transfer
* Arguments     : 	LCD_COLOR 	RGB_Code	data to transfer to LCD driver
*					uint32_t 	count		number of times tosend data
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/09/18	W. Paul			Created
*************************************************************************************************/
void LCD_WriteRAM_n_loop(LCD_COLOR RGB_Code, uint32_t count)
{
	uint32_t	i;

	for(i=0;i<count;i++){
		LCD_RAM_VALUE = RGB_Code;
	}
	return;
}

/*************************************************************************************************
* Function Name : 	LCD_WriteRAM_n_dma
* Description   : 	This Function transfers data to LCD driver using DMA transfer
* Arguments     : 	LCD_COLOR 	RGB_Code	data to transfer to LCD driver
*					uint32_t 	count		number of times tosend data
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/09/18	W. Paul			Created
*************************************************************************************************/
void LCD_WriteRAM_n_dma(LCD_COLOR RGB_Code, uint32_t count)
{

	csp_STM32_DMA2_CH1_config_m2m(	(uint32_t)&RGB_Code, (uint32_t)&LCD_RAM_VALUE	,count);

	return;
}


/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
*******************************************************************************/
U16 LCD_ReadRAM(void)
{
	/* Write 16-bit Index (then Read Reg) */
	// LCD_REG_VALUE = R34; /* Select GRAM Reg */
	/* Read 16-bit Reg */
	return LCD_RAM_VALUE;
}



void LCD_Clear(LCD_COLOR color)
{
	LCD_blank_screen((uint16_t)color);
	return;
}



void LCD_DrawDot(U16 startX, U16 startY, U16 color)
{
	if ((startX < LCD_DISP_WIDTH) && (startY < LCD_DISP_HEIGHT))
	{
		LCD_SetCursor(startX, startY);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);
	}
}

U16 LCD_ReadPixel(U16 startX, U16 startY)
{
	LCD_SetCursor(startX, startY);
	LCD_GRAM_rd_cmd();
	return LCD_ReadRAM();
}

void LCD_DrawLine(U16 startX, U16 startY, U16 endX, U16 endY, LCD_COLOR color)
{
	int p1x = startX;
	int p1y = startY;
	int p2x = endX;
	int p2y = endY;
	int F, x, y, temp;

	if (p1x > p2x){  // Swap points if p1 is on the right of p2
		LCD_DrawLine(p2x, p2y, p1x, p1y, color);
		return;
	}

	// Handle trivial cases separately for algorithm speed up.
	// Trivial case 1: m = +/-INF (Vertical line)
	if (p1x == p2x){
		if (p1y > p2y){  // Swap y-coordinates if p1 is above p2
			//swap(p1y, p2y);
			temp = p1y; p1y = p2y; p2y = temp;
		}
		x = p1x;
		y = p1y;
		while (y <= p2y){
			LCD_DrawDot(x, y, color);
			y++;
		}
		return;
	}
	// Trivial case 2: m = 0 (Horizontal line)
	else if (p1y == p2y){
		x = p1x;
		y = p1y;
		while (x <= p2x){
			LCD_DrawDot(x, y, color);
			x++;
		}
		return;
	}

	{
		int dy            = p2y - p1y;  // y-increment from p1 to p2
		int dx            = p2x - p1x;  // x-increment from p1 to p2
		int dy2           = (dy << 1);  // dy << 1 == 2*dy
		int dx2           = (dx << 1);
		int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
		int dy2_plus_dx2  = dy2 + dx2;


		if (dy >= 0){    // m >= 0
			// Case 1: 0 <= m <= 1 (Original case)
			if (dy <= dx){
				F = dy2 - dx;    // initial F

				x = p1x;
				y = p1y;
				while (x <= p2x){
					LCD_DrawDot(x, y, color);
					if (F <= 0){
						F += dy2;
					}
					else{
						y++;
						F += dy2_minus_dx2;
					}
					x++;
				}
			}
			// Case 2: 1 < m < INF (Mirror about y=x line
			// replace all dy by dx and dx by dy)
			else{
				F = dx2 - dy;    // initial F

				y = p1y;
				x = p1x;
				while (y <= p2y){
					LCD_DrawDot(x, y, color);
					if (F <= 0){
						F += dx2;
					}
					else{
						x++;
						F -= dy2_minus_dx2;
					}
					y++;
				}
			}
		}
		else{    // m < 0
			// Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
			if (dx >= -dy){
				F = -dy2 - dx;    // initial F

				x = p1x;
				y = p1y;
				while (x <= p2x){
					LCD_DrawDot(x, y, color);
					if (F <= 0){
						F -= dy2;
					}
					else{
						y--;
						F -= dy2_plus_dx2;
					}
					x++;
				}
			}
			// Case 4: -INF < m < -1 (Mirror about x-axis and mirror
			// about y=x line, replace all dx by -dy and dy by dx)
			else{
				F = dx2 + dy;    // initial F

				y = p1y;
				x = p1x;
				while (y >= p2y){
					LCD_DrawDot(x, y, color);
					if (F <= 0){
						F += dx2;
					}
					else{
						x++;
						F += dy2_plus_dx2;
					}
					y--;
				}
			}
		}
	}

	return;
}

void LCD_DrawVHLine(U16 startX, U16 startY, U16 Length, U8 Direction, LCD_COLOR color)
{
	if (Direction != LCD_HORIZ_LINE){
		LCD_SetWindow(startX, startX,startY,startY+Length);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_n(color,Length);
	}
	else{
		LCD_SetWindow(startX, startX+Length,startY,startY+1);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_n(color,Length);
	}
	LCD_RegWrite(SSD1963_CMD_NOP,0,0);		//cmd only
	return;
}

void LCD_DrawRect_1(U16 startX, U16 startY, U16 Width, U16 Height, LCD_COLOR color)
{
	// draw horizontal lines
	LCD_DrawVHLine(startX, startY, Width, LCD_HORIZ_LINE, color);
	LCD_DrawVHLine(startX, (startY + Height - 1), Width, LCD_HORIZ_LINE, color);

	// draw vertical lines
	LCD_DrawVHLine(startX, startY, Height, LCD_VERT_LINE, color);
	LCD_DrawVHLine((startX + Width - 1),startY, Height, LCD_VERT_LINE, color);
}

void LCD_DrawRect_n(U16 startX, U16 startY, U16 Width, U16 Height, U16 thick, LCD_COLOR color)
{
	U16 	i;

	for(i=0;i<thick;i++){
		LCD_DrawRect_1(startX, startY,  Width,Height, color);
		startX	+= 1;
		startY	+= 1;
		Height	-= 2;
		Width	-= 2;
	}

}

void LCD_FillRect(U16 startX, U16 startY, U16 Width, U16 Height, LCD_COLOR color)
{
//	int i;
//	int j;
	// draw horizontal lines
//	for (i=0; i<Height; i++)
//	{
//		LCD_DrawVHLine(startX, startY, Width, LCD_HORIZ_LINE, color);
//		startY++;
//	}

	LCD_SetWindow(startX, startX+Width-1,startY,startY+Height);
	LCD_GRAM_wr_cmd();
//	for (i = 0; i < Width; i++){
//		for (j = 0; j < Height; j++){
			LCD_WriteRAM_n(color,(uint32_t)Width*Height);
//		}
//	}
	return;
}

//
//	NOTE: number = 0 <> (grades - 1)
//
LCD_COLOR LCD_GradColor(LCD_COLOR start, LCD_COLOR end, U16 grades, U16 number)
{
	LCD_COLOR result;
	// RGB -> 5:6:5 bits
	int startR = LCD_EXTRACT_RED(start);
	int startG = LCD_EXTRACT_GREEN(start);
	int startB = LCD_EXTRACT_BLUE(start);
	int endR = LCD_EXTRACT_RED(end);
	int endG = LCD_EXTRACT_GREEN(end);
	int endB = LCD_EXTRACT_BLUE(end);
	if (grades < 2){		 grades = 2;	}
	if (number >= grades){ 	number = grades-1;	}
	endR = startR + (((endR - startR) * number)/(grades-1));
	endG = startG + (((endG - startG) * number)/(grades-1));
	endB = startB + (((endB - startB) * number)/(grades-1));
	// reassemble the color
	result = LCD_ASSEMBLE_RGB(endR ,endG ,endB);
	return result;
}

void LCD_FillGradRect(U16 startX, U16 startY, U16 width,U16 height,LCD_COLOR startColor, LCD_COLOR endColor)
{
	int i = 0;
	U16 color = 0;
	// draw rectangle a line at a time.
	for (i = 0; i < height; i++)
	{
		color = LCD_GradColor(startColor, endColor, height, i);
		LCD_DrawVHLine(startX, startY+i, width, LCD_HORIZ_LINE, color);
	}
}

void LCD_FillGradSubRect(U16 orgStartY, U16 orgHeight, LCD_COLOR startColor, LCD_COLOR endColor,U16 startX, U16 startY, U16 height, U16 width)
{
	int i = 0;
	U16 color = 0;
	// draw rectangle a line at a time.
	for (i = startY; i < (startY + height); i++)
	{
		// establish color from full rectangle color range
		color = LCD_GradColor(startColor, endColor, orgHeight, (i - orgStartY));
		LCD_DrawVHLine(startX, i, width, LCD_HORIZ_LINE, color);
	}
}

void LCD_DrawCircle(U16 startX, U16 startY, U16 Radius, LCD_COLOR color)
{
	s32  D;/* Decision Variable */
	u32  CurX;/* Current X Value */
	u32  CurY;/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		LCD_SetCursor(startX + CurX, startY + CurY);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);

		LCD_SetCursor(startX + CurX, startY - CurY);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);

		LCD_SetCursor(startX - CurX, startY + CurY);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);

		LCD_SetCursor(startX - CurX, startY - CurY);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);

		LCD_SetCursor(startX + CurY, startY + CurX);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);

		LCD_SetCursor(startX + CurY, startY - CurX);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);

		LCD_SetCursor(startX - CurY, startY + CurX);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);

		LCD_SetCursor(startX - CurY, startY - CurX);
		LCD_GRAM_wr_cmd();
		LCD_WriteRAM_1(color);

		if (D < 0){
			D += (CurX << 2) + 6;
		}
		else{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

void LCD_FillCircle(U16 startX, U16 startY, U16 Radius, LCD_COLOR color)
{
	s32  D;/* Decision Variable */
	u32  CurX;/* Current X Value */
	u32  CurY;/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY){
		LCD_DrawLine(startX + CurX, startY + CurY, startX + CurX, startY - CurY, color);
		LCD_DrawLine(startX - CurX, startY + CurY, startX - CurX, startY - CurY, color);
		LCD_DrawLine(startX + CurY, startY + CurX, startX + CurY, startY - CurX, color);
		LCD_DrawLine(startX - CurY, startY + CurX, startX - CurY, startY - CurX, color);

		if (D < 0){
			D += (CurX << 2) + 6;
		}
		else{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

void LCD_DrawQuarter(U16 startX, U16 startY, U16 Radius, U8 quadrant, LCD_COLOR color)
{
	s32  D;/* Decision Variable */
	u32  CurX;/* Current X Value */
	u32  CurY;/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY){
		switch(quadrant){
			case 0:
				LCD_DrawDot(startX + CurX, startY - CurY, color);
				LCD_DrawDot(startX + CurY, startY - CurX, color);
				break;
			case 1:
				LCD_DrawDot(startX - CurX, startY - CurY, color);
				LCD_DrawDot(startX - CurY, startY - CurX, color);
				break;
			case 2:
				LCD_DrawDot(startX - CurX, startY + CurY, color);
				LCD_DrawDot(startX - CurY, startY + CurX, color);
				break;
			case 3:
				LCD_DrawDot(startX + CurX, startY + CurY, color);
				LCD_DrawDot(startX + CurY, startY + CurX, color);
				break;
		}
		if (D < 0){
			D += (CurX << 2) + 6;
		}
		else{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

void LCD_FillQuarter(U16 startX, U16 startY, U16 Radius, U8 quadrant, LCD_COLOR color)
{
	s32  D;/* Decision Variable */
	u32  CurX;/* Current X Value */
	u32  CurY;/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY){
		switch(quadrant){
			case 0:
				LCD_DrawLine(startX + CurX, startY - CurY, startX + CurX, startY, color);
				LCD_DrawLine(startX + CurY, startY - CurX, startX + CurY, startY, color);
				break;
			case 1:
				LCD_DrawLine(startX - CurX, startY - CurY, startX - CurX, startY, color);
				LCD_DrawLine(startX - CurY, startY - CurX, startX - CurY, startY, color);
				break;
			case 2:
				LCD_DrawLine(startX - CurX, startY + CurY, startX - CurX, startY, color);
				LCD_DrawLine(startX - CurY, startY + CurX, startX - CurY, startY, color);
				break;
			case 3:
				LCD_DrawLine(startX + CurX, startY + CurY, startX + CurX, startY, color);
				LCD_DrawLine(startX + CurY, startY + CurX, startX + CurY, startY, color);
				break;
		}
		if (D < 0){
			D += (CurX << 2) + 6;
		}
		else{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

void LCD_FillSoftRect(U16 startX, U16 startY,  U16 width,U16 height,U16 cornerSize, LCD_COLOR color)
{
	//fill corners in UR, UL, LL, LR
	LCD_FillQuarter(startX + width - cornerSize, startY + cornerSize, cornerSize,0, color);
	LCD_FillQuarter(startX + cornerSize, startY + cornerSize, cornerSize, 1,color);
	LCD_FillQuarter(startX + cornerSize, startY + height - cornerSize, cornerSize,2, color);
	LCD_FillQuarter(startX + width - cornerSize, startY + height - cornerSize,cornerSize, 3, color);
	// fill the top and bottom rectangles
	LCD_FillRect(startX + cornerSize, startY,  width - (2 * cornerSize),cornerSize + 1,	color);
	LCD_FillRect(startX + cornerSize, startY + height - cornerSize, width - (2 * cornerSize),cornerSize + 1, color);
	// then the main body
	LCD_FillRect(startX, startY + cornerSize, width + 1,height - (2 * cornerSize),	color);
}

void LCD_DrawSoftRect_1(U16 startX, U16 startY, U16 width,U16 height, U16 cornerSize, LCD_COLOR color)
{
	// draw corners in UR, UL, LL, LR
	LCD_DrawQuarter(startX + width - cornerSize	, startY + cornerSize			, cornerSize, 0, color);
	LCD_DrawQuarter(startX + cornerSize			, startY + cornerSize			, cornerSize, 1, color);
	LCD_DrawQuarter(startX + cornerSize			, startY + height - cornerSize	, cornerSize, 2, color);
	LCD_DrawQuarter(startX + width - cornerSize	, startY + height - cornerSize	, cornerSize, 3, color);
	// draw top, bottom, left and right side
	LCD_DrawLine(startX + cornerSize	, startY				, startX + width - cornerSize	, startY						, color);
	LCD_DrawLine(startX + cornerSize	, startY + height		, startX + width - cornerSize	, startY + height				, color);
	LCD_DrawLine(startX					, startY + cornerSize	, startX						, startY + height - cornerSize	, color);
	LCD_DrawLine(startX + width			, startY + cornerSize	, startX + width				, startY + height - cornerSize	, color);
}


void LCD_DrawSoftRect_n(U16 startX, U16 startY, U16 Width, U16 Height, U16 cornerSize, U16 thick, LCD_COLOR color)
{
	U16 	i;

	for(i=0;i<thick-1;i++){
		LCD_DrawSoftRect_2(startX, startY,  Width,Height, cornerSize, color);
		startX	+= 1;
		startY	+= 1;
		Height	-= 2;
		Width	-= 2;
	}

	LCD_DrawSoftRect_1(startX, startY, Width, Height, cornerSize, color);
	return;

}

void LCD_DrawSoftRect_2(U16 startX, U16 startY, U16 width, U16 height, U16 cornerSize, LCD_COLOR color)
{
	LCD_DrawQuarter_2(startX + width - cornerSize, startY + cornerSize,				cornerSize,	0, color);
	LCD_DrawQuarter_2(startX + cornerSize, startY + cornerSize, 					cornerSize,	1, color);
	LCD_DrawQuarter_2(startX + cornerSize, startY + height - cornerSize, 			cornerSize, 2, color);
	LCD_DrawQuarter_2(startX + width - cornerSize, startY + height - cornerSize,	cornerSize,	3, color);
//
//	LCD_FillRect(startX + cornerSize,		startY,						width - 2*cornerSize,	thick,					color);
//	LCD_FillRect(startX + cornerSize,		startY + height - thick,	width - 2*cornerSize,	thick,					color);
//	LCD_FillRect(startX,					startY + cornerSize,		thick, 					height - 2*cornerSize,	color);
//	LCD_FillRect(startX + width - thick,	startY + cornerSize,		thick,					height - 2*cornerSize,	color);

	LCD_DrawLine(startX + cornerSize	, startY				, startX + width - cornerSize	, startY						, color);
	LCD_DrawLine(startX + cornerSize	, startY + height		, startX + width - cornerSize	, startY + height				, color);
	LCD_DrawLine(startX					, startY + cornerSize	, startX						, startY + height - cornerSize	, color);
	LCD_DrawLine(startX + width			, startY + cornerSize	, startX + width				, startY + height - cornerSize	, color);

	return;
}

void LCD_DrawQuarter_2(U16 startX, U16 startY, U16 Radius, U8 quadrant, LCD_COLOR color)
{
	s32  D;/* Decision Variable */
	u32  CurX;/* Current X Value */
	u32  CurY;/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY){
		switch(quadrant){
			case 0:
				LCD_DrawDot(startX + CurX,		startY - CurY,		color);
				LCD_DrawDot(startX + CurX,		startY - CurY + 1,	color);
				LCD_DrawDot(startX + CurY,		startY - CurX,		color);
				LCD_DrawDot(startX + CurY - 1,	startY - CurX,		color);
				break;
			case 1:
				LCD_DrawDot(startX - CurX,		startY - CurY,		color);
				LCD_DrawDot(startX - CurX,		startY - CurY + 1,	color);
				LCD_DrawDot(startX - CurY, 		startY - CurX,		color);
				LCD_DrawDot(startX - CurY + 1,	startY - CurX,		color);
				break;
			case 2:
				LCD_DrawDot(startX - CurX,		startY + CurY,		color);
				LCD_DrawDot(startX - CurX,		startY + CurY - 1,	color);
				LCD_DrawDot(startX - CurY,		startY + CurX,		color);
				LCD_DrawDot(startX - CurY + 1,	startY + CurX,		color);
				break;
			case 3:
				LCD_DrawDot(startX + CurX,		startY + CurY,		color);
				LCD_DrawDot(startX + CurX,		startY + CurY - 1,	color);
				LCD_DrawDot(startX + CurY,		startY + CurX,		color);
				LCD_DrawDot(startX + CurY - 1,	startY + CurX,		color);
				break;
		}
		if (D < 0){
			D += (CurX << 2) + 6;
		}
		else{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}


void LCD_DrawHTrap(U16 startX, U16 startY, U16 startWidth, U16 endX, U16 endY,U16 endWidth, LCD_COLOR color)
{
	// "top" line
	LCD_DrawLine(startX, startY, startX + startWidth, startY, color);
	// "left" side
	LCD_DrawLine(startX, startY, endX, endY, color);
	// "right" side
	LCD_DrawLine(startX + startWidth, startY, endX + endWidth, endY, color);
	// "bottom" line
	LCD_DrawLine(endX, endY, endX + endWidth, endY, color);
}

void LCD_FillHTrap(U16 startX, U16 startY, U16 startWidth, U16 endX, U16 endY,U16 endWidth, LCD_COLOR color)
{
	float width, firstX;
	float lines = (float)startY - (float)endY;	// actually -1
	int inc = -1;
	int i;
	if (lines < 0){
		inc = 1;
		lines = -lines;
	}
	// special case for a single line
	if (lines == 0){
		LCD_DrawLine((startX + endX)/2, startY, (startWidth + endWidth)/2, startY, color);
	}
	else{
		for (i=0; i<=lines; i++){
			width = (float)((startWidth * (lines - i)) + (endWidth * i))/lines;
			firstX = (float)((startX * (lines - i)) + (endX * i))/lines;
			LCD_DrawLine((U16)firstX, startY, (U16)firstX + (U16)width, startY, color);
			startY += inc;
		}
	}
}

LCDBITMAPDEF* LCD_GetBitmapDef(const PSZ name)
{
	// search through the bitmap def table
	PLCDBITMAPDEF *pbmd = bitmapTable;
	// table is null terminated
	while (*pbmd != NULL){
		// compare name
		if (strcmp((*pbmd)->id, name) == 0){
			break;
		}
		// next item
		pbmd++;
	}
	return *pbmd;
}

void LCD_RenderColorBitmap(U16 startX, U16 startY, U16 width,U16 height,const U16* pData)
{
	U16 col;
	U16 row;

	// render the bitmap, one row at a time
	for (row = 0; row < height; row++)
	{
		LCD_SetCursor(startX, startY++);
		LCD_GRAM_wr_cmd();
		for(col = 0; col < width; col++)
		{
			LCD_WriteRAM_1(*pData);
			pData++;
		}
	}
}

void LCD_DispBitmap(U16 startX, U16 startY, const PSZ name)
{
	LCDBITMAPDEF* pbmd;
	// find the bitmap def
	if ((pbmd = LCD_GetBitmapDef(name)) != NULL){
		// validate bitmap type
		ASSERT(pbmd->bitsPerPixel == 16);
		// draw the bitmap
		LCD_RenderColorBitmap(startX, startY,  pbmd->width,pbmd->height,(const U16*)pbmd->pData);
	}
}


LCDBITMAPDEF* LCD_GetBitmapInfo( const PSZ name, U16* pixelX, U16* pixelY, U8* bitsPerPixel)
{
	LCDBITMAPDEF* pbmd;
	// find the bitmap def
	if ((pbmd = LCD_GetBitmapDef(name)) != NULL){
		*pixelX = pbmd->width;
		*pixelY = pbmd->height;
		*bitsPerPixel = pbmd->bitsPerPixel;
		return 	pbmd;
	}
	else{
		return NULL;
	}
}





void LCD_RenderMonoBitmap(U16 startX, U16 startY, const U8* pData, U16 width, U16 height,LCD_COLOR foreColor, LCD_COLOR backColor, BOOLEAN packedBits)
{
	U8 mask = 0x80;
	U16 startstartX = startX;
	U16 col, row;
	ASSERT(pData);          // validate data pointer

	LCD_SetWindow(startX, startX+width-1,startY,startY+height);
	LCD_GRAM_wr_cmd();
	for (row = 0; row < height; row++){
		startX = startstartX;
		for (col = 0; col < width; col++){
			// test for bit active
			if (*pData & mask){
			//	LCD_DrawDot(startX, startY, foreColor);
				LCD_WriteRAM_1(foreColor);
			}
			else // step over the pixel
			{
			//	LCD_DrawDot(startX, startY, backColor);
				LCD_WriteRAM_1(backColor);
			}
			startX++;
			// update mask and data pointer
			mask = mask >> 1;
			if (mask == 0){
				mask = 0x80;
				pData++;
			}
		}
		// increment the row
		startY++;
		// if data ends mid byte, start on next byte
		if (!packedBits && (mask != 0x80)){
			mask = 0x80;
			pData++;
		}
	}
}

void LCD_RenderMonoBitmapTrans(U16 startX, U16 startY, const U8 *pData, U16 width,U16 height, LCD_COLOR color, BOOLEAN packedBits)
{
	U8 mask				= 0x80;
	U16 savestartX 		= startX;
	U16 row;
	U16 col;

	ASSERT(pData);
	for (row = 0; row < height; row++){
		startX = savestartX;
		for (col = 0; col < width; col++){
			// test for bit active
			if (*pData & mask){
				LCD_DrawDot(startX, startY, color);
			}
			startX++;
			// update mask and data pointer
			mask = mask >> 1;
			if (mask == 0){
				mask = 0x80;
				pData++;
			}
		}
		// next row
		startY++;
		// if data ends mid byte, start on next byte
		if (!packedBits && (mask != 0x80)){
			mask = 0x80;
			pData++;
		}

	}
}

void LCD_DispMonoBitmap(U16 startX, U16 startY, const PSZ bitmapName, LCD_COLOR foreColor,LCD_COLOR backColor)
{
	const LCDBITMAPDEF* bitmap = NULL;
	if (bitmap = LCD_GetBitmapDef(bitmapName)){
		// NOTE: bitmaps do NOT pack bits (rows on byte boundary!)
		LCD_RenderMonoBitmap(startX, startY, bitmap->pData, bitmap->width,bitmap->height, foreColor, backColor, FALSE);
	}
	// TODO: log warning here!
	return;
}


void LCD_DispMonoBitmapTrans(U16 startX, U16 startY, const PSZ bitmapName,LCD_COLOR foreColor)
{
	const LCDBITMAPDEF* bitmap = NULL;
	if (bitmap = LCD_GetBitmapDef(bitmapName)){
		// NOTE: bitmaps do NOT pack bits (rows on byte boundary!)
		LCD_RenderMonoBitmapTrans(startX, startY, bitmap->pData, bitmap->width,	bitmap->height, foreColor, FALSE);
	}
	// TODO: log warning here!
	return;
}





//



U16 LCD_GetBitmapHeight(PSZ bitmapName)
{
	LCDBITMAPDEF* pbmd;
	U16 height = 0;
	// find the bitmap def
	if ((pbmd = LCD_GetBitmapDef(bitmapName)) != NULL){
		height = pbmd->height;
	}
	return height;
}

U16 LCD_GetBitmapWidth(PSZ bitmapName)
{
	LCDBITMAPDEF* pbmd;
	U16 width = 0;
	// find the bitmap def
	if ((pbmd = LCD_GetBitmapDef(bitmapName)) != NULL){
		width = pbmd->width;
	}
	return width;
}

typedef LCD_COLOR (*GET_COLOR_FUNC)(U16 xPos, U16 yPos);

//
//	Draw Wu line with user supplied get background color function
//
void LCD_DrawWuLineGetColor (U16 X0, U16 Y0, U16 X1, U16 Y1, LCD_COLOR BaseColor, GET_COLOR_FUNC gcf, U8 edgeBias)
{
	I16 NumLevels = 256;
	U16 IntensityBits = 8;
	U16 IntensityShift;
	U16 ErrorAdj;
	U16 ErrorAcc;
	U16 ErrorAccTemp;
	U16 Weighting;
	U16 WeightingComplementMask;
	I16 DeltaX;
	I16 DeltaY;
	I16 Temp;
	I16 XDir;

	/* Make sure the line runs top to bottom */
	if (Y0 > Y1){
		Temp = Y0;
		Y0 = Y1;
		Y1 = Temp;
		Temp = X0;
		X0 = X1;
		X1 = Temp;
	}
	/* Draw the initial pixel, which is always exactly intersected by
	the line and so needs no weighting */
	LCD_DrawDot(X0, Y0, BaseColor);

	if ((DeltaX = X1 - X0) >= 0){
		XDir = 1;
	}
	else{
		XDir = -1;
		DeltaX = -DeltaX; /* make DeltaX positive */
	}
	/* Special-case horizontal, vertical, and diagonal lines, which
	require no weighting because they go right through the center of
	every pixel */
	if ((DeltaY = Y1 - Y0) == 0){
		/* Horizontal line */
		while (DeltaX-- != 0){
			X0 += XDir;
			LCD_DrawDot(X0, Y0, BaseColor);
		}
	}
	else if (DeltaX == 0){
		/* Vertical line */
		do{
			Y0++;
			LCD_DrawDot(X0, Y0, BaseColor);
		} while (--DeltaY != 0);
	}
	else if (DeltaX == DeltaY){
		/* Diagonal line */
		do{
			X0 += XDir;
			Y0++;
			LCD_DrawDot(X0, Y0, BaseColor);
		} while (--DeltaY != 0);
	}
	else{ /* Line is not horizontal, diagonal, or vertical */
		ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
		/* # of bits by which to shift ErrorAcc to get intensity level */
		IntensityShift = 16 - IntensityBits;
		/* Mask used to flip all bits in an intensity weighting, producing the
		result (1 - intensity weighting) */
		WeightingComplementMask = NumLevels - 1;
		/* Is this an X-major or Y-major line? */
		if (DeltaY > DeltaX){
			/* Y-major line; calculate 16-bit fixed-point fractional part of a
			pixel that X advances each time Y advances 1 pixel, truncating the
			result so that we won't overrun the endpoint along the X axis */
			ErrorAdj = ((U32) DeltaX << 16) / (U32) DeltaY;
			/* Draw all pixels other than the first and last */
			while (--DeltaY){
				ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
				ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
				if (ErrorAcc <= ErrorAccTemp){
					/* The error accumulator turned over, so advance the X coord */
					X0 += XDir;
				}
				Y0++; /* Y-major, so always advance Y */
				/* The IntensityBits most significant bits of ErrorAcc give us the
				intensity weighting for this pixel, and the complement of the
				weighting for the paired pixel */
				Weighting = ErrorAcc >> IntensityShift;

				if (edgeBias == EDGE_BIAS_INSIDE){
					LCD_DrawDot(X0, Y0, BaseColor);
				}
				else{ // outside/none
					LCD_DrawDot(X0, Y0, LCD_GradColor(BaseColor, gcf(X0,Y0), 256, Weighting));
				}

				if (edgeBias == EDGE_BIAS_OUTSIDE){
					LCD_DrawDot(X0 + XDir, Y0, BaseColor);
				}
				else{ // inside/none
					LCD_DrawDot(X0 + XDir, Y0, LCD_GradColor(BaseColor, gcf(X0 + XDir,Y0), 256, (Weighting ^ WeightingComplementMask)));
				}
			}
			/* Draw the final pixel, which is always exactly intersected by the line
			and so needs no weighting */
			LCD_DrawDot(X1, Y1, BaseColor);
		}
		else{ // (DeltaY <= DeltaX)
			/* It's an X-major line; calculate 16-bit fixed-point fractional part of a
			pixel that Y advances each time X advances 1 pixel, truncating the
			result to avoid overrunning the endpoint along the X axis */
			ErrorAdj = ((U32) DeltaY << 16) / (U32) DeltaX;
			/* Draw all pixels other than the first and last */
			while (--DeltaX){
				ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
				ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
				if (ErrorAcc <= ErrorAccTemp){
					/* The error accumulator turned over, so advance the Y coord */
					Y0++;
				}
				X0 += XDir; /* X-major, so always advance X */
				/* The IntensityBits most significant bits of ErrorAcc give us the
				intensity weighting for this pixel, and the complement of the
				weighting for the paired pixel */
				Weighting = ErrorAcc >> IntensityShift;

				if (edgeBias == EDGE_BIAS_INSIDE){
					LCD_DrawDot(X0, Y0, BaseColor);
				}
				else{ // outside/none
					LCD_DrawDot(X0, Y0, LCD_GradColor(BaseColor, gcf(X0, Y0), 256, Weighting));
				}

				if (edgeBias == EDGE_BIAS_OUTSIDE){
					LCD_DrawDot(X0, Y0 + 1,BaseColor);
				}
				else{ // inside/none
					LCD_DrawDot(X0, Y0 + 1, LCD_GradColor(BaseColor, gcf(X0, Y0 + 1), 256, (Weighting ^ WeightingComplementMask)));
				}
			}
			/* Draw the final pixel, which is always exactly intersected by the line
			and so needs no weighting */
			LCD_DrawDot(X1, Y1, BaseColor);
		}
	}
}

static LCD_COLOR saveColor;
static LCD_COLOR LCD_GetFixedColor(U16 xPos, U16 yPos)
{
	return saveColor;
}

//
//	Anti-aliased Line Draw (Wu Line)
//
void LCD_DrawWuLine (U16 X0, U16 Y0, U16 X1, U16 Y1, LCD_COLOR BaseColor, LCD_COLOR BackColor, U8 edgeBias)
{
	saveColor = BackColor;
	LCD_DrawWuLineGetColor (X0, Y0, X1, Y1, BaseColor, LCD_GetFixedColor, edgeBias);
}


/*************************************************************************************************
* Function Name : 	LCD_Output_24BMP_FromScreen
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		07/06/18	W. Paul			Created
*************************************************************************************************/
void LCD_Output_24BMP_FromScreen(U16 start_x, U16 start_y, U16 w, U16 h )
{
	//Declare an object of this type and fill the members according to your file. Use the following as a guide

	BMPHEAD 	bh;
	long		bytesPerLine;
	U16			x;
	U16			y;
	pix565_u	pix565;
	U8*			p;
	U8			extras;
	U16			i;
	U8			output;

	memset((char *)&bh,0,sizeof(BMPHEAD)); /* sets everything to 0 */
	memcpy(bh.id,"BM",2);

	//bh.reserved		= 			//two zero bytes
	bh.headersize		=  54L;		//(for 24 bit images)
	bh.infoSize  		=  0x28L;	//(for 24 bit images)
	bh.width     		=  w;		//width in pixels of your image
	bh.depth     		=  h;		//depth in pixels of your image
	bh.biPlanes  		=  1;		//(for 24 bit images)
	bh.bits      		=  24;		//(for 24 bit images)
	bh.biCompression 	=  0L;		//(no compression)

	bytesPerLine = bh.width * 3;  	//(for 24 bit images)
	extras	=  bytesPerLine%4;
	bytesPerLine	+= extras;

	bh.filesize			=  sizeof(BMPHEAD);
	bh.filesize			+= (long)bytesPerLine * bh.depth;		//calculated size of your file (see below)

	p	=  (U8*)&bh;
	for(i=0;i<sizeof(BMPHEAD);i++){
		putchar(*p);
		p++;
	}

	for (y = h; y > 0; y --){
		LCD_SetWindow(0,w,y-1,y-1);
		LCD_GRAM_rd_cmd();
		//start at the bottom
		for(x=0;x<w;x++){
			pix565.data	=  LCD_RAM_VALUE;
			output	=  pix565.parts.b;		putchar(output << 3);
			output	=  pix565.parts.g;		putchar(output << 2);
			output	=  pix565.parts.r;		putchar(output << 3);
		}
		LCD_RegWrite(SSD1963_CMD_NOP,0,0);		//cmd only
		for(i=0;i<extras;i++){
			putchar(NULL_CHAR);
		}
		LCD_DrawDot(0,y,0xffff);	//WHITE

		app_sys_watchdog_reload();
   }

	return;
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
