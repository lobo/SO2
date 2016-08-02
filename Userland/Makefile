include Makefile.inc

SAMPLE_DATA=imperialMarch.bin

all: syscalls libc shellModule sampleDataModule

ipc: clean syscalls libc ipcModule sampleDataModule

shellModule:
	cd shell; make

ipcModule:
	cd ipcs; make

sampleDataModule:
	printf "18-7,6,0,7-7,6,0,7-7,6,0,7-3,6,0,7-10,3,0,2-7,3,0,8-3,6,0,7-10,3,0,2-7,9,0,10-2,6,1,7-2,6,1,7-2,6,1,7-3,6,1,7-10,5,0,4-6,5,0,8-3,6,0,7-10,3,0,2-7,6,0,7" >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

syscalls:
	cd syscalls; make

libc:
	cd libc; make

clean:
	cd shell; make clean
	cd syscalls; make clean
	cd libc; make clean
	rm -rf *.bin
	rm -rf *.a


.PHONY: ipcModule shellModule syscalls libc all clean ipc
