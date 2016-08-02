#ifndef TYPE_DEFS
#define TYPE_DEFS

#include <stdint.h>
#include <stddef.h>

#define TRUE 1
#define FALSE 0

#define byte unsigned char
#define word short int
#define dword int

#define EXIT_SUCCESS 0
#define EXIT_ERROR 1

typedef unsigned long size_t;
typedef int bool;

typedef enum{
	STDIN,
	STDOUT,
	STDERR
} FILE_DESC;

#endif
