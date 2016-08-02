#ifndef SYSCALLS_HEADER
#define SYSCALLS_HEADER

#include <typeDefs.h>

typedef enum {
READ = 1,
WRITE = 2,
MALLOC = 3,
FREE = 4,
CLS = 5,
EXIT = 6,
SLEEP = 7,
BEEP = 8,
NOTE = 9,
LIST_PROCESS = 10,
CREATE_PROCESS = 11,
DRAW_FRECT = 12,
O_PIPE = 13,
C_PIPE = 14,
W_PIPE = 15,
R_PIPE = 16,
KILL = 17,
DRAWCHAR = 18,
DRAWCIRCLE = 19,
DRAWRECT = 20,
FLUSHBUFFER = 21,
WAITPID = 22,
GETPID = 23,
LIST_IPCS = 24,
MUTE = 25
} syscall_id;

int sys_read(FILE_DESC fd, char * string, size_t length);
void sys_write(FILE_DESC fd, const char * string, size_t length);
void sys_beep();
void sys_play_note(uint8_t note, int duration, uint8_t octave);
void * sys_malloc(size_t size); //no deberia ser una sys_call, pero para no implementar mmap
void sys_free(void* p);
void sys_cls();
void sys_exit(int exit_code);
void sys_sleep(int pid, int ms);
//agregar funcion de list process
int sys_create_process(char * name, void * entry, bool foreground);
void sys_list_ps();
void sys_list_ipcs();
void sys_mute();

void sys_draw_frect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b);

int sys_opipe(int fd);
void sys_cpipe(int fd);

int sys_wpipe(int fd, void* data, unsigned int size);
int sys_rpipe(int fd, void* data, unsigned int size);
void sys_kill(int pid);
int sys_get_pid();

//vesa video
void sys_draw_char(char to, unsigned int x, unsigned int y, int size, unsigned int r, unsigned int g, unsigned int b);
void sys_draw_circle(unsigned int x, unsigned int y, int radius, unsigned int r, unsigned int g, unsigned int b);
void sys_draw_rect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b);
void sys_flush();
void sys_wait_pid(int pid);

#endif
