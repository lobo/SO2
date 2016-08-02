include ../Makefile.inc

all: int80 syscalls link

syscalls:
	$(GCC) $(GCCFLAGS) -I../API -c syscalls.c

int80:
	$(ASM) $(ASMFLAGS) int80.asm -o int80.o

link:
	$(AR) $(ARFLAGS) ../API/lib/libsyscalls.a int80.o syscalls.o

clean:
	rm -rf *.o 
	rm -rf lib/*.a

.PHONY: all clean
