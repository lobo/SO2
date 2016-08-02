#ifndef PIPES_H
#define PIPES_H

#include <semaphore.h>
#include <memmanager.h>
#include <video.h>

int open_pipe(int fd);
void delete_pipe(int fd);
int get_pipe(int fd, void * data, size_t size);
int put_pipe(int fd, const void * data, size_t size);

#endif