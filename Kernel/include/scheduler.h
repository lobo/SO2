#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <memmanager.h>
#include <lib.h>

#define STACK_SIZE			0x10
#define SLEEPING			0
#define ACTIVE				1
#define PAUSED				2
#define WAITING				3

typedef struct
{
	//Registers restore context
	uint64_t gs;
	uint64_t fs;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;

	//iretq hook
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
	uint64_t base;	
} Process_registers;

typedef struct process_t {
	char name[128];
	int pid;
	int ppid;
	int state;
	int sleep_time;
	struct Page* page;
	void * user_stack;
	void * kernel_stack;	
	void * entry_point;
	void * user_stack_page;
	void * kernel_stack_page;
} Process;

typedef struct Process_Node {
	Process* process;
	struct Process_Node* next;
} Process_Node;

Process * new_process(char * name, void * entry_point, bool foreground);
void* switch_kernel_to_user();
void* switch_user_to_kernel(void * rsp);
int enqueue_process(Process* p);
void kill(int pid);
void set_state(int pid, uint8_t state);
Process_Node* get_current();
struct Page * get_process_page();
Process* get_process(int pid);
int get_pid();
int get_ppid();
int get_foreground_pid();
void set_foreground_pid(int pid);
void list_processes();


void sleep_process(int pid, int time);
void wake_up_process(int pid);
void sleep_pid(int pid);
void wait_pid(int pid);
void check_sleep();

#endif