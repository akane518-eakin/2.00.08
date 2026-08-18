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
 * Filename    :  S25FL0xx.h
 * Date Created:  Mon 11 Sep 2017 10:34:36 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _S25FL0xx_H
#define _S25FL0xx_H

/*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************/
#include "stdint.h"


/*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************/
#define SECTOR_SIZE				0xFFFF

#define	DUMMY					0x55

/* Instructions */
/*      Command        			Value      N Description             Addr Dummy  Data */
#define WREN      				0x06    // 1 Write Enable              0   0     0
#define WRDI      				0x04    // 1 Write Disable             0   0     0
#define RDID      				0x9f    // 1 Read Identification       0   0     1-3
#define RDSR      				0x05    // 1 Read Status Register      0   0     >=1
#define EWSR      				0x50    // 1 Write enable status       0   0     0
#define WRSR      				0x01    // 1 Write Status Register     0   0     1
#define READ      				0x03    // 1 Read Data Bytes           3   0     >=1
#define FAST_READ 				0x0b    // 1 Higher speed read         3   1     >=1
#define PAGE_PROGRAM       	 	0x02    // 1 Page Program              3   0     1-256
#define SECTOR_ERASE        	0x20    // 1 Sector Erase              3   0     0
#define BULK_ERASE_32      		0x52    // 2 32K Block Erase           3   0     0
#define BULK_ERASE_64      		0xD8    // 2 64K Block Erase           3   0     0
#define BULK_ERASE 				0xc7    // 1 Bulk Erase                0   0     0
#define RES       				0xab    // 1 Read Electronic Signature 0   3     >=1

#define	S25FL_RSTEN				0x66
#define	S25FL_RST				0x99

/* Status Register Bits */
#define SR_BUSY					0x01
#define SR_WEL					0X02
#define SR_BP0					0x04
#define SR_BP1					0x08
#define SR_BP2					0x10
#define SR_BP3					0x20
#define SR_SEC					0x40
#define SR_BPL					0x80

/*********************************************************************************************************
*                                           VARIABLES
*********************************************************************************************************/


/*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************/
void 	S25FL0xx_spi_fun_sel(uint8_t(*spi_funct_ptr)(uint8_t), void (*chip_select_ptr)(uint8_t));
void 	S25FL0xx_Config(void);

void 	S25FL0xx_BulkErase(uint8_t chip_cs);

void 	S25FL0xx_WriteEnable(uint8_t chip_cs);
void 	S25FL0xx_WriteDisable(uint8_t chip_cs);
void S25FL0xx_SWReset(uint8_t chip_cs);


void 	S25FL0xx_EnableAllRegions(uint8_t chip_cs);
uint8_t S25FL0xx_WaitWriteComplete(uint8_t chip_cs, uint32_t max_ms);
uint8_t	S25FL0xx_ReadStatusRegister(uint8_t chip_cs);
void 	S25FL0xx_4KBSectorErase(uint8_t chip_cs,uint32_t addr);
void 	S25FL0xx_64KBSectorErase(uint8_t chip_cs,uint32_t addr);
void 	S25FL0xx_ReadAndPrint(uint8_t chip_cs,uint32_t addr, uint32_t nbytes);
void 	S25FL0xx_Read(uint8_t chip_cs,uint32_t addr, uint8_t *data_ptr, uint32_t nbytes);
void 	S25FL0xx_WriteBytes(uint8_t chip_cs,uint32_t addr, uint8_t *data_ptr, uint32_t nbytes);
void 	S25FL0xx_EraseMultipleSectors(uint8_t chip_cs,uint32_t start_addr, uint8_t num_of_sectors);


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
