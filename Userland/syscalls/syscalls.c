//Usamos _syscall en asm para llamar a int 0x80. 

#include <syscalls.h>
#include <typeDefs.h>
#include <stdint.h>

extern uint64_t _syscall(uint64_t callid, ...);

int sys_read(FILE_DESC fd, char * string, size_t length){

	return _syscall((uint64_t)READ, (uint64_t) fd, (uint64_t) string, (uint64_t) length);
}

void sys_write(FILE_DESC fd, const char * string, size_t length){
	
	_syscall((uint64_t)WRITE, (uint64_t) fd, (uint64_t) string, (uint64_t) length);
}

void sys_beep(){
	_syscall((uint64_t)BEEP);
}

void sys_play_note(uint8_t note, int duration, uint8_t octave){
	_syscall((uint64_t)NOTE, (uint64_t)note, (uint64_t) duration, (uint64_t) octave);
}

void* sys_malloc(size_t size){

	return (void*) _syscall((uint64_t)MALLOC, (uint64_t) size);
}

void sys_free(void* p){

	_syscall((uint64_t)FREE, (uint64_t) p);
}

void sys_cls(){
	
	_syscall((uint64_t)CLS);
}

void sys_exit(int exit_code) {

	_syscall((uint64_t)EXIT, (uint64_t) exit_code);
}

void sys_sleep(int pid, int ms){

	_syscall((uint64_t)SLEEP, (uint64_t) pid, (uint64_t)ms);
}

void sys_list_ps(){
	_syscall((uint64_t)LIST_PROCESS);
}

void sys_list_ipcs() {
	_syscall((uint64_t) LIST_IPCS);
}

void sys_mute() {
	_syscall((uint64_t) MUTE);
}

int sys_create_process(char * name, void * entry, bool foreground){
	return _syscall((uint64_t)CREATE_PROCESS, (uint64_t) name, (uint64_t) entry, (uint64_t) foreground);
}

void sys_draw_frect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b){
	_syscall((uint64_t)DRAW_FRECT, (uint64_t) x, (uint64_t) y, (uint64_t) l, (uint64_t) w, (uint64_t) r, (uint64_t) g, (uint64_t) b);
}

int sys_opipe(int fd){
	return _syscall((uint64_t)O_PIPE, (uint64_t) fd);
}
void sys_cpipe(int fd){
	_syscall((uint64_t)C_PIPE, (uint64_t) fd);
}
int sys_wpipe(int fd, void* data, unsigned int size){
	return _syscall((uint64_t)W_PIPE, (uint64_t) fd, (uint64_t) data, (uint64_t) size);
}
int sys_rpipe (int fd, void* data, unsigned int size){
	return _syscall((uint64_t)R_PIPE, (uint64_t) fd, (uint64_t) data, (uint64_t) size);
}

void sys_kill(int pid){
	_syscall((uint64_t)KILL, (uint64_t) pid);
}

void sys_draw_char(char to, unsigned int x, unsigned int y, int size, unsigned int r, unsigned int g, unsigned int b){
	_syscall((uint64_t) DRAWCHAR, (uint64_t) to, (uint64_t) x, (uint64_t) y, (uint64_t) size, (uint64_t) r, (uint64_t) g, (uint64_t) b);
}

void sys_draw_circle(unsigned int x, unsigned int y, int radius, unsigned int r, unsigned int g, unsigned int b){
	_syscall((uint64_t) DRAWCIRCLE, (uint64_t) x, (uint64_t) y, (uint64_t) radius, (uint64_t) r, (uint64_t) g, (uint64_t) b);
}

void sys_draw_rect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b){
	_syscall((uint64_t) DRAWRECT, (uint64_t) x, (uint64_t) y, (uint64_t) l, (uint64_t) w, (uint64_t) r, (uint64_t) g, (uint64_t) b);
}

void sys_flush(){
	_syscall((uint64_t) FLUSHBUFFER);
}

void sys_wait_pid(int pid) {
	_syscall((uint64_t) WAITPID, (uint64_t) pid);
}

int sys_get_pid() {
	return _syscall((uint64_t) GETPID);
}