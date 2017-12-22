#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <memory.h>
#include <pthread.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "serial.h"

#define MAGIC __LINE__ 

typedef struct _CTX
{	int magic;
	int h;
	pthread_t th;	//	thread handle
	volatile COMMCB rdcb;
	void * pParam;
}	CTX;
  
 static CTX * get_ctx (int h)
 {	 CTX * ctx = (CTX *) h;
	 if (ctx != 0 && ctx->magic == MAGIC) return ctx;
	 return (CTX *) 0;}

static void * comm_proc (CTX * ctx)
{	enum {BUFFSIZE = 8192};
	static int retval = 0;
	if (ctx == 0)
		pthread_exit (&retval);
	unsigned char buf [BUFFSIZE];
	unsigned int size;
	while (ctx->rdcb != 0)
	{	size = BUFFSIZE;
		if (size != 0 && comm_recv ((int) ctx, buf, size) != 0)
		{	if (size != 0 && ctx->rdcb != 0)
			ctx->rdcb (ctx->pParam, buf, size);}}
	pthread_exit (&retval);}
 
int comm_open (const char * devname, unsigned int baudrate, COMMCB rdcb, void * pParam, unsigned int rdInterval, unsigned int rdTimeout, unsigned int wrTimeoput)
{	CTX * ctx;
	struct termios opt;
	if (devname == 0) return 0;
	ctx= malloc (sizeof (CTX));
	if (ctx == 0) return 0;
	memset (ctx, 0, sizeof (CTX));
	ctx->h = open (devname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (ctx->h < 0)
	{	free ((void *) ctx);
		return 0;}
	tcgetattr (ctx->h, &opt);
	opt.c_cflag = baudrate | CS8 | CLOCAL | CREAD;
	opt.c_iflag = IGNPAR;
	opt.c_oflag = 0;
	opt.c_lflag = 0;
	tcflush (ctx->h, TCIFLUSH);
	if (tcsetattr (ctx->h, TCSANOW, &opt) != 0)
	{	free ((void *) ctx);
		return 0;}
	ctx->rdcb = rdcb;
	ctx->magic = MAGIC;
	pthread_create (&ctx->th, NULL, (void *) &comm_proc, (void *) ctx);
	return (int) ctx;}

int comm_recv (int h, void * buf, int size)
{	CTX * ctx = get_ctx (h);
	if (ctx != 0 && buf != 0 && size != 0)
		return read (ctx->h, buf, size);
	return 0;}

void comm_close (int h)
{	CTX * ctx = (CTX *) get_ctx (h);
	if (ctx) 
	{	ctx->rdcb;
		free ((void *) ctx);
		ctx = 0;}}



