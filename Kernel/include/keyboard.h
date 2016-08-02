#ifndef DRIVER_KEYBOARD_H
#define DRIVER_KEYBOARD_H

#include <stdint.h>
#include <typeDefs.h>
#include <video.h>
#include <audio.h>
#include <lib.h>

#define BUFFER_SIZE 256
#define EMPTY 0


typedef struct {
	uint8_t ascii;
	uint8_t spec_ascii;
} KBCode;

typedef struct {
	uint8_t numLock;
	uint8_t capsLock;
	uint8_t scrollLock;
} KBLocks;

void keyboard_handler(uint64_t sc);
int read_from_buffer(char* buf, size_t length);


#endif
