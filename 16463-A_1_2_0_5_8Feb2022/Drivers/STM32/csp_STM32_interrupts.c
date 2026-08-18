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
 * Filename    :  csp_STM32_interrupt.c
 * Programmer  :  Philip Gillespie
 * Description :  This module is used to initialise external interrupts
 * Compiler    :  GNU GCC Ride 7 Version 1.26.10.0130
 * Target      :  STM32103
 * ST Library  :  Version 1.0.0
 *
 **********************************************************************************************************/

/**********************************************************************************************************
 *                                           INCLUDE FILES
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "csp_STM32_interrupts.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "pcb_interrupts.h"

/**********************************************************************************************************
 *		COMPILER DEFINES
 **********************************************************************************************************/


/**********************************************************************************************************
 *		GLOBALS VARIABLES
 **********************************************************************************************************/


/**********************************************************************************************************
 *		LOCAL VARIABLES
 **********************************************************************************************************/


/**********************************************************************************************************
 *		LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************/

uint8_t	ext_INTx_TRIGGER[16]	=  {	EXT_INT0_TRIGGER,
								EXT_INT1_TRIGGER,
								EXT_INT2_TRIGGER,
								EXT_INT3_TRIGGER,
								EXT_INT4_TRIGGER,
								EXT_INT5_TRIGGER,
								EXT_INT6_TRIGGER,
								EXT_INT7_TRIGGER,
								EXT_INT8_TRIGGER,
								EXT_INT9_TRIGGER,
								EXT_INT10_TRIGGER,
								EXT_INT11_TRIGGER,
								EXT_INT12_TRIGGER,
								EXT_INT13_TRIGGER,
								EXT_INT14_TRIGGER,
								EXT_INT15_TRIGGER 	};


/**********************************************************************************************************
 **********************************************************************************************************/




/**********************************************************************************************************
 * Function Name : EXT_INT_config
 * Description   : This function configures a pin for externally generated interupts on one of the
 *				  processor's 15 GPIO pins.
 * Arguments     : None
 * Returns       : None
 * Notes         : You need to configure:
 *					> GPIO PORT
 *					> GPIO PIN
 *					> Trigger Type (Rising edge OR Falling edge)
 *
 * Version		Date d/m/y	    Programmer          	Reason for Change
 * 1.0.0		    15/11/2010      Philip Gillespie       	Original Created
 **********************************************************************************************************/
void csp_interrupts_config(void)
{
/* Local Variables */

/* Code */
#if(EXT_INT0_EN == 1)
	GPIO_EXTILineConfig(EXT_INT0_PORT , GPIO_PinSource0);
	ext_interupt_endis(0,1);
	csp_STM32_interrupt_EXTI0(1);
#endif
#if(EXT_INT1_EN == 1)
	GPIO_EXTILineConfig(EXT_INT1_PORT , GPIO_PinSource1);
	ext_interupt_endis(1,1);
	csp_STM32_interrupt_EXTI1(1);
#endif
#if(EXT_INT2_EN == 1)
	GPIO_EXTILineConfig(EXT_INT2_PORT , GPIO_PinSource2);
	ext_interupt_endis(2,1);
	csp_STM32_interrupt_EXTI2(1);
#endif
#if(EXT_INT3_EN == 1)
	GPIO_EXTILineConfig(EXT_INT3_PORT , GPIO_PinSource3);
	ext_interupt_endis(3,0);
	csp_STM32_interrupt_EXTI3(1);
#endif
#if(EXT_INT4_EN == 1)
	GPIO_EXTILineConfig(EXT_INT4_PORT , GPIO_PinSource4);
	ext_interupt_endis(4,0);
	csp_STM32_interrupt_EXTI4(1);
#endif


/*********************************************************************************************************
 *		Ext Interrupts 5-9
 ********************************************************************************************************/
#if(EXT_INT5_EN == 1)
	GPIO_EXTILineConfig(EXT_INT5_PORT , GPIO_PinSource5);
	ext_interupt_endis(5,1);
	csp_STM32_interrupt_EXTI9_5(1);
#endif
#if(EXT_INT6_EN == 1)
	GPIO_EXTILineConfig(EXT_INT6_PORT , GPIO_PinSource6);
	ext_interupt_endis(6,0);
	csp_STM32_interrupt_EXTI9_5(1);
#endif
#if(EXT_INT7_EN == 1)
	GPIO_EXTILineConfig(EXT_INT7_PORT , GPIO_PinSource7);
	ext_interupt_endis(7,0);
	csp_STM32_interrupt_EXTI9_5(1);
#endif
#if(EXT_INT8_EN == 1)
	GPIO_EXTILineConfig(EXT_INT8_PORT , GPIO_PinSource8);
	ext_interupt_endis(8,0);
	csp_STM32_interrupt_EXTI9_5(1);
#endif
#if(EXT_INT9_EN == 1)
	GPIO_EXTILineConfig(EXT_INT9_PORT , GPIO_PinSource9);
	ext_interupt_endis(9,0);
	csp_STM32_interrupt_EXTI9_5(1);
#endif

/*********************************************************************************************************
 *		Ext Interrupts 10-15
 ********************************************************************************************************/
#if(EXT_INT10_EN == 1)
	GPIO_EXTILineConfig(EXT_INT10_PORT , GPIO_PinSource10);
	ext_interupt_endis(10,0);
	csp_STM32_interrupt_EXTI15_10(1);
#endif
#if(EXT_INT11_EN == 1)
	GPIO_EXTILineConfig(EXT_INT11_PORT , GPIO_PinSource11);
	ext_interupt_endis(11,0);
	csp_STM32_interrupt_EXTI15_10(1);
#endif
#if(EXT_INT12_EN == 1)
	GPIO_EXTILineConfig(EXT_INT12_PORT , GPIO_PinSource12);
	ext_interupt_endis(12,0);
	csp_STM32_interrupt_EXTI15_10(1);
#endif
#if(EXT_INT13_EN == 1)
	GPIO_EXTILineConfig(EXT_INT13_PORT , GPIO_PinSource13);
	ext_interupt_endis(13,0);
	csp_STM32_interrupt_EXTI15_10(1);
#endif
#if(EXT_INT14_EN == 1)
	GPIO_EXTILineConfig(EXT_INT14_PORT , GPIO_PinSource14);
    	ext_interupt_endis(14,0);
	csp_STM32_interrupt_EXTI15_10(1);
#endif
#if(EXT_INT15_EN == 1)
	GPIO_EXTILineConfig(EXT_INT15_PORT , GPIO_PinSource15);
	ext_interupt_endis(15,0);
	csp_STM32_interrupt_EXTI15_10(1);
#endif

	return;

}


/*************************************************************************************************
* Function Name : 	ext_interupt_endis
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	06/01/16		W. Paul			Created
*************************************************************************************************/
void ext_interupt_endis(uint8_t line,uint8_t endis)
{
	EXTI_InitTypeDef	EXTI_InitStructure;
	uint16_t			line_no;

	line_no	=  0x0001<<line;

	EXTI_InitStructure.EXTI_Line 		= line_no ;				/* GPIO pin 15 must be on EXTI line 15 */
	EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger 	= EXTI_Trigger_Falling;	//(EXTITrigger_TypeDef)ext_INTx_TRIGGER[line];
																//EXTI_Trigger_Rising,  EXTI_Trigger_Falling, EXTI_Trigger_Rising_Falling
	EXTI_InitStructure.EXTI_LineCmd 	= (FunctionalState)endis;
	EXTI_Init(&EXTI_InitStructure);

	return;
}



/**********************************************************************************************************
 * Function Name : csp_STM32_interrupt_EXTI0
 * Description   : This function is used to enable /disable teh interrupt
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		17/07/12	William Paul	Original Created
 **********************************************************************************************************/
#if(EXT_INT0_EN == 1)
void csp_STM32_interrupt_EXTI0(uint8_t en_dis)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel						=  EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=  EXTI0_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  EXTI0_IRQ_SUB_PRI;
	if(en_dis){
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  ENABLE;
	}
	else{
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  DISABLE;
	}
	NVIC_Init(&NVIC_InitStructure);

	return;
}
#endif

/**********************************************************************************************************
 * Function Name : csp_STM32_interrupt_EXTI1
 * Description   : This function is used to enable /disable teh interrupt
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		17/07/12	William Paul	Original Created
 **********************************************************************************************************/
#if(EXT_INT1_EN == 1)
void csp_STM32_interrupt_EXTI1(uint8_t en_dis)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel						=  EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority		=  EXTI1_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  EXTI1_IRQ_SUB_PRI;
	if(en_dis){
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  ENABLE;
	}
	else{
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  DISABLE;
	}
	NVIC_Init(&NVIC_InitStructure);

	return;
}
#endif

/**********************************************************************************************************
 * Function Name : csp_STM32_interrupt_EXTI2
 * Description   : This function is used to enable /disable teh interrupt
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		17/07/12	William Paul	Original Created
 **********************************************************************************************************/
#if(EXT_INT2_EN == 1)
void csp_STM32_interrupt_EXTI2(uint8_t en_dis)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel						=  EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority		=  EXTI2_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  EXTI2_IRQ_SUB_PRI;
	if(en_dis){
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  ENABLE;
	}
	else{
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  DISABLE;
	}
	NVIC_Init(&NVIC_InitStructure);

	return;
}
#endif

/**********************************************************************************************************
 * Function Name : csp_STM32_interrupt_EXTI3
 * Description   : This function is used to enable /disable teh interrupt
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		17/07/12	William Paul	Original Created
 **********************************************************************************************************/
#if(EXT_INT3_EN == 1)
void csp_STM32_interrupt_EXTI3(uint8_t en_dis)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel						=  EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=  EXTI3_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  EXTI3_IRQ_SUB_PRI;
	if(en_dis){
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  ENABLE;
	}
	else{
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  DISABLE;
	}
	NVIC_Init(&NVIC_InitStructure);

	return;
}
#endif

/**********************************************************************************************************
 * Function Name : csp_STM32_interrupt_EXTI4
 * Description   : This function is used to enable /disable teh interrupt
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		17/07/12	William Paul	Original Created
 **********************************************************************************************************/
#if(EXT_INT4_EN == 1)
void csp_STM32_interrupt_EXTI4(uint8_t en_dis)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel						=  EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority		=  EXTI4_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  EXTI4_IRQ_SUB_PRI;
	if(en_dis){
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  ENABLE;
	}
	else{
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  DISABLE;
	}
	NVIC_Init(&NVIC_InitStructure);

	return;
}
#endif

/**********************************************************************************************************
 * Function Name : csp_STM32_interrupt_EXTI9_5
 * Description   : This function is used to enable /disable teh interrupt
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		17/07/12	William Paul	Original Created
 **********************************************************************************************************/
void csp_STM32_interrupt_EXTI9_5(uint8_t en_dis)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel						=  EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority		=  EXTI9_5_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  EXTI9_5_IRQ_SUB_PRI;
	if(en_dis){
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  ENABLE;
	}
	else{
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  DISABLE;
	}
	NVIC_Init(&NVIC_InitStructure);

	return;
}

/**********************************************************************************************************
 * Function Name : csp_STM32_interrupt_EXTI9_5
 * Description   : This function is used to enable /disable teh interrupt
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	Programmer		Reason for Change
 * 1.0.0		17/07/12	William Paul	Original Created
 **********************************************************************************************************/
void csp_STM32_interrupt_EXTI15_10(uint8_t en_dis)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel						=  EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority		=  EXTI15_10_IRQ_PREM_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=  EXTI15_10_IRQ_SUB_PRI;
	if(en_dis){
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  ENABLE;
	}
	else{
		NVIC_InitStructure.NVIC_IRQChannelCmd				=  DISABLE;
	}
	NVIC_Init(&NVIC_InitStructure);

	return;
}

/**********************************************************************************************************
 *		End of file
 **********************************************************************************************************/
