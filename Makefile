# Makefile for testing POSIX AIO functionality

.PHONY: clean

CC       = gcc
LD		   = gcc

LIBS		= -lrt

CFLAGS	= -c -Wall

C_EXT = c
OBJ_EXT = o
EXE_EXT = exe

EXE = aio_test_1.$(EXE_EXT)  
OBJECTS = aio_test_1.$(OBJ_EXT) 

%.o : %.c
	$(CC) $(CFLAGS) $*.$(C_EXT) -o $*.$(OBJ_EXT)

aio_test_1.$(EXE_EXT): aio_test_1.$(OBJ_EXT) $(DRIVER)
	$(LD) -o aio_test_1.$(EXE_EXT) $^ $(LIBS) 


all: $(EXE)
	

clean:
	rm -f $(OBJECTS) $(EXE)
