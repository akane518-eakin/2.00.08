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
 * Filename    :  csp_paracube_O2.c
 * Date Created:  Wed 18 Oct 2017 09:47:38 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "app_pneumatic_ctrl.h"

#include "alg_av.h"
#include "api_IO.h"

#include "csp_paracube_O2.h"



/**********************************************************************************************************
 *		Local Variables
 *********************************************************************************************************/
uint8_t 	(*paracube_getchar)(	uint16_t wait_time_u16,	uint8_t *rec_status_u8);
void 		(*paracube_putchar)(	uint8_t tx_char_u8);

uint32_t	csp_paracube_timeout	=  CSP_PARACUBE_SET_TIME;

uint8_t		paracube_debug			=  0;


/**********************************************************************************************************
 *********************************************************************************************************/



/*************************************************************************************************
* Function Name : 	csp_paracube_fun_sel
* Description   : 	This Function maps the uart to this module of work
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/10/17	W. Paul			Created
*************************************************************************************************/
void csp_paracube_fun_sel (	uint8_t 	(*getchar_ptr)(	uint16_t /*wait_time_u16*/,	uint8_t* /*rec_status_u8*/),
							void 		(*putchar_ptr)(	uint8_t /*tx_char_u8*/)
                        )
{
	paracube_getchar		=  getchar_ptr;
	paracube_putchar		=  putchar_ptr;
	return;
}


/*************************************************************************************************
* Function Name : 	csp_paracube_handler
* Description   : 	This Function should be in the main loop and handels all input data
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/10/17	W. Paul			Created
*************************************************************************************************/
void csp_paracube_handler(void)
{
	uint8_t					rec_status;
	uint8_t					rec_char;
//	float					old;

	static PARACUBE_RX_st	rx_data;

	if(rx_data.init != 1){
		rx_data.init			=  1;
		rx_data.rx_position		=  0;
		rx_data.rx_value		=  0;
		rx_data.err_flags.byte	=  0;
	}

	rec_char	= paracube_getchar(0,&rec_status);
	if(rec_status){
		if(paracube_debug){
			if((rec_char == 0x0d)||(rec_char == 0x0a)){}
			else{
				printf("%c",rec_char);
			}
		}

		rx_data.rx_position		+= 1;
		switch(rec_char){
			case '!':
				rx_data.cmd_out	=  1;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				rx_data.rx_value	*= 10;
				rx_data.rx_value	+= rec_char;
				rx_data.rx_value	-= '0';
				break;
			case '-':
				rx_data.rx_value	*= -1;
				break;

			case 'B':	rx_data.err_flags.bits.B	=  1;	break;
			case 'C':	rx_data.err_flags.bits.C	=  1;	break;
			case 'E':	rx_data.err_flags.bits.E	=  1;	break;
			case 'S':	rx_data.err_flags.bits.S	=  1;	break;
			case 'X':	rx_data.err_flags.bits.X	=  1;	break;

			case 0x0d:
			case 0x0a:
				if(rx_data.cmd_out == 0){	//set high when a command is outputing data
					rx_data.rx_value	/= 10;
					if(paracube_debug){
						printf(" -Paracube %3.1f Flags %2x \r\n",rx_data.rx_value,rx_data.err_flags.byte);
					}
					io_status_st_glb.Paracube_O2_fault		=  rx_data.err_flags.byte;
					if(rx_data.err_flags.byte				== 0){
						io_status_st_glb.Paracube_O2_value	=  rx_data.rx_value;

						if(app_Pneumatics.O2_sensor_sel		== O2_SENSOR_PARACUBE){
							app_Pneumatics.O2_conc_actual	=  alg_av_f(PARACUBE_AV,io_status_st_glb.Paracube_O2_value);
						}
						else{
							paracube_val					=  alg_av_f(PARACUBE_AV,io_status_st_glb.Paracube_O2_value);
						}
					}
					else{
						io_status_st_glb.Paracube_O2_value	=  20.0;
					}
				}
				else{
					printf("\r\n");
				}
				rx_data.cmd_out			=  0;
				rx_data.init			=  0;
				csp_paracube_timeout	=  0;
				break;
		}

	}

	return;
}




/*************************************************************************************************
* Function Name : 	csp_paracube_debug
* Description   : 	This Function enables or disables the debug output
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/10/17	W. Paul			Created
*************************************************************************************************/
uint8_t csp_paracube_debug(uint8_t en_dis)
{
	if(en_dis <=1){	paracube_debug	=  en_dis;	}
	if(en_dis == 2){
		if(	paracube_debug){	paracube_debug	=  0;	}
		else{					paracube_debug	=  1;	}
	}
	return(paracube_debug);
}

/*************************************************************************************************
* Function Name : 	csp_paracube_command
* Description   : 	This Function sends commands to the paracube sensor
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		18/10/17	W. Paul			Created
*************************************************************************************************/
void csp_paracube_command(PARACUBE_CMDS_enum cmd, float value)
{
	char		str[10];
	uint8_t		i;

	switch(cmd){
		case PARACUBE_ID: 						printf("\r\nParacube ID\r\n");				sprintf(str,"!\r");						break;
		case PARACUBE_VER: 						printf("\r\nParacube Ver\r\n");				sprintf(str,"!F\r");					break;
		//switch between digital and anlogue
		case PARACUBE_PRES_COMP_RD: 			printf("\r\nParacube Press Comp Rd\r\n");	sprintf(str,"!P\r");					break;
		case PARACUBE_PRES_COMP_SET: 			printf("\r\nParacube Press Comp Wr\r\n");	sprintf(str,"!P%d\r",(uint8_t)value);	break;
		//enable/disable crc
		case PARACUBE_2_POINT_CAL_LOW:			printf("\r\nParacube 2pt cal low\r\n");		sprintf(str,"!L%3.1f\r",value);			break;
		case PARACUBE_2_POINT_CAL_HIGH:			printf("\r\nParacube 2pt cal high\r\n");	sprintf(str,"!H%3.1f\r",value);			break;
		case PARACUBE_1_POINT_CAL:				printf("\r\nParacube 1pt cal\r\n");			sprintf(str,"!S%3.1f\r",value);			break;
		case PARACUBE_IDENTITY:					printf("\r\nParacube Identity\r\n");		sprintf(str,"!D\r");					break;
		case PARACUBE_RESTORE_CAL:				printf("\r\nParacube Restore Cal\r\n");		sprintf(str,"!R\r");					break;
		case PARACUBE_CRC:						printf("\r\nParacube CRC\r\n");				sprintf(str,"!C%d\r", (uint8_t)value);	break;
	}
	for(i=0;i<strlen(str);i++){
		paracube_putchar(str[i]);
	}
	return;
}


/*
*********************************************************************************************************
* Function Name : csp_paracube_timer
* Description   : This function is used for paracube systick functions
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			08/02/2022			Tim Barr		Original Created
*********************************************************************************************************
*/
void csp_paracube_timer(void)
{
	// Local Variables
	
	// Code
	if(csp_paracube_timeout		>  1){
		csp_paracube_timeout	--;
	}
	
	return;
}


/*
*********************************************************************************************************
* Function Name : csp_paracube_timeout_read
* Description   : This function is used for reading or resetting the rx timeout for the paracube
* Arguments     : None
* Returns       : None
* Notes         : None
*
* Version		Date dd/mm/yyyy		Programmer		Reason for Change
* 1.0.0			08/02/2022			Tim Barr		Original Created
*********************************************************************************************************
*/
uint32_t csp_paracube_timeout_read(uint8_t reset)
{
	// Local Variables
	
	// Code
	if(reset					>  0){
		csp_paracube_timeout	=  CSP_PARACUBE_SET_TIME;
	}
	
	return csp_paracube_timeout;
}


/*
*********************************************************************************************************
*											End of csp_paracube_O2.c
*********************************************************************************************************
*/
