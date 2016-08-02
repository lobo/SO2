#ifndef LIBC_HEADER
#define LIBC_HEADER

#include <stdint.h>
#include <typeDefs.h>
#include <syscalls.h>
#include <stdarg.h>
#include <stdint.h>

int strlen(char * str);
void strcpy(char* dest, const char* src);
int atoi(char* c);
void del_spaces(char* str);
int strcmp(char * str1, char * str2);
void* malloc(size_t length);
void free(void* p);
void printf(char* fmt, ...);
void putchar(const char c);
char getchar();
int scanf(char* c, int length);
void clear_screen();
void beep();
void play_song();
void exit(int exit_code);

int create_process(char * name, void * entry, bool foreground);

// Nuevas aplicaciones del User Space:
void new_game();
void list_ipcs();
void list_ps();
void mute();

void play_note(uint8_t note, int duration, uint8_t octave);

void draw_frect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b);
void kill(int pid);
int get_pid();

int open_pipe(int fd);
int get_pipe(int fd, void * data, unsigned size);
int put_pipe(int fd, void *data, unsigned size);
void delete_pipe(int fd);


void draw_char(char to, unsigned int x, unsigned int y, int size, unsigned int r, unsigned int g, unsigned int b);
void draw_circle(unsigned int x, unsigned int y, int radius, unsigned int r, unsigned int g, unsigned int b);
void draw_rect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b);
void flush_buffer();
void wait_pid(int pid);

#endif

