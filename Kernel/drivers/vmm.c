
#include <vmm.h> 

#include <video.h>
#include <audio.h>

static uint64_t cur_cr3 = 0;
static uint64_t kernel_cr3 = 0;

void _write_cr3(uint64_t c);



bool switch_cr3 (uint64_t c){

	cur_cr3 = c;

	_write_cr3(c);

	return TRUE;

}

void vmm_initialize (){

	//paging activado por defecto en pure64.asm: solo hace falta cargar el cr3.

	cr3 * c= vp_create_tree(KERNEL_SPACE); 

	kernel_cr3 = (uint64_t) c;

	switch_cr3(kernel_cr3);

}


cr3 * vp_create_tree(int user_space) { //pasarle kernel o user y segun cual es setear todas las pags con el flag user o no.


	bool flags = k_mutex(FALSE);


	vp_pml4 * l4 = (vp_pml4 *) k_malloc_no_free(VP_SIZE);

	if (!l4)
		return NULL; //ni libero porque si no hay espacio para esto, no funciona el kernel

	vp_pdpt * l3 = (vp_pdpt *) k_malloc_no_free(VP_SIZE);

	if (!l3){
		return NULL;
	}

	l4->entries[0].p = 1;
	l4->entries[0].rw = 1;
	l4->entries[0].us = 0;
	l4->entries[0].address = (uint64_t) ((uint64_t)l3 / VP_SIZE);

	for (uint64_t z = 0; z < VP_PER_PDPT; z ++){

		uint64_t l2 = (uint64_t) k_malloc_no_free(VP_SIZE);

		l3->entries[z].p = 1;
		l3->entries[z].rw = 1;
		l3->entries[z].us = 0;
		l3->entries[z].address = (l2 / VP_SIZE); 

		for (uint64_t i = 0; i < VP_PER_DIR; i ++){
			
			uint64_t l1 = (uint64_t) k_malloc_no_free(VP_SIZE);
			vp_directory* l2_c = (vp_directory*) l2;

			l2_c->entries[i].p = 1;
			l2_c->entries[i].rw = 1;
			l2_c->entries[i].us = 0;
			l2_c->entries[i].address = (l1 / VP_SIZE); 

			for (uint64_t j = 0; j< VP_PER_TABLE; j++) {

				vp_table* l1_c = (vp_table*) l1;

	   			l1_c->entries[j].p = 1;
				l1_c->entries[j].rw = 1;
				l1_c->entries[j].us = user_space;
				l1_c->entries[j].address = (uint64_t)( ( (i * VP_TABLE_SIZE) + (j * VP_SIZE) + (z * VP_DIR_SIZE) ) / VP_SIZE);

			}
			
			if (!l1){
				return NULL; 
			}

		}
	}

	k_mutex(flags);
   	return l4;
}

/*void vp_delete_tree (cr3* c){


	bool flags = k_mutex(FALSE);

	vp_pdpt * l3 = (vp_pdpt *) (c->entries[0].address);
	vp_directory * l2 = (vp_directory *) (l3->entries[0].address); //un nivel mas de for aca //kmalloc // juego

	for (int i = 0; i < VP_PER_DIR; i ++){
		for (uint64_t j = 0; j < VP_PER_TABLE; j++) {

			vp_table * l1 = (vp_table *) (l2->entries[i].address);
			l1->entries[j].address = 0;

		}
	}

	k_mutex(flags);
}*/
