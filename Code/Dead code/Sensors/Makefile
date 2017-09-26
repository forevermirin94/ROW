CC=gcc
CCFLAGS=-Wall -Werror -g -I/usr/local/include


LD_LIBS= -lcurses -lwiringPi -lwiringPiDev -lpthread -lm  
LD_FLAGS=-L/usr/local/lib


OUTDIR=./bin

all:  sensors

%:    %.c 
	$(CC) $(CCFLAGS) $(LD_FLAGS) $(LD_LIBS)  -o $(OUTDIR)/$@ $<  		


 
