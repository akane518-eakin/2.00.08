/**********************************************************************************************************
 * Envirotronics Ltd
 *
 * Copyright 2012
 * All Rights Reserved
 *
 * File Name    :	csp_STM32_uart.c
 * Date Created :	Tue, 03 Jan 2012  04:50:51 PM
 * Programmer   :	William Paul
 * Compiler     :	Ride 7

 * Description  :	Main Application file
 *
 **********************************************************************************************************/


/*********************************************************************************************************
 *		Include Files
 *********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __ICCARM__	/* IAR compiler being used */
#include <yfuns.h>
#endif

#include "pcb_uart.h"
#include "csp_STM32_uart.h"

#if( UART1_EN == 1)
#include "csp_STM32_uart1.h"
#endif
#if( UART2_EN == 1)
#include "csp_STM32_uart2.h"
#endif
#if( UART3_EN == 1)
#include "csp_STM32_uart3.h"
#endif
#if( UART4_EN == 1)
#include "csp_STM32_uart4.h"
#endif
#if( UART5_EN == 1)
#include "csp_STM32_uart5.h"
#endif
/**********************************************************************************************************
 *		Compiler Defines
 *********************************************************************************************************/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif


void uart_putchar_dump(uint8_t tx_char_u8);
uint8_t uart_getchar_dump(uint16_t wait_time_u16,uint8_t *rec_status_u8);


/**********************************************************************************************************
 *		Global Variables
 *********************************************************************************************************/

/**********************************************************************************************************
 *		Local Variables
 *********************************************************************************************************/
uint8_t 	(*debug_getchar)(	uint16_t wait_time_u16,	uint8_t *rec_status_u8);
void 		(*debug_putchar)(	uint8_t tx_char_u8);




/**********************************************************************************************************
 *********************************************************************************************************/


/*************************************************************************************************
 * Function Name :		uart_debug_fun_sel_A
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	03/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart_debug_fun_sel_A (	uint8_t 	(*debug_getchar_ptr)(	uint16_t /*wait_time_u16*/,	uint8_t */*rec_status_u8*/),
							void 		(*debug_putchar_ptr)(	uint8_t /*tx_char_u8*/)
                        )
{
	debug_getchar		=  debug_getchar_ptr;
	debug_putchar		=  debug_putchar_ptr;
	return;

}

/*************************************************************************************************
 * Function Name :		uart_debug_fun_sel
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	03/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart_debug_fun_sel (unsigned char	port)
{
	switch(port){
#if( UART1_EN == 1)
		case 1:
			uart_debug_fun_sel_A(	uart1_getchar,		uart1_putchar	);
			break;
#endif
#if( UART2_EN == 1)
		case 2:
			uart_debug_fun_sel_A(	uart2_getchar,		uart2_putchar	);
			break;
#endif
#if( UART3_EN == 1)
		case 3:
			uart_debug_fun_sel_A(	uart3_getchar,		uart3_putchar	);
			break;
#endif
#if( UART4_EN == 1)
		case 4:
			uart_debug_fun_sel_A(	uart4_getchar,		uart4_putchar	);
			break;
#endif
#if( UART5_EN == 1)
		case 5:
			uart_debug_fun_sel_A(	uart5_getchar,		uart5_putchar	);
			break;
#endif
		default:
			uart_debug_fun_sel_A(	uart_getchar_dump,	uart_putchar_dump	);
	}
    return;
}

/*************************************************************************************************
 * Function Name :		STM32_uart_config
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	03/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void STM32_uart_config(void)
{
#if( UART1_EN == 1)
#warning UART1_EN is enabled
	uart1_config(UART1_BAUD);
#endif

#if( UART2_EN == 1)
#warning UART2_EN is enabled
	uart2_config(UART2_BAUD);
#endif

#if( UART3_EN == 1)
#warning UART3_EN is enabled
	uart3_config(UART3_BAUD);
#endif

#if( UART4_EN == 1)
#warning UART4_EN is enabled
	uart4_config(UART4_BAUD);
#endif

#if( UART5_EN == 1)
#warning UART5_EN is enabled
	uart5_config(UART5_BAUD);
#endif


	uart_debug_fun_sel(UART_PRINTF_PORT);

	return;
}






#ifdef __ICCARM__	/* IAR compiler being used */

/*************************************************************************************************
 * Function Name :		__write
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	03/01/12	W. Paul		Created
 *
 *************************************************************************************************/
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    debug_putchar(*buffer);
    return(1);
}



/*************************************************************************************************
 * Function Name :		__read
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	03/01/12	W. Paul		Created
 *
 *************************************************************************************************/
size_t __read(int handle, unsigned char * buffer, size_t size)
{
    uint8_t 	err_u8 = 0;
    size_t 		i;

    for(i=0; i<size ;i++)
    {
        *buffer = debug_getchar(0, &err_u8);

#ifdef __READ_ECHO
        debug_putchar(*buffer);
#endif
        if(err_u8==0){
            printf("__read err");
            return(_LLIO_ERROR);
        }
        buffer++;
    }

    return(i);
}

#endif


#ifdef __GNUC__

/*************************************************************************************************
 * Function Name :		__io_putchar
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	03/01/12	W. Paul		Created
 *
 *************************************************************************************************/
int __io_putchar(int ch)
{
	debug_putchar((uint8_t) ch);
	return(ch);
}


/*************************************************************************************************
 * Function Name :		__io_getchar
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	03/01/12	W. Paul		Created
 *
 *************************************************************************************************/
int __io_getchar(void)
{
    int ch;
    uint8_t status;

    ch = debug_getchar(0,&status);

    return(ch);
}


#endif

/*************************************************************************************************
 * Function Name :		uart_timer_control
 * Description   :
 * Arguments     : 		None Listed
 * Returns       : 		None Listed
 * Notes         : 		None Listed
 *
 * Version	Date d/m/y	Programmer	Reason for Change
 * 0.1.0	03/01/12	W. Paul		Created
 *
 *************************************************************************************************/
void uart_timer_control(void)
{
#if( UART1_EN == 1)
	uart1_timer_control();
#endif /*(_USART_1_EN)*/

#if( UART2_EN == 1)
	uart2_timer_control();
#endif

#if( UART3_EN == 1)
	uart3_timer_control();
#endif

#if( UART4_EN == 1)
	//uart4_timer_control();
	uart4_1mS_timer();
#endif

#if( UART5_EN == 1)
	uart5_timer_control();
#endif

	return;
}


/*************************************************************************************************
* Function Name : 	uart_putchar_dump
* Description   : 	This Function dumps data sent to it
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	15/08/16		W. Paul			Created
*************************************************************************************************/
void uart_putchar_dump(uint8_t tx_char_u8)
{
	return;
}

/*************************************************************************************************
* Function Name : 	uart_getchar_dump
* Description   : 	This Function never returns data
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	15/08/16		W. Paul			Created
*************************************************************************************************/
uint8_t uart_getchar_dump(uint16_t wait_time_u16,uint8_t *rec_status_u8)
{
	*rec_status_u8	=  0;

	return(0);
}

/*********************************************************************************************************
 *********************************************************************************************************/
//end of file


