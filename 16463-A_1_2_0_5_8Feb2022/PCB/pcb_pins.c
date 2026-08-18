 /**********************************************************************************************************
 *  Marturion Ltd
 *
 *	Knockmore Hill Business Park
 *	9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2010, Marturion Ltd
 *  All Rights Reserved
 *
 *
 * Filename    :  pcb_pins.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 *********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "pcb_pins.h"


/**********************************************************************************************************
 * Function Name : init_GPIO
 * Description   : This function is used to Configure the different GPIO ports pins.
 *				  This function:
 *					> Sets up pin direction (output OR input)
 *					> Sets output pin at defined state (high OR low)
 * Arguments     : None
 * Returns       : None
 * Notes         : RM0008.pdf page 139
 *
 * Version	Date d/m/y	Programmer          Reason for Change
 * 1.0.0	11/05/2010      Michael Kelly       Original Created
 ********************************************************************************************************* */
void io_pins_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIO Port clock enables*/
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA |
							RCC_APB2Periph_GPIOB |
							RCC_APB2Periph_GPIOC |
							RCC_APB2Periph_GPIOD |
							RCC_APB2Periph_GPIOE |
							RCC_APB2Periph_GPIOF |
							RCC_APB2Periph_GPIOG |
							RCC_APB2Periph_AFIO,	ENABLE);

/**********************************************************************************************************
 *	GPIO port A
 *********************************************************************************************************/
	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_0	|			// UART2 CTS
		GPIO_Pin_1	|			// UART2 RTS
		GPIO_Pin_2	|			// UART2 TX
		GPIO_Pin_3	|			// UART2 RX
		GPIO_Pin_4	|			// AUDIO DAC
		GPIO_Pin_5	|			// SPI_CLK
		GPIO_Pin_6	|			// SPI_MISO
		GPIO_Pin_7	|			// SPI_MOSI
		GPIO_Pin_8	|			// 					LCD_PWM_ST
		GPIO_Pin_9	|			// UART1 TX			RED_LED_PWM
		GPIO_Pin_10	|			// UART1 RX			GREEN_LED_PWM
		GPIO_Pin_11	|			// UART1 CTS		BLUE_LED_PWM
		GPIO_Pin_12	|			// UART1 RTS		NC
		GPIO_Pin_13	|			// PROG-TMS
		GPIO_Pin_14	|			// PROG-TCK
		GPIO_Pin_15	;			// PROG-TDI
	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin =
//		GPIO_Pin_4;				// 					Spirit_SPI_CS
//	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IPD;		//GPIO_Mode_Out_PP
//	GPIO_Init(GPIOA, &GPIO_InitStructure);


/**********************************************************************************************************
 *	GPIO port B
 *********************************************************************************************************/
	GPIO_InitStructure.GPIO_Pin =
	//	GPIO_Pin_0	|			// 					Fanon
	//	GPIO_Pin_1	|			// 					SPK_Mute
	//	GPIO_Pin_2	|			// 					SPK_SHDN
		GPIO_Pin_3	|			// PROG-TDO/SWO
		GPIO_Pin_4	|			// PROG-TRST
	//	GPIO_Pin_5	|			// 					ADC_CS
		GPIO_Pin_6	|			// 	SCL				NC
		GPIO_Pin_7	|			// 	SDA				NC
	//	GPIO_Pin_8	|			// 					SOL_Valve_Nebulizer
	//	GPIO_Pin_9	|			// 					SOL_Valve_O2Cal
		GPIO_Pin_10	|			// SCL/UART3TX
		GPIO_Pin_11	|			// SDA/UART3RX
		GPIO_Pin_12	|			//					NC
		GPIO_Pin_13	|			// SPI2_CLK
		GPIO_Pin_14	|			// SPI2_MISO
		GPIO_Pin_15	;			// SPI2_MOSI
	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_0	|			// 					Fanon
		GPIO_Pin_1	|			// 					SPK_Mute
		GPIO_Pin_2	|			// 					SPK_SHDN
		GPIO_Pin_5	|			// 					PRES_CS_EE
		GPIO_Pin_8	|			// 					SOL_Valve_Nebulizer
		GPIO_Pin_9;				// 					SOL_Valve_O2Cal
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin =
//		GPIO_Pin_9	;			//
//	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_OD;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

/**********************************************************************************************************
 *	GPIO port C
 *********************************************************************************************************/
	GPIO_InitStructure.GPIO_Pin =
	//	GPIO_Pin_0	|			// 				XL		(ADC)
	//	GPIO_Pin_1	|			// 				YD		(ADC)
	//	GPIO_Pin_2	|			// 				XR		(ADC)
	//	GPIO_Pin_3	|			// 				YU		(ADC)
	//	GPIO_Pin_4	|			// 				VBAT	(ADC)
	//	GPIO_Pin_5	|			// 				Fan FB
		GPIO_Pin_6	|			// 				PROP_VALVE_AIR
		GPIO_Pin_7	|			// 				PROP_VALVE_O2
		GPIO_Pin_8	|			// 				PROP_VALVE_LocalAir
	//	GPIO_Pin_9	|			// 				IO_VALVE_VENTURI
		GPIO_Pin_10	|			// UART4TX
		GPIO_Pin_11	|			// UART4RX
		GPIO_Pin_12	|			// UART5TX		PS1 Detect (AIR)
		GPIO_Pin_13	|			// 				NC
		GPIO_Pin_14	|			// OSC32_IN
		GPIO_Pin_15	;			// OSC32_OUT
	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_0	|			// 				XL		(ADC)
		GPIO_Pin_1	|			// 				YD		(ADC)
		GPIO_Pin_2	|			// 				XR		(ADC)
		GPIO_Pin_3	|			// 				YU		(ADC)
		GPIO_Pin_4; 			// 				VBAT	(ADC)
	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_9;				// 				PROP_VALVE_VENTURI
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_5,			// 				Fan FB
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

/**********************************************************************************************************
 *	GPIO port D
 *********************************************************************************************************/
	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_0	|			// D2
		GPIO_Pin_1	|			// D3
	//	GPIO_Pin_2	|			// 					LCD_RESET
		GPIO_Pin_3	|			// 					LCD_CLK
		GPIO_Pin_4	|			// LCD_OE
		GPIO_Pin_5	|			// LCD_WE
		GPIO_Pin_6	|			// 					LCD_TE
		GPIO_Pin_7	|			// LCD_CS
		GPIO_Pin_8	|			// D13
		GPIO_Pin_9	|			// D14
		GPIO_Pin_10	|			// D15
		GPIO_Pin_11	|			// A16
		GPIO_Pin_12	|			// 					NC
		GPIO_Pin_13	|			// 					NC
		GPIO_Pin_14	|			// D0
		GPIO_Pin_15	;			// D1
	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_2;				// 					LCD_RESET
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

/**********************************************************************************************************
 *	GPIO port E
 *********************************************************************************************************/
	GPIO_InitStructure.GPIO_Pin =
	//	GPIO_Pin_0	|			// NBL0				5V_EN
	//	GPIO_Pin_1	|			// NBL1				MCU_PWR_LATCH
		GPIO_Pin_2	|			// 					NC
	//	GPIO_Pin_3	|			// 					WATCHDOG_EN
	//	GPIO_Pin_4	|			// 					WATCHDOG_INPUT
	//	GPIO_Pin_5	|			// 					BUZZ_CTRL
    	GPIO_Pin_6	|			// 					WD_BUZ 
		GPIO_Pin_7	|			// D4
		GPIO_Pin_8	|			// D5
		GPIO_Pin_9	|			// D6
		GPIO_Pin_10	|			// D7
		GPIO_Pin_11	|			// D8
		GPIO_Pin_12	|			// D9
		GPIO_Pin_13	|			// D10
		GPIO_Pin_14	|			// D11
		GPIO_Pin_15	;			// D12
	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_0	|			// NBL0				5V_EN
		GPIO_Pin_1	|			// NBL1				MCU_PWR_LATCH
		GPIO_Pin_3	|			// 					WATCHDOG_EN
		GPIO_Pin_4	|			// 					WATCHDOG_INPUT
		GPIO_Pin_5;				// 					BUZZ_CTRL
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin =
//		GPIO_Pin_3	;
//	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_OD;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);

/**********************************************************************************************************
 *	GPIO port F
 *********************************************************************************************************/
    GPIO_InitStructure.GPIO_Pin =
    	GPIO_Pin_0  	|			//A0		NC
		GPIO_Pin_1  	|			//A1		BUT		(CALIBRATE)
		GPIO_Pin_2  	|			//A2		BUT		(ALARM_CANCEL)
		GPIO_Pin_3  	|			//A3		NC
		GPIO_Pin_4  	|			//A4		BUT		(PWR_ON)
	//	GPIO_Pin_5  	|			//A5		TP6
	//	GPIO_Pin_6  	|			//			TP5
	//	GPIO_Pin_7  	|			//			TP4
	//	GPIO_Pin_8  	|			//			TP3
	//	GPIO_Pin_9  	|			//			TP2
		GPIO_Pin_10 	|			//			TP1
	//	GPIO_Pin_11 	|			//			SPI_CS_MEM0
	//	GPIO_Pin_12 	|			//A6		MEM_ON
	//	GPIO_Pin_13 	|			//A7		SPI_CS_MEM1
		GPIO_Pin_14 	|			//A8		NC
		GPIO_Pin_15;				//A9		NC
    GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_1  	|			//A1		BOOST_EN (30V)
    	GPIO_Pin_5  	|			//A5		TP6
    	GPIO_Pin_6  	|			//			TP5
    	GPIO_Pin_7  	|			//			TP4
    	GPIO_Pin_8  	|			//			TP3
    	GPIO_Pin_9  	|			//			TP2
		GPIO_Pin_11 	|			//			SPI_CS_MEM0
		GPIO_Pin_12 	|			//A6		MEM_ON
		GPIO_Pin_13;				//A7		SPI_CS_MEM1
    GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);


/**********************************************************************************************************
 *	GPIO port G
 *********************************************************************************************************/
    GPIO_InitStructure.GPIO_Pin =
    	GPIO_Pin_0  |				// A10
	//	GPIO_Pin_1  |				// A11		EN_BACKLIGHT
	    GPIO_Pin_2  |				// A12		NC
	    GPIO_Pin_3  |				// A13		NC
	//	GPIO_Pin_4  |				// A14		CUTOUT_MCU
	    GPIO_Pin_5  |				// A15		NC
	    GPIO_Pin_6  |				// 			NC
	//	GPIO_Pin_7  |				// 			RUN_LCD
	//	GPIO_Pin_8  |				// 			EN_LCD
	    GPIO_Pin_9  |				// 			NC
	    GPIO_Pin_10 |				// 			ALCC
	//  GPIO_Pin_11 |				// 			LTC4009_ICL
	//  GPIO_Pin_12 |				// 			LTC4009_ACP
	//  GPIO_Pin_13 |				// 			LTC4009_CHRG
	//	GPIO_Pin_14 |				// 			LTC4009_SHDN
	    GPIO_Pin_15;				//			PS2 Detect (O2)
    GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_11 |				// 			LTC4009_ICL
	    GPIO_Pin_12 |				// 			LTC4009_ACP
	    GPIO_Pin_13;				// 			LTC4009_CHRG
    GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_IPU;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_1  |				// A11		EN_BACKLIGHT
		GPIO_Pin_4  |				// A14		CUTOUT_MCU
		GPIO_Pin_7  |				// 			RUN_LCD
		GPIO_Pin_8  |				// 			EN_LCD
		GPIO_Pin_14;				// 			LTC4009_SHDN
    GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	=  GPIO_Mode_Out_PP;
    GPIO_Init(GPIOG, &GPIO_InitStructure);



	return;
}



/*******************************************************************************
* Function Name  : io_Pins_FSMC_Config
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void io_Pins_FSMC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOD |
  							RCC_APB2Periph_GPIOE |
  							RCC_APB2Periph_GPIOF |
  							RCC_APB2Periph_GPIOG |
                         	RCC_APB2Periph_AFIO, 	ENABLE);

//************************************************
//	Data bus
//************************************************
	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_0 | 		//D2
		GPIO_Pin_1 | 		//D3
		GPIO_Pin_8 |  		//D13
		GPIO_Pin_9 |  		//D14
		GPIO_Pin_10 |  		//D15
		GPIO_Pin_14 | 		//D0
		GPIO_Pin_15; 		//D1
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_7 	| 		//D4
		GPIO_Pin_8 	| 		//D5
		GPIO_Pin_9 	|  		//D6
		GPIO_Pin_10 | 		//D7
		GPIO_Pin_11 |  		//D8
		GPIO_Pin_12 |  		//D9
		GPIO_Pin_13 |  		//D10
		GPIO_Pin_14 | 		//D11
		GPIO_Pin_15; 		//D12
	GPIO_Init(GPIOE, &GPIO_InitStructure);
//************************************************


//************************************************
//	Address bus
//************************************************
//	GPIO_InitStructure.GPIO_Pin =
//    	GPIO_Pin_0  |		//A0
//		GPIO_Pin_1  |		//A1
//		GPIO_Pin_2  |		//A2
//		GPIO_Pin_3  |		//A3
//		GPIO_Pin_4  |		//A4
//		GPIO_Pin_5  |		//A5
//		GPIO_Pin_12 |		//A6
//		GPIO_Pin_13 |		//A7
//		GPIO_Pin_14 |		//A8
//		GPIO_Pin_15;		//A9
//	GPIO_Init(GPIOF, &GPIO_InitStructure);
//
// 	GPIO_InitStructure.GPIO_Pin =
//    	GPIO_Pin_0  |		// A10
//	    GPIO_Pin_1  |		// A11
//	    GPIO_Pin_2  |		// A12
//	    GPIO_Pin_3  |		// A13
//	    GPIO_Pin_4  |		// A14
//	    GPIO_Pin_5;			// A15
//	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_11;		// A16
	//	GPIO_Pin_12;		// A17
	//	GPIO_Pin_13;		// A18
	GPIO_Init(GPIOD, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin =
//		GPIO_Pin_3 | 		//A19
//		GPIO_Pin_4;			//A20
// 	GPIO_Init(GPIOE, &GPIO_InitStructure);

//************************************************
//	Control pins
//************************************************
//	GPIO_InitStructure.GPIO_Pin =
//		GPIO_Pin_0	|		// NBL0
//		GPIO_Pin_1;			// NBL1
//	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =
		GPIO_Pin_4 | 		//NOE
		GPIO_Pin_5 | 		//NWE
		GPIO_Pin_7; 		//		NE1		CS	LCD
	GPIO_Init(GPIOD, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin =
//		GPIO_Pin_9; 		//		NE2		CS	RAM
//	GPIO_Init(GPIOG, &GPIO_InitStructure);


	GPIO_PinRemapConfig(GPIO_Remap_FSMC_NADV,ENABLE);		//enable = No NADV, i2c1 bus needs this line
	return;
}


/*************************************************************************************************
* Function Name : 	io_pins_config1
* Description   : 	This Function sets a pin to a specific mode
* Arguments     : 	GPIO_TypeDef* 		port	GPIOF
*					uint16_t 			pin		GPIO_Pin_13
*					GPIOMode_TypeDef 	mode	GPIO_Mode_IN_FLOATING
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/09/17	W. Paul			Created
*************************************************************************************************/
void io_pins_config1(GPIO_TypeDef* port,uint16_t pin,GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin 	=  pin;
	GPIO_InitStructure.GPIO_Mode	=  mode;
	GPIO_InitStructure.GPIO_Speed	=  GPIO_Speed_50MHz;
	GPIO_Init(port, &GPIO_InitStructure);
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
