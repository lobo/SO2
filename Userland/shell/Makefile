include ../Makefile.inc

MODULE=shellModule.bin
SOURCES=$(wildcard *.c)
GAME_SOURCES = $(wildcard ../game/*.c)

INCLUDE=-I../libc/include -I../API -I../game
LIBS=-L../API/lib -L../syscalls/lib

all: $(MODULE)

$(MODULE): $(SOURCES)
	$(GCC) $(GCCFLAGS) -T shellModule.ld $(SOURCES) $(INCLUDE) $(LIBS) $(GAME_SOURCES) -lc -lsyscalls -o ../$(MODULE)

clean:
	rm -rf *.o

.PHONY: all clean
