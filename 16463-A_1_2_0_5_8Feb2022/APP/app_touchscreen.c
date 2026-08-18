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
 * Filename    :  app_touchscreen.c
 * Date Created:  Thu 14 Sep 2017 11:29:00 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>
#include <string.h>

#include	"app_touchscreen.h"
#include	"api_STM32_touchscreen.h"
#include	"api_audio.h"

/*********************************************************************************************************
 *		Local Functions
 ********************************************************************************************************/
void touch_av(int16_t *x,int16_t *y);
void touch_av_reset(void);



/*********************************************************************************************************
 *		Local Variables
 ********************************************************************************************************/
soft_touch_but_t	soft_but[MAX_SOFT_BUTTONS];	//list of soft button parameters
button_t			button={						//parameters of current touch activity
	.must_release	=  0,
};
touch_av_t			x_av;						//rolling av of x corrds
touch_av_t			y_av;						//rolling av of y corrds
uint32_t			TouchDetectDurationTest	=  0;	//if touch screen is pressed for too long an error is reported

uint8_t				sound_option[NO_OF_SOUND_OPTIONS][2];

/*********************************************************************************************************
 ********************************************************************************************************/


/*************************************************************************************************
* Function Name : 	app_touchscreen_handler
* Description   : 	This Function monitors the touchscreen.
*					if a touch is detected, this function checks if it is inside a soft button area.
* Arguments     : 	void
* Returns       : 	uint8_t		button report		0 to MAX_SOFT_BUTTONS			button press detected
*													0x80+ (0 to MAX_SOFT_BUTTONS)button press and hold detected
*													0xfe	button released for a short time
*													0xff	button rel;eased a long time	(debounce)
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t app_touchscreen_handler(void)
{
  	uint8_t				i;
	uint8_t				touch_detect;
	int16_t 			x;
	int16_t 			y;
	uint16_t			TouchPressure;
	static uint8_t		av_cnt;
	uint32_t			detect_cnt		=  0;
	static uint8_t		reported		=  0;
	static uint8_t		chirp_cnt;



	if(button.init != 1){
		button.init	=  1;
		button.cur_button	=  0xfe;

		sound_option[0][SOUND_PRESS]		=  1;
		sound_option[0][SOUND_PRESS_HOLD]	=  2;
		sound_option[1][SOUND_PRESS]		=  3;
		sound_option[1][SOUND_PRESS_HOLD]	=  3;

	}

	touch_detect	=  api_touch_CoOrds_rd(&x,&y,&TouchPressure);
	api_touch_print();

	if(	button.must_release){
		if(touch_detect==0){	button.must_release = 0;	}
		touch_detect	=  0;
	}

	if(touch_detect){
		if(TouchDetectDurationTest == 0){	TouchDetectDurationTest = 1;}	//start timer

		if(	++av_cnt <= 2){
			x	= -1;	//first touch can be corrupt
			y	= -1;	//-1 should never be an active button
		}
		else{
			touch_av(&x,&y);
		}

		if(av_cnt > 10){
			av_cnt	= 10;
		}
	}
	else{
		TouchDetectDurationTest	=  0;
		touch_av_reset();
		av_cnt	=  0;
	}

	if(av_cnt > 4){
		for(i=0;i<MAX_SOFT_BUTTONS;i++){										//check all soft button options
			if(soft_but[i].valid){												//only check if the soft button is valid
				if(	(x >= soft_but[i].x_s) && (x <=  soft_but[i].x_e)	&&		//check the touch is with in the active area
					(y >= soft_but[i].y_s) && (y <=  soft_but[i].y_e)		)
				{
					detect_cnt	+= 1;

					button.press_current_x	=  x;
					button.press_current_y	=  y;

					if( i != button.cur_button){								//if new button press is detected
						button.cur_button		=  i;
						button.press_duration	=  1;	//start timer
						button.release_duration	=  0;	//stop timer
						button.press_start_x	=  x;
						button.press_start_y	=  y;
						reported				=  0;
					}
					else if(button.press_duration > PRESS_AND_HOLD_DURATION){	//press and hold reported
						button.press_duration	-= PRESS_AND_HOLD_REPEAT;
						if(chirp_cnt == 1){
							chirp_cnt++;
							api_audio_chirp( sound_option[soft_but[i].press_sound][SOUND_PRESS_HOLD] );
						}

						return(i | BUTTON_HOLD);
					}
					else if((button.press_duration > PRESS_DURATION) &&(reported ==0)){			//press reported
						reported	=  1;
						api_audio_chirp(sound_option[soft_but[i].press_sound][SOUND_PRESS]);
						chirp_cnt++;
						return(i);
					}
					else{
						return(BUTTON_PRE_HOLD);	//returns this pre hold output
					}
				}
			}
		}
		return(BUTTON_PRESSED_SOMEWHERE_ELSE);

	}

	// if reach here there is no button pressed
	if(	(detect_cnt == 0)&&
		(button.cur_button !=  0xff)	)		//newly released
	{
		button.cur_button		=  0xff;
		button.press_duration	=  0;	//stop timer
		button.release_duration	=  1;	//start timer
		reported	=  0;
		if(chirp_cnt == 2){
			api_audio_chirp(2);
		}
		chirp_cnt	=  0;
	}

	if(button.release_duration < PRESS_RELEASE){	return(BUTTON_JUST_RELEASED);	}
	else{											return(BUTTON_RELEASED);	}
}

/*************************************************************************************************
* Function Name : 	app_touchscreen_irq
* Description   : 	This Function provides a ms timer to this code block
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
void app_touchscreen_irq(void)
{
	if(TouchDetectDurationTest){
		if(TouchDetectDurationTest < 60000){	TouchDetectDurationTest	+= 1;	}
	}
	if(button.press_duration){
		if(button.press_duration < 60000){		button.press_duration	+= 1;	}
	}
	if(button.release_duration){
		if(button.release_duration < 60000){	button.release_duration	+= 1;	}
	}
	return;
}

/*************************************************************************************************
* Function Name : 	app_touchscreen_button_add
* Description   : 	This Function adds a soft button area to the touch manager
* Arguments     : 	uint8_t id
*					uint16_t x
*					uint16_t y
*					uint16_t w
*					uint16_t h
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
void app_touchscreen_button_add(uint8_t id,uint16_t x,uint16_t y,uint16_t w,uint16_t h)
{
	if(id >=  MAX_SOFT_BUTTONS){
		id	=  MAX_SOFT_BUTTONS-1;
		printf("\r\n ERR - SOFTBUTTON id>max");
	}

	soft_but[id].valid	=  1;
	soft_but[id].x_s	=  (int16_t)x;
	soft_but[id].x_w	=  (int16_t)w;
    soft_but[id].x_e	=  (int16_t)(x + w);
    soft_but[id].y_s	=  (int16_t)y;
    soft_but[id].y_h	=  (int16_t)h;
    soft_but[id].y_e	=  (int16_t)(y + h);

	if(soft_but[id].x_s > X_MAX){	soft_but[id].x_s	=  X_MAX;}
	if(soft_but[id].x_e > X_MAX){	soft_but[id].x_e	=  X_MAX;}
	if(soft_but[id].y_s > Y_MAX){	soft_but[id].y_s	=  Y_MAX;}
	if(soft_but[id].y_e > Y_MAX){	soft_but[id].y_e	=  Y_MAX;}

	soft_but[id].press_sound	=  0;	//default
	return;
}

/*************************************************************************************************
* Function Name : 	app_touchscreen_button_disable
* Description   : 	This Function adds a soft button area to the touch manager
* Arguments     : 	uint8_t id
*					uint16_t x
*					uint16_t y
*					uint16_t w
*					uint16_t h
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
void app_touchscreen_button_disable(uint8_t id)
{
	if(id >=  MAX_SOFT_BUTTONS){
		id	=  MAX_SOFT_BUTTONS-1;
		printf("\r\n ERR - SOFTBUTTON id>max");
	}

	soft_but[id].valid	=  0;

	return;
}

/*************************************************************************************************
* Function Name : 	app_touchscreen_button_enable
* Description   : 	This Function adds a soft button area to the touch manager
* Arguments     : 	uint8_t id
*					uint16_t x
*					uint16_t y
*					uint16_t w
*					uint16_t h
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
void app_touchscreen_button_enable(uint8_t id)
{
	if(id >=  MAX_SOFT_BUTTONS){
		id	=  MAX_SOFT_BUTTONS-1;
		printf("\r\n ERR - SOFTBUTTON id>max");
	}

	soft_but[id].valid	=  1;

	return;
}

/*************************************************************************************************
* Function Name : 	app_touchscreen_button_disable_all
* Description   : 	This Function removes all the soft buttons available
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
void app_touchscreen_button_disable_all(void)
{
	uint8_t		i;

	for(i=0;i<	MAX_SOFT_BUTTONS;i++){
		app_touchscreen_button_disable(i);
	}

	return;
}

/*************************************************************************************************
* Function Name : 	app_touchscreen_button_audio_option
* Description   : 	This Function allows a differnet sound to be played when button is pressed
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		13/08/19	W. Paul			Created
*************************************************************************************************/
void app_touchscreen_button_audio_option(uint8_t id,uint8_t sound)
{
	soft_but[id].press_sound	=  sound;
	return;
}

/*************************************************************************************************
* Function Name : 	app_touchscreen_no_button_press
* Description   : 	This Function is forced form UI to ensure no touch is reported to UI until abutton release is detected
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		23/10/18	W. Paul			Created
*************************************************************************************************/
void app_touchscreen_no_button_press(void)
{
	button.must_release	=  1;

	return;
}

/*************************************************************************************************
* Function Name : 	app_touchscreen_slider_info
* Description   : 	This Function
* Arguments     : 	slider_t *slider	returns the details of the slider position (different formats of result avail)
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
uint8_t app_touchscreen_slider_info(slider_t *slider)
{
	uint8_t		but;

	if(button.must_release == 0){
		but	=  button.cur_button;
		but	&= 0x7F;	//ignore the press and hold flag

		if(button.cur_button < 0xfe){
			slider->x_pix_diff	=  button.press_current_x - button.press_start_x;
			slider->y_pix_diff	=  button.press_current_y - button.press_start_y;

			slider->x_ee_f		=  (float)(button.press_current_x - soft_but[but].x_s)  / (float)soft_but[but].x_w;
			slider->y_ee_f		=  (float)(button.press_current_y - soft_but[but].y_s)  / (float)soft_but[but].y_h;
		}
		return(1);
	}

	return(0);
}




/*************************************************************************************************
* Function Name : 	touch_av
* Description   : 	This Function returns the roling avof the x&y coords
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
void touch_av(int16_t *x,int16_t *y)
{
	x_av.tot			-= x_av.his[x_av.pos];
	x_av.his[x_av.pos]	=  *x;
	x_av.tot			+= x_av.his[x_av.pos];
	if(++x_av.pos >= TOUCH_AV_HIS){	x_av.pos	=  0;		}
	if(++x_av.cnt >= TOUCH_AV_HIS){	x_av.cnt	=  TOUCH_AV_HIS;	}

	*x	=  (int16_t)(x_av.tot / x_av.cnt);

	y_av.tot			-= y_av.his[y_av.pos];
	y_av.his[y_av.pos]	=  *y;
	y_av.tot			+= y_av.his[y_av.pos];
	if(++y_av.pos >= TOUCH_AV_HIS){	y_av.pos	=  0;		}
	if(++y_av.cnt >= TOUCH_AV_HIS){	y_av.cnt	=  TOUCH_AV_HIS;	}

	*y	=  (int16_t)(y_av.tot / y_av.cnt);

	return;
}

/*************************************************************************************************
* Function Name : 	touch_av_reset
* Description   : 	This Function resets the touch av
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		14/09/17	W. Paul			Created
*************************************************************************************************/
void touch_av_reset(void)
{
	x_av.tot	=  0;
	x_av.pos	=  0;
	x_av.cnt	=  0;
	memset(x_av.his,0,sizeof(x_av.his));

	y_av.tot	=  0;
	y_av.pos	=  0;
	y_av.cnt	=  0;
	memset(y_av.his,0,sizeof(y_av.his));

	return;
}

/*************************************************************************************************
* Function Name : 	app_touch_hold_err
* Description   : 	This Function returns a high if the touch screen has been pressed longer than x ms
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		02/05/18	W. Paul			Created
*************************************************************************************************/
uint8_t app_touch_hold_err(void)
{
	if(TouchDetectDurationTest > PRESS_AND_HOLD_ERROR){
		TouchDetectDurationTest	=  PRESS_AND_HOLD_ERROR;
		return(1);
	}
	return(0);
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
