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
 * Filename    :  hal_STM32_uart.h
 * Programmer  :  William Paul
 * Description :  This module is used for all the uart functions.
 *
 **********************************************************************************************************/


/**********************************************************************************************************
 *	INCLUDE FILES
 *********************************************************************************************************/
#include <stdio.h>

#include	"app_system.h"

#include "hal_STM32_uart.h"
#include "csp_STM32_uart.h"
#include "csp_STM32_iwdg.h"
#include "glb_typedefs.h"

/**********************************************************************************************************
 *	COMPILER DEFINES
 *********************************************************************************************************/

/**********************************************************************************************************
 *	GLOBALS VARIABLES
 *********************************************************************************************************/

/**********************************************************************************************************
 *	LOCAL VARIABLES
 *********************************************************************************************************/
uint8_t		active_dot	=  1;

/**********************************************************************************************************
 *********************************************************************************************************/


/*
************************************************************************************************
* Function Name :		BSP_get_num
* Description   :		returns a number from the debug serial port
* Arguments     : 		uint8_t 	base
*						uint8_t 	max_no_chars
* Returns       : 		uint32_t	result
* Notes         : 		None Listed
*
* Version	Date d/m/y	Programmer	Reason for Change
* 0.1.0		28/10/10	W. Paul		Created
*
************************************************************************************************
*/
uint32_t BSP_get_num(uint8_t base,uint8_t max_no_chars)
{
	uint8_t 	rec_status_u8;
	uint8_t 	rec_char_u8;
	uint8_t		char_cnt	=  0;
	uint8_t		loop		=  1;
	uint32_t	result		=  0;

	while(loop){
		rec_char_u8 = debug_getchar(0,&rec_status_u8);
//		rec_char_u8 = debug_getchar_nowait(&rec_status_u8);			/* Check USART 1 */
		if(rec_status_u8){
			switch(rec_char_u8)
			{
				case CR_CHAR:		//return
					loop	=  0;
					break;
				case '.':
					debug_putchar('.');
					if(active_dot){
						loop	=  0;
					}
					break;
				case BCK_CHAR:		//backspace
					if(char_cnt>0){
						char_cnt--;
						result	/= base;
					}
					break;
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
					rec_char_u8	-= 0x20;
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
					if(base == BASE16){
						debug_putchar((char)rec_char_u8);
						char_cnt++;
						result	*= base;
						result	+= rec_char_u8;
						result	-= 0x37;
					}
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
					debug_putchar(rec_char_u8);
					char_cnt++;
					result	*= base;
					result	+= rec_char_u8;
					result	-= 0x30;
					break;
			}
		}

		if(char_cnt >= max_no_chars){
			loop	=  0;
		}
		app_sys_watchdog_reload();
	}
	return(result);
}

/*
************************************************************************************************
* Function Name :		BSP_get_float
* Description   :		returns a number from the debug serial port		a.b
* Arguments     : 		uint8_t 	a
*						uint8_t 	b
* Returns       : 		float	result
* Notes         : 		None Listed
*
* Version	Date d/m/y	Programmer	Reason for Change
* 0.1.0		28/10/10	W. Paul		Created
*
************************************************************************************************
*/
float BSP_get_float(uint8_t a,uint8_t b)
{
	float	value;
	float	value_1;
	uint8_t	i;


	value	=  BSP_get_num(BASE10,a);
	active_dot	=  0;
	value_1	=  BSP_get_num(BASE10,b);
	active_dot	=  1;

	for(i=0;(i<b)&&(value_1>0);i++){
		value_1	/= 10;
	}
	value	+= value_1;

	return(value);
}


/*
************************************************************************************************
* Function Name :		BSP_get_str
* Description   :		returns a string from the debug serial port
* Arguments     : 		uint8_t 	*str
*						uint8_t 	max_no_chars
* Returns       : 		uint8_t		no of chars
* Notes         : 		None Listed
*
* Version	Date d/m/y	Programmer	Reason for Change
* 0.1.0		20/03/12	W. Paul		Created
*
************************************************************************************************
*/
uint8_t BSP_get_str(uint8_t *str,uint8_t max_no_chars)
{
	uint8_t	loop		=  1;
	uint8_t	char_cnt	=  0;
	uint8_t			rec_status_u8;
	uint8_t			rec_char_u8;


	while(loop){
		rec_char_u8 = debug_getchar(0,&rec_status_u8);
//		rec_char_u8 = debug_getchar_nowait(&rec_status_u8);			/* Check USART 1 */
		if(rec_status_u8){
			if(rec_char_u8 == CR_CHAR){				//return
				loop	=  0;
			}
			else if(rec_char_u8 == BCK_CHAR){		//backspace
				if(char_cnt>0){
					char_cnt--;
					//debug_putchar('/b');
					debug_putchar(BCK_CHAR);
				}
			}
			else{
				debug_putchar(rec_char_u8);
				str[char_cnt]	=  rec_char_u8;
				char_cnt++;
			}
		}

		if(char_cnt >= max_no_chars){
			loop	=  0;
		}

		app_sys_watchdog_reload();
	}
	return(char_cnt);
}

/**********************************************************************************************************
 *********************************************************************************************************/
//end of file
