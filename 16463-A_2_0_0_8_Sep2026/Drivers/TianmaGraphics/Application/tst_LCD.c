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
 * Filename    :  tst_LCD.c
 * Date Created:  Mon 25 Sep 2017 10:08:56 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "api_watchdog.h"
#include "api_STM32_touchscreen.h"

#include "csp_LCD_SSD1963.h"
#include "hal_lcd.h"
#include "hal_lcd_text.h"
#include "fonts.h"
#include "Colours.h"

#include "pcb_spi1.h"
#include "pcb_spi2.h"

#include "csp_STM32_uart.h"
#include "hal_STM32_uart.h"

/**********************************************************************************************************
**********************************************************************************************************/



/*************************************************************************************************
* Function Name : 	tst_LCD_menu
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		25/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t tst_LCD_menu(void)
{
/* Local Variables */
	uint8_t				rec_status_u8;
	uint8_t				rec_char_u8;

	uint16_t			x,y,w,h,col;
	uint8_t				cont;
	uint16_t			i,j;
	static uint8_t		test_print_cords	=  0;
	int16_t				X,Y;
	uint16_t			TouchPressure;
	uint8_t				valid;


/* Code */
	rec_char_u8 = debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){

		switch(rec_char_u8){
			case ' ':
				printf("\r\n");
				printf("\r\n****TST LCD menu****");
				printf("\r\nEsc  Exit Menu");
				printf("\r\ni    init driver");
				printf("\r\np    print driver regs");

				printf("\r\nd    draw box");
				printf("\r\nt    test text");
				printf("\r\nb    all screen COLOUR_BLACK");
				printf("\r\nB    all screen COLOUR ??");

				printf("\r\nq    en/dis text active area");
				printf("\r\nO    Screen grab to bmp(via UART)");
				printf("\r\ng    grid");
				printf("\r\n!    test coords");
				break;

			case 0x1b:
				test_print_cords	=  0;
				return(0);
				//break;
			case 'i':
				LCD_Init_SSD1963();
				break;
			case 'p':
				printf("\r\nAll readable registers on SSD1963 Graphics chip");


				LCD_RegRead_print(SSD1963_CMD_GET_PWR_MODE			,1);	//0x0A
				LCD_RegRead_print(SSD1963_CMD_GET_ADDR_MODE			,1);	//0x0B
				LCD_RegRead_print(SSD1963_CMD_GET_PIXEL_FORMAT		,1);	//0x0C
				LCD_RegRead_print(SSD1963_CMD_GET_DISPLAY_MODE		,1);	//0x0D
				LCD_RegRead_print(SSD1963_CMD_GET_SIGNAL_MODE		,1);	//0x0E
				LCD_RegRead_print(SSD1963_CMD_RD_DDB_START			,5);	//0xA1
				LCD_RegRead_print(SSD1963_CMD_GET_PANEL_MODE		,7);	//0xB1
				LCD_RegRead_print(SSD1963_CMD_GET_HORZ_PERIOD		,8);	//0xB5
				LCD_RegRead_print(SSD1963_CMD_GET_VERT_PERIOD		,7);	//0xB7
				LCD_RegRead_print(SSD1963_CMD_GET_GPIO_CONF			,2);	//0xB9
				LCD_RegRead_print(SSD1963_CMD_GET_GPIO_STATUS		,1);	//0xBB
				LCD_RegRead_print(SSD1963_CMD_GET_POST_PROC			,4);	//0xBD
				LCD_RegRead_print(SSD1963_CMD_GET_PWM_CONF			,7);	//0xBF
				LCD_RegRead_print(SSD1963_CMD_GET_LCD_GEN0			,7);	//0xC1
				LCD_RegRead_print(SSD1963_CMD_GET_LCD_GEN1			,7);	//0xC3
				LCD_RegRead_print(SSD1963_CMD_GET_LCD_GEN2			,7);	//0xC5
				LCD_RegRead_print(SSD1963_CMD_GET_LCD_GEN3			,7);	//0xC7
				LCD_RegRead_print(SSD1963_CMD_GET_GPIO0_ROP			,2);	//0xC9
				LCD_RegRead_print(SSD1963_CMD_GET_GPIO1_ROP			,2);	//0xCB
				LCD_RegRead_print(SSD1963_CMD_GET_GPIO2_ROP			,2);	//0xCD
				LCD_RegRead_print(SSD1963_CMD_GET_GPIO3_ROP			,2);	//0xCF
				LCD_RegRead_print(SSD1963_CMD_GET_ABC_DBC_CONF		,1);	//0xD1
				LCD_RegRead_print(SSD1963_CMD_GET_DBC_THRES			,9);	//0xD5
				LCD_RegRead_print(SSD1963_CMD_GET_PLL_MN			,3);	//0xE3
				LCD_RegRead_print(SSD1963_CMD_GET_PLL_STATUS		,1);	//0xE4
				LCD_RegRead_print(SSD1963_CMD_GET_PCLK				,3);	//0xE7
				LCD_RegRead_print(SSD1963_CMD_GET_DATA_INTERFACE	,1);	//0xF1

				break;
			case 'd':
				printf("\r\nx ---\b\b\b");				x		=  BSP_get_num(10,3);
				printf("\r\ny ---\b\b\b");				y		=  BSP_get_num(10,3);
				printf("\r\nw ---\b\b\b");				w		=  BSP_get_num(10,3);
				printf("\r\nh ---\b\b\b");				h		=  BSP_get_num(10,3);
				printf("\r\ncol 0x----\b\b\b\b");		col		=  BSP_get_num(16,4);
				printf("\r\ncontinue -\b");				cont	=  BSP_get_num(10,1);
				if(cont){
					LCD_FillRect(x,y,w,h,col);
					printf("\r\nDone");
				}
				else{
					printf("\r\nAborted");
				}
				break;
			case 't':
           		LCD_DispText_option(100,100,150,20,CENTER_LEFT,COLOUR_WHITE,TRANSPARENT, &fontArial16h, "(100,100)Sz 10");	//x,y,w,h,justification,col,font,txt
				break;
			case 'b':
				LCD_blank_screen(0x0000);
				break;
			case 'B':
				printf("\r\ncol 0x----\b\b\b\b");
				col		=  BSP_get_num(16,4);
				LCD_blank_screen(col);
				break;
			case 'g':
				LCD_blank_screen(COLOUR_GRID);
				break;
			case '1':
				LCD_SetWindow(10,12,10,11);
				LCD_GRAM_wr_cmd();
				for(i=0;i<8;i++){
					LCD_RAM_VALUE = 0xf800+i;
				}
				LCD_RegWrite(SSD1963_CMD_NOP, 0, 0);

				LCD_SetWindow(8,22,8,22);
				LCD_GRAM_rd_cmd();
				for(i=8;i<=22;i++){
					for(j=8;j<=22;j++){
						printf("\r\n%d %d %4x"	,i
												,j
												,LCD_RAM_VALUE);
					}
				}
				LCD_RegWrite(SSD1963_CMD_NOP, 0, 0);
				break;
			case 'q':
				if(debug_textActiveArea){	debug_textActiveArea	=  0;	}
				else{						debug_textActiveArea	=  1;	}

				printf("\r\ndebug_textActiveArea = %d",debug_textActiveArea);
				break;

			case 'O':
				LCD_Output_24BMP_FromScreen(0, 0, LCD_DISP_WIDTH, LCD_DISP_HEIGHT );
				break;
			case '!':
				test_print_cords	= !test_print_cords;
				printf("\r\ntest_print_cords = %d", test_print_cords);
				break;
			default:
					printf("\r\n Invalid Command");
					break;
		}


	}

	if(test_print_cords){
		valid = api_touch_CoOrds_rd(&X,&Y,&TouchPressure);
		if(valid){
			LCD_DrawDot(X,Y,COLOUR_WHITE);
			printf("\r\n%d,%d,%d",X,Y,TouchPressure);
		}
	}
	return(1);
}



/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
