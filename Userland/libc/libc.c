#include <libc.h>

char buffer2[64] = {0};

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

int strlen(char * str){
	int i = 0;
	
	while (str[i] != 0) i++;
	
	return i;
}

void strcpy(char* dest, const char* src){
	int i = 0;
	while ((dest[i] = src[i]) != '\0')
		i++;
}

 //ver de cambiar
int atoi(char* c) {
	
	int d, len, i;

	d = 0;
	i = 0;
	len = strlen(c);
	
	while  (i < len) {
		d = d * 10 + c[i] - '0';
		i++;
	}

	return d;
}

/**
 * Borra espacios extra que pueda haber en un string (y todos los primeros)
 */
void del_spaces(char* str){
	
	char* str2 = str;
	
	while (*str == ' ')	//hago que str apunte al primer char que no sea un espacio
		str++;

	str--; //fix porque despues lo aumento en el while. #AhListoQueVillero
	
	while (*str++ != 0){
		if (*str != ' ')
			*str2++ = *str;
		else{
			if (*(str+1) == ' ')
				continue;
			else
				*str2++ = *str;
			}
	}
}

//ver de cambiar
int strcmp(char * cad1, char * cad2){

	int i;
	int len1 = strlen(cad1);
	int len2 = strlen(cad2);

	if(len1 != len2){
		return len1 - len2;
	}

	for(i = 0; i < len1; i++){
		if(cad1[i] != cad2[i]){
			if (cad1[i] > cad2[i])
				return 1;
			else
				return -1;
		}
	}
	return 0;
}


void* malloc(size_t length) {
	return sys_malloc(length);
}

void free(void* p){

	sys_free(p);

}

void printf(char* fmt, ...) {

	va_list arguments;

	if (fmt == NULL) return;
	
	size_t size = (size_t) strlen(fmt);
	int num;
	int c;
	char* string;
	va_start(arguments, fmt);
	
	//no generamos cadena formateada para hacerlo mas simple.
	
	for (int i = 0; i < size; i++){
		
		if (fmt[i] != '%'){
			sys_write(STDOUT, (char*) (&(fmt[i])), 1);
			continue;
		}
		
		switch (fmt[i+1]) {
				
			case 'd':
				num = (int) va_arg(arguments, int);
				uintToBase(num, string, 10);
				sys_write(STDOUT, string, strlen(string));
				break;
				
			case 'c':
				c = (char) va_arg(arguments, int); //char queda int igual
				sys_write(STDOUT, (char*) &c, 1);
				break;
		
			case 's':
				string = (char*) va_arg(arguments, char*);
				sys_write(STDOUT, string, strlen(string));
				break;
		}
		
		i++; //Me salteo el caracter que me dice el tipo del formateo
					
	}
	

	va_end(arguments);

}

void putchar(const char c) {
	sys_write(STDOUT, &c, 1);
}


char getchar() {

	static char buffer[2] = {0};

	int read = sys_read(STDOUT, buffer, 1);

	if (read == 0) {
		return 0;
	}

	return buffer[0];
}

int scanf(char* c, int length) {
	return sys_read(STDOUT, c, length);
}


void clear_screen() {
	sys_cls();
}

void sleep(int pid, int ms){

	sys_sleep(pid, ms);

}

void exit(int exit_code) {
	sys_exit(exit_code);
}

void beep(){
	sys_beep();
}

void play_note(uint8_t note, int duration, uint8_t octave){
	sys_play_note(note, duration, octave);
}

int create_process(char * name, void * entry, bool foreground){
	return sys_create_process(name, entry, foreground);
}

void draw_frect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b){
	sys_draw_frect(x, y, l, w, r, g, b);
}

void list_ps(){
	sys_list_ps();
}

void kill(int pid){
	sys_kill(pid);
}

int get_pid() {
	return sys_get_pid();
}

int open_pipe(int fd){
	return sys_opipe(fd);
}

void delete_pipe(int fd){
	sys_cpipe(fd);
}

int get_pipe(int fd, void * data, unsigned size){
	return sys_rpipe(fd, data, size);
}

int put_pipe(int fd, void * data, unsigned size){
	return sys_wpipe(fd, data, size);
}

void draw_char(char to, unsigned int x, unsigned int y, int size, unsigned int r, unsigned int g, unsigned int b){
	sys_draw_char(to, x, y, size, r, g, b);
}

void draw_circle(unsigned int x, unsigned int y, int radius, unsigned int r, unsigned int g, unsigned int b){
	sys_draw_circle(x, y, radius, r, g, b);
}

void draw_rect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b){
	sys_draw_rect(x, y, l, w, r, g, b);
}

void flush_buffer(){
	sys_flush();
}

void wait_pid(int pid) {
	sys_wait_pid(pid);
}

void mute() {
	sys_mute();
}

void list_ipcs() {
	sys_list_ipcs();
}


/**
 * Le hago cambio de base a un numero y lo pongo en mi buffer
 * Ref: naiveConsole.c (Catedra Arqui)
 */
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base){
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}
