# parallel makefile, requires MPI

#CC := gcc -g -O3 -D HIGGS #-D TRIPLET #-D DEBUG

CC := mpicc -g -O3 -D MPI -D HIGGS -D TRIPLET

#CC := mpicc -ggdb3 -g -O3 -D MPI

CFLAGS := 

LIBS := -lm

OBJECTS := main.o layout.o comms.o alloc.o init.o parameters.o su2.o measure.o \
	update.o metropolis.o heatbath.o overrelax.o

BINARY := su2

FILES := $(OBJECTS) $(BINARY)

all: su2

su2: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o $(BINARY)

clean:
	rm -f $(FILES) *\~ *\#
