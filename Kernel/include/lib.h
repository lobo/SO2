#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <typeDefs.h>
#include <video.h>
#include <audio.h>

#define INT_BIT 0x1000000000

extern uint8_t sleeping;
extern size_t sleepTimer;

void * memset_8b(void * destination, int32_t c, uint64_t length);
void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
int atoi(char* c);
int strlen(char * str);
int strcpy(char* src, char* dst);
int strncpy(char* src, char* dst, unsigned int n);

char *cpuVendor(char *result);
void _cli();
void _sti();
void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);

int abs(int v);

void easter_egg();

bool k_mutex(bool state);


#endif
