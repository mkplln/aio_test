#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <aio.h>
#include <signal.h>
#include <errno.h>

#define MAX_REQUEST	(32)

#define WIDTH			(640)
#define HEIGHT			(512)

uint16_t writeBuffers[MAX_REQUEST][WIDTH*HEIGHT];

uint16_t dataBuffers[MAX_REQUEST][WIDTH*HEIGHT];

int out_fd = -1;

int frameCount = -1;

char outFilename[255] = "testData";

const char FILE_EXT[255] = ".dat";



int stopFlag = 0;

void sigHandler(int val)
{
  stopFlag = 1;
  close(out_fd);
}

void registerHandlers()
{
  /* Handle interrupts from keyboard */
  printf("Registering event handlers...\n");
  signal(SIGTERM, sigHandler);
  signal(SIGABRT, sigHandler);
  signal(SIGQUIT, sigHandler);
  signal(SIGINT,  sigHandler);
  signal(SIGHUP,  sigHandler);
  signal(SIGTSTP, sigHandler);
}


int main()
{
	struct aioinit aioInit;
	struct aiocb aiocbp[MAX_REQUEST];

	char tmp[255];

	int i = -1;
	
	int err;

	size_t k = 0;

	registerHandlers();

	memset(&aioInit, 0, sizeof(aioInit));
	aioInit.aio_threads = 5;
	aioInit.aio_num = MAX_REQUEST;
	aio_init(&aioInit);

	/* Open initialize output file */
	frameCount = 0;
	sprintf(tmp, "%d", frameCount);

	strcat(outFilename, tmp);
	strcat(outFilename, FILE_EXT);

	out_fd = open(outFilename, O_CREAT|O_TRUNC|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);	

  	if(out_fd < 0) 
  	{
   	printf("Couldn't open output file for writing, exiting!\n");
   	exit(EXIT_FAILURE);
  	}


	for (i = 0; i < MAX_REQUEST; i++)
	{
   	if(posix_memalign((void**)&writeBuffers, 4096, sizeof(2*WIDTH*HEIGHT)) != 0)
   	{
   		printf("Error: memalign\n");
      	exit(EXIT_FAILURE);
   	} 
	
    	if(writeBuffers[i] == NULL)
    	{
      	printf("Error: memalign\n");
      	exit(EXIT_FAILURE);
    	}   

		/* Zero writeBuffers */
 		memset(writeBuffers[i], i, 2*WIDTH*HEIGHT);
	
		/* Zero aiocb */
   	memset(&aiocbp[i], 0, sizeof(struct aiocb));

   	aiocbp[i].aio_fildes = out_fd;
   	aiocbp[i].aio_nbytes = 2*WIDTH*HEIGHT;
   	aiocbp[i].aio_buf = writeBuffers[i];
	}

	while (stopFlag == 0)
	{

 		err = aio_error(&aiocbp[k]);
 		if(err == EINPROGRESS)
  		{
    		printf("Error: Disk throughput cannot keep up at write buffer %lu!\n", k);
    		return -1;
  		}             
  		else
  		{

			printf("%d: %x...\n", k, writeBuffers[k][0]);
    		err = aio_write(&aiocbp[k]);
    		if(err) 
    		{
      		printf("Error: aio_error() %lu : %d\n", k, err);
				printf("aio_filedes=%d\n", aiocbp[k].aio_fildes);
      		return -1;      
    		}      
  		}
          
  		k = (k+1) % MAX_REQUEST;

		usleep(4000);
	}
	
	return 0;
}
