#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "serial.h"
#include "protocol.h"

#define DEVNAME "/dev/ttyUSB1"
#define BAUDRATE 921600

#if 1
void dump (void * param, const unsigned char * buf, const unsigned int bytes)
{	enum {BITS = 5};
	unsigned int i, j, n, m;
	printf ("bytes: %u\n", bytes);
	for (i = 0; i < bytes; i += (1<<BITS))
	{	n = (i + (1<<BITS) <= bytes) ? (1<<BITS): (bytes&((1<<BITS)-1));
		m = (1<<BITS) - n;
		for (j = 0; j < n; j++)	printf ("%2.2x ", buf[i+j]);
		for (j = 0; j < m; j++)	printf ("   ");
		for (j = 0; j < n; j++)	printf ("%c",  ((buf[i+j] >= 0x20) && (buf[i+j] <= 0x7E)) ? buf[i+j]: '.');
		for (j = 0; j < m; j++)	printf (" ");
		printf ("\n");}}
#endif

#define X2F(x,q)	((double)(x)/(double)(1<<(q)))
 
void rdcb (void * param, const unsigned char * buf, const unsigned int siz)
{	//	attach parser
	int h = (int) param;	
	uint32_t i, n, frame;
	STATE state, oldstate = UNDEF;
	if (buf == 0) return;
	if (siz == 0) return;
	//	try to load all data into the backet
	bsend (h, (char *) buf, siz);	
	while ((state = bwait (h)) != oldstate)
	{	switch (state)
		{	default:
			case NOSYNC: break;		
			case HDRRDY: break;		
			case DATRDY:
				n = get_number_of_tlvs (h);
				frame = get_frame_number (h);
				for (i = 0; i < n; i++)
				{	uint32_t j, m, q;
					DSC001 * pdsc;
					OBJ001 * pobj;
					if (get_tlv_type (h, i) != 1) continue;
					pdsc = (DSC001 *) get_tlv_data (h, i);
					pobj = (OBJ001 *) ((unsigned char *) pdsc + sizeof (DSC001));
					if (pdsc == 0) continue;
					if (pobj == 0) continue;
					m = pdsc->NumberOfObjects;
					q = pdsc->Qformat;
					if (m == 0) continue;
					for (j = 0; j < m; j++)
					{	double tmp_p, tmp_x, tmp_y;
						int16_t tmp; 
						tmp = pobj[j].PeakValue;
						tmp_p = X2F (tmp, q);
						tmp = pobj[j].x;
						tmp_x = X2F (tmp, q);
						tmp = pobj[j].y;
						tmp_y = X2F (tmp, q);
						printf ("%4.4u:%2.2u: peak %2.2f, x:%2.2f, y:%2.2f\n", frame, j, tmp_p, tmp_x, tmp_y);}
					break;}
				break;}
		oldstate = state;}}
 
int main (int argc, char* argv [])
{
	int h;
	int backet;
	
	backet = bopen (16*8192);
	
	h = comm_open (DEVNAME, B921600, 0, rdcb /* dump */, (void *) backet, 5, 5, 5);
	
	sleep (1);
	
	comm_close (h);
	
	bclose(backet);
	
	return 0;
}

