// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// 11/15/2021 
#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>

void Sound_Init(void);

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the timer interrupt.
// It starts the sound, and the timer ISR does the output
// feel free to change the parameters
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Init(void);
void playRowClear(void);
void playtetris(void);
void playgameover(void);


#endif
