#include <syscalls.h>

typedef int (*EntryPoint)();
uint8_t debug = FALSE;

int sys_read(FILE_DESC fd, char* string, size_t len){
	return read_from_buffer(string, len);
}

void sys_write(FILE_DESC fd, const char * string, size_t length){

	if(fd == STDERR)
		color_set(RED, BLACK);
	
	if (!debug && get_pid() != get_foreground_pid()) {
 		return;
 	}

	while (length--){
		print_char(*string++);
		update_written_space();
	}
	
	color_set_default();

}

void* sys_malloc(size_t size){
	return heap_malloc(size);
}


void sys_free(void * p){
	heap_free(p);
}

void sys_cls(){
	if (!debug && get_pid() != get_foreground_pid()) {
 		return;
 	}
	clear_screen();
}

void sys_beep(){
	beep();
}

void sys_play_note(uint8_t note, int duration, uint8_t octave){
	playNote(note, duration, octave);
}

int sys_exit(int exit_code){
	return exit_code;
}

void sys_sleep(int pid, int ms){ 
	sleep_process(pid, ms);
}

int sys_create_process(char * name, void * entry, bool foreground){

	Process* new_proc = new_process(name, entry, foreground);
	enqueue_process(new_proc);
	return new_proc->pid;
}

void sys_kill(int pid){
	kill(pid);
}

void sys_list_ps(){
	if (!debug && get_pid() != get_foreground_pid()) {
 		return;
 	}
	list_processes();
}

void sys_list_ipcs() {
	list_pipes();
}

void sys_mute() {
	mute();
}

void sys_draw_frect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b){
	draw_frect(x, y, l, w, r, g, b);
}

int sys_opipe(int fd) {	
	return open_pipe(fd);
} 
void sys_cpipe(int fd) {
	delete_pipe(fd);
}
int sys_wpipe(int fd, void* data, unsigned int size) {
	return put_pipe(fd, data, size);
}
int sys_rpipe (int fd, void* data, unsigned int size) {
	return get_pipe(fd, data, size);
}

void sys_draw_char(char to, unsigned int x, unsigned int y, int size, unsigned int r, unsigned int g, unsigned int b){

	draw_char_buffer(to, x, y, size, r, g, b);
}

void sys_draw_circle(unsigned int x, unsigned int y, int radius, unsigned int r, unsigned int g, unsigned int b){
	draw_fcircle(x, y, radius, r, g, b);
}

void sys_draw_rect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b){
	draw_rect(x, y, l, w, r, g, b);
}

void sys_flush(){
	flush_buffer();
}

void sys_wait_pid(int pid) {
	wait_pid(pid);
}

int sys_get_pid() {
	return get_pid();
}