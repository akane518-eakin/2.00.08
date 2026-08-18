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
 *                                          csp_dac.h
 *
 * Filename    :  csp_dac.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the adc functions.
 * Compiler    :  GNU GCC
 * Target      :  STM32103
 * Version     :  Version 1.1.0
 *********************************************************************************************************/
#ifndef _CSP_DAC_H
#define _CSP_DAC_H



/**********************************************************************************************************
 *                                           DEFINES
 *********************************************************************************************************/
#define	_DAC1_EN
#define	USE_DAC_OUTPUT



/**********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *********************************************************************************************************/
void init_DAC1(void);
void csp_DAC1_write(uint16_t dac_value);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
