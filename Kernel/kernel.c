#include <stdint.h>
#include <video.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <audio.h>
#include <memmanager.h>
#include <vmm.h>
#include <vesa_video.h>
#include <scheduler.h>

#define PIT_FREQ 18
#define PIT_PORT_0 0x40
#define PIT_PORT_1 0x41
#define PIT_PORT_2 0x42
#define PIT_COMMAND_PORT 0x43

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

static int pitTickCounter = 0;	

typedef int (*EntryPoint)();


void print_title(){
	print_string("\t\t\t\t _   _                      _                 _____ _____\n");
	print_string("\t\t\t\t| \\ | |                    | |               |  _  /  ___|\n");
	print_string("\t\t\t\t|  \\| | __ _ _ __ ___   ___| | ___  ___ ___  | | | \\ `--. \n");
	print_string("\t\t\t\t| . ` |/ _` | '_ ` _ \\ / _ \\ |/ _ \\/ __/ __| | | | |`--. \\\n");
	print_string("\t\t\t\t| |\\  | (_| | | | | | |  __/ |  __/\\__ \\__ \\ \\ \\_/ /\\__/ /\n");
	print_string("\t\t\t\t\\_| \\_/\\__,_|_| |_| |_|\\___|_|\\___||___/___/  \\___/\\____/\n");
}

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{

	return (void*)(
		(uint64_t)&endOfKernel
		+ PAGE_SIZE * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void set_pit_frequency(int hz)
{
    int divisor = 1193180 / hz;		/* Calculate our divisor */
    //Set interrupt frequency
    outb(PIT_COMMAND_PORT, 0x36);		/* Set our command byte 0x36 */
    outb(PIT_PORT_0, divisor & 0xFF);	/* Set low byte of divisor */
    outb(PIT_PORT_0, divisor >> 8);		/* Set high byte of divisor */
}

void pit_handler(){

	pitTickCounter++;

	if (((int)pitTickCounter % (int) 200 / 1000) < 1){
		check_sleep();
		pitTickCounter = 0;
	}

	
/*
	

	if ( (pitTickCounter % PIT_FREQ) / 1000 < 1 ){ //handle de ms > 1 hercio / 1000 = 1ms
		check_sleep();
		pitTickCounter = 0;
	}		
*/

}

void * initializeKernelBinary(){


	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	//print_string("Seteo los modulos\n\n");
	loadModules(&endOfKernelBinary, moduleAddresses);

	//print_string("Limpio la BSS\n\n");
	clearBSS(&bss, &endOfKernel - &bss);

	set_pit_frequency(150);

	//print_string("Inicializo la memoria\n\n");
	initialize_memory();
	vmm_initialize();
	vesa_set_video_mode(); 

	audio_init();
	loadSong((char*)sampleDataModuleAddress);

	//print_string("Booteo...\n");
	return getStackBase();
}

int main(){	

	clear_screen();	 

	print_title();
	print_line();
	print_line();

	Process* entry = new_process("Shell", sampleCodeModuleAddress, TRUE);
	enqueue_process(entry);
	
	//int x = 50;
	//int y= 50;
	//while (1){
	//	x+=20;
	//	y;
		//clear_screen();
		//draw_frect(x, y, 15, 15, 255, 255, 255);
    	//flush_buffer();


	//}
	

	print_string("\t\t\t\t\t\t\t        Version 0.0.1\n");
	print_string("\t\t\t\t    Martin Nagelberg - Jeremias Aagaard - Daniel Lobo\n\n");
	print_num_hex(((EntryPoint)sampleCodeModuleAddress)());
	

	return 0xDEADBEEF;
}