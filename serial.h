#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <termios.h>

typedef void (*COMMCB)(void * param, const unsigned char * buf, const unsigned int size);
int comm_open (const char * devname, unsigned int speed, unsigned int parity, COMMCB pRDCB, void * pParam, unsigned int rdInterval, unsigned int rdTimeout, unsigned int wrTimeoput);
int comm_recv (int h, void * buf, int size);
void comm_close (int h);

#endif
