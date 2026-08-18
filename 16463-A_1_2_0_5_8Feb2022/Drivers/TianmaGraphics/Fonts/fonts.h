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
 * Filename    :  fonts.h
 * Date Created:  Thu 14 Dec 2017 04:45:22 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#ifndef _FONTS_H_
#define _FONTS_H_

#include "bfcfont.h"
												//char set avail				font used				Ammendments

extern const BFC_FONT fontArial16h;				//extened all					Ariel Regular 10		0xB2 made into subscript
extern const BFC_FONT fontArial22h;				//extened all					Ariel Regular 14		0xB2 made into subscript
extern const BFC_FONT fontArial26h;				//extened all					Ariel Regular 17		0xB2 made into subscript
extern const BFC_FONT fontArialNarrowBold31h;	//extened all					Ariel Narrow Bold 20	0xB2 made into subscript
extern const BFC_FONT fontArialNarrow37h;		//extened all					Ariel Narrow  24		0xB2 made into subscript
extern const BFC_FONT fontArialNarrow50h;		//extened all					Ariel Narrow  32		0xB2 made into subscript    - 1 off top & 1 off bottom (check character 0xC5)

extern const BFC_FONT fontArial48h;				//letters 'O'&'K' +numbers		Ariel Bold 48			?
extern const BFC_FONT fontArialNarrow60h;		//space +-./0123456789:			Ariel Narrow Bold 60	?

void Fonts_test(void);

#endif
/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
