include ../Makefile.inc

API=-I../API -I./include
LIBRARIES=-L../syscalls/lib

all: libc

libc:
	$(GCC) $(GCCFLAGS) $(API) $(LIBRARIES) -lsyscalls -c libc.c
	$(AR) $(ARFLAGS) ../API/lib/libc.a libc.o

clean:
	rm -rf *.o
	rm -f lib/*.a

.PHONY: all clean
