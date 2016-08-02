#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include <stdint.h>
#include <typeDefs.h>
#include <scheduler.h>
#include <lib.h>

#define BITMAP_BLOCKS (PAGE_SIZE / 512)
#define BITS_ARQ 64
#define BLOCK_SIZE 8

#define PAGE_SIZE 				0x1000
#define PAGE_ALIGN				0xFFFFFFFFFFFFF000
#define MEMORY_SIZE 			(512 * 1024 * 1024) 					// 1Gb
#define NUM_OF_PAGES 			(MEMORY_SIZE/PAGE_SIZE)
#define NUM_OF_PROCESSES		200

#define KERNEL_MEMORY_START		(6 * 0x100000)							//Donde empieza la memoria que va a usar el kernel

typedef struct Page {
	void * page_start;
	uint64_t bitmap[BITMAP_BLOCKS];
	uint64_t tails_bitmap[BITMAP_BLOCKS];
	struct Page * next_page;
} Page;

Page * get_page();
int free_page(void * address);
void initialize_memory();

void * get_mem_process();
void * get_mem_process_node(int pid);

void * malloc(Page * page, size_t size); //podrian sacarse de aca
void free(Page * page, void* ptr); 

void * k_malloc_no_free(size_t size);

void * heap_malloc(size_t size);
void heap_free(void * ptr);




#endif