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
 *  Copyright 2018, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  csp_STM32_dma.h
 * Date Created:  Mon 19 Nov 2018 02:20:50 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _CSP_DMA_H
#define _CSP_DMA_H



/*
*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************
*/
#define DAC_DHR12R1_Address			0x40007408		//12bit DAC1	right aligned
#define DAC_DHR12L1_Address			0x4000740C		//12bit DAC1	left  aligned
#define DAC_DHR8R1_Address			0x40007410		//8bit	DAC1	right aligned
#define DAC_DHR12R2_Address			0x40007414		//12bit DAC2	right aligned
#define DAC_DHR12L2_Address			0x40007418		//12bit DAC2	left  aligned
#define DAC_DHR8R2_Address			0x4000741c		//8bit	DAC2	right aligned
#define DAC_DHR12RD_Address			0x40007420		//12bit	DAC1&2	right aligned
#define DAC_DHR12LD_Address			0x40007424		//12bit	DAC1&2	left  aligned
#define DAC_DHR8RD_Address			0x40007428		//8bit	DAC1&2	right aligned


void csp_STM32_DMA2_CH1_config_m2m(uint32_t Sadd, uint32_t Dadd,uint32_t n);

void csp_STM32_DMA2_CH3_config(uint32_t buf_address, uint32_t buf_len);
void csp_STM32_DMA2_CH3_deconfig(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
