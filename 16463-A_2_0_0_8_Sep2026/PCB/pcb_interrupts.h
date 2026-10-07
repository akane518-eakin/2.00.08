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
 * Filename    :  pcb_interrupts.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/
#ifndef _PCB_INTERRUPTS_H
#define _PCB_INTERRUPTS_H


#define EXT_INT0_EN			0		//Pressure ready
#define EXT_INT1_EN			0
#define EXT_INT2_EN			0
#define EXT_INT3_EN			0
#define EXT_INT4_EN			0

#define EXT_INT5_EN			1
#define EXT_INT6_EN			0
#define EXT_INT7_EN			0
#define EXT_INT8_EN			0
#define EXT_INT9_EN			0

#define EXT_INT10_EN		0
#define EXT_INT11_EN		0
#define EXT_INT12_EN		0
#define EXT_INT13_EN		0
#define EXT_INT14_EN		0
#define EXT_INT15_EN		0  //  For fan speed

#define EXT_INT0_PORT		GPIO_PortSourceGPIOG
#define EXT_INT1_PORT		GPIO_PortSourceGPIOB
#define EXT_INT2_PORT		GPIO_PortSourceGPIOB
#define EXT_INT3_PORT		GPIO_PortSourceGPIOB
#define EXT_INT4_PORT		GPIO_PortSourceGPIOE
#define EXT_INT5_PORT		GPIO_PortSourceGPIOC //b
#define EXT_INT6_PORT		GPIO_PortSourceGPIOB
#define EXT_INT7_PORT		GPIO_PortSourceGPIOB
#define EXT_INT8_PORT		GPIO_PortSourceGPIOC
#define EXT_INT9_PORT		GPIO_PortSourceGPIOB
#define EXT_INT10_PORT		GPIO_PortSourceGPIOB
#define EXT_INT11_PORT		GPIO_PortSourceGPIOB
#define EXT_INT12_PORT		GPIO_PortSourceGPIOB
#define EXT_INT13_PORT		GPIO_PortSourceGPIOB
#define EXT_INT14_PORT		GPIO_PortSourceGPIOB
#define EXT_INT15_PORT		GPIO_PortSourceGPIOB 

#define EXT_INT0_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT1_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT2_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT3_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT4_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT5_TRIGGER	EXTI_Trigger_Falling     //EXTI_Trigger_Rising 
#define EXT_INT6_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT7_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT8_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT9_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT10_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT11_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT12_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT13_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT14_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising
#define EXT_INT15_TRIGGER	EXTI_Trigger_Falling 	//EXTI_Trigger_Rising

#define		EXTI0_IRQ_PREM_PRI			1;
#define		EXTI0_IRQ_SUB_PRI			1;

#define		EXTI1_IRQ_PREM_PRI			1;
#define		EXTI1_IRQ_SUB_PRI			1;

#define		EXTI2_IRQ_PREM_PRI			1;
#define		EXTI2_IRQ_SUB_PRI			1;

#define		EXTI3_IRQ_PREM_PRI			1;
#define		EXTI3_IRQ_SUB_PRI			1;

#define		EXTI4_IRQ_PREM_PRI			1;
#define		EXTI4_IRQ_SUB_PRI			1;

#define		EXTI9_5_IRQ_PREM_PRI		6;
#define		EXTI9_5_IRQ_SUB_PRI			0;

#define		EXTI15_10_IRQ_PREM_PRI		1;
#define		EXTI15_10_IRQ_SUB_PRI		1;




#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file

