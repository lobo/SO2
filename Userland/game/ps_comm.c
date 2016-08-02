#include <ps_comm.h>

void test_sender();

static char msg[] = "Mensaje de prueba!";

void ipcs_test() {

	int child_pid = create_process("IPCs Test Sender", &test_sender, TRUE);
	wait_pid(child_pid);

	int pipe = open_pipe(1);

	if (pipe < 1) {
		printf("Siamo fuori\n");
	}

	char req[128];
	get_pipe(pipe, req, strlen(msg));

	printf("-->Recibo del pipe el mensaje '%s'\n", req);

	kill(get_pid());
}

void test_sender() {

	int pipe = open_pipe(1);

	if (pipe < 1) {
		printf("Siamo fuori\n");
	}

	put_pipe(pipe, (void*) msg, strlen(msg));

	printf("-->Escribo en el pipe el mensaje '%s'\n", msg);

	kill(get_pid());

}