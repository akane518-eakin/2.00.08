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
 * Filename    :  csp_STM32_timer2.c
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/



/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "pcb_timer.h"

#if( TIMER2_EN == 1)
#include "csp_STM32_timer2.h"

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
uint32_t	u32_PrescalerValue_T2_u16;
uint32_t	u32_TimerValue_T2_u16;
uint16_t	u16_PeriodValue_T2_u16;


/**********************************************************************************************************
 *	LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************/


/**********************************************************************************************************
 * Function Name : timer2_config
 * Description   : This function is used to initialise timer1
 * Arguments     :
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		08/11/2010      Philip Gillespie     Original Created
 *********************************************************************************************************/
void timer2_config(void)
{
/* Local Variables */
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
#if(TIMER2_INT_EN == 1)
    NVIC_InitTypeDef			NVIC_InitStructure;
#endif

/* Code */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			/* TIM2 Periph clock enable */


	/* Timer 1 */
	u32_PrescalerValue_T2_u16 					=  8000000;		/* Hz 		 (72000000 	/ Prescale		) - 1	*/
	u32_TimerValue_T2_u16						=  8000;		/* Hz 		 (Prescale	/ TimerValue	) - 1	*/
	u16_PeriodValue_T2_u16						=  (uint16_t)(u32_PrescalerValue_T2_u16 	/ u32_TimerValue_T2_u16);

	TIM_TimeBaseStructure.TIM_Prescaler 		=  (uint16_t) (SystemCoreClock 			/ u32_PrescalerValue_T2_u16)	- 1;
	TIM_TimeBaseStructure.TIM_Period 			=  (uint16_t) u16_PeriodValue_T2_u16	- 1;
	TIM_TimeBaseStructure.TIM_ClockDivision 	=  0;
	TIM_TimeBaseStructure.TIM_CounterMode 		=  TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter =  0x00;
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

/* NOTE */
/* There are 4 channels on this timer */
/* they must be configured seperatly using the functions timer1_ch(n) */

//	this output sets an interrupt for
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

/* Enable Timer 2 Interupt */
#if(TIMER2_INT_EN == 1)
	NVIC_InitStructure.NVIC_IRQChannel 						= TIM2_IRQn;		/* Enable the TIM1 UP Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

  	TIM_ClearFlag(TIM2, TIM_FLAG_Update);				/* Clear TIM2 update pending flag */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);			/* Enable the TIM2 Update Interrupt */
	TIM_Cmd(TIM2, ENABLE);							 	/* TIM1 counter enable */

	return;
}


/**********************************************************************************************************
 * Function Name : timer2_IRQ
 * Description   : This function is used to set the PWM duty cycle of ch1
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : duty_cycle_u16	must be less than the prescalar
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 * 1.0.1			15/ 2/2011		William Paul		extend function
 *********************************************************************************************************/
void timer2_IRQ(void)
{




	return;
}



/*
*********************************************************************************************************
* Function Name : timer2_cnt_rd
* Description   : This function is used to read the cnt var in timer2.
* Arguments     : void
* Returns       : uint16_t
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    11/05/2010      Michael Kelly       Original Created
*********************************************************************************************************
*/
uint16_t timer2_cnt_rd(void)
{
	return (TIM_GetCounter(TIM2));
}

/*
*********************************************************************************************************
* Function Name : timer2_cnt_wr
* Description   : This function is used to write to the cnt var in timer2.
* Arguments     : uint16_t	value
* Returns       : void
* Notes         : None
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    11/05/2010      Michael Kelly       Original Created
*********************************************************************************************************
*/
void timer2_cnt_wr(uint16_t value)
{
	TIM_SetCounter(TIM2, value);
	return;
}



/**********************************************************************************************************
 * Function Name : timer2_ch1_config
 * Description   : This function is used to set the PWM duty cycle of ch1
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : duty_cycle_u16	must be less than the prescalar
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 * 1.0.1			15/ 2/2011		William Paul		extend function
 *********************************************************************************************************/
#if( TIMER2_CH1_EN == 1)
void timer2_ch1_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;

	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode			=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState 	=  TIM_OutputState_Enable;      // TIM_OutputState_Enable 	TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_Pulse			=  (uint16_t)(u16_PeriodValue_T2_u16	* duty_cycle_u16 / 100);
    TIM_OCInitStructure.TIM_OCPolarity		=  TIM_OCPolarity_High;

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
	}
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    return;
}
#endif //#if( TIMER2_CH1_EN == 1)


/**********************************************************************************************************
 * Function Name : timer2_ch2_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 ********************************************************************************************************/
#if( TIMER2_CH2_EN == 1)
void timer2_ch2_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode		=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState =  TIM_OutputState_Enable;			// TIM_OutputState_Enable 	TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_Pulse		=  (uint16_t)(u16_PeriodValue_T2_u16	* duty_cycle_u16 / 100);
    TIM_OCInitStructure.TIM_OCPolarity	=  TIM_OCPolarity_High;

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_1;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
	}

    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    return;
}
#endif //#if( TIMER2_CH2_EN == 1)


/**********************************************************************************************************
 * Function Name : timer2_ch3_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 *********************************************************************************************************/
#if( TIMER2_CH3_EN == 1)
void timer2_ch3_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode		=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState =  TIM_OutputState_Enable;		// TIM_OutputState_Enable 	TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_Pulse		=  (uint16_t)(u16_PeriodValue_T2_u16	* duty_cycle_u16 / 100);
    TIM_OCInitStructure.TIM_OCPolarity	=  TIM_OCPolarity_High;

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
	}

    TIM_OC3Init(TIM2, &TIM_OCInitStructure);

    return;
}
#endif //#if( TIMER2_CH3_EN == 1)



/**********************************************************************************************************
 * Function Name : timer2_ch4_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 *********************************************************************************************************/
#if( TIMER2_CH4_EN == 1)
void timer2_ch4_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode		=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState =  TIM_OutputState_Enable;		// TIM_OutputState_Enable 	TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_Pulse		=  (uint16_t)(u16_PeriodValue_T2_u16	* duty_cycle_u16 / 100);
    TIM_OCInitStructure.TIM_OCPolarity	=  TIM_OCPolarity_High;

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
	}

    TIM_OC4Init(TIM2, &TIM_OCInitStructure);

    return;
}
#endif //#if( TIMER2_CH4_EN == 1)

#endif //#if( TIMER2_EN == 1)
/**********************************************************************************************************
 *                                           End of csp_STM32_timer2.c
 *********************************************************************************************************/
