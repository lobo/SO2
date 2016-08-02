#include <memmanager.h>

static uint64_t* memory_map;	// Donde empieza mi bitmap de las páginas (6to Mb) (16k)
static Page* page_descriptors;	// Donde empieza la memoria donde estan las estructuras de Page
static Process* process_descriptors;  // Donde empiezan mis procesos
static Process_Node* process_node_descriptor; //Donde empiezan los nodos de los procesos
static void* kernel_end_reserved_memory; //Donde el kernel empieza a sacar memoria para las paginas fisicas

void memory_map_allocation(uint64_t bit);
void memory_map_free (uint64_t bit);
int memory_occupied(uint64_t bit);
uint16_t get_free_blocks_start(uint64_t* bitmap, uint16_t blocks_needed);
uint16_t seek_n_free_blocks(uint64_t* bitmap, uint8_t current_set, uint8_t current_bit, uint16_t blocks_needed);
void reserve_memory(Page* page, uint16_t starting_block, uint16_t blocks_needed);
uint16_t get_tail_position(uint64_t* tails_bitmap, uint16_t starting_block);

void memory_map_allocation(uint64_t bit) {
	memory_map[bit / BITS_ARQ] |= (1 << (bit % BITS_ARQ));
	//print_num_bin(memory_map[bit / BITS_ARQ]);
	//print_line();
}

void memory_map_free (uint64_t bit){
	memory_map[bit / BITS_ARQ] &= ~(1 << (bit % BITS_ARQ));
}

int memory_occupied(uint64_t bit) {
	return (memory_map[bit / BITS_ARQ] & (1 << (bit % BITS_ARQ))) ? 1 : 0;
}

int get_free_page_bit() {

	for (int i = 0; i < NUM_OF_PAGES / BITS_ARQ ; i++) {
		if (memory_map[i] != 0xffffffffffffffff) { //chequeo que el bloque de 64bits no este lleno completamente, asi me ahorro iterar ahi
			for (int j = 0; j < BITS_ARQ ; j++) {	
 		
				uint64_t bit = 1 << j;
				if (!(memory_map[i] & bit)) { //si no es 1, es decir, si esta libre, devuelvo
					return i * BITS_ARQ + (j);
				}
			}
		}
	}

	return -1;
}

void * get_page_start() {

	uint64_t free_bit = get_free_page_bit();
	
	if (free_bit == -1) { 		//no hay mas paginas. igual no va a pasar nunca jeje
		return (void *)(-1);
	}

	memory_map_allocation(free_bit);
	
	return (void*) ((uint64_t)kernel_end_reserved_memory + free_bit * PAGE_SIZE); //empiezo a tirar memoria desde la parte que no esta ya reservada y asi me ahorro de inicializar los bits encima.
}

Page* get_page() {

	
	bool flags = k_mutex(FALSE);

	void * address = get_page_start();

	Page* new_page = (Page*) page_descriptors + ( ( (uint64_t) address - (uint64_t) kernel_end_reserved_memory) / PAGE_SIZE) * sizeof(Page); //se comparte bit entre bloque de memoria y page desc
	
	new_page->page_start = address;

	for (int i = 0 ; i < BITMAP_BLOCKS ; i++) {
		new_page->bitmap[i] = 0;
		new_page->tails_bitmap[i] = 0;
	}

	k_mutex(flags);

	return new_page;
}

void * k_malloc_no_free(size_t size){


	bool flags = k_mutex(FALSE);

	int pages_needed = (size / PAGE_SIZE) + 1;

	void * initial_address = get_page_start();

	for (int i = 0; i < pages_needed - 1; i++ ){
		get_page_start();
	}


	k_mutex(flags);

	return initial_address;

}


int free_page(void * address) {

	if((uint64_t)address % PAGE_SIZE != 0){
		return -1;
	}

	uint64_t bit_index = (uint64_t)address / PAGE_SIZE;
	
	if (memory_occupied(bit_index)) {
		memory_map_free(bit_index);
		return 1;
	}

	return 0;
}

void initialize_memory() {

	memory_map = (uint64_t*) KERNEL_MEMORY_START;													// Donde empieza mi bitmap de las páginas (6to Mb) (16k)
	page_descriptors = (Page*) ((uint64_t) memory_map + NUM_OF_PAGES / BITS_ARQ);						// Donde empieza la memoria donde estan las estructuras de Page
	process_descriptors = (Process*) ((uint64_t) page_descriptors + NUM_OF_PAGES * sizeof(Page));		// Donde empiezan mis procesos
	process_node_descriptor = (Process_Node*) ((uint64_t) process_descriptors + NUM_OF_PROCESSES * sizeof(Process));	//Donde empiezan los nodos de los procesos
	kernel_end_reserved_memory = (void*) ((uint64_t) process_node_descriptor + NUM_OF_PROCESSES * sizeof(Process_Node)); //Donde el kernel empieza a sacar memoria para las paginas fisicas

	//alineo la direccion
	if (((uint64_t) kernel_end_reserved_memory) % PAGE_SIZE != 0) {
		kernel_end_reserved_memory = (void*) ((uint64_t) kernel_end_reserved_memory & PAGE_ALIGN);
		kernel_end_reserved_memory = (void*) ((uint64_t) kernel_end_reserved_memory + PAGE_SIZE);
	}

	//inicializo el bitmap con 0's
	for (uint64_t i = 0; i < NUM_OF_PAGES / BITS_ARQ; i++) {	
 		memory_map[i] = 0x0000000000000000;
 	}

 	//seteo todos los pids en -1 para despues poder ver los que estan libres
 	for (uint64_t i = 0; i < NUM_OF_PROCESSES; i++) {
		process_descriptors[i].pid = -1;
 	}
}

void * get_mem_process(){

	for (uint16_t i = 0; i < NUM_OF_PROCESSES; i++ ){

		Process * p = (Process *) ( (uint64_t) process_descriptors + (i * sizeof(Process)) );

		if (p->pid == -1) {
			p->pid = i;
			/*print_string("Devuelvo proceso con pid: ");
			print_num_dec(i);
			print_line();*/
			return (void*) p;
		}
	}

	return (void *)(-1);

}

void * get_mem_process_node(int pid){
	/*print_string("Devuelvo un process node con pid: ");
	print_num_dec(pid);
	print_line();*/
	return (void *) ( process_node_descriptor + (pid * sizeof(Process_Node)) );
}



/**
 * Idea: Divido la pagina en bloques de 8 bytes (64bits) y esa es la minima unidad de memoria que puedo asignar (copado para alinear memoria)
 * Entonces creo un bitmap donde cada bit me indica si ese bloque de memoria está ocupado o no. La posición del bit me indica el bloque de memoria.
 * Para una página de 4k (4096 bytes) necesito: (PageSize / BlockSize * Arquitectura64bits) = 8 uint64_t para describir toda la pag (64bytes para describir 4096bytes)
 * Necesito 2 bitmaps: 1 que me diga los bloques reservados y otro que me diga hasta donde llega la memoria ocupada (la cola de cada bloque reservado) (esto para el free())
 * Esto seguramente debería ir en otro lado, pero ya se moverá cuando se creen los procesos y el Scheduler.
 */

void * heap_malloc(size_t size){
	return malloc(get_process_page(), size);
}

void heap_free(void * ptr){
	free(get_process_page(), ptr);
}

/**
 * malloc function to be called by the kernel.
 */
void* malloc(Page* page, size_t size) {

	uint16_t blocks_needed = (((uint16_t) size % 8) == 0) ? (uint16_t) size / 8 : (uint16_t) size / 8 + 1;
	uint16_t free_block = get_free_blocks_start(page->bitmap, blocks_needed);
	if (free_block == -1) {
		//page->next_page = new_page();		
		return malloc(page->next_page, size);
	}
	reserve_memory(page, free_block, blocks_needed);
	
	return (void*)((uint64_t)page->page_start + free_block * BLOCK_SIZE);
}

/**
 * free function to be called by the kernel
 */
void free(Page* page, void* ptr) {
	uint64_t mem = (uint64_t) ptr;

	//Si la memoria a liberar está en una página siguiente
	if (mem >= ( (uint64_t)page->page_start + PAGE_SIZE)) {
		if (page->next_page == NULL) {
			return; 		//Si la prox pag no existe, kb
		}
		else {
			return free(page->next_page, ptr);
		}
	}

	uint16_t starting_block = (uint16_t) ((mem - ((uint64_t) page->page_start)) / 8);
	uint16_t tail_block = get_tail_position(page->tails_bitmap, starting_block);
	uint8_t current_block_set = (uint8_t) (starting_block / BITS_ARQ);
	uint64_t bits_modifier = (uint64_t) 0xFFFFFFFFFFFFFFFF >> (starting_block % BITS_ARQ);					//alineo los 1s con el inicio del bloque que quiero liberar

	//Si la tail está en otro set de bloques, seteo todo en 0 hasta llegar a ese bloque.
	while (current_block_set < ((uint8_t) (tail_block / BITS_ARQ))) {
		bits_modifier = ~bits_modifier;									//Niego los bits para poder poner en 0 los bloques de memoria que quiero liberar
		page->bitmap[current_block_set] &= bits_modifier;				//Libero la memoria
		current_block_set++;
		bits_modifier = (uint64_t) 0xFFFFFFFFFFFFFFFF;
	}

	bits_modifier >>= (BITS_ARQ - 1 - (tail_block % BITS_ARQ));			//Me quedo solo con 1s hasta donde termina la memoria que quiero liberar
	bits_modifier <<= (BITS_ARQ - 1 - (tail_block % BITS_ARQ));			//Vuelvo a alinear todo porque lo desalinee en el paso anterior
	bits_modifier = ~bits_modifier;										//Niego todos los bits para que queden en 0 los bloques que quiero liberar
	page->bitmap[current_block_set] &= bits_modifier;					//Libero la memoria

	bits_modifier = (uint64_t) 0x0000000000000001 << (BITS_ARQ - 1 - (tail_block % BITS_ARQ));
	bits_modifier = ~bits_modifier;
	page->tails_bitmap[current_block_set] &= bits_modifier;				//Seteo en 0 el bit que indicaba el fin de la memoria que liberé.
}

/**
 * Gets the position of the next tail starting from the starting_block
 */
uint16_t get_tail_position(uint64_t* tails_bitmap, uint16_t starting_block) {
	uint8_t current_block_set = (uint8_t) (starting_block / BITS_ARQ);
	uint64_t current_block = tails_bitmap[current_block_set];
	current_block &= ((uint64_t) 0xFFFFFFFFFFFFFFFF >> (starting_block % BITS_ARQ));			//elimino cualquier tail que pueda tener antes del espacio de memoria que quiero liberar
	uint8_t shift_num = BITS_ARQ - 1;

	for ( ; current_block_set < BITMAP_BLOCKS ; current_block_set++) {			//itero sobre los sets de bloques restantes en busca de la tail
		if (current_block != 0) {																//si mi bloque no es 0, significa que la 1er tail que encuentre es la que busco
			while (current_block >> shift_num == 0) {
				shift_num--;
			}
		}

		current_block = tails_bitmap[current_block_set + 1];									//tengo que copiarlo nomas porque fue necesario para el 1er caso, el resto podria usar directo tails_bitmap. En la ultima iteracion copia basura pero no importa porque no la usa.
	}

	return BITS_ARQ - 1 - shift_num;
}

/**
 * Returns the first free block with enough continuous free blocks found
 */
uint16_t get_free_blocks_start(uint64_t* bitmap, uint16_t blocks_needed) {
	uint8_t i;
	int bit_num;
	uint16_t blocks_missing;

	//print_string("-->Empiezo a recorrer bitmap.\n");
	for (i = 0 ; i < BITMAP_BLOCKS ; i++) {
		bit_num = BITS_ARQ - 1;
		//recorro los bloques en busca de alguno libre
		while (bit_num >= 0) {
			//si encuentro un bloque libre
			if ((bitmap[i] & ((uint64_t) 0x0000000000000001 << bit_num)) == 0) { //Busco en el bloque actual del bitmap el primer bit con 0 (libre)
				//me fijo que tenga suficientes libres como necesito
				if ((blocks_missing = seek_n_free_blocks(bitmap, i, BITS_ARQ - (bit_num + 1), blocks_needed - 1))) {
					/*print_string("--> No encontre libre aca, sigo buscando -- BLOCK: ");
					print_num_dec(i * BITS_ARQ + (BITS_ARQ - (bit_num + 1)));
					print_string(" - curr set: ");
					print_num_bin(bitmap[i]);
					print_string(" - faltantes: ");
					print_num_dec(blocks_missing);
					print_line();*/
					bit_num = ((blocks_missing + (BITS_ARQ - (bit_num + 1))) > BITS_ARQ ? BITS_ARQ : (BITS_ARQ - (bit_num + 1)) + blocks_missing);
				}
				else {
					return i * BITS_ARQ + (BITS_ARQ - (bit_num + 1));
				}
			}
			bit_num--;
		}
	}
	return -1;
}

/**
 * Returns 0 if I found all the free blocks I needed, else return the blocks missing to catch'em all.
 */
uint16_t seek_n_free_blocks(uint64_t* bitmap, uint8_t current_set, uint8_t current_bit, uint16_t blocks_needed) {
	if (blocks_needed == 0) {
		return 0;
	}
	//si ya está reservado o me quedé sin memoria en la página
	if (((bitmap[current_set] >> current_bit) & (uint64_t) 0x0000000000000001) || (current_bit == 0 && current_set == BITMAP_BLOCKS - 1)) {
		return blocks_needed;
	}

	//si tengo que pasar al proximo set de bloques
	if (current_bit == 0 && current_set < 7) {
		return seek_n_free_blocks(bitmap, current_set + 1, current_bit - 1, blocks_needed - 1);
	}

	return seek_n_free_blocks(bitmap, current_set, current_bit - 1, blocks_needed - 1);
}

/**
 * Reserves de memory blocks passed as arguments. Doesn't check blocks' availability
 * Availability should be checked beforehand
 */
void reserve_memory(Page* page, uint16_t starting_block, uint16_t blocks_needed) {
	uint8_t current_block_set = (uint8_t) (starting_block / BITS_ARQ);			//en qué indice de bitmap esta el primer bit a cambiar
	uint64_t reserving_bits;

	while (blocks_needed > 0) {
		//Si tengo para reservar más de lo que puedo en el set actual
		if (blocks_needed > (BITS_ARQ - starting_block % BITS_ARQ)) {
			reserving_bits = (uint64_t) 0xFFFFFFFFFFFFFFFF >> (starting_block % BITS_ARQ);							//Seteo tantos 1s como bloques vaya a reservar en ese set
			page->bitmap[current_block_set] |= reserving_bits;											//Reservo los bloques. (ya están alineados)
			blocks_needed -= ((starting_block % BITS_ARQ) == 0 ? 64 : starting_block % BITS_ARQ);		//Ahora me quedan menos bloques por reservar
			starting_block = (++current_block_set) * BITS_ARQ;											//Tengo que reservar a partir del inicio del proximo set
		}
		else {
			reserving_bits = (uint64_t) 0xFFFFFFFFFFFFFFFF >> (BITS_ARQ - blocks_needed);							//Seteo tantos 1s como bloques vaya a reservar
			reserving_bits <<= (BITS_ARQ - ((starting_block % BITS_ARQ) + blocks_needed));				//Alineo los 1s con los bloques que necesito reservar de este set
			page->bitmap[current_block_set] |= reserving_bits;											//Los reservo
			reserving_bits = (uint64_t) 0x0000000000000001 << (BITS_ARQ - ((starting_block % BITS_ARQ) + blocks_needed));	//Alineo un unico bit al final del espacio reservado en este set
			page->tails_bitmap[current_block_set] |= reserving_bits;									//Marco el último espacio que reservé en mi otro bitmap (para el free())
			blocks_needed = 0;																			//Ya no me quedan bloques por reservar
		}
	}

}