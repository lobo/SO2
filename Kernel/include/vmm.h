#ifndef VIRTUAL_MEM_MANAGER_H
#define VIRTUAL_MEM_MANAGER_H

#include <stdint.h>
#include <memmanager.h>
#include <lib.h>

#define VP_SIZE 4096
#define VP_TABLE_SIZE (2 * 0x100000)
#define VP_DIR_SIZE (1 * 0x40000000)

#define VP_PER_TABLE 512 //15 tablas de 2mb cada una = 30mb de nuestro sistema.
#define VP_PER_DIR 512
#define VP_PER_PDPT 4 //4GB
#define VP_PER_PML4 1

#define KERNEL_SPACE 0
#define USER_SPACE 1

//No se implementan get page, ni free page ya que al estar todo idmapped la dir virtual = a la fisica. ademas, al inicializar
//la memoria, marcamos todas las paginas como presentes y escribibles ya que no manejamos pagefaults ni swapping


enum VP_FLAGS {

	FLAG_PRESENT			=	1,			
	FLAG_WRITABLE			=	2,			
	FLAG_USER				=	4,		
};

typedef struct {
  uint64_t p         : 1;
  uint64_t rw        : 1;
  uint64_t us        : 1;
  uint64_t pwt       : 1;
  uint64_t pcd       : 1;
  uint64_t a         : 1;
  uint64_t ign       : 1;
  uint64_t mbz       : 2;
  uint64_t avl       : 3;
  uint64_t address   : 40;
  uint64_t available : 11;
  uint64_t nx        : 1;
} vp_entry;

typedef struct vp_table {

	vp_entry entries[VP_PER_TABLE];

} vp_table;

typedef struct vp_directory {

	vp_entry entries[VP_PER_DIR];
} vp_directory;

typedef struct vp_pdpt {

	vp_entry entries[VP_PER_PDPT];
} vp_pdpt;

typedef struct vp_pml4 {

	vp_entry entries[VP_PER_PML4];
} vp_pml4;

typedef vp_pml4 cr3;

void vmm_initialize ();

bool switch_cr3 (uint64_t c);

cr3 * vp_create_tree(int user_space);

void vp_delete_tree (cr3* c);


#endif
