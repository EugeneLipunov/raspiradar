#include <stdio.h>
#include "serial.h"
#include "protocol.h"

#define DEVNAME "/dev/ttyUSB1"
#define BAUDRATE 921600
 
 void CB (void * param, const unsigned char * buf, const unsigned int size)
 {
	 
 }
 
int main (int argc, char* argv [])
{
	int h;
	

	h = comm_open (DEVNAME, 921600, CB, 0, 5, 5, 5);

	
	comm_close (h);
	
	return 0;}

