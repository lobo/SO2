#include <scheduler.h>

void* get_stack_address(void* page_start);
void * fill_stack(void * entry_point, void * user_stack);
Process_Node* new_process_slot(Process* process);
void remove_process(int pid);
void next_process();
static char * state_to_string(int state);



Process_Node * current_node = NULL;
//void * kernel_stack = NULL;


int foreground_pid = 0;

void force_schedule(); //asm
void switch_u2k();
void switch_k2u();

int pid_count = 0;

/* Change command from kernel back to user*/
void* switch_kernel_to_user() {

	if (current_node == NULL) {
		return 0;
	}

	next_process();
	return current_node->process->user_stack;
}

/* Change command from user to Kernel */
void* switch_user_to_kernel(void * rsp) {

	if (current_node == NULL) {
		return rsp;
	}
	current_node->process->user_stack = rsp;
	return current_node->process->kernel_stack;
}


Process * new_process(char * name, void * entry_point, bool foreground) {

	bool flags = k_mutex(FALSE);

	Process *p = (Process *) get_mem_process(); //el pid ya viene seteado.
	p->user_stack_page = get_page()->page_start;
	p->kernel_stack_page = get_page()->page_start;
	p->page = get_page();

	/*print_num_hex(p);
	print_line();
	print_num_hex(p->user_stack_page);
	print_line();
	print_num_hex(p->kernel_stack_page);
	print_line();
	print_num_hex(p->page->page_start);
	print_line();*/

	strncpy(name, p->name, 127);

	p->state = PAUSED;
	p->entry_point = entry_point;

	if (foreground) {
		set_foreground_pid(p->pid);
	}

	if (p->pid == 0) {
		p->ppid = 0;
	} else {
		p->ppid = get_pid();
	}

	p->sleep_time = 0;


	p->user_stack = get_stack_address(p->user_stack_page);
	p->kernel_stack = get_stack_address(p->kernel_stack_page);
	p->user_stack = fill_stack(entry_point, p->user_stack);

	pid_count++;

	k_mutex(flags);
	return p;
}

void* get_stack_address(void* page_start) {
	return (void*) ((uint64_t)page_start + PAGE_SIZE - STACK_SIZE - 1);
}

void * fill_stack(void * entry_point, void * user_stack) {
	Process_registers * stack = (Process_registers*) user_stack - 1;
	stack->gs =		0x001;
	stack->fs =		0x002;
	stack->r15 =	0x003;
	stack->r14 =	0x004;
	stack->r13 =	0x005;
	stack->r12 =	0x006;
	stack->r11 =	0x007;
	stack->r10 =	0x008;
	stack->r9 =		0x009;
	stack->r8 =		0x00A;
	stack->rsi =	0x00B;
	stack->rdi =	0x00C;
	stack->rbp =	0x00D;
	stack->rdx =	0x00E;
	stack->rcx =	0x00F;
	stack->rbx =	0x010;
	stack->rax =	0x011;
	stack->rip =	(uint64_t) entry_point;
	stack->cs =		0x008;
	stack->eflags = 0x202;
	stack->rsp =	(uint64_t) &(stack->base);
	stack->ss = 	0x000;
	stack->base =	0x000;

	return stack;
}

Process_Node* new_process_slot(Process* process) {
	
	Process_Node* node = (Process_Node *) get_mem_process_node(process->pid);
	node->process = process;
	//current_node->next = node; 
	return node;
}

/* Add a new process to the scheduling sequence*/
int enqueue_process(Process* p) {

	bool flags = k_mutex(FALSE);

	Process_Node* new_slot = new_process_slot(p);
	if (current_node == NULL) {
		current_node = new_slot;
		current_node->next = current_node;
	}
	else {
		new_slot->next = current_node->next;
		current_node->next = new_slot;

	}

	k_mutex(flags);

	return p->pid;
}

void remove_process(int pid) {
	Process_Node* prev_slot = current_node;
	Process_Node* slot_to_remove = current_node->next;

	if (current_node == NULL) {
		return;
	}

	if (prev_slot == slot_to_remove && prev_slot->process->pid == pid) {
		current_node->process->pid = -1;
		current_node = NULL;
		set_foreground_pid(0);
		return;
	}

	while (slot_to_remove->process->pid != pid) {
		prev_slot = slot_to_remove;
		slot_to_remove = slot_to_remove->next;
	}
	
	if (slot_to_remove->process->pid == get_foreground_pid()) {
		set_foreground_pid(slot_to_remove->process->ppid);
	}

	slot_to_remove->process->pid = -1;
	prev_slot->next = slot_to_remove->next;

	Process* p = get_process(slot_to_remove->process->ppid); 

	if (p->state == WAITING) {
		set_state(slot_to_remove->process->ppid, PAUSED);
	}

}

void kill(int pid) {

	bool flags = k_mutex(FALSE);
	
	if (pid < 0) {
		return;
	}

	bool suicide = FALSE;

	if (get_pid() == pid)
		suicide = !suicide;

	Process_Node * this = current_node;

	do
	{
		if (this->process->ppid == pid){
			this->process->ppid = 0;
		}

		this=this->next;
	} while(this!=current_node);

	remove_process(pid);
	pid_count--;

	if (suicide) {
		force_schedule();
	}

	k_mutex(flags);

}

void set_state(int pid, uint8_t state) {
	Process_Node* start = current_node;
	Process_Node* curr = current_node;

	if (current_node == NULL) {
		return;
	}
	else if (curr == curr->next && curr->process->pid == pid) {
		curr->process->state = state;
		return;
	}

	do {
		curr = curr->next;
	} while (curr != start && curr->process->pid != pid);

	if (curr->process->pid == pid) {
		curr->process->state = state;
	}
}

void set_sleep(int pid, int time) {
	Process_Node* start = current_node;
	Process_Node* curr = current_node;

	if (current_node == NULL) {
		return;
	}
	else if (curr == curr->next && curr->process->pid == pid) {
		curr->process->state = SLEEPING;
		curr->process->sleep_time = time;
		return;
	}

	do {
		curr = curr->next;
	} while (curr != start && curr->process->pid != pid);

	if (curr->process->pid == pid) {
		curr->process->state = SLEEPING;
		curr->process->sleep_time = time;
	}

}

void sleep_pid(int pid){
	//set_state(pid, SLEEPING);
}

void set_wake_up(int pid) {

	Process_Node* start = current_node;
	Process_Node* curr = current_node;

	if (current_node == NULL) {
		return;
	}
	else if (curr == curr->next && curr->process->pid == pid) {
		curr->process->state = PAUSED;
		curr->process->sleep_time = 0;
		return;
	}

	do {
		curr = curr->next;

	} while (curr != start && curr->process->pid != pid);

	if (curr->process->pid == pid) {
		curr->process->state = PAUSED;
		curr->process->sleep_time = 0;
	}

}

void wait_pid(int pid) {
	current_node->process->state = WAITING;
	force_schedule();
}


void sleep_process(int pid, int time) {

	set_sleep(pid, time);

	force_schedule();

}

void wake_up_process(int pid){


	set_wake_up(pid);	

}

void check_sleep(){ //llamar despues del pithandler cuando termina

	Process_Node * this = current_node;
	Process * aux_proc;

	if (current_node == NULL)
		return;
	do
	{
		if (this->process->state == SLEEPING){
			aux_proc = this->process;
			aux_proc->sleep_time--;

			if (aux_proc->sleep_time == 0)
				wake_up_process(aux_proc->pid);

		}

		this=this->next;
	} while (this != current_node);

}

void next_process() {

	Process* current_proc = current_node->process;

	Process * prev_proc;
	Process_Node * this = current_node;


	do {
		prev_proc = current_node->process;
		current_node = current_node->next;

	} while (current_node->process->state != PAUSED && (this != current_node) );


	if (current_proc->state == ACTIVE && prev_proc->pid != current_node->process->pid) {
		set_state(current_proc->pid, PAUSED); //seteo en pausa al que estaba corriendo
	}

	set_state(current_node->process->pid, ACTIVE);
	
}

struct Page * get_process_page(){
	if (current_node == NULL) {
		return 0;
	}
	return current_node->process->page;
}

Process_Node * get_current() {
	return current_node;
}

Process* get_process(int pid) {
	Process_Node* start = current_node;
	Process_Node* curr = current_node;

	do {
		curr = curr->next;
	} while (curr->process->pid != pid && curr != start);
		
	return curr->process;
}

int get_pid() {
	if (current_node == NULL) {
		return 0;
	}
	return current_node->process->pid;
}

int get_ppid() {
	if (current_node == NULL) {
		return 0;
	}
	return current_node->process->ppid;
}

int get_foreground_pid() {
	return foreground_pid;
}

void set_foreground_pid(int pid) {
	foreground_pid = pid;
}

static char * state_to_string(int state) {
	switch (state) {
		case SLEEPING:
			return "SLEEPING";

		case ACTIVE:
			return "ACTIVE";

		case PAUSED:
			return "PAUSED";

		case WAITING:
			return "WAITING";

		default:
			return "UNKNOWN";
	}
}

void list_processes() {
	
	Process_Node * this = current_node;

	do
	{
		print_string("PID: ");
		print_num_dec(this->process->pid);
		print_string(" Estado: ");
		print_string(state_to_string(this->process->state));
		print_string(" Nombre: ");
		print_string(this->process->name);
		print_line();
		this=this->next;
	} while (this != current_node);

}