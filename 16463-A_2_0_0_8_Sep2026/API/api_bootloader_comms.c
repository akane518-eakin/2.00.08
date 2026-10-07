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
 * Filename    :  api_bootloader_comms.c
 * Date Created:  Fri 05 May 2017 10:09:48 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "glb_typedefs.h"

#include	"app_system.h"

#include "api_bootloader_comms.h"
#include "api_bootloader.h"

#include "csp_STM32_uart.h"

#include "csp_STM32_delay.h"
#include "csp_STM32_uart1.h"
#include "stm32f10x_tim.h"

//#include "csp_STM32_iwdg.h"

/**********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES
**********************************************************************************************************/
uint8_t 	(*boot_getchar)(	uint16_t wait_time_u16,	uint8_t *rec_status_u8);
void 		(*boot_putchar)(	uint8_t tx_char_u8);


uint8_t 	api_bootloader_packet_rx(void);
void 		api_bootloader_packet_tx(uint16_t cmd);

void        api_bootloader_app_buffer_flush(void);
uint16_t 	app_Comms_Crc16(char* buffer, uint16_t length);
/**********************************************************************************************************
*                                           LOCAL VARIABLES
**********************************************************************************************************/
packet_t	packet_rx;
packet_t	packet_tx;
uint32_t	nextAddress;

uint8_t		download_res		= 0xff;
/**********************************************************************************************************
**********************************************************************************************************/


/*************************************************************************************************
* Function Name :	api_bootloader_fun_sel
* Description   : 	This Function mapps the comms port to the suite of functions
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	07/01/16		W. Paul			Created
*************************************************************************************************/
void api_bootloader_fun_sel(		uint8_t 	(*getchar_ptr)(	uint16_t wait_time_u16,	uint8_t *rec_status_u8),
							void 	(*putchar_ptr)(	uint8_t tx_char_u8)
                       	 )
{
	boot_getchar		=  getchar_ptr;
	boot_putchar		=  putchar_ptr;
	return;
}

/**********************************************************************************************************
* Function Name : api_bootloader_serial_port_handler
* Description   : This function looks for valid packet in the serial port.
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date d/m/Y	    	Programmer          Reason for Change
* 1.0.0		 	30/09/15	    	Pauric Lynch        Original Created
**********************************************************************************************************/
void api_bootloader_serial_port_handler(uint8_t *debug_mode)
{
	static uint8_t		init		=  0;
	uint16_t			pck_found	=  0;
	static uint32_t		tic_tmr		=  0;
    static uint32_t		tic_repeat_req		=  0;
    static uint8_t      spin        =  0;

	if(init == 0){
		init	=  1;
		memset((void*)packet_rx.raw, 0, sizeof(packet_rx));
		tic_tmr		=  sys_tic_rd() + 1000;
	}

    if(tic_repeat_req != 0){
        if(sys_tic_rd() > tic_repeat_req){
            if(spin > 4){spin = 0;}
            if(spin == 1){
                api_bootloader_app_buffer_flush();
                spin++;
                tic_repeat_req  =  sys_tic_rd() + 1000;
            }
            else{
                api_bootloader_packet_tx(LOAD_FIRMWARE_REQ_NEXT);
                tic_repeat_req  =  sys_tic_rd() + 1000;

            }
        }
    }


	if(*debug_mode == 0){	//0= packet mode, 1= hyperterminal menu

		pck_found	=  api_bootloader_packet_rx();

	    if(pck_found == 1){	//packet received and verified
			switch(packet_rx.format.header.cmd.a){
				case LOAD_FIRMWARE_ECHO:
					api_bootloader_packet_tx(LOAD_FIRMWARE_READY);
					break;
				case LOAD_FIRMWARE_START:
					nextAddress		=  0;
					BootSettings.NewCode.firmwareSize	=  packet_rx.format.data.cmd0003.file_size.a;
					api_bootloader_prepare_ext_flash(FLASH_ADDR_NEW_FIRMWARE);
					api_bootloader_packet_tx(LOAD_FIRMWARE_REQ_NEXT);
					break;
				case LOAD_FIRMWARE_CREATE_CPY:
					api_bootloader_create_safecode(FLASH_ADDR_OLD_COPY_FIRMWARE);
					api_bootloader_packet_tx(LOAD_FIRMWARE_READY);
					break;
				case LOAD_FIRMWARE_CREATE_FACTORY:
					api_bootloader_create_safecode(FLASH_ADDR_FACTORY_FIRMWARE);
					api_bootloader_packet_tx(LOAD_FIRMWARE_READY);
					break;
				case LOAD_FIRMWARE_DATA:
					api_bootloader_packet_InOut(	packet_rx.format.data.cmd0001.start_add.a,
													packet_rx.format.data.cmd0001.data,
													packet_rx.format.header.d_len.a - 4,
													&nextAddress
												);
					if(	nextAddress >= BootSettings.NewCode.firmwareSize){
						//download is complete
						download_res	=  api_bootloaderUpdateSettings();
						api_bootloader_packet_tx(LOAD_FIRMWARE_RESULT);
                        tic_repeat_req  =  0;
					}
					else{
						//request next packet
						api_bootloader_packet_tx(LOAD_FIRMWARE_REQ_NEXT);
                        tic_repeat_req  =  sys_tic_rd() + 500;
                        spin    =  0;
					}
					break;
				case LOAD_FIRMWARE_BOOT_TO_NEW:
					api_bootloader_FlashNewCode(BOOTLOAD_NEWCODE);
					//no ack because sys reset and bootloader should perform upgrade
					break;
				case LOAD_FIRMWARE_BOOT_TO_LAST:
					api_bootloader_FlashNewCode(BOOTLOAD_OLDCODE);
					//no ack because sys reset and bootloader should perform upgrade
					break;
				case LOAD_FIRMWARE_BOOT_TO_FACTORY:
					api_bootloader_FlashNewCode(BOOTLOAD_SAFEFACTORYCODE);
					//no ack because sys reset and bootloader should perform upgrade
					break;

				case LOAD_FIRMWARE_INFO_REQ:
					api_bootloader_packet_tx(LOAD_FIRMWARE_INFO);
					break;
                case LOAD_FIRMWARE_CUR_VERSION_REQ:
   					api_bootloader_packet_tx(LOAD_FIRMWARE_CUR_VERSION);
                    break;
			//	default:

	        }
	    }

		if(pck_found == 2){	//user character received	'ESC' then character
			uart_debug_fun_sel(UART_PRINTF_PORT);
			tic_tmr		=  sys_tic_rd() + 1000;		//switch off print after 1s

			switch(packet_rx.raw[1]){
				case ' ':
					printf("\r\n");
					printf("\r\n***********************************");
					printf("\r\nBootloading menu");
					printf("\r\nS   - safecopy");
					printf("\r\nF   - Factorycopy");
					printf("\r\nU   - upgrade to new code");
					printf("\r\nD   - upgrade to factory code");
					printf("\r\nL   - upgrade to saved copy");
					printf("\r\nI   - info");
					printf("\r\nEsc - Debug Menu");
					printf("\r\n");
					break;
				case 0x1b:	*debug_mode	=  1;
								printf("\r\nDebug Comms Mode");						break;
				case 'S':	api_bootloader_create_safecode(FLASH_ADDR_OLD_COPY_FIRMWARE);
								printf("\r\nSafe copy created");					break;
				case 'F':	api_bootloader_create_safecode(FLASH_ADDR_FACTORY_FIRMWARE);
								printf("\r\nFactory copy created");					break;
				case 'U':	api_bootloader_FlashNewCode(BOOTLOAD_NEWCODE);			break;
				case 'D':	api_bootloader_FlashNewCode(BOOTLOAD_SAFEFACTORYCODE);	break;
				case 'L':	api_bootloader_FlashNewCode(BOOTLOAD_OLDCODE);			break;
				case 'I':	api_bootloader_settings_print();						break;

			}
		}

		if(sys_tic_rd() > tic_tmr){
			tic_tmr	= 0xffffffff;
			uart_debug_fun_sel(DUMP_PRINTF_PORT);	//stop outputting all debug printf
		}
	}
	return;
}



/*************************************************************************************************
* Function Name : 	api_bootloader_packet_rx
* Description   : 	This Function receives packets in the correct format and reports a complete packet received
* Arguments     : 	void
* Returns       : 	uint8_t pkt_found		0-None
*											1-Packet in correct format
*											2-User typed 'Esc' +1character
* Notes         :
* Packet format
*  Header  ------------------------------------------------
*	start of packet identifier		STX (0x02)
*	command byte					1 byte
*	payload length					2 bytes
*	header checksum					1 byte 		= sum of 1st 4 bytes in the header
*	end of payload identifier		0x1F
*  Payload ------------------------------------------------
*	data							x bytes (Max 270)
*  Footer  ------------------------------------------------
*	16bit crc checksum				2bytes
*	end of packet identifier		0x03
*  End     ------------------------------------------------
*  Additional tx bytres - not needed for rx
*	new line bytes					0x0d
*									0x0a
*          ------------------------------------------------
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/05/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_bootloader_packet_rx(void)
{
	uint8_t				rec_char_u8;
	uint8_t				rec_status_u8;
	static uint16_t		str_ptr			=  0;
	uint8_t				pkt_found		=  0;
	uint8_t				h_cs;		//header checksum
	uint16_t			p_len;		//packet length
	U2B_t				p_cs;		//packet checksum
	U2B_t				cal_cs;		//calculated checksum

 	rec_char_u8 = boot_getchar(0,&rec_status_u8);

	if(rec_status_u8){
		do{

			packet_rx.raw[str_ptr++] = rec_char_u8;

			if(packet_rx.format.header.stx == 0x02){
				if(str_ptr >= sizeof(header_t)){
					//if we get this far the header has been received
					if(packet_rx.format.header.eoh != 0x1f){
						str_ptr			=  0;
					}
					h_cs	=  packet_rx.format.header.stx;
					h_cs	+= packet_rx.format.header.cmd.b[0];
	                h_cs	+= packet_rx.format.header.cmd.b[1];
					h_cs	+= packet_rx.format.header.d_len.b[0];
					h_cs	+= packet_rx.format.header.d_len.b[1];
					if(packet_rx.format.header.h_cs != h_cs){
						str_ptr			=  0;
					}
					//if we get this far the header has been verified
					else{
					  	p_len	=  sizeof(header_t);
						p_len	+= packet_rx.format.header.d_len.a;
						p_len	+= 3;
						if(str_ptr == p_len){
							//if we get this far the whole packet has been received
							if(packet_rx.raw[p_len-1] == 0x03){		 //check for endchar
								p_cs.b[0]		=  packet_rx.raw[p_len-3];
								p_cs.b[1]		=  packet_rx.raw[p_len-2];

								cal_cs.a		=  app_Comms_Crc16(&packet_rx.raw[0],p_len-3);
								if(cal_cs.a == p_cs.a){
									pkt_found 		=  1;
									str_ptr			=  0;
									return(pkt_found);
									//packet has been verified
								}
								else{
								 	printf("\r\nPacket CRC Err - Cal %4x Rec %4x",cal_cs.a,p_cs.a );
								}
							}
							str_ptr			=  0;
						}
					}

				}
			}
			else if(packet_rx.format.header.stx == 0x1B){	//esc
				if(	str_ptr == 2){							//second character
					pkt_found		=  2;					//exit get packet with a type two packet
					str_ptr			=  0;
					return(pkt_found);
				}
			}
			else{
				str_ptr			=  0;
			}

			rec_char_u8 = boot_getchar(0,&rec_status_u8);
		}while(rec_status_u8);
	}

	return(pkt_found);
}


/*************************************************************************************************
* Function Name : 	comms_app_transmit
* Description   : 	This Function sends a packet to the application
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	08/01/16		W. Paul			Created
*************************************************************************************************/
void api_bootloader_packet_tx(uint16_t 	cmd)
{
	footer_t			footer;
	uint16_t			p_len;
	uint16_t			i;

	packet_tx.format.header.stx			=  0x02;
	packet_tx.format.header.cmd.a		=  cmd;
//	packet_tx.format.header.d_len.a		=  0;		//updated below
//	packet_tx.format.header.h_cs		=  0;		//updated below
	packet_tx.format.header.eoh			=  0x1F;
	footer.eop							=  0x03;


	switch(cmd){
//		case eg_command1:
//			packet_tx.format.header.d_len.a	=  sizeof(cmd0001_t);
//			packet_tx.format.data.cmd0001.add		=  123;
//			packet_tx.format.data.cmd0001.data[0]	=  12;	//etc
//			break;
		case LOAD_FIRMWARE_READY:
			packet_tx.format.header.d_len.a	=  0;
			break;
		case LOAD_FIRMWARE_REQ_NEXT:
			packet_tx.format.header.d_len.a	=  sizeof(cmd0002_t);
			packet_tx.format.data.cmd0002.next_add.a	=  nextAddress;
			break;
		case LOAD_FIRMWARE_INFO:
            api_bootloader_settings_load();
            packet_tx.format.header.d_len.a	=  sizeof(BootSetting_t);
			memcpy(&packet_tx.format.data.BootSetting.checkByte,&BootSettings.checkByte,sizeof(BootSetting_t) );
			break;
		case LOAD_FIRMWARE_RESULT:
			packet_tx.format.header.d_len.a	=  sizeof(cmd0004_t);
			packet_tx.format.data.cmd0004.result	=  download_res;
			break;
        case LOAD_FIRMWARE_CUR_VERSION:
            packet_tx.format.header.d_len.a	=  sizeof(cmd0005_t);
            packet_tx.format.data.cmd0005.a	=  firmware_version_st_glb.a;
            packet_tx.format.data.cmd0005.b	=  firmware_version_st_glb.b;
            packet_tx.format.data.cmd0005.c	=  firmware_version_st_glb.c;
            packet_tx.format.data.cmd0005.d	=  firmware_version_st_glb.d;
            packet_tx.format.data.cmd0005.date	=  firmware_version_st_glb.day;
            packet_tx.format.data.cmd0005.month	=  firmware_version_st_glb.month;
            packet_tx.format.data.cmd0005.year	=  firmware_version_st_glb.year;
            break;
		//add other command outputs here

		default:
			printf("\r\ntx cmd not listed (%c)",cmd);
	}

	//header cs
	packet_tx.format.header.h_cs	=  packet_tx.format.header.stx;
	packet_tx.format.header.h_cs	+= packet_tx.format.header.cmd.b[0];
   	packet_tx.format.header.h_cs	+= packet_tx.format.header.cmd.b[1];
	packet_tx.format.header.h_cs	+= packet_tx.format.header.d_len.b[0];
	packet_tx.format.header.h_cs	+= packet_tx.format.header.d_len.b[1];

	//footer crc
	p_len		=  sizeof(header_t);
	p_len		+= packet_tx.format.header.d_len.a;
	footer.cs.a	=  app_Comms_Crc16(&packet_tx.raw[0],p_len);

	//add footer to bottom of raw data for transmition
	packet_tx.raw[p_len]	=  footer.cs.b[0];
	packet_tx.raw[p_len+1]	=  footer.cs.b[1];
	packet_tx.raw[p_len+2]	=  footer.eop;

	//tx packet
	for(i=0;i<=p_len+2;i++){
		boot_putchar(packet_tx.raw[i]);
	}
	boot_putchar(0x0d);
	boot_putchar(0x0a);

	return;
}

/*************************************************************************************************
* Function Name : 	api_bootloader_app_buffer_flush
* Description   : 	This Function sends 10 short message strings.  This allows the app to pick up data and
*                   flush its buffer
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/05/17	W. Paul			Created
*************************************************************************************************/
void api_bootloader_app_buffer_flush(void)
{
    uint8_t     i;
    uint8_t     j;

    for(j=0;j<10;j++){
        for(i=0;i<2;i++){
		    boot_putchar(0x5a);
	    }
        Delay(5);
    }
}

/*************************************************************************************************
* Function Name : 	app_Comms_Crc16
* Description   : 	This Function calculates a 16bit crc for the data passed into the function
* Arguments     : 	char* 		buffer
*					uint16_t 	length
* Returns       : 	uint16_t	crc
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/05/17	W. Paul			Created
*************************************************************************************************/
const uint16_t CrcTableA[16]	=  {0x0000,0x1189,0x2312,0x329b,0x4624,0x57ad,0x6536,0x74bf,0x8c48,0x9dc1,0xaf5a,0xbed3,0xca6c,0xdbe5,0xe97e,0xf8f7	};
const uint16_t CrcTableB[16]	=  {0x0000,0x1081,0x2102,0x3183,0x4204,0x5285,0x6306,0x7387,0x8408,0x9489,0xa50a,0xb58b,0xc60c,0xd68d,0xe70e,0xf78f	};
uint16_t app_Comms_Crc16(char* buffer, uint16_t length)
{
    uint8_t		index;
    uint16_t	crc		=  0xffff;

//	app_sys_watchdog_reload();
    while(length--){
        index	= ((uint8_t) crc) ^ *buffer++;
        crc		= (crc >> 8) ^ CrcTableA[index & 0x0f] ^ CrcTableB[index >> 4];

    }
    return(crc);
}


/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
