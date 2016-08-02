#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#define MAX_PIPES 20

typedef struct processQueue processQueue;
typedef struct Semaphore Semaphore ;


Semaphore *create_sem(unsigned value);
void delete_sem(Semaphore *sem);
int wait_sem(Semaphore *sem);
void signal_sem(Semaphore *sem);
unsigned value_sem(Semaphore *sem);
void flush_sem(Semaphore *sem);
void flush_semQueue(Semaphore *sem);

#endif