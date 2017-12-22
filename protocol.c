#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include "protocol.h"

///////////////////////////////////////////////////////////////////////////////

typedef unsigned char MAGIC [8];

typedef struct _RADAR_HDR				//	36 bytes	header
{	MAGIC Magic;						//	8 bytes		-> header sync
	unsigned __int32 Version;			//	4 bytes		-> check the version
	unsigned __int32 PacketLength;		//	4 bytes		-> packet's totoal size
	unsigned __int32 Platform;			//	4 bytes		= 0xA1443
	unsigned __int32 FrameNumber;		//	4 bytes
	unsigned __int32 ClockCounter;		//	4 bytes
	unsigned __int32 NumberOfObjects;	//	4 bytes		unused?
	unsigned __int32 NumberOfTLVs;		//	4 bytes		what's TLV?
}	RADAR_HDR;

typedef struct _RADAR_TLV				//	8 bytes TLV header
{	unsigned __int32 Type;				//	4 bytes type
	unsigned __int32 Length;			//	4 bytes length
}	RADAR_TLV;							//	follows by <Lenght> bytes data block

typedef struct _RADAR_DSC
{	unsigned __int32 NumberOfObjects;	//	2 or 4? Same as before or different?
	unsigned __int32 Qformat;			//	Fixed-point position
}	RADAR_DSC;

MAGIC magic = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};

///////////////////////////////////////////////////////////////////////////////

#define BACKETTAG	__LINE__

///////////////////////////////////////////////////////////////////////////////

typedef	struct _BACKET
{	unsigned __int32 tag;
	unsigned __int32 siz;		//	maximum size
	unsigned __int32 bytes;		//	bytes loaded
	STATE state;				//	states
	char buf [4];				//	variable size data buffer
}	BACKET;

static unsigned __int32 move_head (BACKET * backet, const unsigned __int32 chunk);

///////////////////////////////////////////////////////////////////////////////

static unsigned __int32 move_head (BACKET * backet, unsigned __int32 chunk)
{	if (chunk > backet->bytes) chunk = backet->bytes;
	backet->bytes -= chunk;
	memcpy (&backet->buf [0], &backet->buf [chunk], backet->bytes);
	return chunk;}

///////////////////////////////////////////////////////////////////////////////
//	open the "backet"

int bopen (const int siz)
{	BACKET * backet;
	backet = (BACKET *) malloc (sizeof (*backet) - sizeof(backet->buf) + siz);
	if (backet)
	{	backet->tag = BACKETTAG;
		backet->state = NOSYNC;		//	state: no sync
		backet->siz = siz;			//	total size
		backet->bytes = 0U;}		//	data bytes
	return (int) backet;}

///////////////////////////////////////////////////////////////////////////////
//	close the "backet"

int bclose (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG))
	{	free ((void *) backet);
		return 1;}
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	send (append) data bytes to the backet 

int	bsend (const int h, const char * buf, const unsigned __int32 bytes)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG))
	{	if (backet->bytes + bytes <= backet->siz)
		{	memcpy (&(backet->buf[backet->bytes]), buf, bytes);
			backet->bytes += bytes;}
		return 1;}
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	wait for the states' changes

STATE bwait (const int h)
{	BACKET * backet = (BACKET *) h;
	unsigned int bytes;
	if ((backet) && (backet->tag == BACKETTAG))
	{	switch (backet->state)
		{	case NOSYNC:
				while (backet->bytes >= sizeof(_RADAR_HDR))
				{	unsigned int i;
					if (backet->buf [0] != magic[0])
					{	for (i = 1; i < backet->bytes && backet->buf [i] != magic[0]; i++);
						move_head (backet, i);}
				else
				{	for (i = 1; i < sizeof (MAGIC) && backet->buf [i] == magic [i]; i++);
					if (i < sizeof (MAGIC)) move_head (backet, i);
					else 
					{	backet->state = HDRRDY;
						break;}}}
				break;
			case HDRRDY:
				if (backet->bytes >= ((RADAR_HDR *) &(backet->buf[0]))->PacketLength)
					backet->state = DATRDY;
				break;
			case DATRDY:
				bytes = ((RADAR_HDR*)&(backet->buf[0]))->PacketLength;
				if (backet->bytes >= bytes)
				{	move_head (backet, bytes);
					backet->state = NOSYNC;}
				break;}
		return backet->state;}
	return UNDEF;}

///////////////////////////////////////////////////////////////////////////////
//	get header

void * get_header (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= HDRRDY)
		return (void *) & (backet->buf[0]);
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get version

unsigned __int32 get_version (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= HDRRDY)
		return ((RADAR_HDR *)&(backet->buf[0]))->Version;
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get packet length

unsigned __int32 get_packet_length (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= HDRRDY)
		return ((RADAR_HDR *)&(backet->buf[0]))->PacketLength;
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get packet lenght

unsigned __int32 get_platform (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= HDRRDY)
		return ((RADAR_HDR *)&(backet->buf[0]))->Platform;
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get frame number

unsigned __int32 get_frame_number (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= HDRRDY)
		return ((RADAR_HDR *)&(backet->buf[0]))->FrameNumber;
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get clock counter

unsigned __int32 get_clock_counter (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= HDRRDY)
		return ((RADAR_HDR *)&(backet->buf[0]))->ClockCounter;
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get number of objects

unsigned __int32 get_number_of_objects (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= HDRRDY)
		return ((RADAR_HDR *)&(backet->buf[0]))->NumberOfObjects;
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get number of TLVs

unsigned __int32 get_number_of_tlvs (const int h)
{	BACKET * backet = (BACKET *) h;
	unsigned __int32 n = 0; 
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= HDRRDY)
	{	n = ((RADAR_HDR *)&(backet->buf[0]))->NumberOfTLVs;}
	return n;}

///////////////////////////////////////////////////////////////////////////////
//	get data

void * get_data (const int h)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state >= DATRDY)
		return (void *) ((unsigned char *) & (backet->buf[0]) + sizeof (RADAR_HDR));
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get TLV type

unsigned __int32 get_tlv_type (const int h, unsigned int idx)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state == DATRDY && idx < get_number_of_tlvs(h))
	{	RADAR_TLV * ptlv = (RADAR_TLV *)((unsigned char *)&(backet->buf[0]) + sizeof (RADAR_HDR));
		unsigned int i;
		if (ptlv->Type > 3)
			return 0;
		for (i = 0; i < idx; i++)
		{	ptlv = (RADAR_TLV *)((unsigned char *) ptlv + sizeof (RADAR_TLV) + ptlv->Length);
			if (ptlv->Type > 3) 
				return 0;}
		return ptlv->Type;}
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get TLV length

unsigned __int32 get_tlv_length (const int h, unsigned int idx)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state == DATRDY && idx < get_number_of_tlvs(h))
	{	RADAR_TLV * ptlv = (RADAR_TLV *)((unsigned char *)&(backet->buf[0]) + sizeof (RADAR_HDR));
		unsigned int i;
		if (ptlv->Type > 3)
			return 0;
		for (i = 0; i < idx; i++)
		{	ptlv = (RADAR_TLV *)((unsigned char *) ptlv + sizeof (RADAR_TLV) + ptlv->Length);
			if (ptlv->Type > 3)
				return 0;}
		return ptlv->Length;}
	return 0;}

///////////////////////////////////////////////////////////////////////////////
//	get TLV Data

void * get_tlv_data (const int h, unsigned int idx)
{	BACKET * backet = (BACKET *) h;
	if ((backet) && (backet->tag == BACKETTAG) && backet->state == DATRDY && idx < get_number_of_tlvs(h))
	{	RADAR_TLV * ptlv = (RADAR_TLV *)((unsigned char *)&(backet->buf[0]) + sizeof (RADAR_HDR));
		unsigned int i;
		if (ptlv->Type > 3)
			return 0;
		for (i = 0; i < idx; i++)
		{	ptlv = (RADAR_TLV *)((unsigned char *) ptlv + sizeof (RADAR_TLV) + ptlv->Length);
			if (ptlv->Type > 3) 
				return 0;}
		return (void *) ((unsigned char *) ptlv + sizeof (RADAR_TLV));}
	return 0;}
