#include <audio.h>
#include <syscalls.h>
#include <lib.h>
#include <video.h>
#include <scheduler.h>

#define PIT_FREQ_C2 0x1234DD
#define CUARTA_OCTAVA 0
#define QUINTA_OCTAVA 1
#define MAX_NOTES_IN_SONG 64
#define NOTE_DATA_FIELDS 4

uint8_t audioPlaying = FALSE;

int octaves[2][12] = {
{262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494}, //4ta octava
{523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988} //5ta octava
};

int songBuffer[MAX_NOTES_IN_SONG][NOTE_DATA_FIELDS];
int cantNotes;

uint8_t getAudioPlaying(){

	return audioPlaying;
	
}

void disableAudio(){

	audioPlaying = FALSE;
}

void mute() {
 	uint8_t tmp = inb(0x61) & 0xFC;
 
 	outb(0x61, tmp);

}

void audio_init(){

	outb(0x43, 0xb6); // programo el pit
}


void loadSong(char* songFmt){

	//Formato de lectura: cantidad de notas-nota1-nota2. Cada nota se compone por a,b,c,d- con a=nota, b=duracion, c=octava y d=separacion con proxima nota
	char* songAux = songFmt;

	while (*songFmt != '-') //encapsular
		songFmt++;

	*songFmt = 0;

	cantNotes = atoi(songAux);

	songFmt++;
	songAux = songFmt;

	 for (int i = 0; i < cantNotes; i++){

		for (int j = 0; j < 4; j++){

			while ( (*songFmt != ',') && (*songFmt != '-') && (*songFmt != '\0') ) //fix
				songFmt++;
			*songFmt = 0;

			songBuffer[i][j] = atoi(songAux);

			songFmt++;
			songAux = songFmt;	
		}
	}
}


void playSong(){

	//audioPlaying = TRUE;

	
	//for (int i = 0; i < cantNotes; i++){

	//	if (!audioPlaying) return;

		//playNote(songBuffer[0][0],songBuffer[0][1],songBuffer[0][2]);
		//sys_sleep(songBuffer[i][3]);

	//}
	//audioPlaying = FALSE;

}

void playNote(uint8_t note, int duration, uint8_t octave) //duration esta de mas ahora aca
{

	if (audioPlaying)
		return;

	audioPlaying = TRUE;
	
	int freq_nota = octaves[octave][note];
	int counter;
	
	outb(0x61, inb(0x61) | 3); // activo el speaker
	counter = (PIT_FREQ_C2 / freq_nota); // calculo frec
	outb(0x42, counter & 0xff); // LSB
	outb(0x42, counter >> 8); // MSB
	

	audioPlaying = FALSE;
}

int beep_entry_point(){

	int mypid = get_pid();

	playNote(SI, 1, QUINTA_OCTAVA);
	sys_sleep(mypid, 50);
	mute();
	kill(mypid);

	return 0; //no se va a ejecutar igual
}

void beep(){

	Process *audio_beep = new_process("Beep", &beep_entry_point, FALSE);
	enqueue_process(audio_beep);

}

void playTrainSound(){


	playNote(DO, 3, CUARTA_OCTAVA);
	//sys_sleep(4);
	playNote(DO, 16, CUARTA_OCTAVA);

	//sys_sleep(10);

	playNote(DO, 3, CUARTA_OCTAVA);
	//sys_sleep(4);
	playNote(DO, 16, CUARTA_OCTAVA);


}

