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
 * Filename    :  api_audio.c
 * Date Created:  Wed 06 Sep 2017 09:14:04 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<math.h>

#include	"app_system.h"

#include	"api_audio.h"

#include	"csp_S25FL0xx.h"

#include 	"csp_STM32_delay.h"
#include	"csp_STM32_dac.h"
#include	"csp_STM32_dma.h"


#include 	"pcb_pins.h"

#include 	"stm32f10x_tim.h"

#include 	"csp_STM32_uart.h"
#include	"hal_STM32_uart.h"
#include 	"csp_STM32_I2Cx.h"

/**********************************************************************************************************
 *	NOTES
 **********************************************************************************************************/
//	init_DAC1()  			pcb_startup()
//	timer2_config()			pcb_startup()	8kHz	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
//	api_audio_Timer_IRQ		TIM2_IRQHandler()

/**********************************************************************************************************
 *	LOCAL VARIABLES
 **********************************************************************************************************/
api_audio_t		api_audio	= {
							.volume_desired	= 49,
							.stop_audio		=  0,
							};
I2C_TypeDef* 	AUDIO_I2Cx = I2C2;

uint16_t	DMA_buf_u16[DMA_BUF_SZ];

// Alarm sequence type definitions
typedef struct {
    uint16_t    freq_hz;
    uint32_t    dur_samples;    // @ 8kHz
    uint8_t     next_step;
    uint8_t     is_silence;
} alarm_step_t;

typedef struct {
    const alarm_step_t  *seq;
    uint8_t              steps;
} alarm_profile_t;

// LOW priority: 1 burst
static const alarm_step_t low_priority_seq[] = {
    { 500,  1000,  1, 0 },   // step 0: burst 1 - 500Hz, 125ms
    { 0,   160000,  0, 1 },   // step 1: inter-cycle pause ~20s
};

// MEDIUM priority: 3 bursts
static const alarm_step_t medium_priority_seq[] = {
    { 500,   1000,  1, 0 },  // step 0: burst 1 - 500Hz, 125ms
    { 0,     1100,  2, 1 },  // step 1: gap - 187.5ms
    { 1000,  1000,  3, 0 },  // step 2: burst 2 - 1000Hz, 125ms
    { 0,     1100,  4, 1 },  // step 3: gap - 187.5ms
    { 500,   1000,  5, 0 },  // step 4: burst 3 - 500Hz, 125ms
    { 0,    40000,  0, 1 },  // step 5: inter-cycle pause ~5s
};

// HIGH priority: 10 bursts
static const alarm_step_t high_priority_seq[] = {
    { 500,   1000,  1, 0 },  // step 0: burst 1 - 500Hz,  125ms
    { 0,      050,  2, 1 },  // step 1: gap - 62.5ms
    { 750,   1000,  3, 0 },  // step 2: burst 2 - 750Hz,  125ms
    { 0,      050,  4, 1 },  // step 3: gap - 62.5ms
    { 1000,  1000,  5, 0 },  // step 4: burst 3 - 1000Hz, 125ms
    { 0,     1600,  6, 1 },  // step 5: gap - 250ms
    { 750,   1000,  7, 0 },  // step 6: burst 4 - 750Hz,  125ms
    { 0,      050,  8, 1 },  // step 7: gap - 62.5ms
    { 500,   1000,  9, 0 },  // step 8: burst 5 - 500Hz,  125ms
    { 0,     2600,  10, 1 },  // step 9: gap - 375ms
    { 500,   1000,  11, 0 },  // step 10: burst 6 - 500Hz,  125ms
    { 0,      050,  12, 1 },  // step 11: gap - 62.5ms
    { 750,   1000,  13, 0 },  // step 12: burst 7 - 750Hz,  125ms
    { 0,      050,  14, 1 },  // step 13: gap - 62.5ms
    { 1000,  1000,  15, 0 },  // step 14: burst 8 - 1000Hz, 125ms
    { 0,     1600,  16, 1 },  // step 15: gap - 250ms
    { 750,   1000,  17, 0 },  // step 16: burst 9 - 750Hz,  125ms
    { 0,      050,  18, 1 },  // step 17: gap - 62.5ms
    { 500,   1000,  19, 0 },  // step 18: burst 10 - 500Hz,  125ms
    { 0,    40000,  0, 1 },  // step 19: inter-cycle pause ~5s
};

static const alarm_profile_t alarm_profiles[] = {
    { low_priority_seq,    2  },   // ALARM_PRIORITY_LOW
    { medium_priority_seq, 6  },   // ALARM_PRIORITY_MEDIUM
    { high_priority_seq,   20 },   // ALARM_PRIORITY_HIGH
};

/**********************************************************************************************************
 *	FUNCTION PROTOPTYPES
 **********************************************************************************************************/
void 		api_audio_file_download(uint32_t file_des_add);
void 		api_audio_dma_to_DAC1_start(void);
void 		api_audio_dma_to_DAC1_stop(void);
uint8_t		api_audio_file_rd(uint32_t file_loc);
void 		api_audio_TopUpBuf(void);
void 		api_audio_get_next_sample(void);

void 		api_audio_volume_wr(void);

/**********************************************************************************************************
 **********************************************************************************************************/



/*************************************************************************************************
* Function Name : 	api_audio_manager
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	06/05/14		W. Paul			Created
*************************************************************************************************/
void api_audio_manager(void)
{
	uint8_t			check;

	if(api_audio.init != API_AUDIO_INIT){
		api_audio.init					=  API_AUDIO_INIT;
		api_audio.state					=  AUDIO_IDLE;

		api_audio.start_quiet			=  0;
		api_audio.stop_audio			=  0;

//		api_audio.volume_desired		=  50;
		api_audio.volume_act			=  0xff;
		AUDIO_AMP_ON();
		AUDIO_MUTE_ON_HI();

		api_audio.DMA_buf_len			=  DMA_BUF_SZ;
		api_audio.address_ptr			=  0;	//depending on format this may increase by 1/2 bytes / sample
		api_audio.sample_no				=  0;

		api_audio.DAC_DMA_buf_wr_pos	=  0;

		api_audio.file_address			=  0;
		api_audio.file_sz_samples		=  0;
		api_audio.file_format			=  8;

		api_audio_dma_to_DAC1_stop();

	}

	api_audio_volume_wr();

	switch(api_audio.state){
		case AUDIO_IDLE:

			break;
		case AUDIO_START_PLAY:
			check	=  api_audio_file_rd(api_audio.file_address);
			if(check == 0){
				api_audio.state	=  AUDIO_IDLE;
				printf("\r\nAudio file not avail 0x%x",api_audio.file_address);
			}
			else{
				printf("\r\nVol %d",api_audio.volume_act);
				api_audio.state		=  AUDIO_START_PLAY1;
			}
			api_audio.DMA_buf_len	=  DMA_BUF_SZ;
			break;
		case AUDIO_START_PLAY1:
                        api_audio.sample_no  = 0;
                        api_audio_TopUpBuf();
                        api_audio_dma_to_DAC1_start();
                        if(api_audio.sin_amp == 0){
                        // Silence step - no quiet period needed, amp already muted
                        api_audio.start_quiet = 0;
                        DMA_Cmd(DMA2_Channel3, ENABLE);
                        } else {
                        // Tone step - apply quiet period to avoid click on amp power up
                        api_audio.start_quiet = START_QUIET_PERIOD;
                        AUDIO_AMP_ON();
                        }
                        api_audio.state = AUDIO_PLAY_INPROGRESS;
                        break;
		case AUDIO_PLAY_INPROGRESS:

			break;
		case AUDIO_STOP_PLAY:
    AUDIO_MUTE_ON_HI();
    if(api_audio.file_format == GEN_SINE_REPEAT_WAIT){
        api_audio.state = AUDIO_IDLE;
        printf("\r\nSTOP_PLAY: priority=%d step=%d", 
               api_audio.alarm_priority, 
               api_audio.multiTone_part);
        api_audio_alarm((ALARM_PRIORITY_e)api_audio.alarm_priority, api_audio.multiTone_part);
    }
    else{
        api_audio.state = AUDIO_IDLE;
    }
    break;
		default:
			api_audio.state	=  AUDIO_IDLE;
	}
	return;
}

/*
************************************************************************************************
* Function Name :	api_audio_menu
* Description   :	This function finds out where to store the audio file(wav)
* Arguments     : 	None
* Returns       : 	None
* Notes         : 	none
*
* Version	Date d/m/y	Programmer			Reason for Change
* 0.1.0		04/04/11	William Paul		Created
*
************************************************************************************************
*/
uint8_t api_audio_menu(void)
{
/* local variables */
	uint8_t			rec_status_u8;
	uint8_t			rec_char_u8;
	uint32_t 		file_add;


	rec_char_u8 = debug_getchar(0,&rec_status_u8);
	if(rec_status_u8){												/* Check if data is present */
		switch(rec_char_u8){
			case ' ':
				printf("\r\n***API Audio menu****");
				printf("\r\nEsc Exit Menu");
				printf("\r\nd   download file");
				printf("\r\np   play file");
				printf("\r\n+-  Volume");
				printf("\r\ncC  Chirp");
				printf("\r\nv   Low priority alarm");
                                printf("\r\nV   Medium priority alarm");
                                printf("\r\nb   High priority alarm");

				printf("\r\ns   Gen sine wave audio");
				printf("\r\nS   stop sine wave audio");
				break;
			case 'd':
				printf("\r\nEnter File add to download (0x)\r\n0x------\b\b\b\b\b\b");
				file_add = BSP_get_num(16,6);
				api_audio_file_download(file_add);
				break;
			case 'p':
				printf("\r\nEnter File add to play (0x)\r\n0x------\b\b\b\b\b\b");
				file_add = BSP_get_num(16,6);
				api_audio_play_file(file_add);
				break;
			case '-':
				api_audio_volume_per_change(AUDIO_AMP,-2);
				break;
			case '+':
				api_audio_volume_per_change(AUDIO_AMP,2);
				break;

			case 'c':	api_audio_chirp(1);	printf("\r\nChirp 1");	break;
			case 'C':	api_audio_chirp(2);	printf("\r\nChirp 2");	break;

			case 'v':   api_audio_alarm(ALARM_PRIORITY_LOW,    0); printf("\r\nLow priority alarm");    break;
                        
                        case 'V':   api_audio_alarm(ALARM_PRIORITY_MEDIUM, 0); printf("\r\nMedium priority alarm"); break;

                        case 'b':   api_audio_alarm(ALARM_PRIORITY_HIGH,   0); printf("\r\nHigh priority alarm");   break;

			case 'i':
				init_DAC1();
				api_audio.init	= 0;
				printf("\r\nDAC init");
				break;
			case 's':
				api_audio.state				=  AUDIO_START_PLAY1;
				api_audio.file_format		=  GEN_SINE_x1;	//sine mode
				printf("\r\nEnter Sine freq: ----\b\b\b\b");
				api_audio.sin_freq	=  BSP_get_num(10,4);

				api_audio.sin_amp				=  50;
				api_audio.file_sz_samples		=  4000;
				api_audio.DMA_buf_len			=  16;	//8kHz/500
				printf("\r\nSine wave %fHz Amp%d %dsamples",api_audio.sin_freq,api_audio.sin_amp,api_audio.file_sz_samples);
				break;
			case  'S':
				api_audio_play_file_stop();
				break;
			case '1':	AUDIO_AMP_ON();			printf("\r\nDAC amp on");			break;
			case '0':	AUDIO_AMP_OFF();		printf("\r\nDAC amp off");			break;
			case '4':	AUDIO_MUTE_OFF_LO();	printf("\r\nMute off");			break;			//turn off mute
			case '5':	AUDIO_MUTE_ON_HI();		printf("\r\nMute on");			break;			//mute enabled

			case 0x1b:	//esc
				return(0);
		}
	}
	api_audio_manager();
	return(1);
}


/*
************************************************************************************************
* Function Name :	api_audio_file_download
* Description   :	This function is used to save a wav file to memory
* Arguments     : 	None
* Returns       : 	None
* Notes         : 	none
*
* Version	Date d/m/y	Programmer			Reason for Change
* 0.1.0		24/07/13	Aaron Duignan		Created
*
************************************************************************************************
*/
#define	AUD_DOWNLOAD_BUF	0x100
void api_audio_file_download(uint32_t file_des_add)
{
	uint8_t		rec_status_u8;
	uint8_t		rec_char_u8;
	uint32_t	mem_add	=  file_des_add;
	uint32_t	bytes_rec;
	uint8_t		rx_buf[AUD_DOWNLOAD_BUF];
	uint16_t	i= 0;
	wav_file_t	wav_header;
	uint8_t		*ptr;



	printf("\r\nDownloading WAVE file now\r\n");

	csp_erase_64kb_sectors(file_des_add,1);

	ptr	=  (uint8_t*)&wav_header.h.ChunkID[0];
	for(i=0;i<sizeof(wav_header);i++){
		do{
			rec_char_u8 = debug_getchar(0,&rec_status_u8);
			app_sys_watchdog_reload();
		}while(rec_status_u8 == 0);
		*ptr	=  rec_char_u8;
		ptr++;

	}
	if(	(wav_header.h.ChunkID[0]	== 'R') &&
		(wav_header.h.ChunkID[1]	== 'I') &&
		(wav_header.h.ChunkID[2]	== 'F') &&
		(wav_header.h.ChunkID[3]	== 'F') 	){

		csp_mem_wr((uint8_t*)&wav_header.h.ChunkID[0] ,mem_add, sizeof(wav_header));
		mem_add	+= sizeof(wav_header);

		//debug print RIFF header
		printf("\r\nHeader \r\n");
		ptr	=  (uint8_t*)&wav_header.h.ChunkID[0];
		for(i=0;i<sizeof(wav_header);i++){
			printf("%2x ",*ptr);
			ptr++;
		}
		printf("\r\n");

		printf("\r\n%x \r\n",wav_header.dh.SubChunk2Size);
		i=0;
		for(bytes_rec=0;bytes_rec<wav_header.dh.SubChunk2Size;){
			rec_char_u8 = debug_getchar(0,&rec_status_u8);
			if(rec_status_u8){
				rx_buf[i]	=  rec_char_u8;
				i++;
				bytes_rec++;
			}
			if(i >= AUD_DOWNLOAD_BUF){
				printf("\r%x",bytes_rec);
				csp_mem_wr(&rx_buf[0] ,mem_add, sizeof(rx_buf));
				mem_add	+= AUD_DOWNLOAD_BUF;
				i	=  0;
			}
			app_sys_watchdog_reload();
		}
		csp_mem_wr(&rx_buf[0] ,mem_add, i);
		mem_add	+= i;

		printf("\r\nFile loc %x-%x",file_des_add,mem_add);
	}

	else{
		printf("\r\nDownload failed - Not a RIFF file");
	}
	return;
}



/*************************************************************************************************
* Function Name : 	api_audio_volume_per_wr
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	06/05/14		W. Paul			Created
*************************************************************************************************/
void api_audio_volume_per_wr(api_audio_Volume_type_enum source,uint8_t volume)
{
	if(volume > 100){	volume	=  100;	}
	switch(source){
		case AUDIO_AMP:		api_audio.volume_desired	=  (uint8_t)volume;		break;
		case AUDIO_DIG1:	api_audio.volume_dig1		=  (uint8_t)volume;		break;
		case AUDIO_DIG2:	api_audio.volume_dig2		=  (uint8_t)volume;		break;
	}

	return;
}

/*************************************************************************************************
* Function Name : 	api_audio_volume_per_rd
* Description   : 	This Function sets the desired volum on the chip
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
uint8_t api_audio_volume_per_rd(api_audio_Volume_type_enum source)
{
	switch(source){
		case AUDIO_AMP:		return(api_audio.volume_act);
		case AUDIO_DIG1:	return(api_audio.volume_dig1);
		case AUDIO_DIG2:	return(api_audio.volume_dig2);
	}

	return(0);
}

/*************************************************************************************************
* Function Name : 	api_audio_volume_per_change
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	06/05/14		W. Paul			Created
*************************************************************************************************/
void api_audio_volume_per_change(api_audio_Volume_type_enum source,int8_t volume_change)
{
	int8_t		volume;

	volume		=  	 api_audio_volume_per_rd(source);
	volume		+=  volume_change;

	if(volume < 0){		volume	=  0;	}
	if(volume > 100){	volume	=  100;	}

	api_audio_volume_per_wr(source,(uint8_t)volume);

	return;
}

/*************************************************************************************************
* Function Name : 	api_audio_volume_wr
* Description   : 	This Function sets the desired volum on the chip
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
void api_audio_volume_wr(void)
{
	uint8_t		temp_out;
	uint8_t		temp_in;

	if(api_audio.volume_act != api_audio.volume_desired){

	//change from percentage to 0-64 levels
		temp_out	=  api_audio.volume_desired;
		temp_out	/= 2;		//100% = 50
		temp_out	+= 13;		//13 is the quietest
		if(temp_out > 63){	temp_out	=  63;	}

		I2C_BufferWr(AUDIO_I2Cx, &temp_out, 1, I2C_MAX9768_ADD);
		I2C_BufferRd(AUDIO_I2Cx, &temp_in,	1, I2C_MAX9768_ADD);

	//change from  0-63 levels to percentage
		if(temp_in < 13){	temp_in	=  13;	}
		api_audio.volume_act	=  temp_in;
		api_audio.volume_act	-= 13;
		api_audio.volume_act	*= 2;

		printf("\r\nVol = des %d  act %d",api_audio.volume_desired,api_audio.volume_act);
	}
	return;
}



/*************************************************************************************************
* Function Name : 	api_audio_file_rd
* Description   : 	This Function
* Arguments     : 	uint32_t file_loc
* Returns       : 	0	file not useable
*				1	file is good
* Notes         :
	uint32_t		api_audio.file_address
	uint32_t		api_audio.file_sz_samples
	uint8_t			api_audio.file_format


* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	06/05/14		W. Paul			Created
*************************************************************************************************/
uint8_t	api_audio_file_rd(uint32_t file_loc)
{
	wav_file_t		WaveFileHeader; /*Contains all the header format for a wav file*/
	uint32_t		mem_add;

	mem_add	=  file_loc;

	csp_mem_rd((uint8_t*)&WaveFileHeader ,mem_add, sizeof(WaveFileHeader));
	if(  (WaveFileHeader.h.ChunkID[0] == 'R') &&
          (WaveFileHeader.h.ChunkID[1] == 'I') &&
          (WaveFileHeader.h.ChunkID[2] == 'F') &&
          (WaveFileHeader.h.ChunkID[3] == 'F')   ) {		//RIFF
		printf("\r\nAudio Play File");
		printf("\r\nNo of channels:  %d",WaveFileHeader.ih.NumChannels);	//Mono = 1, Stereo = 2, etc.
		printf("\r\nSample rate:     %d",WaveFileHeader.ih.Samplerate);		//8000, 44100, etc.
		printf("\r\nByte rate:       %d",WaveFileHeader.ih.ByteRate);		//== SampleRate * NumChannels * BitsPerSample/8
		printf("\r\nBlock align:     %d",WaveFileHeader.ih.BlockAlign);
		printf("\r\nbits per sample: %d",WaveFileHeader.ih.BitsPerSample);
		printf("\r\nno of samples:   %d",WaveFileHeader.dh.SubChunk2Size);

		WaveFileHeader.dh.SubChunk2Size	/= WaveFileHeader.ih.BitsPerSample;
		WaveFileHeader.dh.SubChunk2Size	*= 8;

		//bsp_AT45_rd_main_open(mem_chip,mem_page,mem_add);
		mem_add = mem_add + sizeof(WaveFileHeader);//set address to start of file

		api_audio.address_ptr		=  mem_add;						//next mem read from here
		api_audio.file_sz_samples	=  WaveFileHeader.dh.SubChunk2Size;	//no of samples
		api_audio.file_format		=  WaveFileHeader.ih.BitsPerSample;	//8/16 bit
		return(1);		//1 file is good
	}

	return(0);			//0 file is bad
}

/*************************************************************************************************
* Function Name : 	api_audio_TopUpBuf
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         :
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0	06/05/14		W. Paul			Created
*************************************************************************************************/
void api_audio_TopUpBuf(void)
{
	uint16_t	fill_qty;

	api_audio.DAC_DMA_buf_rd_pos	=  api_audio.sample_no % api_audio.DMA_buf_len;

	if(api_audio.sample_no == 0){
		fill_qty	=  api_audio.DMA_buf_len;
	}
	else{
		fill_qty	=  api_audio.DAC_DMA_buf_rd_pos;
		if(api_audio.DAC_DMA_buf_rd_pos < api_audio.DAC_DMA_buf_wr_pos){
			fill_qty	+= api_audio.DMA_buf_len;
		}
		fill_qty	-= api_audio.DAC_DMA_buf_wr_pos;
	}
	fill_qty	-= 4;


	do{
		api_audio_get_next_sample();
		fill_qty--;
	}while(fill_qty);

	return;
}

/*************************************************************************************************
* Function Name : 	api_audio_get_next_sample
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		05/10/17	W. Paul			Created
* 0.2.0         11/06/26        A. Kane                 Silence Handling Updated
*************************************************************************************************/
void api_audio_get_next_sample(void)
{
	double		x_sin;
	double		y_sin;

	uint16_t	i;
	union{
		uint8_t			uc_a[4];
		uint16_t		ui_b[2];
		uint32_t		ui_c;
	}u4_temp;

	switch(api_audio.file_format){
		case GEN_SINE_x1:
		case GEN_SINE_REPEAT:
			for(i=0;i<api_audio.DMA_buf_len;i++){
				api_audio.address_ptr	+=  1;
				x_sin	=  api_audio.address_ptr;
				x_sin	*= api_audio.sin_freq;
				x_sin	*= 6.284;	//2PI
				x_sin	/= 8000;
				y_sin = 1.00 * sin(x_sin)
                                        + 0.50 * sin(2.0 * x_sin)
                                        + 0.25 * sin(3.0 * x_sin)
                                        + 0.125 * sin(4.0 * x_sin);

				y_sin *= (api_audio.sin_amp / 1.875);
				y_sin	+= 0x0800;
				DMA_buf_u16[i]	=  (uint16_t)y_sin;
			}
			api_audio.file_format	+= 1;
			break;
		case GEN_SINE_x1_WAIT:
                case GEN_SINE_REPEAT_WAIT:
                    if(api_audio.sin_amp == 0){
                        for(i = 0; i < api_audio.DMA_buf_len; i++){
                            DMA_buf_u16[i] = 0x0800;    // midpoint = silence for this DAC
                        }
                    }
                        break;
		case 16:
			csp_mem_rd((uint8_t*)&u4_temp.uc_a ,api_audio.address_ptr, 2);
			api_audio.address_ptr	+=  2;
			u4_temp.ui_b[0]		+= 0x8000;
			u4_temp.ui_b[0]		=  u4_temp.ui_b[0] >> 4;
			u4_temp.ui_b[0]		&= 0x0fff;
			DMA_buf_u16[api_audio.DAC_DMA_buf_wr_pos]	=  u4_temp.ui_b[0];
			break;
		case 8:
			csp_mem_rd((uint8_t*)&u4_temp.uc_a ,api_audio.address_ptr, 1);
			api_audio.address_ptr += 1;
			u4_temp.ui_b[0]		=  u4_temp.ui_b[0] << 4;
			u4_temp.ui_b[0]		&= 0x0fff;
			DMA_buf_u16[api_audio.DAC_DMA_buf_wr_pos]	=  u4_temp.ui_b[0];
			break;
	}

	if(++api_audio.DAC_DMA_buf_wr_pos >= api_audio.DMA_buf_len){
		api_audio.DAC_DMA_buf_wr_pos	=  0;
	}
	return;
}

/*
************************************************************************************************
* Function Name :	api_audio_play_file
* Description   :	This function is used to play a wav file
* Arguments     : 	None
* Returns       : 	None
* Notes         : 	none
*
* Version	Date d/m/y	Programmer			Reason for Change
* 0.1.0		24/07/13	Aaron Duignan		Created
*
************************************************************************************************
*/
void api_audio_play_file(uint32_t file_loc)
{
	api_audio.file_address		=  file_loc;
	api_audio.state				=  AUDIO_START_PLAY;

	return;
}
/*
************************************************************************************************
* Function Name :	api_audio_play_file_stop
* Description   :	This function is used to play a wav file
* Arguments     : 	None
* Returns       : 	None
* Notes         : 	none
*
* Version	Date d/m/y	Programmer			Reason for Change
* 0.1.0		24/07/13	Aaron Duignan		Created
*
************************************************************************************************
*/
void api_audio_play_file_stop(void)
{
	if(api_audio.state){
		api_audio.stop_audio	=  1;
//		api_audio.state				=  AUDIO_STOP_PLAY;
//		api_audio.file_format		=  GEN_SINE_x1;
//		api_audio.file_sz_samples	=  0;
	}
	
	return;
}

/*
************************************************************************************************
* Function Name :	api_audio_play_status
* Description   :	This function is used to play a wav file
* Arguments     : 	None
* Returns       : 	None
* Notes         : 	none
*
* Version	Date d/m/y	Programmer			Reason for Change
* 0.1.0		24/07/13	Aaron Duignan		Created
*
************************************************************************************************
*/
api_audio_state_enum api_audio_play_status(void)
{

	return(api_audio.state);
}

/*
************************************************************************************************
* Function Name :		api_audio_dma_to_DAC1_start
* Description   :		This function is used initialise the DAC1 in the system.
* Arguments     : 		None Listed
* Returns       : 		None Listed
* Notes         : 		None Listed
*
* Version	Date d/m/y	Programmer	Reason for Change
* 0.1.0		04/10/10	W. Paul		Created
*
************************************************************************************************
*/
void api_audio_dma_to_DAC1_start(void)
{
	csp_STM32_DMA2_CH3_config((uint32_t)&DMA_buf_u16,api_audio.DMA_buf_len);
	TIM_Cmd(TIM2, ENABLE);				//start timer
	api_audio.sample_no	=  0;

	return;
}

/*
************************************************************************************************
* Function Name :		api_audio_dma_to_DAC1_stop
* Description   :		This function is used initialise the DAC1 in the system.
* Arguments     : 		None Listed
* Returns       : 		None Listed
* Notes         : 		None Listed
*
* Version	Date d/m/y	Programmer	Reason for Change
* 0.1.0		04/10/10	W. Paul		Created
*
************************************************************************************************
*/
void api_audio_dma_to_DAC1_stop(void)
{

	DMA_Cmd(DMA2_Channel3, DISABLE);
	TIM_Cmd(TIM2, DISABLE);
	api_audio.DAC_DMA_buf_wr_pos		=  0;


	return;
}


void api_audio_Timer_IRQ(void)
{
	if(api_audio.start_quiet > 0){
		if(--api_audio.start_quiet == 0){
			DMA_Cmd(DMA2_Channel3, ENABLE);
			AUDIO_MUTE_OFF_LO();			//turn off mute
		}
	}
	else{
		api_audio_get_next_sample();

		if(++api_audio.sample_no >= api_audio.file_sz_samples ){
			api_audio_dma_to_DAC1_stop();
			AUDIO_MUTE_ON_HI();	//mute enabled
            api_audio.state		=  AUDIO_STOP_PLAY;
		}
	}
}

/*************************************************************************************************
* Function Name : 	api_audio_chirp
* Description   : 	This Function is used when a button is pressed
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		22/01/18	W. Paul			Created
*************************************************************************************************/
void api_audio_chirp(uint8_t type)
{
	if(api_audio.state	==  AUDIO_IDLE){
		api_audio.state				=  AUDIO_START_PLAY1;
		api_audio.file_format		=  GEN_SINE_x1;	//sine mode

		switch(type)
		{
			case 1:		//touchscreen press
			  	api_audio.address_ptr		=  0;
				api_audio.sin_freq			=  500;	//Hz
				api_audio.sin_amp			=  api_audio.volume_dig1;		//10-100
				api_audio.sin_amp			*= 12;
				api_audio.sin_amp			+= 400;
				api_audio.file_sz_samples	=  200;	//8kHz	200 = 1/40sec
				api_audio.DMA_buf_len		=  16;	//8kHz/500
				break;
			case 2:		//touchscreen press	& hold
			  	api_audio.address_ptr		=  0;
				api_audio.sin_freq			=  1000;	//Hz
				api_audio.sin_amp			=  api_audio.volume_dig1;
				api_audio.sin_amp			*= 9;
				api_audio.sin_amp			+= 400;
				api_audio.file_sz_samples	=  75;	//8kHz	200 = 1/40sec
				api_audio.DMA_buf_len		=  8;	//8kHz/1000
				break;
			case 3:		//touchscreen option not avail
			  	api_audio.address_ptr		=  0;
				api_audio.sin_freq			=  400;	//Hz
				api_audio.sin_amp			=  api_audio.volume_dig1;
				api_audio.sin_amp			*= 9;
				api_audio.sin_amp			+= 400;
				api_audio.file_sz_samples	=  2000;	//8kHz	200 = 1/40sec
				api_audio.DMA_buf_len		=  20;		//8kHz/400
				break;
		}
	}
	
	return;
}

/*************************************************************************************************
* Function Name : 	api_audio_alarm
* Description   : 	This Function is used for alarms
* Arguments     : 	ALARM_PRIORITY_e priority, uint8_t step
* Returns       : 	uint8_t
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/06/2026	A.Kane			Created
*************************************************************************************************/

uint8_t api_audio_alarm(ALARM_PRIORITY_e priority, uint8_t step)
{
    if(priority >= 3){ 
        return 1; 
    }
    const alarm_profile_t *profile = &alarm_profiles[priority];
    if(step >= profile->steps){ 
        return 1; 
    }
    
    if( (step == 0) &&
        (api_audio.alarm_priority != priority) &&
        (api_audio.state != AUDIO_IDLE) ){
        api_audio_play_file_stop();
        api_audio_alarm_reset();
    }

    if(api_audio.state == AUDIO_IDLE)
    {
        if((step == 0) && (api_audio.stop_audio == 1)){
            api_audio.stop_audio = 0;
            return 1;
        }

        const alarm_step_t *s = &profile->seq[step];

        api_audio.state             = AUDIO_START_PLAY1;
        api_audio.address_ptr       = 0;
        api_audio.file_sz_samples   = s->dur_samples;
        api_audio.multiTone_part    = s->next_step;
        api_audio.alarm_priority    = priority; 

        if(s->is_silence){
            api_audio.file_format   = GEN_SINE_REPEAT_WAIT;
            api_audio.sin_amp       = 0;
            api_audio.DMA_buf_len   = 16;
            AUDIO_MUTE_ON_HI();
        } else {
            api_audio.file_format   = GEN_SINE_REPEAT;
            api_audio.sin_freq      = s->freq_hz;
            api_audio.sin_amp       = (api_audio.volume_dig2 * 15) + 400;
            api_audio.DMA_buf_len   = (8000 / s->freq_hz);
            AUDIO_MUTE_OFF_LO(); 
        }
    }
    else if((api_audio.state == AUDIO_STOP_PLAY)       ||
            (api_audio.state == AUDIO_PLAY_INPROGRESS) ){
        return 1;
    }

    return 0;
}

/*************************************************************************************************
* Function Name : 	api_audio_alarm_reset
* Description   : 	This Function is used for alarms reset
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		11/06/2026	A.Kane			Created
*************************************************************************************************/

void api_audio_alarm_reset(void)
{
    api_audio.multiTone_part  = 0;
    api_audio.stop_audio      = 0;
    api_audio.alarm_priority  = (uint8_t)ALARM_PRIORITY_LOW;
    api_audio.state           = AUDIO_IDLE;
    api_audio.file_format     = GEN_SINE_x1;
    api_audio.file_sz_samples = 0;
}

/*
*********************************************************************************************************
*											End of api_audio.c
*********************************************************************************************************
*/
