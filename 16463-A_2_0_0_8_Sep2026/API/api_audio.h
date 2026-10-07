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
 * Filename    :  api_audio.h
 * Date Created:  Wed 06 Sep 2017 09:10:33 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
 #ifndef _API_AUDIO_H
#define _API_AUDIO_H

#include <stdint.h>
#include "pcb_pins.h"


/**********************************************************************************************************
											#defines
 **********************************************************************************************************/
//#define I2C_MAX9768_ADD			0x94	//addr2= 0	addr1 = 1
//#define I2C_MAX9768_ADD			0x95	//addr2= 1	addr1 = 0
#define I2C_MAX9768_ADD				0x96	//addr2= 1	addr1 = 1

#define	API_AUDIO_INIT				0x98

#define	START_QUIET_PERIOD			400 //400 = 1/20s = 50ms

#define	GEN_SINE_x1					33
#define	GEN_SINE_x1_WAIT			34
#define	GEN_SINE_REPEAT				35
#define	GEN_SINE_REPEAT_WAIT		36


#define	DMA_BUF_SZ					200
/**********************************************************************************************************
 **********************************************************************************************************/
#define AUDIO_POWER_ON_HI()		asm("nop");
#define AUDIO_POWER_OFF_LO()	asm("nop");

#define AUDIO_MUTE_ON_HI()		PinSet(SPK_MUTE,1)	//mute speaker
#define AUDIO_MUTE_OFF_LO()		PinSet(SPK_MUTE,0)	//turn off mute

#define AUDIO_SHDN_OFF_HI()		PinSet(SPK_SHDN,1)	//Device shutdown off
#define AUDIO_SHDN_ON_LO()		PinSet(SPK_SHDN,0)

#define AUDIO_AMP_ON()			AUDIO_POWER_ON_HI(); 							AUDIO_SHDN_OFF_HI();
#define AUDIO_AMP_OFF()			AUDIO_POWER_OFF_LO(); 	AUDIO_MUTE_ON_HI();		AUDIO_SHDN_ON_LO();

/**********************************************************************************************************
 **********************************************************************************************************/
typedef struct{
	uint8_t	ChunkID[4];			//should be the letters	"RIFF"
	uint32_t	ChunkSize;		// = 36 + SubChunk2Size,
								//or more precisely:   4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
								//This is the size of the rest of the chunk following this number.
								//This is the size of the entire file in bytes minus 8 bytes
								//for the two fields not included in this count: ChunkID and ChunkSize.
	uint32_t	Format;			//should be the letters	"WAVE"
}wav_file_header_t;

//The WAVEINFOHEADER structure contains information about the wav file
typedef struct{
	uint32_t	SubChunk1ID;	//should be the letters	"fmt "
	uint32_t	SubChunk1Size;
	uint16_t	AudioFormat;	//PCM = 1 (i.e. Linear quantization). Values other than 1 indicate some form of compression.
	uint16_t	NumChannels;	//Mono = 1, Stereo = 2, etc.
	uint32_t	Samplerate;		//8000, 44100, etc.
	uint32_t	ByteRate;		//== SampleRate * NumChannels * BitsPerSample/8
	uint16_t	BlockAlign;
	uint16_t	BitsPerSample;	//8, 16, etc.
}wav_file_info_header_t;				//fmt sub-chunk

//The WAVEINFOHEADER structure contains information about the wav file
typedef struct{
	uint32_t	SubChunk2ID;	//should be the letters	"data"
	uint32_t	SubChunk2Size;	//== NumSamples * NumChannels * BitsPerSample/8
								//This is the number of bytes in the data.
								//You can also think of this as the size of the read of the subchunk following this number.
}wav_file_data_header_t;				//data sub-chunk

typedef struct{
	wav_file_header_t			h;
	wav_file_info_header_t		ih;
	wav_file_data_header_t		dh;
}wav_file_t;


/**********************************************************************************************************
 **********************************************************************************************************/
typedef enum{
	AUDIO_IDLE				=  0,
	AUDIO_START_PLAY		=  10,
	AUDIO_START_PLAY1		=  11,
	AUDIO_PLAY_INPROGRESS	=  20,
	AUDIO_STOP_PLAY			=  30,
}api_audio_state_enum;

typedef enum {
    ALARM_PRIORITY_LOW    = 0,
    ALARM_PRIORITY_MEDIUM = 1,
    ALARM_PRIORITY_HIGH   = 2,
} ALARM_PRIORITY_e;

typedef enum{
	AUDIO_AMP,
	AUDIO_DIG1,
	AUDIO_DIG2,
}api_audio_Volume_type_enum;

typedef struct
{
	uint8_t					init;
	api_audio_state_enum	                state;

	uint8_t					volume_desired;
	uint8_t					volume_act;

	uint8_t					volume_dig1;
	uint8_t					volume_dig2;

	uint32_t				start_quiet;
	uint8_t					stop_audio;

	uint32_t				address_ptr;
	uint32_t				sample_no;

	uint16_t				DAC_DMA_buf_wr_pos;
	uint16_t				DAC_DMA_buf_rd_pos;
	uint16_t				DMA_buf_len;

	uint32_t				file_address;
	uint32_t				file_sz_samples;
	uint8_t					file_format;

	uint32_t				timer_ms;

	double					sin_freq;
	uint16_t				sin_amp;

	uint8_t                                 multiTone_part;
        uint8_t                                 alarm_priority;
}api_audio_t;



/**********************************************************************************************************
 **********************************************************************************************************/
void 	api_audio_manager(void);
void 	api_audio_Timer_IRQ(void);

uint8_t	api_audio_menu(void);


void 	api_audio_play_file(uint32_t file_loc);
void 	api_audio_play_file_stop(void);

uint8_t api_audio_volume_per_rd(api_audio_Volume_type_enum source);
void 	api_audio_volume_per_wr(api_audio_Volume_type_enum source,uint8_t volume);
void 	api_audio_volume_per_change(api_audio_Volume_type_enum source,int8_t volume_change);


api_audio_state_enum api_audio_play_status(void);

void	api_audio_chirp(uint8_t type);

uint8_t  api_audio_alarm(ALARM_PRIORITY_e priority, uint8_t step);
void     api_audio_alarm_reset(void);

#endif	// __API_AUDIO_H


/*
*********************************************************************************************************
*											End of api_audio.h
*********************************************************************************************************
*/
