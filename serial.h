#ifndef _SERIAL_H_
#define _SERIAL_H_

typedef void (*COMMCB)(void * param, const unsigned char * buf, const unsigned int size);
int comm_open (const char * devname, unsigned int baudrate, COMMCB pRDCB, void * pParam, unsigned int rdInterval, unsigned int rdTimeout, unsigned int wrTimeoput);
int comm_recv (int h, void * buf, int size);
void comm_close (int h);

#endif	_SERIAL_H_