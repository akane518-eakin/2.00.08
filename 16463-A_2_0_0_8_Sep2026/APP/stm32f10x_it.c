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
 * Filename    :  stm32f10x_it.c
 * Date Created:  Tue 04 Apr 2017 09:34:16 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "glb_typedefs.h"

#include "stm32f10x_it.h"
#include "pcb_timer.h"

#include "pcb_uart.h"
#if( UART1_EN == 1)
#include "csp_STM32_uart.h"
#include "pcb_uart1.h"
#include "csp_STM32_uart1.h"
#endif
#if( UART2_EN == 1)
#include "csp_STM32_uart.h"
#include "pcb_uart2.h"
#include "csp_STM32_uart2.h"
#endif
#if( UART3_EN == 1)
#include "pcb_uart3.h"
#include "csp_STM32_uart3.h"
#endif
#if( UART4_EN == 1)
#include "csp_STM32_uart.h"
#include "pcb_uart4.h"
#include "csp_STM32_uart4.h"
#endif
#if( UART5_EN == 1)
#include "pcb_uart5.h"
#include "csp_STM32_uart5.h"
#endif

#include "app_UI.h"
#include "app_pneumatic_ctrl.h"
#include "app_touchscreen.h"
#include "app_button.h"
#include "app_patient_pressure.h"
#include "app_selfcheck.h"

#include "api_audio.h"
#include "api_STM32_touchscreen.h"
#include "api_stopwatch.h"
#include "api_LEDs.h"

#include "csp_paracube_O2.h"


uint32_t	tmr3_ms;	//SS Fan 241019
float		tacho;		//SS Fan 241019
uint8_t		tacho_flag;	//SS Fan 241019
uint32_t	tacho_tmr;	//SS Fan 241019
uint32_t	tacho_rd;	//SS Fan 241019


/********************************************************************************************************
* Function Name : NMI_Handler
* Description   : This function is used for when a NMI (Non-Maskable Interrupt) exception.
* Arguments     : None
* Returns       : None
* Notes         : Refer to Document RM0008.pdf page 82
*				Non maskable interrupt. The RCC Clock Security System (CSS) is linked to the NMI vector.
*				Type of priority: fixed
*				Priority		: -3
*				Address			: 0x0000_0004
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
********************************************************************************************************/
void NMIException(void)
{
	printf("\n\rNMI :%u", __LINE__);
    while (1){
  	}
}



/*********************************************************************************************************
* Function Name : HardFault_Handler
* Description   : This function is used for when a Hard Fault exception occurs
* Arguments     : None
* Returns       : None
* Notes         : 	HardFault:  All class of fault
*					Type of priority: fixed
*					Priority		: -1
*					Address			: 0x0000_000C
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
*********************************************************************************************************/
void HardFault_Handler(void)
{
	printf ( "\r\nSerious error occurred in file% s, line% d \n", __FILE__, __LINE__);
	while (1){
	}
}

/**********************************************************************************************************
* Function Name : MemManage_Handler
* Description   : This function is used for when a Memory Manage exception occurs
* Arguments     : None
* Returns       : None
* Notes         : 	MemManage:  AMemory management
*					Type of priority: settable
*					Priority		: 0
*					Address			: 0x0000_0010
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
**********************************************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
	printf("\n\rFault Mem :%u", __LINE__);
	while (1){
	}
}

/**********************************************************************************************************
* Function Name : BusFault_Handler
* Description   : This function is used for when a Bus Fault exception occurs
* Arguments     : None
* Returns       : None
* Notes         : 	Pre-fetch fault, memory access fault
*					Type of priority: settable
*					Priority		: 1
*					Address			: 0x0000_0014
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
**********************************************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
	printf("\n\rFault Bus :%u", __LINE__);
	while (1){
	}
}


/**********************************************************************************************************
* Function Name : UsageFault_Handler
* Description   : This function is used for when a Usage Fault exception occurs
* Arguments     : None
* Returns       : None
* Notes         : 	Undefined instruction or illegal state
*					Type of priority: settable
*					Priority		: 2
*					Address			: 0x0000_0018
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
**********************************************************************************************************/
void UsageFaultException(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
	printf("\n\rFault Usage :%u", __LINE__);
	while (1){
	}
}

/**********************************************************************************************************
* Function Name : SVC_Handler
* Description   : This function is used for setting a service call
* Arguments     : None
* Returns       : None
* Notes         : 	System service call via SWI instruction
*					Type of priority: settable
*					Priority		: 3
*					Address			: 0x0000_002C
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
**********************************************************************************************************/
void SVCHandler(void)
{
}

/**********************************************************************************************************
* Function Name : DebugMonitor
* Description   : This function is used for handling Debug Monitor exception.
* Arguments     : None
* Returns       : None
* Notes         : 	Debug Monitor
*					Type of priority: settable
*					Priority		: 4
*					Address			: 0x0000_0030
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
**********************************************************************************************************/
void DebugMonitor(void)
{
}

/**********************************************************************************************************
* Function Name : PendSV_Handler
* Description   : This function is used for handling PendSVC exception.
* Arguments     : None
* Returns       : None
* Notes         : 	Pendable request for system service
*					Type of priority: settable
*					Priority		: 5
*					Address			: 0x0000_0038
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
**********************************************************************************************************/
void PendSVC(void)
{
}

/**********************************************************************************************************
* Function Name : SysTick_Handler
* Description   : This function is used for handling SysTick Handler.
* Arguments     : None
* Returns       : None
* Notes         : 	Pendable request for system service
*					Type of priority: settable
*					Priority		: 6
*					Address			: 0x0000_003C
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
**********************************************************************************************************/
void SysTick_Handler(void)
{
	PinSet(TP4,1);

	Decrement_TimingDelay();

	uart_timer_control();

	app_UI_irq();
	app_touchscreen_irq();
	app_button_irq();
	api_Stopwatch_IRQ();
	api_LED_irq();
	app_selfcheck_1ms_irq();
	app_PP_1ms_IRQ();
	
	if(++tacho_tmr	>  2000){
		tacho_tmr	=  0;
		tacho		=  0;
	}
	
	csp_paracube_timer();
	
	PinSet(TP4,0);
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
}

/**********************************************************************************************************
* Function Name : RTC_IRQHandler
* Description   : This function is used for handling RTC interrupts
* Arguments     : None
* Returns       : None
* Notes         :
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    18/11/2010      Philip Gillespie     Original Created
**********************************************************************************************************/

void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET){
		RTC_ClearITPendingBit(RTC_IT_SEC);			/* Clear the RTC Second interrupt */
	}


	return;
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line0);
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line1);
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line2);											// Clear the Interrupt
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line3);

}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line4);
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel2_IRQHandler
* Description    : This function handles DMA1 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel3_IRQHandler
* Description    : This function handles DMA1 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel4_IRQHandler
* Description    : This function handles DMA1 Channel 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel5_IRQHandler
* Description    : This function handles DMA1 Channel 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SCE_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define		TACHO_N		15
float		tacho_buf[TACHO_N];
uint8_t		tacho_i			=  0;

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line5) != RESET){
		// Local Variables
		uint32_t	us_read;
		uint32_t	tacho_calc;
		uint32_t	tacho_last;
		float		sort_buf[TACHO_N];
		float		temp_f;
		uint8_t		i;
		uint8_t		j;
	
		// Code
		us_read				=  TIM3->CNT;		// 10kHz timer
		tacho_last			=  tacho_rd;
		tacho_calc			=  tmr3_ms;
		tacho_calc			*= 100;
		tacho_calc			+= us_read;
		tacho_calc			-= tacho_last;
	
		tmr3_ms				=  0;
		tacho_rd			=  us_read;
	
		tacho_buf[tacho_i]	=  3600000.0;
		tacho_buf[tacho_i]	/= (float)tacho_calc;
	
		if(++tacho_i	>= TACHO_N){	tacho_i	=  0;	}
	
		memcpy(sort_buf, tacho_buf, 4 * TACHO_N);
		for(i=0;i<TACHO_N;i++){
			for(j=i+1;j<TACHO_N;j++){
				if(sort_buf[i]	>  sort_buf[j]){
					temp_f		=  sort_buf[i];
					sort_buf[i]	=  sort_buf[j];
					sort_buf[j]	=  temp_f;
				}
			}
		}
	
		tacho				=  sort_buf[(TACHO_N + 1) / 2];
	
		tacho_flag			=  1;
		tacho_tmr			=  0;
		
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	
//	if(EXTI_GetFlagStatus(EXTI_Line6) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line6);
//	}

//	if(EXTI_GetFlagStatus(EXTI_Line7) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line7);
//	}

//	if(EXTI_GetFlagStatus(EXTI_Line8) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line8);
//	}

//	if(EXTI_GetFlagStatus(EXTI_Line9) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line9);
//	}

}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
}

/**********************************************************************************************************
* Function Name : TIM1_UP_IRQHandler
* Description   : This function is used for handling TIM1 overflow and update interrupt
* Arguments     : None
* Returns       : None
* Notes         : 	TIM1 Update interrupt
*					Type of priority: settable
*					Priority		: 32
*					Address			: 0x0000_00A4
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
* 1.0.1			18/11/2010		Philip Gillespie	Added pre-compile
**********************************************************************************************************/
#if(	TIMER1_INT_EN == 1)
#warning TIM1_UP_IRQHandler is enabled
//void TIM1_UP_TIM10_IRQHandler(void)
void TIM1_UP_IRQHandler(void)			//HD
{
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET){
  		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		api_touch_handler_IRQ();
	}


}
#endif

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{

}



/**********************************************************************************************************
* Function Name : TIM2_IRQHandler
* Description   : This function is
* Arguments     : None
* Returns       : None
* Notes         : Type of priority : settable
				  Priority		   : 35
			      Address		   : 0x0000_00B0
*
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    18/11/2010      Philip Gillespie     Original Created
* 1.0.1			18/11/2010		Philip Gillespie	Added pre-compile
**********************************************************************************************************/
#if(	TIMER2_INT_EN == 1)
#warning TIM2_IRQHandler is enabled

void TIM2_IRQHandler(void)
{
	PinSet(TP3,1);

	/* Clear the TIM2 Update pending bit */
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	api_audio_Timer_IRQ();

	PinSet(TP3,0);
	return;
}
#endif

/**********************************************************************************************************
* Function Name : TIM3_IRQHandler
* Description   : This function is
* Arguments     : None
* Returns       : None
* Notes         : Type of priority : settable
*				  Priority		   : 36
*				  Address		   : 0x0000_00B4
*
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    18/11/2010      Philip Gillespie     Original Created
* 1.0.1			18/11/2010		Philip Gillespie	Added pre-compile
**********************************************************************************************************/
#if(	TIMER3_INT_EN == 1)
#warning TIM3_IRQHandler is enabled

void TIM3_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	/* Clear the TIM3 Update pending bit */
	tmr3_ms++; //SS Fan 241019
	
	return;    //SS Fan 241019
}
#endif

/**********************************************************************************************************
* Function Name : TIM4_IRQHandler
* Description   : This function is
* Arguments     : None
* Returns       : None
* Notes         : Type of priority : settable
*				  Priority		   : 37
*				  Address		   : 0x0000_00B8
*
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    18/11/2010      Philip Gillespie     Original Created
* 1.0.1			18/11/2010		Philip Gillespie	Added pre-compile
**********************************************************************************************************/
#if(	TIMER4_INT_EN == 1)
#warning TIM4_IRQHandler is enabled

void TIM4_IRQHandler(void)
{
/* Code */
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	/* Clear the TIM4 Update pending bit */
	app_pneumatics_ctrl_irq();
}

#endif

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
}

/**********************************************************************************************************
* Function Name : USART1_IRQHandler
* Description   : This function is used for handling UART1 interrupts
* Arguments     : None
* Returns       : None
* Notes         : 	RM0008 - Page 697
*					USART1 global interrupt
*					Type of priority: settable
*					Priority		: 44
*					Address			: 0x0000_00D4
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
* 1.0.1			18/11/2010		Philip Gillespie	Added pre-compile
**********************************************************************************************************/
#if(	UART1_EN == 1)
#warning _USART_1_IRQ_EN is enabled

void USART1_IRQHandler(void)
{
	uart1_IRQ();
	return;
}
#endif

/**********************************************************************************************************
* Function Name : USART2_IRQHandler
* Description   : This function is used for handling UART2 interrupts
* Arguments     : None
* Returns       : None
* Notes         : 	RM0008 - Page 697
*					USART2 global interrupt
*					Type of priority: settable
*					Priority		: 45
*					Address			: 0x0000_00D8
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
* 1.0.1			03/11/2010		Philip Gillespie	Added Flow Control
* 1.0.2			18/11/2010		Philip Gillespie	Added pre-compile
* 1.0.3			02/04/2012		Pauric Lynch		Added uart2 IRQ Function
													Wrong precompile added.
**********************************************************************************************************/
#if( UART2_EN == 1)
#warning _USART_2_IRQ_EN is enabled

void USART2_IRQHandler(void)
{
	uart2_IRQ();

}
#endif

/**********************************************************************************************************
* Function Name : USART3_IRQHandler
* Description   : This function is used for handling UART3 interrupts
* Arguments     : None
* Returns       : None
* Notes         : 	RM0008 - Page 697
*					USART2 global interrupt
*					Type of priority: settable
*					Priority		: 46
*					Address			: 0x0000_00DC
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    13/08/2010      Stephen Serplus     Original Created
* 1.0.1			18/11/2010		Philip Gillespie	Added pre-compile
**********************************************************************************************************/
#if(	UART3_EN == 1)
#warning _USART_3_IRQ_EN is enabled

void USART3_IRQHandler(void)
{
	uart3_IRQ();
    return;

}
#endif

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/



//	if(EXTI_GetFlagStatus(EXTI_Line10) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line10);
//	}

//	if(EXTI_GetFlagStatus(EXTI_Line11) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line11);
//	}

//	if(EXTI_GetFlagStatus(EXTI_Line12) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line12);
//	}

//	if(EXTI_GetFlagStatus(EXTI_Line13) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line13);
//	}

//	if(EXTI_GetFlagStatus(EXTI_Line14) != RESET){
//		EXTI_ClearITPendingBit(EXTI_Line14);
//	}

//	if(EXTI_GetFlagStatus(EXTI_Line15) != RESET){
	//EXTI_ClearITPendingBit(EXTI_Line15);




/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET){
  		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);


	}

	return;
}

/*******************************************************************************
* Function Name  : TIM8_TRG_COM_IRQHandler
* Description    : This function handles TIM8 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_CC_IRQHandler
* Description    : This function handles TIM8 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_CC_IRQHandler(void)
{

  return;
}

/*******************************************************************************
* Function Name  : ADC3_IRQHandler
* Description    : This function handles ADC3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : FSMC_IRQHandler
* Description    : This function handles FSMC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void)
{
}

/**********************************************************************************************************
* Function Name : UART4_IRQHandler
* Description   : This function is used for handling UART4 interrupts
* Arguments     : None
* Returns       : None
* Notes         :
*					USART4 global interrupt
*					Type of priority: settable
*					Priority		: 59
*					Address			: 0x0000_0110
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    18/11/2010      Philip Gillespie     Original Created
**********************************************************************************************************/
#if(	UART4_EN == 1)
#warning _UART_4_IRQ_EN is enabled

void UART4_IRQHandler(void)
{
	uart4_IRQ();

}

#endif

/**********************************************************************************************************
* Function Name : UART5_IRQHandler
* Description   : This function is used for handling UART5 interrupts
* Arguments     : None
* Returns       : None
* Notes         :
*					USART4 global interrupt
*					Type of priority: settable
*					Priority		: 60
*					Address			: 0x0000_0114
*
* Version		Date d/m/y	    Programmer          Reason for Change
* 1.0.0		    18/11/2010      Philip Gillespie     Original Created
**********************************************************************************************************/
#if(	UART5_EN == 1)
#warning _UART_5_IRQ_EN is enabled

void UART5_IRQHandler(void)
{
	uart5_IRQ();

}

#endif

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : This function handles DMA2 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : This function handles DMA2 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : This function handles DMA2 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel4_5_IRQHandler
* Description    : This function handles DMA2 Channel 4 and DMA2 Channel 5
*                  interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel4_5_IRQHandler(void)
{
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
