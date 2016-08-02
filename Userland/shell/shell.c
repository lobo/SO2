#include <stdint.h>
#include <libc.h>
#include <typeDefs.h>
#include <syscalls.h>
#include <game.h>
#include <ps_comm.h>

#define MAX_INPUT 256

//para borrar
char * v = (char*)0xB8000 + 79 * 2;

extern char bss;
extern char endOfBinary;

static char user[] = "PoorUser:$ ";
char buffer[MAX_INPUT] = {0};
uint16_t buffer_position = 0;
static char * all_cmds[] = {"clear", "play", "help", "beep", "game", "ps", "ipcs", "test_ipc"};
static int cmds_amount = 8;

//ver si hace falta
void * memset(void * destiny, int32_t c, uint64_t length);

int cmd_parse(char* cmd);
void cmd_clear();
void cmd_help(char* param);
//void cmd_play();
void cmd_beep();
void cmd_game();
void cmd_ps();
void cmd_ipcs();
void cmd_test_ipc();

int main() {
	
	char c;

	printf(user);

	while (1) {
		while ((c = getchar()) != '\n') {
			if (buffer_position >= MAX_INPUT - 1 && c != '\b') {
				cmd_beep();
				continue;
			}
			
			switch (c) {
				case '\b':
					if (buffer_position > 0) {
						putchar('\b');
						buffer_position--;
					} else {
						cmd_beep();
					}
					break;
				
				default:
					buffer[buffer_position++] = c;
					putchar(c);
					break;
			}
		}

		buffer[buffer_position] = '\0';			//end of string
		putchar('\n');
		cmd_parse(buffer);

		printf(user);
		buffer_position = 0;
	}

	return 0xDEADBEEF;
}

//ver si hace falta
/*void * memset(void * destiation, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destiation;

	while(length--)
		dst[length] = chr;

	return destiation;
}*/

int cmd_parse(char* cmd){
	
	int i = 0;
	
	del_spaces(cmd);
	
	while(cmd[i] != ' ' && cmd[i] != '\0')
		i++;
	
	cmd[i] = 0;	//pongo un cero al final del comando y lo separo de los args si los hay
	
	for (int j = 0; j < cmds_amount; j++)
		if (strcmp(all_cmds[j], cmd) == 0){
			switch (j) {
				case 0:
					cmd_clear();
					return TRUE;
				/*case 1:
					cmd_play();
					return TRUE;*/
				case 2:
					cmd_help(cmd + i + 1);
					return TRUE;
				case 3:
					cmd_beep();
					return TRUE;
				case 4:
					cmd_game();
					return TRUE;
				case 5: 
					cmd_ps();
					return TRUE;
				case 6:
					cmd_ipcs();
					return TRUE;
				case 7:
					cmd_test_ipc();
					return TRUE;
			}
		}
	
	if (*cmd == 0)
		return FALSE;
	
	printf("Comando no reconocido. Use el comando 'help' para ver el listado de comandos.\n");
	return FALSE;
}

void cmd_clear(){
	clear_screen();
}

void cmd_help(char* param){
	
	for (int i = 0; i < cmds_amount; i++)
		if (strcmp(all_cmds[i], param) == 0){
			switch (i) {
				case 0:
					printf("\n\tclear: Borra todo el contenido de la pantalla y pone el cursor al principio de esta.\n\n");
					return;
				/*case 1:
					printf("\n\tplay: Reproduce un archivo precargado utilizando el PCSpeaker.\n\n");
					return;*/
				case 2:
					printf("\n\thelp: Muestra los comandos del sistema.\n\n");
					return;
				case 3:
					printf("\n\tbeep: Reproduce un sonido 'beep'.\n\n");
					return;
				case 4:
					printf("\n\tgame: Empieza un nuevo juego.\n\n");
					return;
				case 5:
					printf("\n\tps: Lista los procesos con sus propiedades.\n\n");
					return;
				case 6:
					printf("\n\tipcs: Muestra lista de estructuras IPS creadas en el sistema.\n\n");
					return;
				case 7:
					printf("\n\ttest_ipc: Corre un proceso que prueba los Pipes.\n\n");
					return;
			
			}
		}
	
	printf("\nListado de comandos:\n\n");
	printf("\tclear: Limpia la pantalla\n");
	//printf("\tplay: Reproduce un archivo con el PCSpeaker\n");
	printf("\thelp: Abre la lista de comandos\n");
	printf("\tbeep: Reproduce un sonido 'beep'\n");

	// Nuevas aplicaciones del User Space:
	printf("\tgame: Empieza un nuevo juego\n");
	printf("\tps: Lista los procesos con sus propiedades\n");
	printf("\tipcs: Muestra lista de estructuras IPCs creadas en el sistema\n");
	printf("\ttest_ipc: Ejecuta un proceso de prueba de IPCs\n");
	printf("\nPara ver como utilizar cada comando ejecutar 'help' seguido del comando\n");
	printf("Ejemplo: help play\n\n");
}

/*void cmd_play(){
	printf("Reproduciendo musica epica. Presione ESC para cancelar.\n");
	play_note(0, 2, 0);
}*/

void cmd_beep(){
	beep();
}

void cmd_game(){

	//sleep(0, 2);
	//sleep(0, 2);
	int child_pid = create_process("Snake game", &game_entry_point, TRUE);
	wait_pid(child_pid);
	cmd_clear();
}

void cmd_ps(){
	printf("\nProcesos en ejecucion: \n");
	list_ps();
	putchar('\n');
}

void cmd_ipcs() {
	putchar('\n');
	list_ipcs();
	putchar('\n');
}

void cmd_test_ipc() {
	int ipcs_pid = create_process("TEST-IPCs", &ipcs_test, TRUE);
	wait_pid(ipcs_pid);
}