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
 * Filename    :  glb_typedefs.h
 * Date Created:  Tue 04 Apr 2017 08:58:18 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef __GLB_TYPEDEFS_H
#define __GLB_TYPEDEFS_H

#include "stdbool.h"


//	ASCII characters
#define	NULL_CHAR			0x00
#define	STX_CHAR			0x02
#define	ETX_CHAR			0x03
#define	ACK_CHAR			0x06
#define	BCK_CHAR			0x08
#define	LF_CHAR				0x0A
#define	CR_CHAR				0x0D
#define	NAK_CHAR			0x15
#define	ESC_CHAR			0x1B
#define	SPACE_CHAR			0x20
#define	ESK_MARK_CHAR		0x21
#define	COMMA_CHAR			0x2C
#define	DASH_SIGN_CHAR		0x2D
#define	DOT_SIGN_CHAR		0x2E
#define	FORWARD_SL_CHAR		0x2F
#define	EQUALS_SIGN_CHAR	0x3D
#define	QUESTION_MARK_CHAR	0x3F
#define	UNDERSCORE_CHAR		0x5F


#define CODE_VER_STR	"MARTURIONLtd"		//12chars
#define CODE_END_STR	"CEND"



/**********************************************************************************************************
*                                           TYPEDEFS
**********************************************************************************************************/
__packed typedef struct {
	uint8_t 		a;
	uint8_t 		b;
	uint8_t 		c;
	uint16_t 		d;
	uint8_t 		day;
	uint8_t 		month;
	uint8_t 		year;
	uint8_t const	*time_p;  /* pointer to date-time string */
	uint8_t const	*date_p;  /* pointer to date-time string */
}sw_version_t;


extern const sw_version_t         firmware_version_st_glb;


#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
