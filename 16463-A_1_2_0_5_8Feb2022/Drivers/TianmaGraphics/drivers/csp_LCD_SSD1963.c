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
 * Filename    :  csp_LCD_SSD1963.c
 * Date Created:  Wed 13 Sep 2017 03:32:51 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "csp_LCD_SSD1963.h"
#include "Colours.h"

#include "csp_STM32_FSMC.h"
#include "csp_STM32_delay.h"
#include "pcb_pins.h"

/*********************************************************************************************************
 *		Notes
 ********************************************************************************************************/
// this SSD1963 is setup to drive the 7inch 800x480 MLT070W50 LCD
// ensure 'hal_LCD.c'has included this file


/*********************************************************************************************************
 *		Local Variables
 ********************************************************************************************************/
volatile uint16_t* LCD_REG_PTR 	= (uint16_t*)LCD_BASE;					// register write
volatile uint16_t* LCD_RAM_PTR 	= (uint16_t*)(LCD_BASE + LCD_OFFSET);	// data read/write


/*********************************************************************************************************
 *		Local Functions
 ********************************************************************************************************/
void		LCD_RegWrite( uint8_t RegAddress, uint16_t *RegData, uint16_t n);
void		LCD_RegWrite1(uint8_t RegAddress, uint16_t RegData);
void		LCD_RegRead(  uint8_t RegAddress, uint16_t *RegData, uint16_t n);
uint16_t	LCD_RegRead1( uint8_t RegAddress);

LCD_ROTATION_enum		rotation	=  LCD_ROTATE_0;

/*********************************************************************************************************
 ********************************************************************************************************/

/*************************************************************************************************
* Function Name : 	LCD_Check_Reset
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		22/11/19	T. Barr			Created
*************************************************************************************************/
uint8_t LCD_Check_Reset(void)
{
	LCD_STATUS_enum		status	=  LCD_STATUS_OK;
	uint16_t			prod_id[5];
	
    LCD_RegRead(SSD1963_CMD_RD_DDB_START, prod_id, 5);
	if(prod_id[0] != 0x01){	status	=  LCD_STATUS_FAIL;	}
	if(prod_id[1] != 0x57){	status	=  LCD_STATUS_FAIL;	}
	if(prod_id[2] != 0x61){	status	=  LCD_STATUS_FAIL;	}
	if(prod_id[3] != 0x01){	status	=  LCD_STATUS_FAIL;	}
	if(prod_id[4] != 0xff){	status	=  LCD_STATUS_FAIL;	}
	
	if(status	== LCD_STATUS_FAIL){
		LCD_Init_SSD1963();
		
		return 1;
	}
	
	return 0;
}


/*************************************************************************************************
* Function Name : 	LCD_Init_SSD1963
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/04/17	W. Paul			Created
*************************************************************************************************/
void LCD_Init_SSD1963(void)
{
	LCD_STATUS_enum		status	=  LCD_STATUS_OK;
	uint16_t			prod_id[5];
	uint16_t			reg_data[10];
	union{
		uint32_t	a;
		uint8_t		b[4];
	}u4B;
	union{
		uint16_t	a;
		uint8_t		b[2];
	}u2B;

	csp_FSMC_NE1_Config(FSMC_STARTUP);
	PinSet(LCD_RESET,0);	//reset the chip
	LCD_RegRead1(SSD1963_CMD_NOP);
	Delay(5);
	PinSet(LCD_RESET,1);	//release from reset
	Delay(120);
	LCD_RegWrite(SSD1963_CMD_EXIT_SLEEP, reg_data, 0);	//exit sleep
	Delay(20);
	/* check for SSD1963 */

    LCD_RegRead(SSD1963_CMD_RD_DDB_START,prod_id,5);
//	prod_id[0]	supplier id
//	prod_id[1]	supplier id
//	prod_id[2]	supplier id
//	prod_id[3]	product revision
//	prod_id[4]

	if(prod_id[0] != 0x01){	status	=  LCD_STATUS_FAIL;	}
	if(prod_id[1] != 0x57){	status	=  LCD_STATUS_FAIL;	}
//	if(prod_id[1] != 0x5B){	status	=  LCD_STATUS_FAIL;	}
	if(prod_id[2] != 0x61){	status	=  LCD_STATUS_FAIL;	}
	if(prod_id[3] != 0x01){	status	=  LCD_STATUS_FAIL;	}
	if(prod_id[4] != 0xff){	status	=  LCD_STATUS_FAIL;	}

	if(status == LCD_STATUS_OK){

		/* NOTE: SSD1963 is running slow at this point (~10MHz)
		 * may need to use slow FSMC until PLL is running */

		/* configure PLL dividers/multipliers */
		reg_data[0]	=  MULTIPLIER_N;
		reg_data[1]	=  DIVIDER_M;
		reg_data[2]	=  0x54;
		LCD_RegWrite(SSD1963_CMD_SET_PLL_MN, reg_data, 3);

		/* enable PLL */
		LCD_RegWrite1(SSD1963_CMD_PLL_START,0x01);
		Delay(50); /* delay 50 ms */

		/* switch to PLL */
		LCD_RegWrite1(SSD1963_CMD_PLL_START,0x03);

		/* speed up fsmc access */
		csp_FSMC_NE1_Config(FSMC_HIGH_SPEED);

		LCD_RegWrite(SSD1963_CMD_SOFT_RESET,0,0);
		Delay(50); /* delay 50 ms */

		/* set display pixel clock */
		u4B.a		=  LCDC_FPR;
		reg_data[0]	=  u4B.b[2];
		reg_data[1]	=  u4B.b[1];
		reg_data[2]	=  u4B.b[0];
		LCD_RegWrite(SSD1963_CMD_SET_PCLK, reg_data, 3);

		/* configure panel size */
		//A5 1= 24bit TFT
		//A4 0= Disable FRC or dithering
		//A3 0= TFT FRC enable
		//A2 0= LSHIFT: Data latch in falling edge
		//A1 0= LLINE: Active low
		//A0 0= LFRAME: Active low
		reg_data[0]	=  0x20;
		//B6 0= TFT Mode
		//B5 0= TFT Mode
		reg_data[1]	=  0x00;
		u2B.a		=  LCD_DISP_WIDTH-1;
		reg_data[2]	=  u2B.b[1];
		reg_data[3]	=  u2B.b[0];
		u2B.a		=  LCD_DISP_HEIGHT-1;
		reg_data[4]	=  u2B.b[1];
		reg_data[5]	=  u2B.b[0];
          //G543  000 = RGB
          //G210  000 = RGB
        reg_data[6]	=  0x00;
        LCD_RegWrite(SSD1963_CMD_SET_PANEL_MODE, reg_data, 7);

		/* configure Horz display period */
		u2B.a		=  LCD_HORI_TOTAL-1;
		reg_data[0]	=  u2B.b[1];					//P1
		reg_data[1]	=  u2B.b[0];					//P2
		u2B.a		=  LCD_HORI_BACK_PORCH_TOTAL & 0x07FF;
		reg_data[2]	=  u2B.b[1];					//P3
		reg_data[3]	=  u2B.b[0];					//P4
		reg_data[4]	=  LCD_HORI_PULSE_WIDTH - 1;	//P5
		reg_data[5]	=  0x00;						//P6
		reg_data[6]	=  0x00;						//P7
		reg_data[7]	=  0x00;						//P8
		LCD_RegWrite(SSD1963_CMD_SET_HORZ_PERIOD, reg_data, 8);

		/* configure Vert display period */
		u2B.a		=  LCD_VERT_TOTAL-1;
		reg_data[0]	=  u2B.b[1];
		reg_data[1]	=  u2B.b[0];
		u2B.a		=  LCD_VERT_BACK_PORCH_TOTAL & 0x07FF;
		reg_data[2]	=  u2B.b[1];
		reg_data[3]	=  u2B.b[0];
		reg_data[4]	=  LCD_VERT_PULSE_WIDTH - 1;
		reg_data[5]	=  0x00;
		reg_data[6]	=  0x00;
		LCD_RegWrite(SSD1963_CMD_SET_VERT_PERIOD, reg_data, 7);

		/* 16-bit (565 format) */
		LCD_RegWrite1(SSD1963_CMD_SET_DATA_INTERFACE,0x03);		//16bit RGB 565 uProcessor interface
		LCD_RegWrite1(SSD1963_CMD_SET_PIXEL_FORMAT,0x50);		//24bit pix data to LCD

		//Note the SSD1963 chip is always powered
		//power up the LCD
//		PinSet(RUN_LCD,1);		//0 = LCD 3v3 on	//now in brightness control
//		PinSet(LCD_MODE,1);							//now in brightness control
//		Delay(5);
		PinSet(EN_LCD,1);


		//GPIO pins on LCD driver
			// b7	gpio3 (mcu-0 lcd-1)			0	// Dither
			// b6	gpio2 (mcu-0 lcd-1)			0	// U/D
			// b5	gpio1 (mcu-0 lcd-1)			0	// L/R
			// b4	gpio0 (mcu-0 lcd-1)			0	// unused
			// b3	gpio3 (input-0 output-1)	0
			// b2	gpio2 (input-0 output-1)	1
			// b1	gpio1 (input-0 output-1)	1
			// b0	gpio0 (input-0 output-1)	1
		reg_data[0]	=  0x0F;
			// b7-1 n/a  0
			// b0	GPIO 0 is normal / sleep	1
		reg_data[1]	=  0x01;
		LCD_RegWrite(SSD1963_CMD_SET_GPIO_CONF, reg_data, 2);

		//GPIO values
			// b7-4 n/a  0
			// b3	gpio3	output 1    Dither= (1-LCD 6bit resolution)(0-LCD 8bit resoltution)	565format-so dither on
			// b2	gpio2	output 1	U/D		1 origin top (furthest from ribbon)
			// b1	gpio1	output 1	L/R		1 origin left
			// b0	gpio0	output 0	notused
		LCD_RegWrite1(SSD1963_CMD_SET_GPIO_VAL, 0x0e);

		LCD_RegWrite1(SSD1963_CMD_SET_ADDR_MODE,(uint8_t)rotation);

		/* turn display on */
		LCD_RegWrite1(SSD1963_CMD_SET_TEAR_ON,0x00);

		Delay(100);
		LCD_blank_screen(0);	//0 = COLOUR_BLACK

		LCD_DisplayOn();
	}
	else{

		printf("\r\nInit LCD fail");
	}
	return;
}

/*************************************************************************************************
* Function Name : 	LCD_RegWrite
* Description   : 	This Function writes many vars to registers
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/09/17	W. Paul			Created
*************************************************************************************************/
void LCD_RegWrite(uint8_t RegAddress, uint16_t *RegData, uint16_t n)
{
	uint16_t	i;


	LCD_REG_VALUE = RegAddress;

	for(i=0;i<n;i++){
		LCD_RAM_VALUE = *RegData;
		RegData++;
	}

	return;
}




/*************************************************************************************************
* Function Name : 	LCD_RegWrite1
* Description   : 	This Function writes 1 var to a register
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/09/17	W. Paul			Created
*************************************************************************************************/
void LCD_RegWrite1(uint8_t RegAddress, uint16_t RegData)
{

	LCD_REG_VALUE = RegAddress;
	LCD_RAM_VALUE = RegData;


	return;
}

/*************************************************************************************************
* Function Name : 	LCD_RegRead
* Description   : 	This Function reads many registers
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/09/17	W. Paul			Created
*************************************************************************************************/
void LCD_RegRead(uint8_t RegAddress, uint16_t *RegData, uint16_t n)
{
	uint16_t	i;

	LCD_REG_VALUE = RegAddress;

	for(i=0;i<n;i++){
		*RegData = LCD_RAM_VALUE;
		RegData++;
	}


	return;
}

/*************************************************************************************************
* Function Name : 	LCD_RegRead1
* Description   : 	This Function reads one registers
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/09/17	W. Paul			Created
*************************************************************************************************/
uint16_t LCD_RegRead1(uint8_t RegAddress)
{
	uint16_t	RegData;

	LCD_REG_VALUE = RegAddress;
	RegData = LCD_RAM_VALUE;

	return(RegData);
}

/*************************************************************************************************
* Function Name : 	LCD_RegRead_print
* Description   : 	This Function reads the register data and prints it
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/09/17	W. Paul			Created
*************************************************************************************************/
void LCD_RegRead_print(uint8_t RegAddress, uint16_t n)
{
	uint16_t		RegData[10];
	uint8_t			i;

	LCD_RegRead(RegAddress,RegData, n);

	printf("\r\nReg %02x    ",RegAddress);
	for(i=0;i<n;i++){
		printf("%02x ",RegData[i]);
	}

	return;
}


/*************************************************************************************************
* Function Name : 	LCD_DisplayOn
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
void LCD_DisplayOn(void)
{
	LCD_RegWrite(SSD1963_CMD_ON_DISPLAY,0,0);

	LCD_brightness(25);
	return;
}

/*************************************************************************************************
* Function Name : 	LCD_DisplayOff
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
void LCD_DisplayOff(void)
{
	LCD_RegWrite(SSD1963_CMD_OFF_DISPLAY,0,0);

	LCD_brightness(0);

	return;
}


/***********************************************************************************************
* Function Name : 	LCD_SSD1963_SleepMode
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
void LCD_SSD1963_SleepMode(uint8_t en_dis)
{
	if(en_dis){	LCD_RegWrite(SSD1963_CMD_ENT_SLEEP,0,0);	}
	else{		LCD_RegWrite(SSD1963_CMD_EXIT_SLEEP,0,0);	}

	return;
}

/*************************************************************************************************
* Function Name : 	LCD_SSD1963_DeepSleepMode
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		27/11/18	W. Paul			Created
*************************************************************************************************/
void LCD_SSD1963_DeepSleepMode(uint8_t en_dis)
{
	if(en_dis){
		LCD_RegWrite(SSD1963_CMD_ENT_DEEP_SLEEP,0,0);
		csp_FSMC_NE1_Config(FSMC_OFF);				//shutdown STM FSMC bus
	}
	else{
		csp_FSMC_NE1_Config(FSMC_HIGH_SPEED);		//pickup where left off
		LCD_RegWrite(SSD1963_CMD_NOP,0,0);
		LCD_RegWrite(SSD1963_CMD_NOP,0,0);
	}
	return;
}



void LCD_SetCursor(uint16_t startX, uint16_t startY)
{
	LCD_SetWindow(startX, LCD_DISP_WIDTH, startY,LCD_DISP_HEIGHT);
	return;
}


/*************************************************************************************************
* Function Name : 	LCD_SetWindow
* Description   : 	This Function sets the window on teh screen for pixccel entry
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/09/17	W. Paul			Created
*************************************************************************************************/
void LCD_SetWindow(uint16_t startX, uint16_t endX,uint16_t startY, uint16_t endY)
{
	uint16_t	output[4];
	union{
		uint16_t	a;
		uint8_t		b[2];
	}u2B;

	if(endX >= LCD_DISP_WIDTH){		endX = LCD_DISP_WIDTH-1;	}
	if(endY >= LCD_DISP_HEIGHT){	endY = LCD_DISP_HEIGHT-1;	}

	if((rotation==LCD_ROTATE_90)||(rotation==LCD_ROTATE_270)){
//		printf("\r\nRot 90/270");
		/* Set Column Address */
		u2B.a	=  startY;
		output[0]	=  u2B.b[1];
		output[1]	=  u2B.b[0];
		u2B.a	=  endY;
		output[2]	=  u2B.b[1];
		output[3]	=  u2B.b[0];
		LCD_RegWrite(SSD1963_CMD_SET_COLUMN, output, 4);

		/* Set Page Address */
		u2B.a	=  startX;
		output[0]	=  u2B.b[1];
		output[1]	=  u2B.b[0];
		u2B.a	=  endX;
		output[2]	=  u2B.b[1];
		output[3]	=  u2B.b[0];
		LCD_RegWrite(SSD1963_CMD_SET_PAGE, output, 4);
	}
	else{
//		printf("\r\nRot 0/180");
		/* Set Column Address */
		u2B.a	=  startX;
		output[0]	=  u2B.b[1];
		output[1]	=  u2B.b[0];
		u2B.a	=  endX;
		output[2]	=  u2B.b[1];
		output[3]	=  u2B.b[0];
		LCD_RegWrite(SSD1963_CMD_SET_COLUMN, output, 4);

		/* Set Page Address */
		u2B.a	=  startY;
		output[0]	=  u2B.b[1];
		output[1]	=  u2B.b[0];
		u2B.a	=  endY;
		output[2]	=  u2B.b[1];
		output[3]	=  u2B.b[0];
		LCD_RegWrite(SSD1963_CMD_SET_PAGE, output, 4);
	}
	return;
}

void LCD_GRAM_wr_cmd(void)
{
	LCD_RegWrite(SSD1963_CMD_WR_MEMSTART,0,0);		//cmd only
	return;
}

void LCD_GRAM_rd_cmd(void)
{
	LCD_RegWrite(SSD1963_CMD_RD_MEMSTART,0,0);		//cmd only
	return;
}

/*************************************************************************************************
* Function Name : 	LCD_brightness
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/09/17	W. Paul			Created
*************************************************************************************************/
void LCD_brightness(uint8_t level)
{
  	uint16_t	output[3];

	//	printf("\r\nWrite PWM %d",brightness);
	output[0]	=  0x10;		//frequency of PWM
	output[1]	=  level;		//mark to space ratio
	output[2]	=  0x01;		//enable
	LCD_RegWrite(SSD1963_CMD_SET_PWM_CONF, output, 3);

	if(level == 0){
		PinSet(BACKLIGHT_EN,0);		//backlight boost circuit off
		PinSet(RUN_LCD,0);			//1 = LCD 3v3 off	(power to LCD panel)
//		PinSet(LCD_MODE,0);
	}
	else{
		PinSet(BACKLIGHT_EN,1);		//backlight boost circuit on
		PinSet(RUN_LCD,1);			//0 = LCD 3v3 on	(power to LCD panel)
//		PinSet(LCD_MODE,1);			//lcd mode is normal
	}

	return;
}

/*************************************************************************************************
* Function Name : 	LCD_blank_screen
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/09/17	W. Paul			Created
*************************************************************************************************/
void LCD_blank_screen(uint16_t col)
{
	uint16_t	x;
	uint16_t	y;

	LCD_SetWindow(0, LCD_DISP_WIDTH-1,0, LCD_DISP_HEIGHT-1);
	LCD_GRAM_wr_cmd();

	if(col != COLOUR_GRID){
		for(y=0;y<LCD_DISP_HEIGHT;y++){
			for(x=0;x<LCD_DISP_WIDTH;x++){
				LCD_RAM_VALUE = col;
			}
		}
	}
	else{
		for(y=0;y<LCD_DISP_HEIGHT;y++){
			for(x=0;x<LCD_DISP_WIDTH;x++){
				if(		(y%100==0)||(x%100==0)){	col =  COLOUR_WHITE;	}
				else if((y% 50==0)||(x% 50==0)){	col =  COLOUR_RED;		}
				else if((y% 10==0)&&(x% 10==0)){	col =  COLOUR_GREEN;	}
				else if((y%  5==0)&&(x%  5==0)){	col =  COLOUR_BLUE;		}
				else{								col =  COLOUR_BLACK;	}

				if(	(y==0)					||
					(y==LCD_DISP_HEIGHT-1)	||
					(x==0)					||
					(x==LCD_DISP_WIDTH-1)		){	col	=  COLOUR_WHITE;	}

				LCD_RAM_VALUE	=  col;
			}
		}
	}

	return;
}

void LCD_WaitTear(void)
{
	uint8_t	sig;

	do{
		sig	=  PinRead(LCD_TEAR_INPUT);
	}while(sig==0);

	return;

}



 /**********************************************************************************************************
 **********************************************************************************************************/
//end of file
