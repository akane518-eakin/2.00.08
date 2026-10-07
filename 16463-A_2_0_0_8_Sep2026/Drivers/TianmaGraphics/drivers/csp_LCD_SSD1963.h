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
 * Filename    :  csp_LCD_SSD1963.h
 * Date Created:  Thu 14 Sep 2017 09:11:13 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _CSP_LCD_SSD1963_H
#define _CSP_LCD_SSD1963_H

#include 	"csp_STM32_FSMC.h"


#define LCD_BASE       		Bank1_SRAM1_ADDR			// start of bank 1
#define LCD_OFFSET			0x00030000					// RS is A16	set all bits to 1 means u can use any Address line

extern volatile uint16_t* 	LCD_REG_PTR;
extern volatile uint16_t* 	LCD_RAM_PTR;

#define LCD_REG_VALUE		*LCD_REG_PTR
#define LCD_RAM_VALUE		*LCD_RAM_PTR

/*********************************************************************************************************
 *		LCD specifics
 ********************************************************************************************************/
//MLT070W50
#define LCD_DISP_WIDTH				800		// pixels
#define LCD_DISP_HEIGHT				480		// pixels
#define REFRESH_RATE				60		// Hz

#define LCD_HORI_PULSE_WIDTH		30
#define LCD_HORI_BACK_PORCH			16
#define LCD_HORI_FRONT_PORCH		54		//100

#define LCD_VERT_PULSE_WIDTH		10
#define LCD_VERT_BACK_PORCH			13
#define LCD_VERT_FRONT_PORCH		7		//30

/*********************************************************************************************************
 *		Calculated Vars
 ********************************************************************************************************/
#define LCD_HORI_TOTAL				(LCD_HORI_PULSE_WIDTH + LCD_HORI_BACK_PORCH + LCD_DISP_WIDTH  + LCD_HORI_FRONT_PORCH)	//30+16+800+54 = 900
#define LCD_VERT_TOTAL				(LCD_VERT_PULSE_WIDTH + LCD_VERT_BACK_PORCH + LCD_DISP_HEIGHT + LCD_VERT_FRONT_PORCH)	//10+13+480+ 7 = 510
#define LCD_HORI_BACK_PORCH_TOTAL	(LCD_HORI_PULSE_WIDTH + LCD_HORI_BACK_PORCH)
#define LCD_VERT_BACK_PORCH_TOTAL	(LCD_VERT_PULSE_WIDTH + LCD_VERT_BACK_PORCH)

#define PIXEL_CLOCK					(uint32_t)(LCD_HORI_TOTAL * LCD_VERT_TOTAL * REFRESH_RATE)	//900 * 510 *60	=  27540000  0x1A43a20

#define OSC_FREQ					10000000	/* Hz */	//this is the crystal on the SSD1963
#define MULTIPLIER_N				60
#define DIVIDER_M					2

#define VCO_FREQ					(uint32_t)(OSC_FREQ * (MULTIPLIER_N + 1))	/* Hz, VCO Frequency > 250MHz and < 800Mhz */		//600M
#define PLL_FREQ					(uint32_t)(VCO_FREQ / (DIVIDER_M + 1))		/* Hz */											//200M
#define LCDC_FPR					(uint32_t)((((float)((float)PIXEL_CLOCK * 1048576) / (float)PLL_FREQ)) - 1)						//0x23403

#if (OSC_FREQ * MULTIPLIER_N) >= 800000000ul
	#error "VCO > 800MHz, check PLL config"
#endif

#if (OSC_FREQ * MULTIPLIER_N) <= 250000000ul
	#error "VCO < 250MHz, check PLL config"
#endif


/*********************************************************************************************************
 *		Driver Command Table
 ********************************************************************************************************/
#define SSD1963_CMD_NOP					0x00
#define SSD1963_CMD_SOFT_RESET			0x01
#define SSD1963_CMD_GET_PWR_MODE		0x0A
#define SSD1963_CMD_GET_ADDR_MODE		0x0B
#define SSD1963_CMD_GET_PIXEL_FORMAT	0x0C
#define SSD1963_CMD_GET_DISPLAY_MODE	0x0D
#define SSD1963_CMD_GET_SIGNAL_MODE		0x0E
#define SSD1963_CMD_GET_DIAGNOSTIC		0x0F
#define SSD1963_CMD_ENT_SLEEP			0x10
#define SSD1963_CMD_EXIT_SLEEP			0x11
#define SSD1963_CMD_ENT_PARTIAL_MODE	0x12
#define SSD1963_CMD_ENT_NORMAL_MODE		0x13
#define SSD1963_CMD_EXIT_INVERT_MODE	0x20
#define SSD1963_CMD_ENT_INVERT_MODE		0x21
#define SSD1963_CMD_SET_GAMMA			0x26
#define SSD1963_CMD_OFF_DISPLAY			0x28
#define SSD1963_CMD_ON_DISPLAY			0x29
#define SSD1963_CMD_SET_COLUMN			0x2A
#define SSD1963_CMD_SET_PAGE			0x2B
#define SSD1963_CMD_WR_MEMSTART			0x2C
#define SSD1963_CMD_RD_MEMSTART			0x2E
#define SSD1963_CMD_SET_PARTIAL_AREA	0x30
#define SSD1963_CMD_SET_SCROLL_AREA		0x33
#define SSD1963_CMD_SET_TEAR_OFF		0x34
#define SSD1963_CMD_SET_TEAR_ON			0x35
#define SSD1963_CMD_SET_ADDR_MODE		0x36
#define SSD1963_CMD_SET_SCROLL_START	0x37
#define SSD1963_CMD_EXIT_IDLE_MODE		0x38
#define SSD1963_CMD_ENT_IDLE_MODE		0x39
#define SSD1963_CMD_SET_PIXEL_FORMAT	0x3A
#define SSD1963_CMD_WR_MEM_AUTO			0x3C
#define SSD1963_CMD_RD_MEM_AUTO			0x3E
#define SSD1963_CMD_SET_TEAR_SCANLINE	0x44
#define SSD1963_CMD_GET_SCANLINE		0x45
#define SSD1963_CMD_RD_DDB_START		0xA1
#define SSD1963_CMD_RD_DDB_AUTO			0xA8
#define SSD1963_CMD_SET_PANEL_MODE		0xB0
#define SSD1963_CMD_GET_PANEL_MODE		0xB1
#define SSD1963_CMD_SET_HORZ_PERIOD		0xB4
#define SSD1963_CMD_GET_HORZ_PERIOD		0xB5
#define SSD1963_CMD_SET_VERT_PERIOD		0xB6
#define SSD1963_CMD_GET_VERT_PERIOD		0xB7
#define SSD1963_CMD_SET_GPIO_CONF		0xB8
#define SSD1963_CMD_GET_GPIO_CONF		0xB9
#define SSD1963_CMD_SET_GPIO_VAL		0xBA
#define SSD1963_CMD_GET_GPIO_STATUS		0xBB
#define SSD1963_CMD_SET_POST_PROC		0xBC
#define SSD1963_CMD_GET_POST_PROC		0xBD
#define SSD1963_CMD_SET_PWM_CONF		0xBE
#define SSD1963_CMD_GET_PWM_CONF		0xBF
#define SSD1963_CMD_SET_LCD_GEN0		0xC0
#define SSD1963_CMD_GET_LCD_GEN0		0xC1
#define SSD1963_CMD_SET_LCD_GEN1		0xC2
#define SSD1963_CMD_GET_LCD_GEN1		0xC3
#define SSD1963_CMD_SET_LCD_GEN2		0xC4
#define SSD1963_CMD_GET_LCD_GEN2		0xC5
#define SSD1963_CMD_SET_LCD_GEN3		0xC6
#define SSD1963_CMD_GET_LCD_GEN3		0xC7
#define SSD1963_CMD_SET_GPIO0_ROP		0xC8
#define SSD1963_CMD_GET_GPIO0_ROP		0xC9
#define SSD1963_CMD_SET_GPIO1_ROP		0xCA
#define SSD1963_CMD_GET_GPIO1_ROP		0xCB
#define SSD1963_CMD_SET_GPIO2_ROP		0xCC
#define SSD1963_CMD_GET_GPIO2_ROP		0xCD
#define SSD1963_CMD_SET_GPIO3_ROP		0xCE
#define SSD1963_CMD_GET_GPIO3_ROP		0xCF
#define SSD1963_CMD_SET_ABC_DBC_CONF	0xD0
#define SSD1963_CMD_GET_ABC_DBC_CONF	0xD1
#define SSD1963_CMD_SET_DBC_HISTO_PTR	0xD2
#define SSD1963_CMD_GET_DBC_HISTO_PTR	0xD3
#define SSD1963_CMD_SET_DBC_THRES		0xD4
#define SSD1963_CMD_GET_DBC_THRES		0xD5
#define SSD1963_CMD_SET_ABM_TMR			0xD6
#define SSD1963_CMD_GET_ABM_TMR			0xD7
#define SSD1963_CMD_SET_AMB_LVL0		0xD8
#define SSD1963_CMD_GET_AMB_LVL0		0xD9
#define SSD1963_CMD_SET_AMB_LVL1		0xDA
#define SSD1963_CMD_GET_AMB_LVL1		0xDB
#define SSD1963_CMD_SET_AMB_LVL2		0xDC
#define SSD1963_CMD_GET_AMB_LVL2		0xDD
#define SSD1963_CMD_SET_AMB_LVL3		0xDE
#define SSD1963_CMD_GET_AMB_LVL3		0xDF
#define SSD1963_CMD_PLL_START			0xE0
#define SSD1963_CMD_PLL_STOP			0xE1
#define SSD1963_CMD_SET_PLL_MN			0xE2
#define SSD1963_CMD_GET_PLL_MN			0xE3
#define SSD1963_CMD_GET_PLL_STATUS		0xE4
#define SSD1963_CMD_ENT_DEEP_SLEEP		0xE5
#define SSD1963_CMD_SET_PCLK			0xE6
#define SSD1963_CMD_GET_PCLK			0xE7
#define SSD1963_CMD_SET_DATA_INTERFACE	0xF0
#define SSD1963_CMD_GET_DATA_INTERFACE	0xF1
#define SSD1963_CMD_GET_BUS_READY		0xFF


/*********************************************************************************************************
 *		Structures & enums
 ********************************************************************************************************/
typedef enum{
	LCD_STATUS_OK,
	LCD_STATUS_FAIL,
}LCD_STATUS_enum;

typedef enum{
	LCD_ROTATE_0	=  0x01,
	LCD_ROTATE_90	=  0x20,
	LCD_ROTATE_180	=  0x02,
	LCD_ROTATE_270	=  0x23,
}LCD_ROTATION_enum;

/*********************************************************************************************************
 *		Global Functions
 ********************************************************************************************************/
void		LCD_Init_SSD1963(void);
uint8_t		LCD_Check_Reset(void);

void 		LCD_RegWrite(uint8_t RegAddress, uint16_t *RegData, uint16_t n);
void 		LCD_RegWrite1(uint8_t RegAddress, uint16_t RegData);
void 		LCD_RegRead(uint8_t RegAddress, uint16_t *RegData, uint16_t n);
uint16_t	LCD_RegRead1(uint8_t RegAddress);

void 		LCD_RegRead_print(uint8_t RegAddress, uint16_t n);

void		LCD_DisplayOn(void);
void		LCD_DisplayOff(void);

void		LCD_SetCursor(uint16_t startX, uint16_t startY);
void 		LCD_SetWindow(uint16_t startX, uint16_t endX,uint16_t startY, uint16_t endY);

void		LCD_GRAM_wr_cmd(void);
void		LCD_GRAM_rd_cmd(void);

void 		LCD_brightness(uint8_t level);
void 		LCD_WaitTear(void);
void 		LCD_blank_screen(uint16_t col);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
