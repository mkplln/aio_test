#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <aio.h>

#define MAX_REQUEST	(32)

#define WIDTH			(640)
#define HEIGHT			(512)

uint16_t writeBuffers[MAX_REQUEST][WIDTH*HEIGHT];

uint16_t dataBuffers[MAX_REQUEST][WIDTH*HEIGHT];

int out_fd = -1;

int frameCount = -1;

char *outFilename = "testData.dat";



int main()
{
	struct aioinit aioInit;
	struct aiocb aiocbp[MAX_REQUEST];

	int i = -1;

	memset(&aioInit, 0, sizeof(aioInit));
	aioInit.aio_threads = 1;
	aioInit.aio_num = MAX_REQUEST;
	aio_init(&aioInit);


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
 		memset(writeBuffers[i], 0, 2*WIDTH*HEIGHT);
	
		/* Zero aiocb */
   	memset(&aiocbp[i], 0, sizeof(struct aiocb));

   	aiocbp[i].aio_fildes = out_fd;
   	aiocbp[i].aio_nbytes = 2*WIDTH*HEIGHT;
   	aiocbp[i].aio_buf = writeBuffers[i];
	}


	for (i = 0; i < MAX_REQUEST; i++)
	{

   	if(posix_memalign((void**)&dataBuffers, 4096, sizeof(2*WIDTH*HEIGHT)) != 0)
   	{
   		printf("Error: memalign\n");
      	exit(EXIT_FAILURE);
   	} 
	
    	if(dataBuffers[i] == NULL)
    	{
      	printf("Error: memalign\n");
      	exit(EXIT_FAILURE);
    	}   
	
		memset(dataBuffers[i], i, 2*WIDTH*HEIGHT);
		
		printf("%d: %x...\n", i, dataBuffers[i][0]);
	}
	
	
	return 0;
}
