#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <typeDefs.h>
#include <lib.h>
#include <video.h>
#include <audio.h>
#include <keyboard.h>	
#include <scheduler.h>
#include <vesa_video.h>
#include <memmanager.h>
#include <pipe.h>

int sys_read(FILE_DESC fd, char * string, size_t length);
void sys_write(FILE_DESC fd, const char * string, size_t length);
void* sys_malloc(size_t size); //no deberia ser una sys_call, pero para no implementar mmap
void sys_free(void* p);
void sys_cls();
int sys_exit(int exit_code);
void sys_sleep(int pid, int ms);

void sys_list_ps();

int sys_create_process(char * name, void * entry, bool foreground);
void sys_kill(int pid);
int sys_get_pid();

void sys_list_ipcs();
void sys_mute();

int sys_opipe(int fd);
void sys_cpipe(int fd);
int sys_wpipe(int fd, void* data, unsigned int size);
int sys_rpipe (int fd, void* data, unsigned int size);


#endif
