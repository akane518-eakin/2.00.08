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
 * Filename    :  csp_S25FL0xx.h
 * Date Created:  Tue 04 Apr 2017 09:44:38 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _csp_S25FL0xx_H
#define _csp_S25FL0xx_H


#include "pcb_mem_map_flash.h"

/*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************/
#define S25FL_PAGE_SIZE				0x0100
#define S25FL_4KB_SECTOR_SIZE			0x1000

#define	DONT_CORRECT	0
#define	CORRECT			1

/*********************************************************************************************************
*                                           VARIABLES
*********************************************************************************************************/


/*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************/
void csp_mem_config(void);
void csp_mem_deconfig(void);

void csp_mem_rd( uint8_t *data_ptr, uint32_t addr, uint32_t nbytes);
void csp_mem_wr( uint8_t *data_ptr, uint32_t addr, uint32_t nbytes);

void csp_sys_mem_wr(uint8_t *data_ptr, uint32_t addr, uint32_t nbytes);

void csp_erase_4kb_sectors(uint32_t addr);
void csp_erase_64kb_sectors(uint32_t addr, uint8_t nSectors);

void csp_mem_dump(uint32_t addr, uint16_t nPages);

uint8_t csp_S25FL0xx_chip_select(uint32_t *address,uint8_t correct);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
