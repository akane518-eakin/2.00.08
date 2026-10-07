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
 * Filename    :  csp_STM32_I2Cx.h
 * Date Created:  Wed 04 Oct 2017 03:33:42 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _CSP_STM32_I2CX_H
#define _CSP_STM32_I2CX_H

#define	USE_F1

#if defined(USE_F1)
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#elif defined(USE_F2)
#include "stm32f2xx.h"
#include "stm32f2xx_i2c.h"
#elif defined(USE_F4)
#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#else
#warning No Standard Peripheral Drivers Selected
#endif

/**********************************************************************************************************
 **********************************************************************************************************/
#define CR1_POS_Set           ((uint16_t)0x0800)
#define CR1_POS_Reset         ((uint16_t)0xF7FF)

typedef enum{
    Error	= 0,
    Success = !Error,
}Status;


void I2C1_init(void);
void I2C2_init(void);

Status I2C_BufferRd(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead,  uint8_t SlaveAdd_8bit);
Status I2C_BufferWr(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAdd_8bit);
Status I2C_BufferWrRd(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite,uint32_t NumByteToRead, uint8_t SlaveAdd_8bit );

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
