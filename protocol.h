#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

//	STATE
typedef enum _STATE
{	UNDEF = 0,		//	undefined state
	NOSYNC = 1,		//	looking for the magic
	HDRRDY = 2,		//	header is ready
	DATRDY = 3,		//	data is ready
	DSCTAG = 4		//	8 bytes, fixed
}	STATE;
//
int bopen (const int siz);
int bclose (const int h);
int bsend (const int h, const char * buf, const uint32_t num);
//
STATE bwait (const int h);
//
void * get_header (const int h);
void * get_data (const int h);
//
uint32_t get_version (const int h);
uint32_t get_packet_length (const int h);
uint32_t get_platform (const int h);
uint32_t get_frame_number (const int h);
uint32_t get_clock_counter (const int h);
uint32_t get_number_of_objects (const int h);
uint32_t get_number_of_tlvs (const int h);
//
uint32_t get_tlv_type (const int h, uint32_t idx);
uint32_t get_tlv_length (const int h, uint32_t idx);
void * get_tlv_data (const int h, uint32_t idx);

typedef struct _DSC001
{	uint32_t NumberOfObjects:16;
	uint32_t Qformat:16;
}	DSC001;

typedef struct _OBJ001
{	uint32_t RangeIndex:16;
	uint32_t DopplerIndex:16;
	uint32_t PeakValue:16;
	uint32_t x:16;
	uint32_t y:16;
	uint32_t z:16;
}	OBJ001;

#endif	//	 _PROTOCOL_H_
