#ifndef AUDIO_H
#define AUDIO_H

#include <lib.h>

typedef enum {
	DO = 0,
	DO_SOSTENIDO = 1,
	RE = 2,
	RE_SOSTENIDO = 3,
	MI = 4,
	FA = 5,
	FA_SOSTENIDO = 6,
	SOL = 7,
	SOL_SOSTENIDO = 8,
	LA = 9,
	LA_SOSTENIDO = 10,
	SI = 11,
} notes;

void mute();
uint8_t getAudioPlaying();
void disableAudio();
void audio_init();
void loadSong(char* songFmt);
void playSong();
void playNote(uint8_t note, int duration,uint8_t octave);
void beep();
void playTrainSound();

#endif

