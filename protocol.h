#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

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
int bsend (const int h, const char * buf, const unsigned __int32 num);
//
STATE bwait (const int h);
//
void * get_header (const int h);
void * get_data (const int h);
//
unsigned __int32 get_version (const int h);
unsigned __int32 get_packet_length (const int h);
unsigned __int32 get_platform (const int h);
unsigned __int32 get_frame_number (const int h);
unsigned __int32 get_clock_counter (const int h);
unsigned __int32 get_number_of_objects (const int h);
unsigned __int32 get_number_of_tlvs (const int h);
//
unsigned __int32 get_tlv_type (const int h, unsigned int idx);
unsigned __int32 get_tlv_length (const int h, unsigned int idx);
void * get_tlv_data (const int h, unsigned int idx);

typedef struct _DSC001
{	unsigned __int32 NumberOfObjects:16;
	unsigned __int32 Qformat:16;
}	DSC001;

typedef struct _OBJ001
{	unsigned __int32 RangeIndex:16;
	unsigned __int32 DopplerIndex:16;
	unsigned __int32 PeakValue:16;
	unsigned __int32 x:16;
	unsigned __int32 y:16;
	unsigned __int32 z:16;
}	OBJ001;

#endif	//	 _PROTOCOL_H_
