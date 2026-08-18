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
 * Filename    :  csp_STM32_timer8.c
 * Date Created:  Wed 06 Sep 2017 02:56:59 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/


/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "pcb_timer.h"

#if( TIMER8_EN == 1)
#include "csp_STM32_timer8.h"

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
uint32_t	u32_PrescalerValue_T8_u16;
uint32_t	u32_TimerValue_T8_u16;
uint16_t	u16_PeriodValue_T8_u16;


/**********************************************************************************************************
 *	LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************/

/*************************************************************************************************
* Function Name : 	timer8_config
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		06/09/17	W. Paul			Created
*************************************************************************************************/
void timer8_config(void)
{
/* Local Variables */
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
#if(TIMER8_INT_EN == 1)
    NVIC_InitTypeDef			NVIC_InitStructure;
#endif

/* Code */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);			/* TIM8 Periph clock enable */

	u32_PrescalerValue_T8_u16 						=  10000*3600;	/* Hz 		 (24000000 	/ Prescale		) - 1	*/
	u32_TimerValue_T8_u16							=    3600;		/* Hz 		 (Prescale	/ TimerValue	) - 1	*/
	u16_PeriodValue_T8_u16							=  (uint16_t)(u32_PrescalerValue_T8_u16	/ u32_TimerValue_T8_u16);

	TIM_TimeBaseStructure.TIM_Prescaler 			=  (uint16_t) (SystemCoreClock			/ u32_PrescalerValue_T8_u16) - 1;
	TIM_TimeBaseStructure.TIM_Period 				=  (uint16_t) (u16_PeriodValue_T8_u16) - 1;
//	TIM_TimeBaseStructure.TIM_Period 				=  0xff00;
	TIM_TimeBaseStructure.TIM_ClockDivision 		=  0;
	TIM_TimeBaseStructure.TIM_CounterMode 			=  TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter		=  0x00;
  	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

/* NOTE */
/* There are 4 channels on this timer */
/* they must be configured seperatly using the functions timer8_ch(n) */

/* Enable Timer 1 Interupt */
#if(TIMER8_INT_EN == 1)
#if		defined(STM32F10X_HD)
	NVIC_InitStructure.NVIC_IRQChannel 				= TIM8_UP_IRQn;	/* Enable the TIM8 UP Interrupt */
#elif	defined(STM32F10X_MD_VL)
	#warning TIMER8_INT_EN is not available
#endif

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);				/* Enable the TIM8 Update Interrupt */
#endif

 	TIM_ClearFlag(TIM8, TIM_FLAG_Update);					/* Clear TIM8 update pending flag */
	TIM_Cmd(TIM8, ENABLE);									/* TIM8 counter enable */

	return;
}


/*************************************************************************************************
* Function Name : 	timer8_IRQ
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		06/09/17	W. Paul			Created
*************************************************************************************************/
void timer8_IRQ(void)
{




	return;
}



/**********************************************************************************************************
 * Function Name : timer8_ch1_config
 * Description   : This function is used to set the PWM duty cycle of ch1
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : duty_cycle_u16	must be less than the prescalar
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 * 1.0.1			15/ 2/2011		William Paul		extend function
 *********************************************************************************************************/
#if( TIMER8_CH1_EN == 1)
void timer8_ch1_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;

	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

	TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode			=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState 	=  TIM_OutputState_Enable;      // TIM_OutputState_Enable 	TIM_OutputState_Disable
  	TIM_OCInitStructure.TIM_OutputNState	=  TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse			=  (uint16_t)(duty_cycle_u16);
    TIM_OCInitStructure.TIM_OCPolarity		=  TIM_OCPolarity_High;				//	TIM_OCPolarity_High			TIM_OCPolarity_Low
	TIM_OCInitStructure.TIM_OCNPolarity		=  TIM_OCNPolarity_Low;				//	TIM_OCNPolarity_High		TIM_OCNPolarity_Low
	TIM_OCInitStructure.TIM_OCIdleState		=  TIM_OCIdleState_Reset;			//	TIM_OCIdleState_Set			TIM_OCIdleState_Reset
	TIM_OCInitStructure.TIM_OCNIdleState	=  TIM_OCNIdleState_Set;			//	TIM_OCNIdleState_Set		TIM_OCNIdleState_Reset

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
		}
	}
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);
    TIM_CtrlPWMOutputs(TIM8,ENABLE);

//	TIM_ClearFlag(TIM8, TIM_IT_CC1);
//	TIM_ITConfig(TIM8, TIM_IT_CC1, ENABLE);

    return;
}
#endif //#if( TIMER8_CH1_EN == 1)


/**********************************************************************************************************
 * Function Name : timer8_ch2_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 ********************************************************************************************************/
#if( TIMER8_CH2_EN == 1)
void timer8_ch2_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

     TIM_OCInitStructure.TIM_OCMode			=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState 	=  TIM_OutputState_Enable;      // TIM_OutputState_Enable 	TIM_OutputState_Disable
  	TIM_OCInitStructure.TIM_OutputNState	=  TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse			=  (uint16_t)(duty_cycle_u16);
    TIM_OCInitStructure.TIM_OCPolarity		=  TIM_OCPolarity_High;				//	TIM_OCPolarity_High			TIM_OCPolarity_Low
	TIM_OCInitStructure.TIM_OCNPolarity		=  TIM_OCNPolarity_Low;				//	TIM_OCNPolarity_High		TIM_OCNPolarity_Low
	TIM_OCInitStructure.TIM_OCIdleState		=  TIM_OCIdleState_Reset;			//	TIM_OCIdleState_Set			TIM_OCIdleState_Reset
	TIM_OCInitStructure.TIM_OCNIdleState	=  TIM_OCNIdleState_Set;			//	TIM_OCNIdleState_Set		TIM_OCNIdleState_Reset

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
		}
	}

    TIM_OC2Init(TIM8, &TIM_OCInitStructure);
    TIM_CtrlPWMOutputs(TIM8,ENABLE);

    return;
}
#endif //#if( TIMER8_CH2_EN == 1)


/**********************************************************************************************************
 * Function Name : timer8_ch3_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 *********************************************************************************************************/
#if( TIMER8_CH3_EN == 1)
void timer8_ch3_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode			=  TIM_OCMode_PWM1;				// TIM_OCMode_Timing 		TIM_OCMode_PWM1
    TIM_OCInitStructure.TIM_OutputState 	=  TIM_OutputState_Enable;      // TIM_OutputState_Enable 	TIM_OutputState_Disable
  	TIM_OCInitStructure.TIM_OutputNState	=  TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse			=  (uint16_t)(duty_cycle_u16);
    TIM_OCInitStructure.TIM_OCPolarity		=  TIM_OCPolarity_High;				//	TIM_OCPolarity_High			TIM_OCPolarity_Low
	TIM_OCInitStructure.TIM_OCNPolarity		=  TIM_OCNPolarity_Low;				//	TIM_OCNPolarity_High		TIM_OCNPolarity_Low
	TIM_OCInitStructure.TIM_OCIdleState		=  TIM_OCIdleState_Reset;			//	TIM_OCIdleState_Set			TIM_OCIdleState_Reset
	TIM_OCInitStructure.TIM_OCNIdleState	=  TIM_OCNIdleState_Set;			//	TIM_OCNIdleState_Set		TIM_OCNIdleState_Reset

	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_8;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
		}
	}

    TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
    return;
}
#endif //#if( TIMER8_CH3_EN == 1)



/**********************************************************************************************************
 * Function Name : timer8_ch4_config
 * Description   : This function is used to set the PWM duty cycle
 * Arguments     : uint16_t duty_cycle_u16
 * Returns       : None
 * Notes         : None
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    11/05/2010      Michael Kelly       Original Created
 *********************************************************************************************************/
#if( TIMER8_CH4_EN == 1)
void timer8_ch4_config(uint16_t duty_cycle_u16)
{
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	GPIO_InitTypeDef 			GPIO_InitStructure;
	static uint8_t				io_pin_configured	=  0;

    TIM_OCInitStructure.TIM_OCMode			=  TIM_OCMode_PWM1;				//	TIM_OCMode_Timing 			TIM_OCMode_PWM1		TIM_OCMode_Toggle
    TIM_OCInitStructure.TIM_OutputState 	=  TIM_OutputState_Enable;			//	TIM_OutputState_Enable 		TIM_OutputState_Disable
    TIM_OCInitStructure.TIM_OutputNState	=  TIM_OutputNState_Disable;		//	TIM_OutputNState_Enable 	TIM_OutputNState_Disable
	TIM_OCInitStructure.TIM_Pulse			=  (uint16_t)(duty_cycle_u16);
    TIM_OCInitStructure.TIM_OCPolarity		=  TIM_OCPolarity_High;				//	TIM_OCPolarity_High			TIM_OCPolarity_Low
	TIM_OCInitStructure.TIM_OCNPolarity		=  TIM_OCNPolarity_Low;				//	TIM_OCNPolarity_High		TIM_OCNPolarity_Low
	TIM_OCInitStructure.TIM_OCIdleState		=  TIM_OCIdleState_Reset;			//	TIM_OCIdleState_Set			TIM_OCIdleState_Reset
	TIM_OCInitStructure.TIM_OCNIdleState	=  TIM_OCNIdleState_Set;			//	TIM_OCNIdleState_Set		TIM_OCNIdleState_Reset


	if(TIM_OCInitStructure.TIM_OutputState == TIM_OutputState_Enable){
		if(io_pin_configured == 0){
			io_pin_configured	=  1;
			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
		}
	}

    TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
    return;
}
#endif //#if( TIMER8_CH4_EN == 1)

#endif //#if( TIMER8_EN == 1)
/**********************************************************************************************************
 *                                           End of csp_STM32_timer8.c
 *********************************************************************************************************/
