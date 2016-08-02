#include <semaphore.h>
#include <scheduler.h>

struct processQueue{
	int pid;
	struct processQueue	* next;	
};


typedef struct Semaphore{
	processQueue * queue;
	int		value;
	processQueue * last;
} semaphore_t;

static semaphore_t semaphores[MAX_PIPES];
static uint8_t sems_in_use[MAX_PIPES] = {0};

//create_sem - aloca un semaforo y establece su cuenta inicial.
Semaphore * create_sem(unsigned value) {
	int i;
	int sem_id = -1;
	for (i = 0 ; i < MAX_PIPES ; i++) {
		if (sems_in_use[i] == 0) {
			sem_id = i;
			break;
		}
	}
	if (sem_id == -1) {
		return NULL;
	}

	Semaphore * sem = &sems_in_use[sem_id];
	sem->value = value;
	sem->queue = NULL;
	sem->last = NULL;
	return sem;
}


//delete_sem - da de baja un semaforo.
void delete_sem(Semaphore *sem){
	flush_semQueue(sem);
	heap_free(sem);
}


int wait_sem(Semaphore *sem) {
	if(sem->value > 0){
		sem->value--;
		return 1;
	}

	struct processQueue* newProc = heap_malloc(sizeof(struct processQueue));

	newProc->pid = get_pid();
	newProc->next = NULL;

	if(sem->queue == NULL){

		sem->queue->pid = newProc->pid;
		sem->queue = newProc;
		sem->last = newProc;
	}
	else{
		sem->last->next = newProc;
		sem->last = newProc;
	}

	//sleep_pid(newProc->pid);
	if(sem->value > 0){
		sem->value--;
		return 1;
	}
	return 0;
}


/*
signal_sem - senaliza un semaforo.
Despierta a la primera tarea de la cola o incrementa la cuenta si la cola
esta vacia.
*/
void signal_sem(Semaphore *sem){
	if ( sem->queue == NULL ){
		sem->value++;
		return;
	}
	//int pid = sem->queue->pid;
	sem->queue = sem->queue->next;
	//wake_up_process(pid);
}

//value_sem - informa la cuenta de un semaforo.
unsigned value_sem(Semaphore *sem) {
	return sem->value;
}

//flush_sem - despierta todas las tareas que esperan en un semaforo.
void flush_sem(Semaphore *sem) {
	sem->value = -1;
	while(sem->queue != NULL){
		signal_sem(sem);
	}
	flush_semQueue(sem);
}

void flush_semQueue(Semaphore *sem) {
	struct processQueue * aux;
	while(sem->queue != NULL){
		aux = sem->queue;
		sem->queue = sem->queue->next;
		heap_free(aux);
	}
}