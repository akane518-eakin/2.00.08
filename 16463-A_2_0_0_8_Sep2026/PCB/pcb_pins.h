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
 * Filename    :  pcb_pins.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/

#ifndef _PCB_PIN_H
#define _PCB_PIN_H


/**********************************************************************************************************
 *	INCLUDE FILES
 **********************************************************************************************************/
#include "stm32f10x_gpio.h"
#include "csp_STM32_delay.h"


#define	SPK_MUTE				GPIOB, GPIO_Pin_1
#define	SPK_SHDN				GPIOB, GPIO_Pin_2

#define	SOL_VALVE_NEBULIZER		GPIOB, GPIO_Pin_8	//SV1
#define	SOL_VALVE_O2_CAL		GPIOB, GPIO_Pin_9	//SV2
#define	SOL_VALVE_VENTURI_EN	GPIOC, GPIO_Pin_9	//SV3

#define	IO_PRESSURE_AIR			GPIOC, GPIO_Pin_12	//PS1
#define	IO_PRESSURE_O2			GPIOG, GPIO_Pin_15	//PS2

#define	IO_FAN					GPIOB, GPIO_Pin_0

#define	SPI2_CS_ADC				GPIOB, GPIO_Pin_5
#define	SPI2_CS_PRESURE_ADC		GPIOB, GPIO_Pin_12
#define	SPI2_CS_PRESURE_EE		GPIOB, GPIO_Pin_6

#define	LCD_RESET				GPIOD, GPIO_Pin_2
#define	LCD_TEAR_INPUT			GPIOD, GPIO_Pin_6

#define	PWR_5VEN				GPIOE, GPIO_Pin_0
#define	PWR_LATCH				GPIOE, GPIO_Pin_1
#define	WATCHDOG_EN				GPIOE, GPIO_Pin_3
#define	WATCHDOG_INPUT			GPIOE, GPIO_Pin_4
#define	WATCHDOG_BUZ_CTRL		GPIOE, GPIO_Pin_5
#define	WATCHDOG_WD_BUZ			GPIOE, GPIO_Pin_6


#define FAN_FB					GPIOC, GPIO_Pin_5


#define EN_30V                  GPIOF, GPIO_Pin_1           // 30 EN to be on in therapy mode and calibration mode                             
#define	BUT_1_ALARM				GPIOF, GPIO_Pin_2
#define	BUT_2_ON_OFF			GPIOF, GPIO_Pin_4

#define	TP6						GPIOF, GPIO_Pin_5			//pneumatics
#define	TP5						GPIOF, GPIO_Pin_6			//UI
#define	TP4						GPIOF, GPIO_Pin_7			//1ms tic
#define	TP3						GPIOF, GPIO_Pin_8			//Audio 8Khz
#define	TP2						GPIOF, GPIO_Pin_9			//tog on main loop
#define	TP1						GPIOF, GPIO_Pin_10

#define	SPI1_CS_MEM0			GPIOF, GPIO_Pin_11
#define	SPI_MEM_PWR				GPIOF, GPIO_Pin_12
#define	SPI1_CS_MEM1			GPIOF, GPIO_Pin_13

#define	PRES_DRDY				GPIOG, GPIO_Pin_0
#define	BACKLIGHT_EN			GPIOG, GPIO_Pin_1

#define	SAFTY_CUTOUT			GPIOG, GPIO_Pin_4
#define	RUN_LCD					GPIOG, GPIO_Pin_7
#define	EN_LCD					GPIOG, GPIO_Pin_8
#define	ALCC					GPIOG, GPIO_Pin_10
#define	LTC4009_ICL				GPIOG, GPIO_Pin_11
#define	LTC4009_ACP				GPIOG, GPIO_Pin_12
#define	LTC4009_CHRG			GPIOG, GPIO_Pin_13
#define	LTC4009_SHDN			GPIOG, GPIO_Pin_14

#define	SPI2_MISO				GPIOB, GPIO_Pin_14
#define	SPI1_MISO				GPIOA, GPIO_Pin_6


#define PinSet(pin,state)		((state) ? GPIO_SetBits(pin) : GPIO_ResetBits(pin) )
#define PinRead(pin)			(GPIO_ReadInputDataBit(pin) )


/*******************************************************************************************************************************************/
/* USART 1 flow-control lines */
#define UART1_RTS_PORT					GPIOA
#define UART1_RTS_PIN					GPIO_Pin_12
#define UART1_CTS_PORT					GPIOA
#define UART1_CTS_PIN					GPIO_Pin_11

#define mCSP_USART1_RTS_PIN_HI			GPIO_SetBits(			UART1_RTS_PORT, UART1_RTS_PIN)
#define mCSP_USART1_RTS_PIN_LOW			GPIO_ResetBits(		UART1_RTS_PORT, UART1_RTS_PIN)
#define mCSP_USART1_CTS_READ			GPIO_ReadInputDataBit(	UART1_CTS_PORT, UART1_CTS_PIN)
#define mCSP_USART1_RX_READ				GPIO_ReadInputDataBit(	GPIOA, GPIO_Pin_10)

/* USART 2 flow-control lines */
#define UART2_RTS_PORT					GPIOA
#define UART2_RTS_PIN					GPIO_Pin_1
#define UART2_CTS_PORT					GPIOA
#define UART2_CTS_PIN					GPIO_Pin_0

#define mCSP_USART2_RTS_PIN_HI			GPIO_SetBits(		UART2_RTS_PORT, UART2_RTS_PIN)
#define mCSP_USART2_RTS_PIN_LOW			GPIO_ResetBits(		UART2_RTS_PORT, UART2_RTS_PIN)
#define mCSP_USART2_CTS_READ				GPIO_ReadInputDataBit(	UART2_CTS_PORT, UART2_CTS_PIN)

/* USART 3 flow-control lines */
#define UART3_RTS_PORT					GPIOA
#define UART3_RTS_PIN					GPIO_Pin_12
#define UART3_CTS_PORT					GPIOA
#define UART3_CTS_PIN					GPIO_Pin_11

#define mCSP_USART3_RTS_PIN_HI			GPIO_SetBits(			UART3_RTS_PORT, UART3_RTS_PIN)
#define mCSP_USART3_RTS_PIN_LOW			GPIO_ResetBits(		UART3_RTS_PORT, UART3_RTS_PIN)
#define mCSP_USART3_CTS_READ				GPIO_ReadInputDataBit(	UART3_CTS_PORT, UART3_CTS_PIN)

/* UART 4 flow-control lines */
#define UART4_RTS_PORT					GPIOA
#define UART4_RTS_PIN					GPIO_Pin_12
#define UART4_CTS_PORT					GPIOA
#define UART4_CTS_PIN					GPIO_Pin_11

#define mCSP_UART4_RTS_PIN_HI				GPIO_SetBits(			UART4_RTS_PORT, UART4_RTS_PIN)
#define mCSP_UART4_RTS_PIN_LOW			GPIO_ResetBits(		UART4_RTS_PORT, UART4_RTS_PIN)
#define mCSP_UART4_CTS_READ				GPIO_ReadInputDataBit(	UART4_CTS_PORT, UART4_CTS_PIN)

/* UART 5 flow-control lines */
#define UART5_RTS_PORT					GPIOE
#define UART5_RTS_PIN					GPIO_Pin_0
#define UART5_CTS_PORT					GPIOE
#define UART5_CTS_PIN					GPIO_Pin_1

#define mCSP_UART5_RTS_PIN_HI				GPIO_SetBits(			UART5_RTS_PORT, UART5_RTS_PIN)
#define mCSP_UART5_RTS_PIN_LOW			GPIO_ResetBits(		UART5_RTS_PORT, UART5_RTS_PIN)
#define mCSP_UART5_CTS_READ				GPIO_ReadInputDataBit(	UART5_CTS_PORT, UART5_CTS_PIN)

/*********************************************************************************************************
 *	FUNCTION PROTOTYPES
 ********************************************************************************************************/
void io_pins_config(void);
void io_Pins_FSMC_Config(void);
void io_pins_config1(GPIO_TypeDef* port,uint16_t pin,GPIOMode_TypeDef mode);

#endif
//end of file
