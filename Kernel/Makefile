include Makefile.inc

KERNEL = kernel.bin
SOURCES_C = $(wildcard *.c)
SOURCES_DRIVERS_C = $(wildcard drivers/*.c)
SOURCES_ASM = $(wildcard asm/*.asm)
SOURCES = $(SOURCES_C) $(SOURCES_DRIVERS_C) $(SOURCES_ASM)
OBJECTS = $(SOURCES_C:.c=.o) $(SOURCES_ASM:.asm=.o) $(SOURCES_DRIVERS_C:.c=.o)
LOADERSRC = loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin
	rm -rf drivers/*.o *.o *.bin

.PHONY: all clean
