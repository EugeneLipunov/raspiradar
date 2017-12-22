#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "serial.h"
#include "protocol.h"

#define DEVNAME "/dev/ttyUSB1"
#define BAUDRATE 921600
 
void rdcb (void * param, const unsigned char * buf, const unsigned int size)
{
	 printf("%u bytes\n", size);
}
 
int main (int argc, char* argv [])
{
	int h;
	
	h = comm_open (DEVNAME, 921600, &rdcb, 0, 5, 5, 5);
	sleep (5);
	comm_close (h);
	printf("exit\n");
	
	return 0;
}

