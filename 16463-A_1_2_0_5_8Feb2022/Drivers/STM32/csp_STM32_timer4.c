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
 * Filename    :  csp_STM32_timer4.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the Timer 3 functions.
 *
 **********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "pcb_timer.h"

#if( TIMER4_EN == 1)
#include "csp_STM32_timer4.h"

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

/**********************************************************************************************************
 *	COMPILER DEFINES
 *********************************************************************************************************/


/**********************************************************************************************************
 *	GLOBALS VARIABLES
 *********************************************************************************************************/


/**********************************************************************************************************
 *	LOCAL VARIABLES
 *********************************************************************************************************/
uint32_t	u32_PrescalerValue_T4;
uint32_t	u32_TimerValue_T4;
uint16_t	u16_PeriodValue_T4;


/**********************************************************************************************************
 *	LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************/


/**********************************************************************************************************
 * Function Name : timer3_config
 * Description   : This function is used to initialise timer3
 * Arguments     :
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		08/11/2010      Philip Gillespie     Original Created
 *********************************************************************************************************/
void timer4_config(void)
{
/* Local Variables */
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;


/* Code */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			/* TIM4 Periph clock enable */

	u32_PrescalerValue_T4 	=  15000;		/* Hz 	 (72000000 	/ Prescale	) - 1	*/
	u32_TimerValue_T4		=  150;			/* Hz 	 (Prescale	/ TimerValue	) - 1	*/
	u16_PeriodValue_T4		=  (uint16_t)(u32_PrescalerValue_T4 	/ u32_TimerValue_T4);

	TIM_TimeBaseStructure.TIM_Prescaler 		=  (uint16_t)(SystemCoreClock / u32_PrescalerValue_T4)	- 1;
	TIM_TimeBaseStructure.TIM_Period 			=  (uint16_t)(u16_PeriodValue_T4) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision 	=  0;
	TIM_TimeBaseStructure.TIM_CounterMode 		=  TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter =  0x00;
  	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);


/* NOTE */
/* There are 4 channels on this timer */
/* they must be configured seperatly using the functions timerX_ch(n) */

/* Enable Timer 3 Interupt */
#ifdef TIMER4_INT_EN
    NVIC_InitTypeDef			NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel 						= TIM4_IRQn;		/* Enable the TIM3 UP Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

  	TIM_ClearFlag(TIM4, TIM_FLAG_Update);				/* Clear TIM3 update pending flag */
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);			/* Enable the TIM3 Update Interrupt */
	TIM_Cmd(TIM4, ENABLE);							 	/* TIM1 counter enable */

	return;
}


/**********************************************************************************************************
 * Function Name : timer4_IRQ
 * Description   : This function is used to set the PWM duty cycle of ch1
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : duty_cycle_u16	must be less than the prescalar
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 * 1.0.1			15/ 2/2011		William Paul		extend function
 *********************************************************************************************************/
void timer4_IRQ(void)
{




	return;
}



/**********************************************************************************************************
 * Function Name : timer4_ch1_config
 * Description   : This function is used to set the PWM duty cycle of ch1
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : duty_cycle_u16	must be less than the prescalar
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 * 1.0.1			15/ 2/2011		William Paul		extend function
 *********************************************************************************************************/
#if( TIMER4_CH1_EN == 1)
void timer4_ch1_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;

	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;


    TIM_OCInitStructure.TIM_OCMode			=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState 	=  TIM_OutputState_Enable;      // TIM_OutputState_Enable 	TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_Pulse			=  (uint16_t)(u16_PeriodValue_T4	* duty_cycle_u16 / 100);
    TIM_OCInitStructure.TIM_OCPolarity		=  TIM_OCPolarity_High;

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
		}
	}
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);						// Check if this works without this line

    return;
}
#endif //#if( TIMER4_CH1_EN == 1)


/**********************************************************************************************************
 * Function Name : timer4_ch2_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 ********************************************************************************************************/
#if( TIMER4_CH2_EN == 1)
void timer4_ch2_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode		=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState =  TIM_OutputState_Enable;			// TIM_OutputState_Enable 	TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_Pulse		=  (uint16_t)(u16_PeriodValue_T4	* duty_cycle_u16 / 100);
    TIM_OCInitStructure.TIM_OCPolarity	=  TIM_OCPolarity_High;

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
	}

    TIM_OC2Init(TIM4, &TIM_OCInitStructure);

    return;
}
#endif //#if( TIMER4_CH2_EN == 1)


/**********************************************************************************************************
 * Function Name : timer4_ch3_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 *********************************************************************************************************/
#if( TIMER4_CH3_EN == 1)
void timer4_ch3_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode		=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState =  TIM_OutputState_Enable;		// TIM_OutputState_Enable 	TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_Pulse		=  (uint16_t)(u16_PeriodValue_T4	* duty_cycle_u16 / 100);
    TIM_OCInitStructure.TIM_OCPolarity	=  TIM_OCPolarity_High;

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
		}
	}

    TIM_OC3Init(TIM4, &TIM_OCInitStructure);

    return;
}
#endif //#if( TIMER4_CH3_EN == 1)



/**********************************************************************************************************
 * Function Name : timer4_ch4_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 *********************************************************************************************************/
#if( TIMER4_CH4_EN == 1)
void timer4_ch4_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode		=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState =  TIM_OutputState_Enable;		// TIM_OutputState_Enable 	TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_Pulse		=  (uint16_t)(u16_PeriodValue_T4	* duty_cycle_u16 / 100);
    TIM_OCInitStructure.TIM_OCPolarity	=  TIM_OCPolarity_High;

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_1;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
		}
	}

    TIM_OC4Init(TIM4, &TIM_OCInitStructure);

    return;
}
#endif //#if( TIMER4_CH4_EN == 1)

#endif //#if( TIMER4_EN == 1)
/**********************************************************************************************************
 *                                           End of csp_STM32_timer4.c
 *********************************************************************************************************/
