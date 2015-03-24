#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <aio.h>
#include <sys/statvfs.h>

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

  	struct statvfs sbuf;

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

   if (statvfs("/home/mp/aio_proj/aio_test_1.c", &sbuf) < 0)
   {
   	return -1;
   }
     
   printf("sbuf.f_bsize=%lu\n", sbuf.f_bsize);
   printf("sbuf_frsize=%lu\n", sbuf.f_frsize);
   printf("sbuf.f_blocks=%lu\n", sbuf.f_blocks);
   printf("sbuf.f_bfree=%lu\n", sbuf.f_bfree);
   printf("sbuf.f_bavail=%lu\n", sbuf.f_bavail);


   printf("Bytes left: %lu \n", sbuf.f_bsize * sbuf.f_bavail/1024);
	printf("Percent left: %f: \n", (double) (sbuf.f_blocks - sbuf.f_bfree) / (double) (sbuf.f_blocks - sbuf.f_bfree + sbuf.f_bavail) *100.0);

	return 0;
}
