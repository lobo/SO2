#include <pipe.h>

#define PIPE_SIZE 400

typedef struct Pipe {
	Semaphore * sem;
	size_t users;
	size_t size;
	size_t avail;
	void * buf;
	void * head;
	void * tail;
	void * end;
} pipe_t;

static pipe_t pipes[MAX_PIPES];
static int pipes_in_use[MAX_PIPES] = {0};


int open_pipe(int fd) {
	if (fd >= MAX_PIPES) {
		return -1;
	}
	if (pipes[fd].users > 0) {
		pipes[fd].users++;
		return fd;
	}

	pipes_in_use[fd] = 1;
	pipes[fd].head = pipes[fd].tail = pipes[fd].buf = heap_malloc(pipes[fd].size = PIPE_SIZE);
	pipes[fd].end = pipes[fd].buf + (uint64_t) PIPE_SIZE;
	pipes[fd].avail = PIPE_SIZE;
	pipes[fd].users = 1;
	pipes[fd].sem = create_sem(1);
	return fd;
}

void delete_pipe(int fd) {
	if(pipes[fd].users > 1) {
		pipes[fd].users--;
		return;
	}
	pipes[fd].users = 0;
	pipes_in_use[fd] = 0;
	flush_sem(pipes[fd].sem); 
	delete_sem(pipes[fd].sem);
	heap_free(pipes[fd].buf);
	heap_free(pipes[fd].head);
	heap_free(pipes[fd].tail);
	heap_free(pipes[fd].end);
}

int get_pipe(int fd, void * data, size_t size) {
	if(pipes[fd].users == 0) {
		return -1;
	}

	if(size < 1 || size > pipes[fd].size) {
		return -1;
	}

	//si me piden mas de lo que tiene el buffer
	if (pipes[fd].head - size < pipes[fd].buf) {
		return -1;
	}

	if(!wait_sem(pipes[fd].sem)) {
		return -1; // setea que nadie me toque ese pipe, te da 0 si alguien lo esta usando
	}

	memcpy(data, pipes[fd].head - size, size);
	pipes[fd].head -= size;
	pipes[fd].avail += size;

	signal_sem(pipes[fd].sem); // libera el semaforo, ya cualquiera puede accederlo

	return 0;
}

/*Si no hay lugar para escribir devuelve*/
int put_pipe(int fd, const void * source, size_t size) {

	if (pipes[fd].users < 1) {
		return -1;
	}
	if (size > pipes[fd].avail) {
		return -1;
	}

	memcpy(pipes[fd].head, source, size);
	pipes[fd].head += size;
	pipes[fd].avail -= size;

	signal_sem(pipes[fd].sem);
	
	return size;
}

void list_pipes() {
	bool empty = TRUE;
	int i;

	for (i = 0 ; i < MAX_PIPES ; i++) {
		if (pipes_in_use[i] == 1) {
			empty = FALSE;
			print_string("Pipe ");
			print_num_dec(i);
			print_string(" --> ");
			print_num_dec(pipes[i].users);
			print_string(" users\n");
		}
	}
	if (empty) {
		print_string("No se encontraron pipes en uso.\n");
	}
}