#include <lib.h>

#define MAX_LENGTH 100000000 //10mb
#define MALLOC_BUFFER 0x50000

uint8_t sleeping = 0; //Comienza despierto
size_t sleepTimer = 0;

uint64_t _get_flags();

bool k_mutex(bool state) {

	uint64_t flags = _get_flags();


	if (state)
		_sti();
	else
		_cli();


	return ( (flags & ( 1 << 9)) != 0);
}

void * memset_8b(void * destination, int32_t c, uint64_t length)
{
	uint64_t chr = (uint64_t)c;
	uint64_t * dst = (uint64_t*) destination;

	while(length-=8)
		dst[length] = chr;

	return destination;
}

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

int strlen(char * str){
	int i = 0;
	
	while (str[i] != '\0') i++;

	return i;
}

int strcpy(char* src, char* dst) {
	int i = 0;
	while (src[i] != '\0') {
		dst[i] = src[i];
		i++;
	}
	dst[i] = src[i];
	return i;
}

int strncpy(char* src, char* dst, unsigned int n) {
	int i = 0;
	while (src[i] != '\0' && n > 0) {
		dst[i] = src[i];
		i++;
		n--;
	}
	dst[i] = '\0';
	return i;
}

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

void easter_egg(){

	print_line();
	print_string("\t       ____                           \n");
   	print_string("\t  _||__|  |  ______   ______   ______ \n");
  	print_string("\t (   SO   | | nota | |  =   | | 1010 |\n");
  	print_string("\t /-()---() ~ ()--() ~ ()--() ~ ()--()\n");
	print_line();

	playTrainSound();
	
}


int abs(int v) {
  return v * ( (v<0) * (-1) + (v>0));
}
