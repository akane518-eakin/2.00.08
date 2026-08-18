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
 * Filename    :  pcb_startup.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "pcb_startup.h"


#include "pcb_pins.h"

#include "pcb_spi1.h"
#include "pcb_spi2.h"
#include "pcb_timer.h"


#include "csp_STM32_clks.h"
#include "csp_STM32_interrupts.h"
#include "csp_STM32_rtc.h"

#include "csp_STM32_fsmc.h"
#include "csp_STM32_dac.h"
#include "csp_STM32_adc1.h"
#include "csp_STM32_adc2.h"

#include "csp_STM32_spi.h"
#include "csp_STM32_spi1.h"
#include "csp_STM32_spi2.h"

#include "csp_STM32_timer1.h"
#include "csp_STM32_timer2.h"
#include "csp_STM32_timer3.h"
#include "csp_STM32_timer4.h"
#include "csp_STM32_timer8.h"

#include "csp_STM32_uart.h"
#include "csp_STM32_uart1.h"
#include "csp_STM32_uart2.h"
#include "csp_STM32_uart4.h"
#include "csp_STM32_uart5.h"

#include "csp_STM32_iwdg.h"

#include "csp_STM32_I2Cx.h"

#include "S25FL0xx.h"
#include "csp_S25FL0xx.h"
#include "hal_LCD.h"
#include "csp_paracube_O2.h"
#include "csp_LCD_SSD1963.h"

#include "csp_AD7794.h"
#include "api_watchdog.h"
#include "api_reset.h"
#include "api_IO.h"
#include "api_RTC.h"
#include "api_leds.h"
#include "api_bootloader_comms.h"

/**********************************************************************************************************
 *	Local Functions
 *********************************************************************************************************/
void pcb_map_functions(void);


/**********************************************************************************************************
*********************************************************************************************************/


/**********************************************************************************************************
 * Function Name : pcb_startup
 * Description   :
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0
 *********************************************************************************************************/
void pcb_startup(uint8_t mode)
{
/* processor init */
	if(mode == STARTUP_BASIC){
		STM32_startup(INT_VECT_OFFSET);					// Set up Clock

		io_pins_config();								// Set up input and output pins

		api_io_power_sys(0);				//keep alive not needed in this mode
		api_io_power_5V_sensor(0);	//1
		PinSet(EN_LCD,0);
		PinSet(RUN_LCD,0);

		pcb_map_functions();							// Map pcb functions
		STM32_uart_config();							// Set up Uarts

		api_reset_source(0);
		STM32_internal_RTC_config(RTC_SRC_EXT32768);	// Set up STM32 Internal RTC

		I2C2_init();

		csp_ADC1_config();

		timer1_config();			//75Hz		LEDs & touchscreen
		api_LED(LED_PWR		,LED_ON		,0x80);
		api_LED(LED_ALM		,LED_OFF	,0x00);
		api_LED(LED_BATG	,LED_OFF	,0x00);
		api_LED(LED_BATR	,LED_OFF	,0x00);

		spi1_cs_manager(0xff);							//disbale all SPI cs
		spi1_config(SPI_MODE_0,10000000);

		csp_mem_config();								// Configure the Flash Chip

//		watchdog_init(0x61A);	//5sec watchdog
//		api_watchdog_init(1);	//Enable	//0 = watchdog disabled
	}
	if(mode == STARTUP_FULLY){
		api_io_power_sys(1);				//keep alive
		PinSet(EN_30V, 1);

		api_watchdog_init(0);	//Disable	//0 = watchdog disabled
		api_io_power_5V_sensor(1);
		io_Pins_FSMC_Config();

		csp_FSMC_NE1_Config(FSMC_STARTUP);

		csp_mem_config();
		S25FL0xx_Config();

		csp_interrupts_config();						// Set up interrupts

//		I2C_LowLevel_Init(I2C2);

		init_DAC1();									// Set up DAC 1

//		csp_ADC2_config();
		timer2_config();			//4kHz		audio  DMA->DAC
		timer3_config();			//36kHz		Fan pwm
		timer4_config();			//100Hz		ADC sampler switch(samples at 10Hz)
		timer8_config();			//2000Hz	PWM valves
		api_io_fan(0);

		spi2_cs_manager(0xff);							//disbale all SPI cs
		spi2_config(SPI_MODE_0,10000000);

		LCD_Init_SSD1963();
		
		csp_paracube_timeout_read(1);		// Reset paracube error timeout
		
//		watchdog_init(0x61A);	//5sec watchdog
//		api_watchdog_init(1);	//Enable	//0 = watchdog disabled
	}

#ifdef IAR_DEBUG
	printf("\r\n***************");
	printf("\r\n** IAR DEBUG **");
	printf("\r\n***************");
#endif

	return;
}

/**********************************************************************************************************
* Function Name : pcb_map_functions
* Description   :
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0
*********************************************************************************************************/
void pcb_map_functions(void)
{
	S25FL0xx_spi_fun_sel(	spi1_rdwr, spi1_cs_manager	);
	AD7794_spi_fun_sel (	spi2_rdwr, spi2_cs_manager	);

	api_bootloader_fun_sel(	uart2_getchar,	uart2_putchar);
	csp_paracube_fun_sel(	uart4_getchar,	uart4_putchar);

	api_RTC_map(	csp_STM32_RTC_time);
	return;
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
