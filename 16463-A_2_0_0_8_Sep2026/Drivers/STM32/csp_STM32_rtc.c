/**********************************************************************************************************
 *                                           Marturion Ltd
 *
 *                                           Knockmore Hill Business Park
 *                                           9 Ferguson Drive
 *                                           Lisburn
 *                                           Co. Antrim
 *                                           Northern Ireland
 *                                           BT28 2EX
 *
 *
 *                  Copyright 2010, Marturion Ltd, Lisburn, Co. Antrim, Northern Ireland
 *                                          All Rights Reserved
 *
 *
 *
 * Filename    :  csp_STM32_rtc.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the adc functions.
 * Compiler    :  GNU GCC
 * Target      :  STM32103
 * Version     :  Version 1.0.0
 **********************************************************************************************************/

/**********************************************************************************************************
 *                                           INCLUDE FILES
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_bkp.h"

#include "csp_STM32_rtc.h"

#define 	RTCClockOutput_Enable	0

time_t	startup_RTC_time;
uint8_t	startup_RTC_status	=  0xff;		// 0	= Good
											// 1	= Backup regs not set, initialise
											// 2	= backup reg set, sync err -reinitialised
											// 0xff = na


/**********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************/


/**********************************************************************************************************
 * Function Name : STM32_internal_RTC_config
 * Description   : This function is used to initalise the RTC
 * Arguments     : uint8_t source	either 	RTC_SRC_EXT32768
 *									or		RTC_SRC_INT40000
 * Returns       : None
 * Notes         :  RTC_SRC_EXT32768	1
 *					RTC_SRC_INT40000	2
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		2/3/2011        William Paul       	Original Created
 * 1.0.1		30/3/2012       Aaron Duignan       Updated for alarms and LSE clock for 32kHZ
 **********************************************************************************************************/
void STM32_internal_RTC_config(uint8_t source)
{

    NVIC_Configuration();					//	 NVIC configuration

   	startup_RTC_time	=  csp_STM32_RTC_rd();

	/* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    if (BKP_ReadBackupRegister(BKP_DR1) != BK_REG_RTC_CONFIGED){

        RTC_Configuration(source);		//RTC Configuration
		startup_RTC_status	=  1;
        BKP_WriteBackupRegister(BKP_DR1, BK_REG_RTC_CONFIGED);
    }
    else{
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET){
//            printf("\r\n\n Power On Reset occurred....");
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET){
//            printf("\r\n\n External Reset occurred....");
        }

		startup_RTC_status		=  0;

        if( RTC_WaitForSynchro() ){				// Wait for RTC registers synchronization
        	RTC_Configuration(source);				// RTC Configuration
        	startup_RTC_status	=  2;
	        BKP_WriteBackupRegister(BKP_DR1, BK_REG_RTC_CONFIGED);
        }
        else{

        }
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);	// Enable the RTC Second
        RTC_WaitForLastTask();				//Wait until last write operation on RTC registers has finished
    }

#if(RTCClockOutput_Enable == 1)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);		// Enable PWR and BKP clocks
    PWR_BackupAccessCmd(ENABLE);							// Allow access to BKP Domain

	// To output RTCCLK/64 on Tamper pin, the tamper functionality must be disabled
    BKP_TamperPinCmd(DISABLE);								// Disable the Tamper Pin
    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);	//Enable RTC Clock Output on Tamper Pin
#endif

//    RCC_ClearFlag();	// Clear reset flags
}





/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(uint8_t source)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

	if(source == RTC_SRC_EXT32768){
    	/* Enable LSE */
    	RCC_LSEConfig(RCC_LSE_ON);
    	/* Wait till LSE is ready */
    	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    	{}
    	/* Select LSE as RTC Clock Source */
    	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    }
    if(source == RTC_SRC_INT40000){
    	RCC_LSICmd(ENABLE);
	   	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    	{}
    	/* Select LSI as RTC Clock Source */
    	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    }

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
//    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

	if(source == RTC_SRC_EXT32768){
    	/* Set RTC prescaler: set RTC period to 1sec */
    	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
    }
	if(source == RTC_SRC_INT40000){
		/* Set RTC prescaler: set RTC period to 1sec */
    	RTC_SetPrescaler(39999); /* RTC period = RTCCLK/RTC_PR = (40 KHz)/(39999+1) */
	}
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

/**********************************************************************************************************
 * Function Name : csp_STM32_RTC_wr
 * Description   : This function is used to initalise the RTC
 * Arguments     : time_t 	value_u32
 * Returns       : None
 * Notes         : This function:
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    2/3/2011      William Paul       Original Created
 **********************************************************************************************************/
void csp_STM32_RTC_wr(time_t value_u32)
{
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	/* Change the current time */
	RTC_SetCounter((uint32_t)value_u32);
	/* Wait until last write operation on RTC registers has finished */
	//RTC_WaitForLastTask();

	return;
}


/**********************************************************************************************************
 * Function Name : csp_STM32_RTC_rd
 * Description   : This function is used to initalise the RTC
 * Arguments     : void
 * Returns       : uint32_t 	value_u32
 * Notes         : This function:
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    2/3/2011      William Paul       Original Created
 **********************************************************************************************************/
time_t csp_STM32_RTC_rd(void)
{
	uint32_t	value_u32;
	value_u32	=  RTC_GetCounter();

	return((time_t)value_u32);
}

/**********************************************************************************************************
 * Function Name : csp_STM32_RTC_time
 * Description   : This function is used to initalise the RTC
 * Arguments     : void
 * Returns       : uint32_t 	value_u32
 * Notes         : This function:
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    2/3/2011      William Paul       Original Created
 **********************************************************************************************************/
time_t csp_STM32_RTC_time(time_t *t_p)
{
	if(t_p != 0){
		csp_STM32_RTC_wr(*t_p);
	}
	return( csp_STM32_RTC_rd() );
}


/**********************************************************************************************************
 * Function Name : csp_ALARM_wr
 * Description   : This function is used to initalise the RTC
 * Arguments     : uint32_t 	value_u32
 * Returns       : None
 * Notes         : This function:
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		    2/3/2011      William Paul       Original Created
 **********************************************************************************************************/
void csp_ALARM_wr(time_t value_u32)
{
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	/* Change the current time */
	RTC_SetAlarm((uint32_t)value_u32);
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	return;
}

/**********************************************************************************************************
 * Function Name : RTC_NVIC_Configuration
 * Description   : This function is used to configure the alamr interrupts
 * Arguments     : None
 * Returns       : None
 * Notes         : This function:
 *
 * Version		Date d/m/y	    Programmer          Reason for Change
 * 1.0.0		30/3/2012       Aaron Duignan       Original Created
 **********************************************************************************************************/
void RTC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  //EXTI_InitTypeDef EXTI_InitStructure;

  EXTI_DeInit();

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}




/**********************************************************************************************************
**********************************************************************************************************/
//end of file
